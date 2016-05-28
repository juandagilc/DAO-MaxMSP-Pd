/* Common header files ********************************************************/
#include "vdelay~common.h"

/* Function prototypes ********************************************************/
void *vdelay_new(t_symbol *s, short argc, t_atom *argv);
void vdelay_free(t_vdelay *x);

void vdelay_float(t_vdelay *x, double farg);
void vdelay_assist(t_vdelay *x, void *b, long msg, long arg, char *dst);

/* The 'initialization' routine ***********************************************/
int C74_EXPORT main()
{
	/* Initialize the class */
	vdelay_class = class_new("vdelay~",
							 (method)vdelay_new,
							 (method)vdelay_free,
							 sizeof(t_vdelay), 0, A_GIMME, 0);
	
	/* Bind the DSP method, which is called when the DACs are turned on */
	class_addmethod(vdelay_class, (method)vdelay_dsp, "dsp", A_CANT, 0);
	
	/* Bind the float method, which is called when floats are sent to inlets */
	class_addmethod(vdelay_class, (method)vdelay_float, "float", A_FLOAT, 0);
	
	/* Bind the assist method, which is called on mouse-overs to inlets and outlets */
	class_addmethod(vdelay_class, (method)vdelay_assist, "assist", A_CANT, 0);
	
	/* Add standard Max methods to the class */
	class_dspinit(vdelay_class);
	
	/* Register the class with Max */
	class_register(CLASS_BOX, vdelay_class);
	
	/* Print message to Max window */
	object_post(NULL, "vdelay~ • External was loaded");
	
	/* Return with no error */
	return 0;
}

/* The inlets/outlets indexes *************************************************/
enum INLETS {INPUT, DELAY, FEEDBACK, NUM_INLETS};
enum OUTLETS {OUTPUT, NUM_OUTLETS};

/* The 'new instance' routine *************************************************/
void *vdelay_new(t_symbol *s, short argc, t_atom *argv)
{
	/* Instantiate a new object */
	t_vdelay *x = (t_vdelay *)object_alloc(vdelay_class);
	
	/* Create signal inlets */
	dsp_setup((t_pxobject *)x, NUM_INLETS);
	
	/* Create signal outlets */
	outlet_new((t_object *)x, "signal");
	
	/* Avoid sharing memory among audio vectors */
	x->obj.z_misc |= Z_NO_INPLACE;
	
	/* Initialize input arguments */
	float max_delay_time = DEFAULT_MAX_DELAY_TIME;
	float delay_time = DEFAULT_DELAY_TIME;
	float feedback = DEFAULT_FEEDBACK;
	
	/* Parse arguments passed from object */
	atom_arg_getfloat(&max_delay_time, 0, argc, argv);
	atom_arg_getfloat(&delay_time, 1, argc, argv);
	atom_arg_getfloat(&feedback, 2, argc, argv);
	
	/* Check validity of passed arguments */
	if (max_delay_time < MINIMUM_MAX_DELAY_TIME) {
		max_delay_time = MINIMUM_MAX_DELAY_TIME;
		object_warn((t_object *)x, "Invalid argument: Maximum delay time set to %.4f[ms]", max_delay_time);
	}
	else if (max_delay_time > MAXIMUM_MAX_DELAY_TIME) {
		max_delay_time = MAXIMUM_MAX_DELAY_TIME;
		object_warn((t_object *)x, "Invalid argument: Maximum delay time set to %.4f[ms]", max_delay_time);
	}
	
	if (delay_time < MINIMUM_DELAY_TIME) {
		delay_time = MINIMUM_DELAY_TIME;
		object_warn((t_object *)x, "Invalid argument: Delay time set to %.4f[ms]", delay_time);
	}
	else if (delay_time > MAXIMUM_DELAY_TIME) {
		delay_time = MAXIMUM_DELAY_TIME;
		object_warn((t_object *)x, "Invalid argument: Delay time set to %.4f[ms]", delay_time);
	}
	
	if (feedback < MINIMUM_FEEDBACK) {
		feedback = MINIMUM_FEEDBACK;
		object_warn((t_object *)x, "Invalid argument: Feedback factor set to %.4f", feedback);
	}
	else if (feedback > MAXIMUM_FEEDBACK) {
		feedback = MAXIMUM_FEEDBACK;
		object_warn((t_object *)x, "Invalid argument: Feedback factor set to %.4f", feedback);
	}
	
	/* Initialize state variables */
	x->max_delay_time = max_delay_time;
	x->delay_time = delay_time;
	x->feedback = feedback;
	
	x->fs = sys_getsr();
	
	x->delay_length = (x->max_delay_time*1e-3 * x->fs) + 1;
	x->delay_bytes = x->delay_length * sizeof(float);
	x->delay_line = (float *)sysmem_newptr(x->delay_bytes);
	
	if (x->delay_line == NULL) {
		object_error((t_object *)x, "Cannot allocate memory for this object");
		return NULL;
	}
	
	for (int ii = 0; ii < x->delay_length; ii++) {
		x->delay_line[ii] = 0.0;
	}
	
	x->write_idx = 0;
	x->read_idx = 0;
	
	/* Print message to Max window */
	object_post((t_object *)x, "Object was created");
	
	/* Return a pointer to the new object */
	return x;
}

/* The 'free instance' routine ************************************************/
void vdelay_free(t_vdelay *x)
{
	/* Remove the object from the DSP chain */
	dsp_free((t_pxobject *)x);
	
	/* Free allocated dynamic memory */
	sysmem_freeptr(x->delay_line);
	
	/* Print message to Max window */
	object_post((t_object *)x, "Memory was freed");
}

/* The 'float' method *********************************************************/
void vdelay_float(t_vdelay *x, double farg)
{
	long inlet = ((t_pxobject *)x)->z_in;
	
	switch (inlet) {
		case DELAY:
			if (farg < MINIMUM_DELAY_TIME) {
				farg = MINIMUM_DELAY_TIME;
				object_warn((t_object *)x, "Invalid argument: Delay time set to %.4f[ms]", farg);
			}
			else if (farg > MAXIMUM_DELAY_TIME) {
				farg = MAXIMUM_DELAY_TIME;
				object_warn((t_object *)x, "Invalid argument: Delay time set to %.4f[ms]", farg);
			}
			x->delay_time = farg;
			break;
			
		case FEEDBACK:
			if (farg < MINIMUM_FEEDBACK) {
				farg = MINIMUM_FEEDBACK;
				object_warn((t_object *)x, "Invalid argument: Feedback factor set to %.4f", farg);
			}
			else if (farg > MAXIMUM_FEEDBACK) {
				farg = MAXIMUM_FEEDBACK;
				object_warn((t_object *)x, "Invalid argument: Feedback factor set to %.4f", farg);
			}
			x->feedback = farg;
			break;
	}
	
	/* Print message to Max window */
	object_post((t_object *)x, "Receiving floats");
}

/* The 'assist' method ********************************************************/
void vdelay_assist(t_vdelay *x, void *b, long msg, long arg, char *dst)
{
	/* Document inlet functions */
	if (msg == ASSIST_INLET) {
		switch (arg) {
			case INPUT: sprintf(dst, "(signal) Input"); break;
			case DELAY: sprintf(dst, "(signal/float) Delay"); break;
			case FEEDBACK: sprintf(dst, "(signal/float) Feedback"); break;
		}
	}
	
	/* Document outlet functions */
	else if (msg == ASSIST_OUTLET) {
		switch (arg) {
			case OUTPUT: sprintf(dst, "(signal) Output"); break;
		}
	}
}

/******************************************************************************/
