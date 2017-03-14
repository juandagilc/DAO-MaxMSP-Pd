/* Common header files ********************************************************/
#include "cleaner~common.h"

/* Function prototypes ********************************************************/
void *cleaner_new(t_symbol *s, short argc, t_atom *argv);

/* The 'initialization' routine ***********************************************/
#ifdef WIN32
__declspec(dllexport) void cleaner_tilde_setup(void);
#endif
void cleaner_tilde_setup(void)
{
    /* Initialize the class */
    cleaner_class = class_new(gensym("cleaner~"),
                              (t_newmethod)cleaner_new,
                              (t_method)cleaner_free,
                              sizeof(t_cleaner), 0, A_GIMME, 0);

    /* Specify signal input, with automatic float to signal conversion */
    CLASS_MAINSIGNALIN(cleaner_class, t_cleaner, x_f);

    /* Bind the DSP method, which is called when the DACs are turned on */
    class_addmethod(cleaner_class, (t_method)cleaner_dsp, gensym("dsp"), 0);

    /* Print message to Max window */
    post("cleaner~ • External was loaded");
}

/* The 'new instance' routine *************************************************/
void *cleaner_new(t_symbol *s, short argc, t_atom *argv)
{
    /* Instantiate a new object */
    t_cleaner *x = (t_cleaner *)pd_new(cleaner_class);

    return cleaner_common_new(x, argc, argv);
}

/******************************************************************************/
