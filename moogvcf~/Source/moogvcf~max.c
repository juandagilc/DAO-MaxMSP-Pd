/* Common header files ********************************************************/
#include "moogvcf~common.h"

/* Function prototypes ********************************************************/
void *moogvcf_new(t_symbol *s, short argc, t_atom *argv);

void moogvcf_float(t_moogvcf *x, double farg);
void moogvcf_assist(t_moogvcf *x, void *b, long msg, long arg, char *dst);

/* The 'initialization' routine ***********************************************/
int C74_EXPORT main()
{
    /* Initialize the class */
    moogvcf_class = class_new("moogvcf~",
                              (method)moogvcf_new,
                              (method)moogvcf_free,
                              sizeof(t_moogvcf), 0, A_GIMME, 0);

    /* Bind the DSP method, which is called when the DACs are turned on */
    class_addmethod(moogvcf_class, (method)moogvcf_dsp, "dsp", A_CANT, 0);

    /* Bind the float method, which is called when floats are sent to inlets */
    class_addmethod(moogvcf_class, (method)moogvcf_float, "float", A_FLOAT, 0);

    /* Bind the assist method, which is called on mouse-overs to inlets and outlets */
    class_addmethod(moogvcf_class, (method)moogvcf_assist, "assist", A_CANT, 0);

    /* Add standard Max methods to the class */
    class_dspinit(moogvcf_class);

    /* Register the class with Max */
    class_register(CLASS_BOX, moogvcf_class);

    /* Print message to Max window */
    object_post(NULL, "moogvcf~ • External was loaded");

    /* Return with no error */
    return 0;
}

/* The 'new instance' routine *************************************************/
void *moogvcf_new(t_symbol *s, short argc, t_atom *argv)
{
    /* Instantiate a new object */
    t_moogvcf *x = (t_moogvcf *)object_alloc(moogvcf_class);

    return moogvcf_common_new(x, argc, argv);
}

/******************************************************************************/






/* The 'float' method *********************************************************/
void moogvcf_float(t_moogvcf *x, double farg)
{
    // nothing
}

/* The 'assist' method ********************************************************/
void moogvcf_assist(t_moogvcf *x, void *b, long msg, long arg, char *dst)
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
