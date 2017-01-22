/* Common header files ********************************************************/
#include "dynstoch~common.h"

/* Function prototypes ********************************************************/
void *dynstoch_new(t_symbol *s, short argc, t_atom *argv);

void dynstoch_float(t_dynstoch *x, double farg);
void dynstoch_assist(t_dynstoch *x, void *b, long msg, long arg, char *dst);

/* The 'initialization' routine ***********************************************/
int C74_EXPORT main()
{
    /* Initialize the class */
    dynstoch_class = class_new("dynstoch~",
                               (method)dynstoch_new,
                               (method)dynstoch_free,
                               sizeof(t_dynstoch), 0, A_GIMME, 0);

    /* Bind the DSP method, which is called when the DACs are turned on */
    class_addmethod(dynstoch_class, (method)dynstoch_dsp, "dsp", A_CANT, 0);

    /* Bind the float method, which is called when floats are sent to inlets */
    class_addmethod(dynstoch_class, (method)dynstoch_float, "float", A_FLOAT, 0);

    /* Bind the assist method, which is called on mouse-overs to inlets and outlets */
    class_addmethod(dynstoch_class, (method)dynstoch_assist, "assist", A_CANT, 0);

    /* Add standard Max methods to the class */
    class_dspinit(dynstoch_class);

    /* Register the class with Max */
    class_register(CLASS_BOX, dynstoch_class);

    /* Print message to Max window */
    object_post(NULL, "dynstoch~ • External was loaded");

    /* Return with no error */
    return 0;
}

/* The 'new instance' routine *************************************************/
void *dynstoch_new(t_symbol *s, short argc, t_atom *argv)
{
    /* Instantiate a new object */
    t_dynstoch *x = (t_dynstoch *)object_alloc(dynstoch_class);

    return common_new(x, argc, argv);
}

/******************************************************************************/






/* The 'float' method *********************************************************/
void dynstoch_float(t_dynstoch *x, double farg)
{
    // nothing
}

/* The 'assist' method ********************************************************/
void dynstoch_assist(t_dynstoch *x, void *b, long msg, long arg, char *dst)
{
    /* Document inlet functions */
    if (msg == ASSIST_INLET) {
        switch (arg) {
            case I_INPUT: sprintf(dst, "(signal) Input"); break;
            case I_FREQUENCY: sprintf(dst, "(signal) Cutoff frequency"); break;
            case I_RESONANCE: sprintf(dst, "(signal) Resonance"); break;
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
