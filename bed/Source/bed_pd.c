/* Header files required by Pd ************************************************/
#include "m_pd.h"
#include <math.h>
#include <string.h>
#include "stdlib.h"

/* The class pointer **********************************************************/
static t_class *bed_class;

/* The object structure *******************************************************/
typedef struct _bed {
    t_object obj;

    t_symbol *b_name;
    t_garray *buffer;

    long b_valid;
    long b_frames;
    float *b_samples;
    float b_sr;

    float *undo_samples;
    long undo_start;
    long undo_frames;
    long can_undo;
    long undo_resize;
    long undo_cut;
} t_bed;

/* Function prototypes ********************************************************/
void *bed_new(t_symbol *s);
void bed_free(t_bed *x);

void bed_info(t_bed *x);
void bed_bufname(t_bed *x, t_symbol *name);
void bed_normalize(t_bed *x, t_symbol *msg, short argc, t_atom *argv);
void bed_fadein(t_bed *x, t_floatarg fadetime);
void bed_fadeout(t_bed *x, t_floatarg fadetime);
void bed_cut(t_bed *x, t_floatarg start, t_floatarg end);
void bed_paste (t_bed *x, t_symbol *destname);
void bed_reverse(t_bed *x);
void bed_ring_modulation(t_bed *x, t_floatarg frequency);
void bed_shuffle_n_segments(t_bed *x, t_floatarg segments);
void bed_undo(t_bed *x);

/* The initialization routine *************************************************/
#ifdef WIN32
__declspec(dllexport) void bed_setup(void);
#endif
void bed_setup(void)
{
    /* Initialize the class */
    bed_class = class_new(gensym("bed"),
                          (t_newmethod)bed_new,
                          (t_method)bed_free,
                          sizeof(t_bed), 0, A_SYMBOL, 0);

    /* Bind the object-specific methods */
    class_addmethod(bed_class, (t_method)bed_info, gensym("info"), 0);
    class_addmethod(bed_class, (t_method)bed_bufname, gensym("name"), A_SYMBOL, 0);
    class_addmethod(bed_class, (t_method)bed_normalize, gensym("normalize"), A_GIMME, 0);
    class_addmethod(bed_class, (t_method)bed_fadein, gensym("fadein"), A_FLOAT, 0);
    class_addmethod(bed_class, (t_method)bed_fadeout, gensym("fadeout"), A_FLOAT, 0);
    class_addmethod(bed_class, (t_method)bed_cut, gensym("cut"), A_FLOAT, A_FLOAT, 0);
    class_addmethod(bed_class, (t_method)bed_paste, gensym("paste"), A_SYMBOL, 0);
    class_addmethod(bed_class, (t_method)bed_reverse, gensym("reverse"), 0);
    class_addmethod(bed_class, (t_method)bed_ring_modulation, gensym("ring"), A_FLOAT, 0);
    class_addmethod(bed_class, (t_method)bed_shuffle_n_segments, gensym("shuffle_n"), A_FLOAT, 0);
    class_addmethod(bed_class, (t_method)bed_undo, gensym("undo"), 0);

    /* Print message to Max window */
    post("bed • External was loaded");
}

/* The new and free instance routines *****************************************/
void *bed_new(t_symbol *s)
{
    /* Instantiate a new object */
    t_bed *x = (t_bed *)pd_new(bed_class);

    /* Parse passed argument */
    x->b_name = s;

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
    freebytes(x->undo_samples, x->undo_frames);

    /* Print message to Max window */
    post("bed • Object was deleted");
}

/* The object-specific methods ************************************************/
int bed_attach_buffer(t_bed *x)
{
    t_symbol *b_name;
    float *b_samples;
    int b_frames;

    b_name = x->b_name;
    x->b_valid = 0;

    if (!(x->buffer = (t_garray *)pd_findbyclass(b_name, garray_class))) {
        if (b_name->s_name) {
            post("bed • \"%s\" is not a valid buffer", x->b_name->s_name);
        }
        return (int)x->b_valid;
    }

    if (!garray_getfloatarray(x->buffer, &b_frames, &b_samples)) {
        post("bed • \"%s\" is not a valid buffer", x->b_name->s_name);
        return (int)x->b_valid;
    } else {
        x->b_valid = 1;
        x->b_frames = (long)b_frames;
        x->b_samples = b_samples;
        x->b_sr = sys_getsr();

        if (x->b_sr <= 0) {
            x->b_sr = 44100.0;
        }
    }
    return (int)x->b_valid;
}

