/* Header files required by Max ***********************************************/
#include "ext.h"
#include "ext_obex.h"
#include "buffer.h"

/* The class pointer **********************************************************/
static t_class *bed_class;

/* The object structure *******************************************************/
typedef struct _bed {
    t_object obj;

    t_symbol *b_name;
    t_buffer *buffer;

    float *undo_samples;
    long undo_start;
    long undo_frames;
    long can_undo;
    long undo_resize;
    long undo_cut;
} t_bed;

/* Function prototypes ********************************************************/
void *bed_new(t_symbol *s, short argc, t_atom *argv);
void bed_free(t_bed *x);

void bed_info(t_bed *x);
void bed_dblclick(t_bed *x);
void bed_bufname(t_bed *x, t_symbol *name);
void bed_normalize(t_bed *x, t_symbol *msg, short argc, t_atom *argv);
void bed_fadein(t_bed *x, double fadetime);
void bed_fadeout(t_bed *x, double fadetime);
void bed_cut(t_bed *x, double start, double end);
void bed_paste (t_bed *x, t_symbol *destname);
void bed_reverse(t_bed *x);
void bed_ring_modulation(t_bed *x, double frequency);
void bed_shuffle_n_segments(t_bed *x, long segments);
void bed_undo(t_bed *x);

/* The initialization routine *************************************************/
int C74_EXPORT main()
{
    /* Initialize the class */
    bed_class = class_new("bed",
                          (method)bed_new,
                          (method)bed_free,
                          (long)sizeof(t_bed), 0, A_GIMME, 0);

    /* Bind the object-specific methods */
    class_addmethod(bed_class, (method)bed_info, "info", 0);
    class_addmethod(bed_class, (method)bed_dblclick, "dblclick", A_CANT, 0);
    class_addmethod(bed_class, (method)bed_bufname, "name", A_SYM, 0);
    class_addmethod(bed_class, (method)bed_normalize, "normalize", A_GIMME, 0);
    class_addmethod(bed_class, (method)bed_fadein, "fadein", A_FLOAT, 0);
    class_addmethod(bed_class, (method)bed_fadeout, "fadeout", A_FLOAT, 0);
    class_addmethod(bed_class, (method)bed_cut, "cut", A_FLOAT, A_FLOAT, 0);
    class_addmethod(bed_class, (method)bed_paste, "paste", A_SYM, 0);
    class_addmethod(bed_class, (method)bed_reverse, "reverse", 0);
    class_addmethod(bed_class, (method)bed_ring_modulation, "ring", A_FLOAT, 0);
    class_addmethod(bed_class, (method)bed_shuffle_n_segments, "shuffle_n", A_LONG, 0);
    class_addmethod(bed_class, (method)bed_undo, "undo", 0);

    /* Register the class with Max */
    class_register(CLASS_BOX, bed_class);

    /* Print message to Max window */
    object_post(NULL, "bed • External was loaded");

    /* Return with no error */
    return 0;
}

/* The new and free instance routines *****************************************/
void *bed_new(t_symbol *s, short argc, t_atom *argv)
{
    /* Instantiate a new object */
    t_bed *x = (t_bed *)object_alloc(bed_class);

    /* Parse passed argument */
    atom_arg_getsym(&x->b_name, 0, argc, argv);

    /* Initialize some state variables */
    x->undo_cut = 0;

    /* Print message to Max window */
    post("bed • Object was created");

    /* Return a pointer to the new object */
    return x;
}

void bed_free(t_bed *x)
{
    /* Free allocated dynamic memory */
    sysmem_freeptr(x->undo_samples);

    /* Print message to Max window */
    post("bed • Object was deleted");
}

/* The object-specific methods ************************************************/
int bed_attach_buffer(t_bed *x)
{
    t_object *o;
    o = x->b_name->s_thing;

    if (o == NULL) {
        post("bed • \"%s\" is not a valid buffer", x->b_name->s_name);
        return 0;
    }

    if (ob_sym(o) == gensym("buffer~")) {
        x->buffer = (t_buffer *)o;
        return 1;
    } else {
        return 0;
    }
}

