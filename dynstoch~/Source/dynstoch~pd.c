/* Common header files ********************************************************/
#include "dynstoch~common.h"

/* Function prototypes ********************************************************/
void *dynstoch_new(t_symbol *s, short argc, t_atom *argv);

/* The 'initialization' routine ***********************************************/
void dynstoch_tilde_setup(void)
{
    /* Initialize the class */
    dynstoch_class = class_new(gensym("dynstoch~"),
                               (t_newmethod)dynstoch_new,
                               (t_method)dynstoch_free,
                               sizeof(t_dynstoch), 0, A_GIMME, 0);

    /* Specify signal input, with automatic float to signal conversion */
    CLASS_MAINSIGNALIN(dynstoch_class, t_dynstoch, x_f);

    /* Bind the DSP method, which is called when the DACs are turned on */
    class_addmethod(dynstoch_class, (t_method)dynstoch_dsp, gensym("dsp"), 0);

    /* Print message to Max window */
    post("dynstoch~ • External was loaded");
}

/* The 'new instance' routine *************************************************/
void *dynstoch_new(t_symbol *s, short argc, t_atom *argv)
{
    /* Instantiate a new object */
    t_dynstoch *x = (t_dynstoch *)pd_new(dynstoch_class);

    return common_new(x, argc, argv);
}

/******************************************************************************/