int bed_attach_any_buffer(t_garray **destbuf, t_symbol *b_name)
{
    if (!(*destbuf = (t_garray *)pd_findbyclass(b_name, garray_class))) {
        if (b_name->s_name) {
            post("bed • \"%s\" is not a valid buffer", b_name->s_name);
        }
        return 0;
    }
    return 1;
}

/******************************************************************************/
void bed_info(t_bed *x)
{
    if (!bed_attach_buffer(x)) {
        return;
    }

    post("bed • Information:");
    post("    buffer name: %s", x->b_name->s_name);
    post("    frame count: %d", x->b_frames);
    post("    validity: %d", x->b_valid);
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

    if (!x->b_valid) {
        post("bed • Not a valid buffer!");
        return;
    }

    long chunksize = x->b_frames * sizeof(float);
    if (x->undo_samples == NULL) {
        x->undo_samples = getbytes(chunksize);
    } else {
        long oldsize = x->undo_frames * sizeof(float);
        x->undo_samples = resizebytes(x->undo_samples, oldsize, chunksize);
    }

    if (x->undo_samples == NULL) {
        error("bed • Cannot allocate memory for undo");
        x->can_undo = 0;
        return;
    } else {
        x->can_undo = 1;
        x->undo_start = 0;
        x->undo_frames = x->b_frames;
        x->undo_resize = 0;
        x->undo_cut = 0;
        memcpy(x->undo_samples, x->b_samples, chunksize);
    }

    float maxamp = 0.0;
    for (int ii = 0; ii < x->b_frames; ii++) {
        if (maxamp < fabs(x->b_samples[ii])) {
            maxamp = fabs(x->b_samples[ii]);
        }
    }

    float rescale;
    if (maxamp > 1e-6) {
        rescale = newmax / maxamp;
    } else {
        post("bed • Amplitude is too low to rescale: %.2f", maxamp);
        return;
    }

    for (int ii = 0; ii < x->b_frames; ii++) {
        x->b_samples[ii] *= rescale;
    }

    garray_redraw(x->buffer);
}

void bed_fadein(t_bed *x, t_floatarg fadetime)
{
    if (!bed_attach_buffer(x)) {
        return;
    }

    if (!x->b_valid) {
        post("bed • Not a valid buffer!");
        return;
    }

    long fadeframes = fadetime * 0.001 * x->b_sr;
    if (fadetime <= 0 || fadeframes > x->b_frames) {
        post("bed • %.0fms is not a valid fade-in time", fadetime);
        return;
    }

    long chunksize = fadeframes * sizeof(float);
    if (x->undo_samples == NULL) {
        x->undo_samples = getbytes(chunksize);
    } else {
        long oldsize = x->undo_frames * sizeof(float);
        x->undo_samples = resizebytes(x->undo_samples, oldsize, chunksize);
    }

    if (x->undo_samples == NULL) {
        error("bed • Cannot allocate memory for undo");
        x->can_undo = 0;
        return;
    } else {
        x->can_undo = 1;
        x->undo_start = 0;
        x->undo_frames = fadeframes;
        x->undo_resize = 0;
        x->undo_cut = 0;
        memcpy(x->undo_samples, x->b_samples, chunksize);
    }

    for (int ii = 0; ii < fadeframes; ii++) {
        x->b_samples[ii] *= (float)ii / (float)fadeframes;
    }

    garray_redraw(x->buffer);
}

void bed_fadeout(t_bed *x, t_floatarg fadetime)
{
    if (!bed_attach_buffer(x)) {
        return;
    }

    if (!x->b_valid) {
        post("bed • Not a valid buffer!");
        return;
    }

    long fadeframes = fadetime * 0.001 * x->b_sr;
    if (fadetime <= 0 || fadeframes > x->b_frames) {
        post("bed • %.0fms is not a valid fade-out time", fadetime);
        return;
    }

    long chunksize = fadeframes * sizeof(float);
    if (x->undo_samples == NULL) {
        x->undo_samples = getbytes(chunksize);
    } else {
        long oldsize = x->undo_frames * sizeof(float);
        x->undo_samples = resizebytes(x->undo_samples, oldsize, chunksize);
    }

    if (x->undo_samples == NULL) {
        error("bed • Cannot allocate memory for undo");
        x->can_undo = 0;
        return;
    } else {
        x->can_undo = 1;
        x->undo_start = x->b_frames - fadeframes;
        x->undo_frames = fadeframes;
        x->undo_resize = 0;
        x->undo_cut = 0;
        memcpy(x->undo_samples, x->b_samples + x->undo_start, chunksize);
    }

    for (int ii = (int)x->undo_start; ii < x->undo_start + fadeframes; ii++) {
        x->b_samples[ii] *= 1 - (float)(ii - x->undo_start) / (float)fadeframes;
    }

    garray_redraw(x->buffer);
}

