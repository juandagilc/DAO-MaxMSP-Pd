/* Common header files ********************************************************/
#include "scrubber~common.h"

/* Function prototypes ********************************************************/
void *scrubber_new(t_symbol *s, short argc, t_atom *argv);

/* The 'initialization' routine ***********************************************/
#ifdef WIN32
__declspec(dllexport) void scrubber_tilde_setup(void);
#endif
void scrubber_tilde_setup(void)
{
    /* Initialize the class */
    scrubber_class = class_new(gensym("scrubber~"),
                               (t_newmethod)scrubber_new,
                               (t_method)scrubber_free,
                               sizeof(t_scrubber), 0, A_GIMME, 0);

    /* Specify signal input, with automatic float to signal conversion */
    CLASS_MAINSIGNALIN(scrubber_class, t_scrubber, x_f);

    /* Bind the DSP method, which is called when the DACs are turned on */
    class_addmethod(scrubber_class, (t_method)scrubber_dsp, gensym("dsp"), 0);

    /* Bind the object-specific methods */
    class_addmethod(scrubber_class, (t_method)scrubber_sample, gensym("sample"), 0);
    class_addmethod(scrubber_class, (t_method)scrubber_overlap, gensym("overlap"), A_GIMME, 0);
    class_addmethod(scrubber_class, (t_method)scrubber_resize, gensym("resize"), A_GIMME, 0);

    /* Print message to Max window */
    post("scrubber~ • External was loaded");
}

/* The 'new instance' routine *************************************************/
void *scrubber_new(t_symbol *s, short argc, t_atom *argv)
{
    /* Instantiate a new object */
    t_scrubber *x = (t_scrubber *)pd_new(scrubber_class);

    return scrubber_common_new(x, argc, argv);
}

/******************************************************************************/
