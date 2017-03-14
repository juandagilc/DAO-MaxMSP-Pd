/* Common header files ********************************************************/
#include "windowvec~common.h"

/* Function prototypes ********************************************************/
void *windowvec_new(t_symbol *s, short argc, t_atom *argv);

void windowvec_float(t_windowvec *x, double farg);
void windowvec_assist(t_windowvec *x, void *b, long msg, long arg, char *dst);

/* The 'initialization' routine ***********************************************/
int C74_EXPORT main()
{
    /* Initialize the class */
    windowvec_class = class_new("windowvec~",
                                (method)windowvec_new,
                                (method)windowvec_free,
                                sizeof(t_windowvec), 0, A_GIMME, 0);

    /* Bind the DSP method, which is called when the DACs are turned on */
    class_addmethod(windowvec_class, (method)windowvec_dsp, "dsp", A_CANT, 0);

    /* Bind the float method, which is called when floats are sent to inlets */
    class_addmethod(windowvec_class, (method)windowvec_float, "float", A_FLOAT, 0);

    /* Bind the assist method, which is called on mouse-overs to inlets and outlets */
    class_addmethod(windowvec_class, (method)windowvec_assist, "assist", A_CANT, 0);

    /* Add standard Max methods to the class */
    class_dspinit(windowvec_class);

    /* Register the class with Max */
    class_register(CLASS_BOX, windowvec_class);

    /* Print message to Max window */
    object_post(NULL, "windowvec~ • External was loaded");

    /* Return with no error */
    return 0;
}

/* The 'new instance' routine *************************************************/
void *windowvec_new(t_symbol *s, short argc, t_atom *argv)
{
    /* Instantiate a new object */
    t_windowvec *x = (t_windowvec *)object_alloc(windowvec_class);

    return windowvec_common_new(x, argc, argv);
}

/******************************************************************************/






/* The 'float' method *********************************************************/
void windowvec_float(t_windowvec *x, double farg)
{
    // nothing
}

/* The 'assist' method ********************************************************/
void windowvec_assist(t_windowvec *x, void *b, long msg, long arg, char *dst)
{
    /* Document inlet functions */
    if (msg == ASSIST_INLET) {
        switch (arg) {
            case I_INPUT: sprintf(dst, "(signal) Input"); break;
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
