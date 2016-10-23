/* Common header files ********************************************************/
#include "bed_common.h"

/* Function prototypes ********************************************************/
void *bed_new(t_symbol *s, short argc, t_atom *argv);

/* The 'initialization' routine ***********************************************/
void bed_setup(void)
{
    /* Initialize the class */
    bed_class = class_new(gensym("bed"),
                          (t_newmethod)bed_new,
                          (t_method)bed_free,
                          sizeof(t_bed), 0, A_GIMME, 0);

    /* Specify signal input, with automatic float to signal conversion */
    CLASS_MAINSIGNALIN(bed_class, t_bed, x_f);

    /* Bind the DSP method, which is called when the DACs are turned on */
    class_addmethod(bed_class, (t_method)bed_dsp, gensym("dsp"), 0);

    /* Bind the object-specific methods */
    class_addmethod(bed_class, (t_method)bed_list, gensym("list"), A_GIMME, 0);
    class_addmethod(bed_class, (t_method)bed_freqlist, gensym("freqlist"), A_GIMME, 0);
    class_addmethod(bed_class, (t_method)bed_durlist, gensym("durlist"), A_GIMME, 0);

    class_addmethod(bed_class, (t_method)bed_shuffle_freqs, gensym("shuffle_freqs"), 0);
    class_addmethod(bed_class, (t_method)bed_shuffle_durs, gensym("shuffle_durs"), 0);
    class_addmethod(bed_class, (t_method)bed_shuffle, gensym("shuffle"), 0);

    class_addmethod(bed_class, (t_method)bed_set_tempo, gensym("tempo"), A_GIMME, 0);
    class_addmethod(bed_class, (t_method)bed_set_elastic_sustain, gensym("elastic_sustain"), A_GIMME, 0);
    class_addmethod(bed_class, (t_method)bed_set_sustain_amplitude, gensym("sustain_amplitude"), A_GIMME, 0);
    class_addmethod(bed_class, (t_method)bed_set_adsr, gensym("adsr"), A_GIMME, 0);

    class_addmethod(bed_class, (t_method)bed_manual_override, gensym("manual_override"), A_GIMME, 0);
    class_addmethod(bed_class, (t_method)bed_trigger_sent, gensym("bang"), 0);
    class_addmethod(bed_class, (t_method)bed_play_backwards, gensym("play_backwards"), A_GIMME, 0);

    /* Print message to Max window */
    post("bed • External was loaded");
}

/* The 'new instance' routine *************************************************/
void *bed_new(t_symbol *s, short argc, t_atom *argv)
{
    /* Instantiate a new object */
    t_bed *x = (t_bed *)pd_new(bed_class);

    return common_new(x, argc, argv);
}

/******************************************************************************/
