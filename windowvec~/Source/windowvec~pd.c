/* Common header files ********************************************************/
#include "windowvec~common.h"

/* Function prototypes ********************************************************/
void *windowvec_new(t_symbol *s, short argc, t_atom *argv);

/* The 'initialization' routine ***********************************************/
#ifdef WIN32
__declspec(dllexport) void windowvec_tilde_setup(void);
#endif
void windowvec_tilde_setup(void)
{
    /* Initialize the class */
    windowvec_class = class_new(gensym("windowvec~"),
                                (t_newmethod)windowvec_new,
                                (t_method)windowvec_free,
                                sizeof(t_windowvec), 0, A_GIMME, 0);

    /* Specify signal input, with automatic float to signal conversion */
    CLASS_MAINSIGNALIN(windowvec_class, t_windowvec, x_f);

    /* Bind the DSP method, which is called when the DACs are turned on */
    class_addmethod(windowvec_class, (t_method)windowvec_dsp, gensym("dsp"), 0);

    /* Bind the object-specific methods */
    // nothing

    /* Print message to Max window */
    post("windowvec~ • External was loaded");
}

/* The 'new instance' routine *************************************************/
void *windowvec_new(t_symbol *s, short argc, t_atom *argv)
{
    /* Instantiate a new object */
    t_windowvec *x = (t_windowvec *)pd_new(windowvec_class);

    return windowvec_common_new(x, argc, argv);
}

/******************************************************************************/