int bed_attach_any_buffer(t_buffer **b, t_symbol *b_name)
{
    t_object *o;
    o = b_name->s_thing;

    if (o == NULL) {
        return 0;
    }

    if (ob_sym(o) == gensym("buffer~")) {
        *b = (t_buffer *)o;
        if (*b == NULL) {
            post("bed • \"%s\" is not a valid buffer", b_name->s_name);
            return 0;
        }
        return 1;
    } else {
        post("bed • \"%s\" is not a valid buffer", b_name->s_name);
        return 0;
    }
}

/******************************************************************************/
void bed_info(t_bed *x)
{
    if (!bed_attach_buffer(x)) {
        return;
    }

    t_buffer *b;
    b = x->buffer;

    post("bed • Information:");
    post("    buffer name: %s", b->b_name->s_name);
    post("    frame count: %d", b->b_frames);
    post("    channel count: %d", b->b_nchans);
    post("    validity: %d", b->b_valid);
    post("    in-use status: %d", b->b_inuse);
}

void bed_dblclick(t_bed *x)
{
    if (!bed_attach_buffer(x)) {
        return;
    }

    t_buffer *b;
    b = x->buffer;

    object_method(&b->b_obj, gensym("dblclick"));
}

void bed_bufname(t_bed *x, t_symbol *name)
{
    x->b_name = name;
}

/******************************************************************************/
void bed_normalize(t_bed *x, t_symbol *msg, short argc, t_atom *argv)
{
    if (argc > 1) {
        error("bed • The message must have at most two members");
        return;
    }

    float newmax = 1.0;
    if (argc == 1) {
        newmax = atom_getfloat(argv);
    }

    if (!bed_attach_buffer(x)) {
        return;
    }

    t_buffer *b;
    b = x->buffer;

    ATOMIC_INCREMENT(&b->b_inuse);

    if (!b->b_valid) {
        ATOMIC_DECREMENT(&b->b_inuse);
        post("bed • Not a valid buffer!");
        return;
    }

    long chunksize = b->b_frames * b->b_nchans * sizeof(float);
    if (x->undo_samples == NULL) {
        x->undo_samples = (float *)sysmem_newptr(chunksize);
    } else {
        x->undo_samples = (float *)sysmem_resizeptr(x->undo_samples, chunksize);
    }

    if (x->undo_samples == NULL) {
        error("bed • Cannot allocate memory for undo");
        x->can_undo = 0;
        ATOMIC_DECREMENT(&b->b_inuse);
        return;
    } else {
        x->can_undo = 1;
        x->undo_start = 0;
        x->undo_frames = b->b_frames;
        x->undo_resize = 0;
        x->undo_cut = 0;
        sysmem_copyptr(b->b_samples, x->undo_samples, chunksize);
    }

    float maxamp = 0.0;
    for (int ii = 0; ii < b->b_frames * b->b_nchans; ii++) {
        if (maxamp < fabs(b->b_samples[ii])) {
            maxamp = fabs(b->b_samples[ii]);
        }
    }

    float rescale;
    if (maxamp > 1e-6) {
        rescale = newmax / maxamp;
    } else {
        post("bed • Amplitude is too low to rescale: %.2f", maxamp);
        ATOMIC_DECREMENT(&b->b_inuse);
        return;
    }

    for (int ii = 0; ii < b->b_frames * b->b_nchans; ii++) {
        b->b_samples[ii] *= rescale;
    }

    object_method(&b->b_obj, gensym("dirty"));
    ATOMIC_DECREMENT(&b->b_inuse);
}

