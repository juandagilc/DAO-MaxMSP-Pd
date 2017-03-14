/* Common header files ********************************************************/
#include "retroseq~common.h"

/* Function prototypes ********************************************************/
void *retroseq_new(t_symbol *s, short argc, t_atom *argv);

/* The 'initialization' routine ***********************************************/
#ifdef WIN32
__declspec(dllexport) void retroseq_tilde_setup(void);
#endif
void retroseq_tilde_setup(void)
{
    /* Initialize the class */
    retroseq_class = class_new(gensym("retroseq~"),
                               (t_newmethod)retroseq_new,
                               (t_method)retroseq_free,
                               sizeof(t_retroseq), 0, A_GIMME, 0);

    /* Specify signal input, with automatic float to signal conversion */
    CLASS_MAINSIGNALIN(retroseq_class, t_retroseq, x_f);

    /* Bind the DSP method, which is called when the DACs are turned on */
    class_addmethod(retroseq_class, (t_method)retroseq_dsp, gensym("dsp"), 0);

    /* Bind the object-specific methods */
    class_addmethod(retroseq_class, (t_method)retroseq_list, gensym("list"), A_GIMME, 0);
    class_addmethod(retroseq_class, (t_method)retroseq_freqlist, gensym("freqlist"), A_GIMME, 0);
    class_addmethod(retroseq_class, (t_method)retroseq_durlist, gensym("durlist"), A_GIMME, 0);

    class_addmethod(retroseq_class, (t_method)retroseq_shuffle_freqs, gensym("shuffle_freqs"), 0);
    class_addmethod(retroseq_class, (t_method)retroseq_shuffle_durs, gensym("shuffle_durs"), 0);
    class_addmethod(retroseq_class, (t_method)retroseq_shuffle, gensym("shuffle"), 0);

    class_addmethod(retroseq_class, (t_method)retroseq_set_tempo, gensym("tempo"), A_GIMME, 0);
    class_addmethod(retroseq_class, (t_method)retroseq_set_elastic_sustain, gensym("elastic_sustain"), A_GIMME, 0);
    class_addmethod(retroseq_class, (t_method)retroseq_set_sustain_amplitude, gensym("sustain_amplitude"), A_GIMME, 0);
    class_addmethod(retroseq_class, (t_method)retroseq_set_adsr, gensym("adsr"), A_GIMME, 0);

    class_addmethod(retroseq_class, (t_method)retroseq_manual_override, gensym("manual_override"), A_GIMME, 0);
    class_addmethod(retroseq_class, (t_method)retroseq_trigger_sent, gensym("bang"), 0);
    class_addmethod(retroseq_class, (t_method)retroseq_play_backwards, gensym("play_backwards"), A_GIMME, 0);

    /* Print message to Max window */
    post("retroseq~ • External was loaded");
}

/* The 'new instance' routine *************************************************/
void *retroseq_new(t_symbol *s, short argc, t_atom *argv)
{
    /* Instantiate a new object */
    t_retroseq *x = (t_retroseq *)pd_new(retroseq_class);

    return retroseq_common_new(x, argc, argv);
}

/******************************************************************************/
