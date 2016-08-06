/* Common header files ********************************************************/
#include "retroseq~common.h"

/* Function prototypes ********************************************************/
void *retroseq_new(t_symbol *s, short argc, t_atom *argv);

/* The 'initialization' routine ***********************************************/
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
    class_addmethod(retroseq_class, (t_method)retroseq_build_sine, gensym("sine"), 0);
    class_addmethod(retroseq_class, (t_method)retroseq_build_triangle, gensym("triangle"), 0);
    class_addmethod(retroseq_class, (t_method)retroseq_build_sawtooth, gensym("sawtooth"), 0);
    class_addmethod(retroseq_class, (t_method)retroseq_build_square, gensym("square"), 0);
    class_addmethod(retroseq_class, (t_method)retroseq_build_pulse, gensym("pulse"), 0);
    class_addmethod(retroseq_class, (t_method)retroseq_build_list, gensym("list"), A_GIMME, 0);
    class_addmethod(retroseq_class, (t_method)retroseq_fadetime, gensym("fadetime"), A_GIMME, 0);
    class_addmethod(retroseq_class, (t_method)retroseq_fadetype, gensym("fadetype"), A_GIMME, 0);
	
	/* Print message to Max window */
	post("retroseq~ • External was loaded");
}

/* The 'new instance' routine *************************************************/
void *retroseq_new(t_symbol *s, short argc, t_atom *argv)
{
	/* Instantiate a new object */
	t_retroseq *x = (t_retroseq *)pd_new(retroseq_class);
    
	return common_new(x, argc, argv);
}

/******************************************************************************/
