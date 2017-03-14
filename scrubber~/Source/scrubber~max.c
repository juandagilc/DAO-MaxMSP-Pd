/* Common header files ********************************************************/
#include "scrubber~common.h"

/* Function prototypes ********************************************************/
void *scrubber_new(t_symbol *s, short argc, t_atom *argv);

void scrubber_float(t_scrubber *x, double farg);
void scrubber_assist(t_scrubber *x, void *b, long msg, long arg, char *dst);

/* The 'initialization' routine ***********************************************/
int C74_EXPORT main()
{
    /* Initialize the class */
    scrubber_class = class_new("scrubber~",
                               (method)scrubber_new,
                               (method)scrubber_free,
                               sizeof(t_scrubber), 0, A_GIMME, 0);

    /* Bind the DSP method, which is called when the DACs are turned on */
    class_addmethod(scrubber_class, (method)scrubber_dsp, "dsp", A_CANT, 0);

    /* Bind the float method, which is called when floats are sent to inlets */
    class_addmethod(scrubber_class, (method)scrubber_float, "float", A_FLOAT, 0);

    /* Bind the assist method, which is called on mouse-overs to inlets and outlets */
    class_addmethod(scrubber_class, (method)scrubber_assist, "assist", A_CANT, 0);

    /* Bind the object-specific methods */
    class_addmethod(scrubber_class, (method)scrubber_sample, "sample", 0);
    class_addmethod(scrubber_class, (method)scrubber_overlap, "overlap", A_FLOAT, 0);
    class_addmethod(scrubber_class, (method)scrubber_resize, "resize", A_FLOAT, 0);

    /* Add standard Max methods to the class */
    class_dspinit(scrubber_class);

    /* Register the class with Max */
    class_register(CLASS_BOX, scrubber_class);

    /* Print message to Max window */
    object_post(NULL, "scrubber~ • External was loaded");

    /* Return with no error */
    return 0;
}

/* The 'new instance' routine *************************************************/
void *scrubber_new(t_symbol *s, short argc, t_atom *argv)
{
    /* Instantiate a new object */
    t_scrubber *x = (t_scrubber *)object_alloc(scrubber_class);

    return scrubber_common_new(x, argc, argv);
}

/******************************************************************************/






/* The 'float' method *********************************************************/
void scrubber_float(t_scrubber *x, double farg)
{
    // nothing

    object_post((t_object *)x, "Receiving floats");
}

/* The 'assist' method ********************************************************/
void scrubber_assist(t_scrubber *x, void *b, long msg, long arg, char *dst)
{
    /* Document inlet functions */
    if (msg == ASSIST_INLET) {
        switch (arg) {
            case I_REAL: sprintf(dst, "(signal) Real part"); break;
            case I_IMAG: sprintf(dst, "(signal) Imaginary part"); break;
            case I_SPEED: sprintf(dst, "(signal) Speed"); break;
            case I_POSITION: sprintf(dst, "(signal) Position"); break;
        }
    }

    /* Document outlet functions */
    else if (msg == ASSIST_OUTLET) {
        switch (arg) {
            case O_REAL: sprintf(dst, "(signal) Real part"); break;
            case O_IMAG: sprintf(dst, "(signal) Imaginary part"); break;
        }
    }
}

/******************************************************************************/