void bed_fadein(t_bed *x, double fadetime)
{
    if (!bed_attach_buffer(x)) {
        return;
    }

    t_buffer *b;
    b = x->buffer;

    ATOMIC_INCREMENT(&b->b_inuse);

    if (!b->b_valid) {
        ATOMIC_DECREMENT(&b->b_inuse);
        post("bed • Not a valid buffer!");
        return;
    }

    long fadeframes = fadetime * 0.001 * b->b_sr;
    if (fadetime <= 0 || fadeframes > b->b_frames) {
        post("bed • %.0fms is not a valid fade-in time", fadetime);
        ATOMIC_DECREMENT(&b->b_inuse);
        return;
    }

    long chunksize = fadeframes * b->b_nchans * sizeof(float);
    if (x->undo_samples == NULL) {
        x->undo_samples = (float *)sysmem_newptr(chunksize);
    } else {
        x->undo_samples = (float *)sysmem_resizeptr(x->undo_samples, chunksize);
    }

    if (x->undo_samples == NULL) {
        error("bed • Cannot allocate memory for undo");
        x->can_undo = 0;
        ATOMIC_DECREMENT(&b->b_inuse);
        return;
    } else {
        x->can_undo = 1;
        x->undo_start = 0;
        x->undo_frames = fadeframes;
        x->undo_resize = 0;
        x->undo_cut = 0;
        sysmem_copyptr(b->b_samples, x->undo_samples, chunksize);
    }

    for (int ii = 0; ii < fadeframes; ii++) {
        for (int jj = 0; jj < b->b_nchans; jj++) {
            b->b_samples[(ii * b->b_nchans) + jj] *=
                (float)ii / (float)fadeframes;
        }
    }

    object_method(&b->b_obj, gensym("dirty"));
    ATOMIC_DECREMENT(&b->b_inuse);
}

void bed_fadeout(t_bed *x, double fadetime)
{
    if (!bed_attach_buffer(x)) {
        return;
    }

    t_buffer *b;
    b = x->buffer;

    ATOMIC_INCREMENT(&b->b_inuse);

    if (!b->b_valid) {
        ATOMIC_DECREMENT(&b->b_inuse);
        post("bed • Not a valid buffer!");
        return;
    }

    long fadeframes = fadetime * 0.001 * b->b_sr;
    if (fadetime <= 0 || fadeframes > b->b_frames) {
        post("bed • %.0fms is not a valid fade-out time", fadetime);
        ATOMIC_DECREMENT(&b->b_inuse);
        return;
    }

    long chunksize = fadeframes * b->b_nchans * sizeof(float);
    if (x->undo_samples == NULL) {
        x->undo_samples = (float *)sysmem_newptr(chunksize);
    } else {
        x->undo_samples = (float *)sysmem_resizeptr(x->undo_samples, chunksize);
    }

    if (x->undo_samples == NULL) {
        error("bed • Cannot allocate memory for undo");
        x->can_undo = 0;
        ATOMIC_DECREMENT(&b->b_inuse);
        return;
    } else {
        x->can_undo = 1;
        x->undo_start = b->b_frames - fadeframes;
        x->undo_frames = fadeframes;
        x->undo_resize = 0;
        x->undo_cut = 0;
        sysmem_copyptr(b->b_samples + x->undo_start, x->undo_samples, chunksize);
    }

    for (int ii = (int)x->undo_start; ii < x->undo_start + fadeframes; ii++) {
        for (int jj = 0; jj < b->b_nchans; jj++) {
            b->b_samples[(ii * b->b_nchans) + jj] *=
                1 - (float)(ii - x->undo_start) / (float)fadeframes;
        }
    }

    object_method(&b->b_obj, gensym("dirty"));
    ATOMIC_DECREMENT(&b->b_inuse);
}

