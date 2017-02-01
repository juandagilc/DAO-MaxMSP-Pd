/* Common header files ********************************************************/
#include "mirror~common.h"

/* Function prototypes ********************************************************/
void *mirror_new(void);
void mirror_free(t_mirror *x);

/* The 'initialization' routine ***********************************************/
#ifdef WIN32
__declspec(dllexport) void mirror_tilde_setup(void);
#endif
void mirror_tilde_setup(void)
{
	/* Initialize the class */
	mirror_class = class_new(gensym("mirror~"),
							 (t_newmethod)mirror_new,
							 (t_method)mirror_free,
							 sizeof(t_mirror), 0, 0);
	
	/* Specify signal input, with automatic float to signal conversion */
	CLASS_MAINSIGNALIN(mirror_class, t_mirror, x_f);
	
	/* Bind the DSP method, which is called when the DACs are turned on */
	class_addmethod(mirror_class, (t_method)mirror_dsp, gensym("dsp"), 0);
	
	/* Print message to Max window */
	post("mirror~ • External was loaded");
}

/* The 'new instance' routine *************************************************/
void *mirror_new(void)
{
	/* Instantiate a new object */
	t_mirror *x = (t_mirror *) pd_new(mirror_class);
	
	/* Create signal inlets */
	// Pd creates one by default
	
	/* Create signal outlets */
	outlet_new(&x->obj, gensym("signal"));
	
	/* Print message to Max window */
	post("mirror~ • Object was created");
	
	/* Return a pointer to the new object */
	return x;
}

/* The 'free instance' routine ************************************************/
void mirror_free(t_mirror *x)
{
	// Nothing to free
	
	/* Print message to Max window */
	post("mirror~ • Memory was freed");
}

/******************************************************************************/