void bed_cut(t_bed *x, t_floatarg start, t_floatarg end)
{
    if (!bed_attach_buffer(x)) {
        return;
    }

    if (!x->b_valid) {
        post("bed • Not a valid buffer!");
        return;
    }

    long startframe = start * 0.001 * x->b_sr;
    long endframe = end * 0.001 * x->b_sr;
    long cutframes = endframe - startframe;

    if (startframe < 0 || endframe > x->b_frames || startframe > endframe) {
        post("bed • %.0fms and %.0fms are not valid cut times", start, end);
        return;
    }

    long chunksize = cutframes * sizeof(float);
    if (x->undo_samples == NULL) {
        x->undo_samples = getbytes(chunksize);
    } else {
        long oldsize = x->undo_frames * sizeof(float);
        x->undo_samples = resizebytes(x->undo_samples, oldsize, chunksize);
    }

    if (x->undo_samples == NULL) {
        error("bed • Cannot allocate memory for undo");
        x->can_undo = 0;
        return;
    } else {
        x->can_undo = 1;
        x->undo_start = startframe;
        x->undo_frames = cutframes;
        x->undo_resize = 1;
        x->undo_cut = 1;
        memcpy(x->undo_samples, x->b_samples + startframe, chunksize);
    }

    long bufferframes = x->b_frames;
    long buffersize = bufferframes * sizeof(float);
    float *local_buffer = getbytes(buffersize);
    if (local_buffer == NULL) {
        error("bed • Cannot allocate memory for undo");
        x->can_undo = 0;
        return;
    } else {
        memcpy(local_buffer, x->b_samples, buffersize);
    }

    garray_resize(x->buffer, x->b_frames - cutframes);

    chunksize = startframe * sizeof(float);
    memcpy(x->b_samples, local_buffer, chunksize);
    chunksize = (bufferframes - endframe) * sizeof(float);
    memcpy(x->b_samples + startframe, local_buffer + endframe, chunksize);

    freebytes(local_buffer, buffersize);

    garray_redraw(x->buffer);
}