void bed_cut(t_bed *x, double start, double end)
{
    if (!bed_attach_buffer(x)) {
        return;
    }

    t_buffer *b;
    b = x->buffer;

    ATOMIC_INCREMENT(&b->b_inuse);

    if (!b->b_valid) {
        ATOMIC_DECREMENT(&b->b_inuse);
        post("bed • Not a valid buffer!");
        return;
    }

    long startframe = start * 0.001 * b->b_sr;
    long endframe = end * 0.001 * b->b_sr;
    long cutframes = endframe - startframe;

    if (startframe < 0 || endframe > b->b_frames || startframe > endframe) {
        post("bed • %.0fms and %.0fms are not valid cut times", start, end);
        ATOMIC_DECREMENT(&b->b_inuse);
        return;
    }

    long chunksize = cutframes * b->b_nchans * sizeof(float);
    if (x->undo_samples == NULL) {
        x->undo_samples = (float *)sysmem_newptr(chunksize);
    } else {
        x->undo_samples = (float *)sysmem_resizeptr(x->undo_samples, chunksize);
    }

    if (x->undo_samples == NULL) {
        error("bed • Cannot allocate memory for undo");
        x->can_undo = 0;
        ATOMIC_DECREMENT(&b->b_inuse);
        return;
    } else {
        x->can_undo = 1;
        x->undo_start = startframe;
        x->undo_frames = cutframes;
        x->undo_resize = 1;
        x->undo_cut = 1;
        sysmem_copyptr(b->b_samples + (startframe * b->b_nchans),
                       x->undo_samples, chunksize);
    }

    long bufferframes = b->b_frames;
    long buffersize = bufferframes * b->b_nchans * sizeof(float);
    float *local_buffer = (float *)sysmem_newptr(buffersize);
    if (local_buffer == NULL) {
        error("bed • Cannot allocate memory for undo");
        x->can_undo = 0;
        ATOMIC_DECREMENT(&b->b_inuse);
        return;
    } else {
        sysmem_copyptr(b->b_samples, local_buffer, buffersize);
    }

    ATOMIC_DECREMENT(&b->b_inuse);
    t_atom rv;
    object_method_long(&b->b_obj, gensym("sizeinsamps"),
                       (b->b_frames - cutframes), &rv);
    ATOMIC_INCREMENT(&b->b_inuse);

    chunksize = startframe * b->b_nchans * sizeof(float);
    sysmem_copyptr(local_buffer, b->b_samples, chunksize);
    chunksize = (bufferframes - endframe) * b->b_nchans * sizeof(float);
    sysmem_copyptr(local_buffer + (endframe * b->b_nchans),
                   b->b_samples + (startframe * b->b_nchans),
                   chunksize);

    sysmem_freeptr(local_buffer);

    object_method(&b->b_obj, gensym("dirty"));
    ATOMIC_DECREMENT(&b->b_inuse);
}

void bed_paste (t_bed *x, t_symbol *destname)
{
    if (x->can_undo && x->undo_cut) {
        if (!bed_attach_buffer(x)) {
            return;
        }

        t_buffer *destbuf = NULL;
        if (bed_attach_any_buffer(&destbuf, destname)) {
            if (x->buffer->b_nchans != destbuf->b_nchans) {
                post("bed • Different number of channels of origin (%d) "
                     "and number of channel of destination (%d)",
                     x->buffer->b_nchans, destbuf->b_nchans);
                return;
            }

            t_atom rv;
            object_method_long(&destbuf->b_obj, gensym("sizeinsamps"),
                               x->undo_frames, &rv);
            ATOMIC_INCREMENT(&destbuf->b_inuse);
            long chunksize = x->undo_frames * destbuf->b_nchans * sizeof(float);
            sysmem_copyptr(x->undo_samples, destbuf->b_samples, chunksize);
            ATOMIC_DECREMENT(&destbuf->b_inuse);

        } else {
            post("bed • \"%s\" is not a valid buffer", destname->s_name);
            return;
        }
    } else {
        post("bed • Nothing to paste");
        return;
    }
}

