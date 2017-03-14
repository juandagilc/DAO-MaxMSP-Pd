/* Common header files ********************************************************/
#include "cleaner~common.h"

/* Function prototypes ********************************************************/
void *cleaner_new(t_symbol *s, short argc, t_atom *argv);

void cleaner_float(t_cleaner *x, double farg);
void cleaner_assist(t_cleaner *x, void *b, long msg, long arg, char *dst);

/* The 'initialization' routine ***********************************************/
int C74_EXPORT main()
{
    /* Initialize the class */
    cleaner_class = class_new("cleaner~",
                              (method)cleaner_new,
                              (method)cleaner_free,
                              sizeof(t_cleaner), 0, A_GIMME, 0);

    /* Bind the DSP method, which is called when the DACs are turned on */
    class_addmethod(cleaner_class, (method)cleaner_dsp, "dsp", A_CANT, 0);

    /* Bind the float method, which is called when floats are sent to inlets */
    class_addmethod(cleaner_class, (method)cleaner_float, "float", A_FLOAT, 0);

    /* Bind the assist method, which is called on mouse-overs to inlets and outlets */
    class_addmethod(cleaner_class, (method)cleaner_assist, "assist", A_CANT, 0);

    /* Add standard Max methods to the class */
    class_dspinit(cleaner_class);

    /* Register the class with Max */
    class_register(CLASS_BOX, cleaner_class);

    /* Print message to Max window */
    object_post(NULL, "cleaner~ • External was loaded");

    /* Return with no error */
    return 0;
}

/* The 'new instance' routine *************************************************/
void *cleaner_new(t_symbol *s, short argc, t_atom *argv)
{
    /* Instantiate a new object */
    t_cleaner *x = (t_cleaner *)object_alloc(cleaner_class);

    return cleaner_common_new(x, argc, argv);
}

/******************************************************************************/






/* The 'float' method *********************************************************/
void cleaner_float(t_cleaner *x, double farg)
{
    long inlet = ((t_pxobject *)x)->z_in;

    switch (inlet) {
        case I_THRESHOLD:
            if (farg < MINIMUM_THRESHOLD) {
                farg = MINIMUM_THRESHOLD;
                object_warn((t_object *)x, "cleaner~ • Invalid argument: Minimum threshold value set to %.4f", farg);
            }
            else if (farg > MAXIMUM_THRESHOLD) {
                farg = MAXIMUM_THRESHOLD;
                object_warn((t_object *)x, "cleaner~ • Invalid argument: Maximum threshold value set to %.4f", farg);
            }
            x->threshold_value = farg;
            break;

        case I_ATTENUATION:
            if (farg < MINIMUM_ATTENUATION) {
                farg = MINIMUM_ATTENUATION;
                object_warn((t_object *)x, "cleaner~ • Invalid argument: Minimum attenuation value set to %.4f", farg);
            }
            else if (farg > MAXIMUM_ATTENUATION) {
                farg = MAXIMUM_ATTENUATION;
                object_warn((t_object *)x, "cleaner~ • Invalid argument: Maximum attenuation value set to %.4f", farg);
            }
            x->attenuation_value = farg;
            break;
    }
}

/* The 'assist' method ********************************************************/
void cleaner_assist(t_cleaner *x, void *b, long msg, long arg, char *dst)
{
    /* Document inlet functions */
    if (msg == ASSIST_INLET) {
        switch (arg) {
            case I_INPUT: sprintf(dst, "(signal) Input"); break;
            case I_THRESHOLD: sprintf(dst, "(signal/float) Threshold"); break;
            case I_ATTENUATION: sprintf(dst, "(signal/float) Attenuation"); break;
        }
    }

    /* Document outlet functions */
    else if (msg == ASSIST_OUTLET) {
        switch (arg) {
            case O_OUTPUT: sprintf(dst, "(signal) Output"); break;
        }
    }
}

/******************************************************************************/
