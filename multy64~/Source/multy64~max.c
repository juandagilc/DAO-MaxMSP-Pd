/* Common header files ********************************************************/
#include "multy64~common.h"

/* Function prototypes ********************************************************/
void *multy64_new(t_symbol *s, short argc, t_atom *argv);

void multy64_float(t_multy64 *x, double farg);
void multy64_assist(t_multy64 *x, void *b, long msg, long arg, char *dst);

/* The 'initialization' routine ***********************************************/
int C74_EXPORT main()
{
    /* Initialize the class */
    multy64_class = class_new("multy64~",
                              (method)multy64_new,
                              (method)multy64_free,
                              sizeof(t_multy64), 0, A_GIMME, 0);

    /* Bind the DSP method, which is called when the DACs are turned on */
    class_addmethod(multy64_class, (method)multy64_dsp, "dsp", A_CANT, 0);
    class_addmethod(multy64_class, (method)multy64_dsp64, "dsp64", A_CANT, 0);

    /* Bind the float method, which is called when floats are sent to inlets */
    class_addmethod(multy64_class, (method)multy64_float, "float", A_FLOAT, 0);

    /* Bind the assist method, which is called on mouse-overs to inlets and outlets */
    class_addmethod(multy64_class, (method)multy64_assist, "assist", A_CANT, 0);

    /* Add standard Max methods to the class */
    class_dspinit(multy64_class);

    /* Register the class with Max */
    class_register(CLASS_BOX, multy64_class);

    /* Print message to Max window */
    object_post(NULL, "multy64~ • External was loaded");

    /* Return with no error */
    return 0;
}

/* The 'new instance' routine *************************************************/
void *multy64_new(t_symbol *s, short argc, t_atom *argv)
{
    /* Instantiate a new object */
    t_multy64 *x = (t_multy64 *)object_alloc(multy64_class);

    return multy64_common_new(x, argc, argv);
}

/******************************************************************************/






/* The 'float' method *********************************************************/
void multy64_float(t_multy64 *x, double farg)
{
    // nothing
}

/* The 'assist' method ********************************************************/
void multy64_assist(t_multy64 *x, void *b, long msg, long arg, char *dst)
{
    /* Document inlet functions */
    if (msg == ASSIST_INLET) {
        switch (arg) {
            case I_INPUT1: sprintf(dst, "(signal) Input 1"); break;
            case I_INPUT2: sprintf(dst, "(signal) Input 2"); break;
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