void bed_reverse(t_bed *x)
{
    if (!bed_attach_buffer(x)) {
        return;
    }

    t_buffer *b;
    b = x->buffer;

    ATOMIC_INCREMENT(&b->b_inuse);

    if (!b->b_valid) {
        ATOMIC_DECREMENT(&b->b_inuse);
        post("bed • Not a valid buffer!");
        return;
    }

    long chunksize = b->b_frames * b->b_nchans * sizeof(float);
    if (x->undo_samples == NULL) {
        x->undo_samples = (float *)sysmem_newptr(chunksize);
    } else {
        x->undo_samples = (float *)sysmem_resizeptr(x->undo_samples, chunksize);
    }

    if (x->undo_samples == NULL) {
        error("bed • Cannot allocate memory for undo");
        x->can_undo = 0;
        ATOMIC_DECREMENT(&b->b_inuse);
        return;
    } else {
        x->can_undo = 1;
        x->undo_start = 0;
        x->undo_frames = b->b_frames;
        x->undo_resize = 0;
        x->undo_cut = 0;
        sysmem_copyptr(b->b_samples, x->undo_samples, chunksize);
    }

    float temp;
    for (int ii = 0; ii < ceil(b->b_frames / 2); ii++) {
        for (int jj = 0; jj < b->b_nchans; jj++) {
            temp = b->b_samples[(ii * b->b_nchans) + jj];
            b->b_samples[(ii * b->b_nchans) + jj] =
                b->b_samples[(b->b_frames - 1 -  ii * b->b_nchans) + jj];
            b->b_samples[(b->b_frames - 1 -  ii * b->b_nchans) + jj] = temp;
        }
    }

    object_method(&b->b_obj, gensym("dirty"));
    ATOMIC_DECREMENT(&b->b_inuse);
}

void bed_ring_modulation(t_bed *x, double frequency)
{
    if (!bed_attach_buffer(x)) {
        return;
    }

    t_buffer *b;
    b = x->buffer;

    ATOMIC_INCREMENT(&b->b_inuse);

    if (!b->b_valid) {
        ATOMIC_DECREMENT(&b->b_inuse);
        post("bed • Not a valid buffer!");
        return;
    }

    long chunksize = b->b_frames * b->b_nchans * sizeof(float);
    if (x->undo_samples == NULL) {
        x->undo_samples = (float *)sysmem_newptr(chunksize);
    } else {
        x->undo_samples = (float *)sysmem_resizeptr(x->undo_samples, chunksize);
    }

    if (x->undo_samples == NULL) {
        error("bed • Cannot allocate memory for undo");
        x->can_undo = 0;
        ATOMIC_DECREMENT(&b->b_inuse);
        return;
    } else {
        x->can_undo = 1;
        x->undo_start = 0;
        x->undo_frames = b->b_frames;
        x->undo_resize = 0;
        x->undo_cut = 0;
        sysmem_copyptr(b->b_samples, x->undo_samples, chunksize);
    }

    float twopi = 8.0 * atan(1.0);
    float oneoversr = 1.0 / b->b_sr;
    for (int ii = 0; ii < b->b_frames; ii++) {
        for (int jj = 0; jj < b->b_nchans; jj++) {
            b->b_samples[(ii * b->b_nchans) + jj] *=
                sin(twopi * frequency * ii * oneoversr);
        }
    }

    object_method(&b->b_obj, gensym("dirty"));
    ATOMIC_DECREMENT(&b->b_inuse);
}

void bed_shuffle_n_segments(t_bed *x, long segments)
{
    if (!bed_attach_buffer(x)) {
        return;
    }

    t_buffer *b;
    b = x->buffer;

    ATOMIC_INCREMENT(&b->b_inuse);

    if (!b->b_valid) {
        ATOMIC_DECREMENT(&b->b_inuse);
        post("bed • Not a valid buffer!");
        return;
    }

    long chunksize = b->b_frames * b->b_nchans * sizeof(float);
    if (x->undo_samples == NULL) {
        x->undo_samples = (float *)sysmem_newptr(chunksize);
    } else {
        x->undo_samples = (float *)sysmem_resizeptr(x->undo_samples, chunksize);
    }

    if (x->undo_samples == NULL) {
        error("bed • Cannot allocate memory for undo");
        x->can_undo = 0;
        ATOMIC_DECREMENT(&b->b_inuse);
        return;
    } else {
        x->can_undo = 1;
        x->undo_start = 0;
        x->undo_frames = b->b_frames;
        x->undo_resize = 0;
        x->undo_cut = 0;
        sysmem_copyptr(b->b_samples, x->undo_samples, chunksize);
    }

    long totallength = b->b_frames;
    long basesegmentlength = ceil(totallength / segments);

    long randomsegment;
    long start;
    long end;
    long bufferlength;
    long buffersize;
    float *local_buffer = NULL;

    while (segments > 0) {
        randomsegment = rand() % segments;
        start = randomsegment * basesegmentlength;
        end = start + basesegmentlength;

        if (end > totallength) {
            end = totallength;
        }

        bufferlength = (end - start);
        buffersize = bufferlength * b->b_nchans * sizeof(float);
        if (local_buffer == NULL) {
            local_buffer = (float *)sysmem_newptr(buffersize);
        } else {
            local_buffer = (float *)sysmem_resizeptr(local_buffer, buffersize);
        }
        sysmem_copyptr(b->b_samples + (start * b->b_nchans),
                       local_buffer,
                       buffersize);

        for (long ii = end; ii < totallength; ii++) {
            for (int jj = 0; jj < b->b_nchans; jj++) {
                b->b_samples[((ii - bufferlength) * b->b_nchans) + jj] =
                    b->b_samples[(ii * b->b_nchans) + jj];
            }
        }
        sysmem_copyptr(local_buffer,
                       b->b_samples + (totallength - bufferlength) * b->b_nchans,
                       buffersize);

        totallength -= bufferlength;
        segments--;
    }

    sysmem_freeptr(local_buffer);

    object_method(&b->b_obj, gensym("dirty"));
    ATOMIC_DECREMENT(&b->b_inuse);
}

