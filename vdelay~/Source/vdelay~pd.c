/* Common header files ********************************************************/
#include "vdelay~common.h"

/* Function prototypes ********************************************************/
void *vdelay_new(void);
void vdelay_free(t_vdelay *x);

/* The 'initialization' routine ***********************************************/
void vdelay_tilde_setup(void)
{
	/* Initialize the class */
	vdelay_class = class_new(gensym("vdelay~"),
							 (t_newmethod)vdelay_new,
							 (t_method)vdelay_free,
							 sizeof(t_vdelay), 0, 0);
	
	/* Specify signal input, with automatic float to signal conversion */
	CLASS_MAINSIGNALIN(vdelay_class, t_vdelay, x_f);
	
	/* Bind the DSP method, which is called when the DACs are turned on */
	class_addmethod(vdelay_class, (t_method)vdelay_dsp, gensym("dsp"), 0);
	
	/* Print message to Max window */
	post("vdelay~ • External was loaded");
}

/* The 'new instance' routine *************************************************/
void *vdelay_new(void)
{
	/* Instantiate a new object */
	t_vdelay *x = (t_vdelay *) pd_new(vdelay_class);
	
	/* Create signal inlets */
	inlet_new(&x->obj, &x->obj.ob_pd, gensym("signal"), gensym("signal"));
	
	/* Create signal outlets */
	outlet_new(&x->obj, gensym("signal"));
	
	/* Print message to Max window */
	post("vdelay~ • Object was created");
	
	/* Return a pointer to the new object */
	return x;
}

/* The 'free instance' routine ************************************************/
void vdelay_free(t_vdelay *x)
{
	// Nothing to free
	
	/* Print message to Max window */
	post("vdelay~ • Memory was freed");
}

/******************************************************************************/
