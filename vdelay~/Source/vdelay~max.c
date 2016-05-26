/* Common header files ********************************************************/
#include "vdelay~common.h"

/* Function prototypes ********************************************************/
void *vdelay_new(void);
void vdelay_free(t_vdelay *x);
void vdelay_assist(t_vdelay *x, void *b, long msg, long arg, char *dst);

/* The 'initialization' routine ***********************************************/
int C74_EXPORT main()
{
	/* Initialize the class */
	vdelay_class = class_new("vdelay~",
							 (method)vdelay_new,
							 (method)vdelay_free,
							 sizeof(t_vdelay), 0, 0);
	
	/* Bind the DSP method, which is called when the DACs are turned on */
	class_addmethod(vdelay_class, (method)vdelay_dsp, "dsp", A_CANT, 0);
	
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
enum INLETS {INPUT1, INPUT2, NUM_INLETS};
enum OUTLETS {OUTPUT, NUM_OUTLETS};

/* The 'new instance' routine *************************************************/
void *vdelay_new(void)
{
	/* Instantiate a new object */
	t_vdelay *x = (t_vdelay *)object_alloc(vdelay_class);
	
	/* Create signal inlets */
	dsp_setup((t_pxobject *)x, NUM_INLETS);
	
	/* Create signal outlets */
	outlet_new((t_object *)x, "signal");
	
	/* Print message to Max window */
	object_post((t_object *)x, "Object was created");
	
	/* Return a pointer to the new object */
	return x;
}

/* The 'free instance' routine ************************************************/
void vdelay_free(t_vdelay *x)
{
	dsp_free((t_pxobject *)x);
	
	/* Print message to Max window */
	object_post((t_object *)x, "Memory was freed");
}

/* The 'assist' method ********************************************************/
void vdelay_assist(t_vdelay *x, void *b, long msg, long arg, char *dst)
{
	/* Document inlet functions */
	if (msg == ASSIST_INLET) {
		switch (arg) {
			case INPUT1: sprintf(dst, "(signal) Input"); break;
			case INPUT2: sprintf(dst, "(signal) Input"); break;
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
