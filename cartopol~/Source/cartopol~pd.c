/* Common header files ********************************************************/
#include "cartopol~common.h"

/* Function prototypes ********************************************************/
void *cartopol_new(t_symbol *s, short argc, t_atom *argv);

/* The 'initialization' routine ***********************************************/
#ifdef WIN32
__declspec(dllexport) void cartopol_tilde_setup(void);
#endif
void cartopol_tilde_setup(void)
{
    /* Initialize the class */
    cartopol_class = class_new(gensym("cartopol~"),
                               (t_newmethod)cartopol_new,
                               (t_method)cartopol_free,
                               sizeof(t_cartopol), 0, A_GIMME, 0);

    /* Specify signal input, with automatic float to signal conversion */
    CLASS_MAINSIGNALIN(cartopol_class, t_cartopol, x_f);

    /* Bind the DSP method, which is called when the DACs are turned on */
    class_addmethod(cartopol_class, (t_method)cartopol_dsp, gensym("dsp"), 0);

    /* Bind the object-specific methods */
    // nothing

    /* Print message to Max window */
    post("cartopol~ • External was loaded");
}

/* The 'new instance' routine *************************************************/
void *cartopol_new(t_symbol *s, short argc, t_atom *argv)
{
    /* Instantiate a new object */
    t_cartopol *x = (t_cartopol *)pd_new(cartopol_class);

    return cartopol_common_new(x, argc, argv);
}

/******************************************************************************/
