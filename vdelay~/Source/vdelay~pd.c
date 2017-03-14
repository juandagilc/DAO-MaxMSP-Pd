/* Common header files ********************************************************/
#include "vdelay~common.h"

/* Function prototypes ********************************************************/
void *vdelay_new(t_symbol *s, short argc, t_atom *argv);

/* The 'initialization' routine ***********************************************/
#ifdef WIN32
__declspec(dllexport) void vdelay_tilde_setup(void);
#endif
void vdelay_tilde_setup(void)
{
	/* Initialize the class */
	vdelay_class = class_new(gensym("vdelay~"),
							 (t_newmethod)vdelay_new,
							 (t_method)vdelay_free,
							 sizeof(t_vdelay), 0, A_GIMME, 0);
	
	/* Specify signal input, with automatic float to signal conversion */
	CLASS_MAINSIGNALIN(vdelay_class, t_vdelay, x_f);
	
	/* Bind the DSP method, which is called when the DACs are turned on */
	class_addmethod(vdelay_class, (t_method)vdelay_dsp, gensym("dsp"), 0);
	
	/* Print message to Max window */
	post("vdelay~ • External was loaded");
}

/* The 'new instance' routine *************************************************/
void *vdelay_new(t_symbol *s, short argc, t_atom *argv)
{
	/* Instantiate a new object */
	t_vdelay *x = (t_vdelay *)pd_new(vdelay_class);
	return vdelay_common_new(x, argc, argv);
}

/******************************************************************************/
