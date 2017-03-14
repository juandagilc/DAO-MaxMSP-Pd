/* Common header files ********************************************************/
#include "multy64~common.h"

/* Function prototypes ********************************************************/
void *multy64_new(t_symbol *s, short argc, t_atom *argv);

/* The 'initialization' routine ***********************************************/
#ifdef WIN32
__declspec(dllexport) void multy64_tilde_setup(void);
#endif
void multy64_tilde_setup(void)
{
    /* Initialize the class */
    multy64_class = class_new(gensym("multy64~"),
                              (t_newmethod)multy64_new,
                              (t_method)multy64_free,
                              sizeof(t_multy64), 0, A_GIMME, 0);

    /* Specify signal input, with automatic float to signal conversion */
    CLASS_MAINSIGNALIN(multy64_class, t_multy64, x_f);

    /* Bind the DSP method, which is called when the DACs are turned on */
    class_addmethod(multy64_class, (t_method)multy64_dsp, gensym("dsp"), 0);

    /* Bind the object-specific methods */
    // nothing

    /* Print message to Max window */
    post("multy64~ • External was loaded");
}

/* The 'new instance' routine *************************************************/
void *multy64_new(t_symbol *s, short argc, t_atom *argv)
{
    /* Instantiate a new object */
    t_multy64 *x = (t_multy64 *)pd_new(multy64_class);

    return multy64_common_new(x, argc, argv);
}

/******************************************************************************/
