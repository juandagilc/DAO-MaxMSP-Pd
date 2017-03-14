/* Common header files ********************************************************/
#include "vpdelay~common.h"

/* Function prototypes ********************************************************/
void *vpdelay_new(t_symbol *s, short argc, t_atom *argv);

/* The 'initialization' routine ***********************************************/
#ifdef WIN32
__declspec(dllexport) void vpdelay_tilde_setup(void);
#endif
void vpdelay_tilde_setup(void)
{
	/* Initialize the class */
	vpdelay_class = class_new(gensym("vpdelay~"),
							  (t_newmethod)vpdelay_new,
							  (t_method)vpdelay_free,
							  sizeof(t_vpdelay), 0, A_GIMME, 0);
	
	/* Specify signal input, with automatic float to signal conversion */
	CLASS_MAINSIGNALIN(vpdelay_class, t_vpdelay, x_f);
	
	/* Bind the DSP method, which is called when the DACs are turned on */
	class_addmethod(vpdelay_class, (t_method)vpdelay_dsp, gensym("dsp"), 0);
	
	/* Print message to Max window */
	post("vpdelay~ • External was loaded");
}

/* The 'new instance' routine *************************************************/
void *vpdelay_new(t_symbol *s, short argc, t_atom *argv)
{
	/* Instantiate a new object */
	t_vpdelay *x = (t_vpdelay *)pd_new(vpdelay_class);
	return vpdelay_common_new(x, argc, argv);
}

/******************************************************************************/
