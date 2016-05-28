/* Common header files ********************************************************/
#include "vdelay~common.h"

/* Function prototypes ********************************************************/
void *vdelay_new(t_symbol *s, short argc, t_atom *argv);
void vdelay_free(t_vdelay *x);

/* The 'initialization' routine ***********************************************/
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
	
	/* Create signal inlets */
	inlet_new(&x->obj, &x->obj.ob_pd, gensym("signal"), gensym("signal"));
	inlet_new(&x->obj, &x->obj.ob_pd, gensym("signal"), gensym("signal"));
	
	/* Create signal outlets */
	outlet_new(&x->obj, gensym("signal"));
	
	/* Initialize input arguments */
	float max_delay = DEFAULT_MAX_DELAY;
	float delay = DEFAULT_DELAY;
	float feedback = DEFAULT_FEEDBACK;
	
	/* Parse arguments passed from object */
	if (argc > A_FEEDBACK) { feedback = atom_getfloatarg(A_FEEDBACK, argc, argv); }
	if (argc > A_DELAY) { delay = atom_getfloatarg(A_DELAY, argc, argv); }
	if (argc > A_MAX_DELAY) { max_delay = atom_getfloatarg(A_MAX_DELAY, argc, argv); }
	
	/* Check validity of passed arguments */
	if (max_delay < MINIMUM_MAX_DELAY) {
		max_delay = MINIMUM_MAX_DELAY;
		post("vdelay~ • Invalid argument: Maximum delay time set to %.4f[ms]", max_delay);
	}
	else if (max_delay > MAXIMUM_MAX_DELAY) {
		max_delay = MAXIMUM_MAX_DELAY;
		post("vdelay~ • Invalid argument: Maximum delay time set to %.4f[ms]", max_delay);
	}
	
	if (delay < MINIMUM_DELAY) {
		delay = MINIMUM_DELAY;
		post("vdelay~ • Invalid argument: Delay time set to %.4f[ms]", delay);
	}
	else if (delay > MAXIMUM_DELAY) {
		delay = MAXIMUM_DELAY;
		post("vdelay~ • Invalid argument: Delay time set to %.4f[ms]", delay);
	}
	
	if (feedback < MINIMUM_FEEDBACK) {
		feedback = MINIMUM_FEEDBACK;
		post("vdelay~ • Invalid argument: Feedback factor set to %.4f", feedback);
	}
	else if (feedback > MAXIMUM_FEEDBACK) {
		feedback = MAXIMUM_FEEDBACK;
		post("vdelay~ • Invalid argument: Feedback factor set to %.4f", feedback);
	}
	
	/* Initialize state variables */
	x->max_delay = max_delay;
	x->delay = delay;
	x->feedback = feedback;
	
	x->fs = sys_getsr();
	
	x->delay_length = (x->max_delay * 1e-3 * x->fs) + 1;
	x->delay_bytes = x->delay_length * sizeof(float);
	x->delay_line = (float *)getbytes(x->delay_bytes);
	
	if (x->delay_line == NULL) {
		post("vdelay~ • Cannot allocate memory for this object");
		return NULL;
	}
	
	for (int ii = 0; ii < x->delay_length; ii++) {
		x->delay_line[ii] = 0.0;
	}
	
	x->write_idx = 0;
	x->read_idx = 0;
	
	/* Print message to Max window */
	post("vdelay~ • Object was created");
	
	/* Return a pointer to the new object */
	return x;
}

/* The 'free instance' routine ************************************************/
void vdelay_free(t_vdelay *x)
{
	/* Free allocated dynamic memory */
	freebytes(x->delay_line, x->delay_bytes);
	
	/* Print message to Max window */
	post("vdelay~ • Memory was freed");
}

/******************************************************************************/
