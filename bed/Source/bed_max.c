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
} t_bed;

/* Function prototypes ********************************************************/
void *bed_new(t_symbol *s, short argc, t_atom *argv);
void bed_free(t_bed *x);

int bed_attach_buffer(t_bed *x);
void bed_info(t_bed *x);
void bed_bufname(t_bed *x, t_symbol *name);
void bed_normalize(t_bed *x, t_symbol *msg, short argc, t_atom *argv);
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
    class_addmethod(bed_class, (method)bed_bufname, "name", A_SYM, 0);
    class_addmethod(bed_class, (method)bed_normalize, "normalize", A_GIMME, 0);
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

    /* Print message to Max window */
    post("bed • Object was created");

    /* Return a pointer to the new object */
    return x;
}

void bed_free(t_bed *x)
{
    /* Print message to Max window */
    post("bed • Object was deleted");
}

/* The object-specific methods ************************************************/
int bed_attach_buffer(t_bed *x)
{
    t_object *o;
    o = x->b_name->s_thing;

    if (o == NULL) {
        post("\"%s\" is not a valid buffer", x->b_name->s_name);
        return 0;
    }

    if (ob_sym(o) == gensym("buffer~")) {
        x->buffer = (t_buffer *)o;
        return 1;
    } else {
        return 0;
    }
}

void bed_info(t_bed *x)
{
    if (!bed_attach_buffer(x)) {
        return;
    }

    t_buffer *b;
    b = x->buffer;

    post("buffer name: %s", b->b_name->s_name);
    post("frame count: %d", b->b_frames);
    post("channel count: %d", b->b_nchans);
    post("validity: %d", b->b_valid);
    post("in-use status: %d", b->b_inuse);
}

void bed_bufname(t_bed *x, t_symbol *name)
{
    x->b_name = name;
}

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
        post("bed • Amplitude is too low to rescale: %f", maxamp);
        ATOMIC_DECREMENT(&b->b_inuse);
        return;
    }

    for (int ii = 0; ii < b->b_frames * b->b_nchans; ii++) {
        b->b_samples[ii] *= rescale;
    }

    object_method(&b->b_obj, gensym("dirty"));
    ATOMIC_DECREMENT(&b->b_inuse);
}

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