void bed_paste (t_bed *x, t_symbol *destname)
{
    if (x->can_undo && x->undo_cut) {
        if (!bed_attach_buffer(x)) {
            return;
        }

        t_garray *destbuf = NULL;
        int destbuf_b_frames;
        float *destbuf_b_samples;
        if (bed_attach_any_buffer(&destbuf, destname)) {
            if (!garray_getfloatarray(destbuf, &destbuf_b_frames, &destbuf_b_samples)) {
                post("bed • \"%s\" is not a valid buffer", destname->s_name);
                return;

            }
            garray_resize(destbuf, x->undo_frames);
            if (!garray_getfloatarray(destbuf, &destbuf_b_frames, &destbuf_b_samples)) {
                post("bed • \"%s\" is not a valid buffer", destname->s_name);
                return;

            }

            long chunksize = x->undo_frames * sizeof(float);
            memcpy(destbuf_b_samples, x->undo_samples, chunksize);

            garray_redraw(destbuf);

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

    if (!x->b_valid) {
        post("bed • Not a valid buffer!");
        return;
    }

    long chunksize = x->b_frames * sizeof(float);
    if (x->undo_samples == NULL) {
        x->undo_samples = getbytes(chunksize);
    } else {
        long oldsize = x->undo_frames * sizeof(float);
        x->undo_samples = resizebytes(x->undo_samples, oldsize, chunksize);
    }

    if (x->undo_samples == NULL) {
        error("bed • Cannot allocate memory for undo");
        x->can_undo = 0;
        return;
    } else {
        x->can_undo = 1;
        x->undo_start = 0;
        x->undo_frames = x->b_frames;
        x->undo_resize = 0;
        x->undo_cut = 0;
        memcpy(x->undo_samples, x->b_samples, chunksize);
    }

    float temp;
    for (int ii = 0; ii < ceil(x->b_frames / 2); ii++) {
        temp = x->b_samples[ii];
        x->b_samples[ii] = x->b_samples[x->b_frames - 1 -  ii];
        x->b_samples[x->b_frames - 1 -  ii] = temp;
    }

    garray_redraw(x->buffer);
}

void bed_ring_modulation(t_bed *x, t_floatarg frequency)
{
    if (!bed_attach_buffer(x)) {
        return;
    }

    if (!x->b_valid) {
        post("bed • Not a valid buffer!");
        return;
    }

    long chunksize = x->b_frames * sizeof(float);
    if (x->undo_samples == NULL) {
        x->undo_samples = getbytes(chunksize);
    } else {
        long oldsize = x->undo_frames * sizeof(float);
        x->undo_samples = resizebytes(x->undo_samples, oldsize, chunksize);
    }

    if (x->undo_samples == NULL) {
        error("bed • Cannot allocate memory for undo");
        x->can_undo = 0;
        return;
    } else {
        x->can_undo = 1;
        x->undo_start = 0;
        x->undo_frames = x->b_frames;
        x->undo_resize = 0;
        x->undo_cut = 0;
        memcpy(x->undo_samples, x->b_samples, chunksize);
    }

    float twopi = 8.0 * atan(1.0);
    float oneoversr = 1.0 / x->b_sr;
    for (int ii = 0; ii < x->b_frames; ii++) {
        x->b_samples[ii] *= sin(twopi * frequency * ii * oneoversr);
    }

    garray_redraw(x->buffer);
}

void bed_shuffle_n_segments(t_bed *x, t_floatarg segments)
{
    if (!bed_attach_buffer(x)) {
        return;
    }

    if (!x->b_valid) {
        post("bed • Not a valid buffer!");
        return;
    }

    long chunksize = x->b_frames * sizeof(float);
    if (x->undo_samples == NULL) {
        x->undo_samples = getbytes(chunksize);
    } else {
        long oldsize = x->undo_frames * sizeof(float);
        x->undo_samples = resizebytes(x->undo_samples, oldsize, chunksize);
    }

    if (x->undo_samples == NULL) {
        error("bed • Cannot allocate memory for undo");
        x->can_undo = 0;
        return;
    } else {
        x->can_undo = 1;
        x->undo_start = 0;
        x->undo_frames = x->b_frames;
        x->undo_resize = 0;
        x->undo_cut = 0;
        memcpy(x->undo_samples, x->b_samples, chunksize);
    }

    long totallength = x->b_frames;
    long basesegmentlength = ceil(totallength / segments);

    long randomsegment;
    long start;
    long end;
    long bufferlength;
    long buffersize = 0;
    float *local_buffer = NULL;

    while (segments > 0) {
        randomsegment = rand() % (long)segments;
        start = randomsegment * basesegmentlength;
        end = start + basesegmentlength;

        if (end > totallength) {
            end = totallength;
        }

        bufferlength = (end - start);
        buffersize = bufferlength * sizeof(float);
        if (local_buffer == NULL) {
            local_buffer = getbytes(buffersize);
        } else {
            long oldsize = x->undo_frames * sizeof(float);
            local_buffer = resizebytes(local_buffer, oldsize, chunksize);
        }
        memcpy(local_buffer,
               x->b_samples + start,
               buffersize);

        for (long ii = end; ii < totallength; ii++) {
            x->b_samples[ii - bufferlength] = x->b_samples[ii];
        }
        memcpy(x->b_samples + (totallength - bufferlength),
               local_buffer,
               buffersize);

        totallength -= bufferlength;
        segments--;
    }

    freebytes(local_buffer, buffersize);
    
    garray_redraw(x->buffer);
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

    if (!x->b_valid) {
        post("bed • Not a valid buffer!");
        return;
    }

    if (x->undo_cut) {
        long bufferframes = x->b_frames;
        long buffersize = bufferframes * sizeof(float);
        float *local_buffer = getbytes(buffersize);
        if (local_buffer == NULL) {
            error("bed • Cannot allocate memory for undo");
            x->can_undo = 0;
            return;
        } else {
            memcpy(local_buffer, x->b_samples, buffersize);
        }

        garray_resize(x->buffer, bufferframes + x->undo_frames);

        long chunksize = x->undo_start * sizeof(float);
        memcpy(x->b_samples, local_buffer, chunksize);

        chunksize = x->undo_frames * sizeof(float);
        memcpy(x->b_samples + x->undo_start, x->undo_samples, chunksize);
        chunksize = (bufferframes - x->undo_start) * sizeof(float);
        memcpy(x->b_samples + x->undo_start + x->undo_frames,
               local_buffer + x->undo_start, chunksize);

        freebytes(local_buffer, buffersize);

        x->undo_cut = 0;

        garray_redraw(x->buffer);
        return;
    }
    
    if (x->undo_resize) {
        garray_resize(x->buffer, x->undo_frames);
    }
    
    long chunksize = x->undo_frames * sizeof(float);
    memcpy(x->b_samples + x->undo_start, x->undo_samples, chunksize);
    x->can_undo = 0;
    
    garray_redraw(x->buffer);
}

/******************************************************************************/
