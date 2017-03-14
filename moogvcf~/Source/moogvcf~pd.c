/* Common header files ********************************************************/
#include "moogvcf~common.h"

/* Function prototypes ********************************************************/
void *moogvcf_new(t_symbol *s, short argc, t_atom *argv);

/* The 'initialization' routine ***********************************************/
#ifdef WIN32
__declspec(dllexport) void moogvcf_tilde_setup(void);
#endif
void moogvcf_tilde_setup(void)
{
    /* Initialize the class */
    moogvcf_class = class_new(gensym("moogvcf~"),
                              (t_newmethod)moogvcf_new,
                              (t_method)moogvcf_free,
                              sizeof(t_moogvcf), 0, A_GIMME, 0);

    /* Specify signal input, with automatic float to signal conversion */
    CLASS_MAINSIGNALIN(moogvcf_class, t_moogvcf, x_f);

    /* Bind the DSP method, which is called when the DACs are turned on */
    class_addmethod(moogvcf_class, (t_method)moogvcf_dsp, gensym("dsp"), 0);

    /* Print message to Max window */
    post("moogvcf~ • External was loaded");
}

/* The 'new instance' routine *************************************************/
void *moogvcf_new(t_symbol *s, short argc, t_atom *argv)
{
    /* Instantiate a new object */
    t_moogvcf *x = (t_moogvcf *)pd_new(moogvcf_class);

    return moogvcf_common_new(x, argc, argv);
}

/******************************************************************************/