/******************************************************************************/

void bed_undo(t_bed *x)
{
    if (!x->can_undo) {
        post("bed • Nothing to undo");
        return;
    }

    if (!bed_attach_buffer(x)) {
        return;
    }

    t_buffer *b;
    b = x->buffer;

    ATOMIC_INCREMENT(&b->b_inuse);

    if (!b->b_valid) {
        ATOMIC_DECREMENT(&b->b_inuse);
        post("bed • Not a valid buffer!");
        return;
    }

    if (x->undo_cut) {
        long bufferframes = b->b_frames;
        long buffersize = bufferframes * b->b_nchans * sizeof(float);
        float *local_buffer = (float *)sysmem_newptr(buffersize);
        if (local_buffer == NULL) {
            error("bed • Cannot allocate memory for undo");
            x->can_undo = 0;
            ATOMIC_DECREMENT(&b->b_inuse);
            return;
        } else {
            sysmem_copyptr(b->b_samples, local_buffer, buffersize);
        }

        ATOMIC_DECREMENT(&b->b_inuse);
        t_atom rv;
        object_method_long(&b->b_obj, gensym("sizeinsamps"),
                           (bufferframes + x->undo_frames), &rv);
        ATOMIC_INCREMENT(&b->b_inuse);

        long chunksize = x->undo_start * b->b_nchans * sizeof(float);
        sysmem_copyptr(local_buffer, b->b_samples, chunksize);

        chunksize = x->undo_frames * b->b_nchans * sizeof(float);
        sysmem_copyptr(x->undo_samples,
                       b->b_samples + (x->undo_start * b->b_nchans),
                       chunksize);
        chunksize = (bufferframes - x->undo_start) * b->b_nchans * sizeof(float);
        sysmem_copyptr(local_buffer + (x->undo_start * b->b_nchans),
                       b->b_samples + (x->undo_start + x->undo_frames) * b->b_nchans,
                       chunksize);

        sysmem_freeptr(local_buffer);

        x->undo_cut = 0;
        
        object_method(&b->b_obj, gensym("dirty"));
        ATOMIC_DECREMENT(&b->b_inuse);
        return;
    }

    if (x->undo_resize) {
        ATOMIC_DECREMENT(&b->b_inuse);
        t_atom rv;
        object_method_long(&b->b_obj, gensym("sizeinsamps"), x->undo_frames, &rv);
        ATOMIC_INCREMENT(&b->b_inuse);
    }

    long chunksize = x->undo_frames * b->b_nchans * sizeof(float);
    sysmem_copyptr(x->undo_samples, b->b_samples + x->undo_start, chunksize);
    x->can_undo = 0;

    object_method(&b->b_obj, gensym("dirty"));
    ATOMIC_DECREMENT(&b->b_inuse);
}

/******************************************************************************/
