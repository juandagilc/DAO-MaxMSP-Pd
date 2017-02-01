/* Common header files ********************************************************/
#include "multy~common.h"

/* Function prototypes ********************************************************/
void *multy_new(void);
void multy_free(t_multy *x);

/* The 'initialization' routine ***********************************************/
#ifdef WIN32
__declspec(dllexport) void multy_tilde_setup(void);
#endif
void multy_tilde_setup(void)
{
	/* Initialize the class */
	multy_class = class_new(gensym("multy~"),
							 (t_newmethod)multy_new,
							 (t_method)multy_free,
							 sizeof(t_multy), 0, 0);
	
	/* Specify signal input, with automatic float to signal conversion */
	CLASS_MAINSIGNALIN(multy_class, t_multy, x_f);
	
	/* Bind the DSP method, which is called when the DACs are turned on */
	class_addmethod(multy_class, (t_method)multy_dsp, gensym("dsp"), 0);
	
	/* Print message to Max window */
	post("multy~ • External was loaded");
}

/* The 'new instance' routine *************************************************/
void *multy_new(void)
{
	/* Instantiate a new object */
	t_multy *x = (t_multy *) pd_new(multy_class);
	
	/* Create signal inlets */
	inlet_new(&x->obj, &x->obj.ob_pd, gensym("signal"), gensym("signal"));
	
	/* Create signal outlets */
	outlet_new(&x->obj, gensym("signal"));
	
	/* Print message to Max window */
	post("multy~ • Object was created");
	
	/* Return a pointer to the new object */
	return x;
}

/* The 'free instance' routine ************************************************/
void multy_free(t_multy *x)
{
	// Nothing to free
	
	/* Print message to Max window */
	post("multy~ • Memory was freed");
}

/******************************************************************************/
