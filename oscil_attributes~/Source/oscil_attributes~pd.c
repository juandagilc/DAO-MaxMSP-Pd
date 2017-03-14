/* Common header files ********************************************************/
#include "oscil_attributes~common.h"

/* Function prototypes ********************************************************/
void *oscil_attributes_new(t_symbol *s, short argc, t_atom *argv);

/* The 'initialization' routine ***********************************************/
#ifdef WIN32
__declspec(dllexport) void oscil_attributes_tilde_setup(void);
#endif
void oscil_attributes_tilde_setup(void)
{
	/* Initialize the class */
	oscil_attributes_class = class_new(gensym("oscil_attributes~"),
                                       (t_newmethod)oscil_attributes_new,
                                       (t_method)oscil_attributes_free,
                                       sizeof(t_oscil_attributes), 0, A_GIMME, 0);
	
	/* Specify signal input, with automatic float to signal conversion */
	CLASS_MAINSIGNALIN(oscil_attributes_class, t_oscil_attributes, x_f);
	
	/* Bind the DSP method, which is called when the DACs are turned on */
	class_addmethod(oscil_attributes_class, (t_method)oscil_attributes_dsp, gensym("dsp"), 0);
    
    /* Bind the object-specific methods */
    class_addmethod(oscil_attributes_class, (t_method)oscil_attributes_build_sine, gensym("sine"), 0);
    class_addmethod(oscil_attributes_class, (t_method)oscil_attributes_build_triangle, gensym("triangle"), 0);
    class_addmethod(oscil_attributes_class, (t_method)oscil_attributes_build_sawtooth, gensym("sawtooth"), 0);
    class_addmethod(oscil_attributes_class, (t_method)oscil_attributes_build_square, gensym("square"), 0);
    class_addmethod(oscil_attributes_class, (t_method)oscil_attributes_build_pulse, gensym("pulse"), 0);
    class_addmethod(oscil_attributes_class, (t_method)oscil_attributes_build_list, gensym("list"), A_GIMME, 0);
    class_addmethod(oscil_attributes_class, (t_method)oscil_attributes_fadetime, gensym("fadetime"), A_GIMME, 0);
    class_addmethod(oscil_attributes_class, (t_method)oscil_attributes_fadetype, gensym("fadetype"), A_GIMME, 0);
	
	/* Print message to Max window */
	post("oscil_attributes~ • External was loaded");
}

/* The 'new instance' routine *************************************************/
void *oscil_attributes_new(t_symbol *s, short argc, t_atom *argv)
{
	/* Instantiate a new object */
	t_oscil_attributes *x = (t_oscil_attributes *)pd_new(oscil_attributes_class);
    
	return oscil_attributes_common_new(x, argc, argv);
}

/******************************************************************************/
