/* Common header files ********************************************************/
#include "poltocar~common.h"

/* Function prototypes ********************************************************/
void *poltocar_new(t_symbol *s, short argc, t_atom *argv);

/* The 'initialization' routine ***********************************************/
#ifdef WIN32
__declspec(dllexport) void poltocar_tilde_setup(void);
#endif
void poltocar_tilde_setup(void)
{
    /* Initialize the class */
    poltocar_class = class_new(gensym("poltocar~"),
                               (t_newmethod)poltocar_new,
                               (t_method)poltocar_free,
                               sizeof(t_poltocar), 0, A_GIMME, 0);

    /* Specify signal input, with automatic float to signal conversion */
    CLASS_MAINSIGNALIN(poltocar_class, t_poltocar, x_f);

    /* Bind the DSP method, which is called when the DACs are turned on */
    class_addmethod(poltocar_class, (t_method)poltocar_dsp, gensym("dsp"), 0);

    /* Bind the object-specific methods */
    // nothing

    /* Print message to Max window */
    post("poltocar~ • External was loaded");
}

/* The 'new instance' routine *************************************************/
void *poltocar_new(t_symbol *s, short argc, t_atom *argv)
{
    /* Instantiate a new object */
    t_poltocar *x = (t_poltocar *)pd_new(poltocar_class);

    return poltocar_common_new(x, argc, argv);
}

/******************************************************************************/
