/* Common header files ********************************************************/
#include "mirror~common.h"

/* Function prototypes ********************************************************/
void *mirror_new(void);
void mirror_free(t_mirror *x);
void mirror_assist(t_mirror *x, void *b, long msg, long arg, char *dst);

/* The 'initialization' routine ***********************************************/
int C74_EXPORT main()
{
	/* Initialize the class */
	mirror_class = class_new("mirror~",
							 (method)mirror_new,
							 (method)mirror_free,
							 sizeof(t_mirror), 0, 0);
	
	/* Bind the DSP method, which is called when the DACs are turned on */
	class_addmethod(mirror_class, (method)mirror_dsp, "dsp", A_CANT, 0);
	
	/* Bind the assist method, which is called on mouse-overs to inlets and outlets */
	class_addmethod(mirror_class, (method)mirror_assist, "assist", A_CANT, 0);
	
	/* Add standard Max methods to the class */
	class_dspinit(mirror_class);
	
	/* Register the class with Max */
	class_register(CLASS_BOX, mirror_class);
	
	/* Print message to Max window */
	object_post(NULL, "mirror~ • External was loaded");
	
	/* Return with no error */
	return 0;
}

/* The inlets/outlets indexes *************************************************/
enum INLETS {INPUT1, NUM_INLETS};
enum OUTLETS {OUTPUT1, NUM_OUTLETS};

/* The 'new instance' routine *************************************************/
void *mirror_new(void)
{
	/* Instantiate a new object */
	t_mirror *x = (t_mirror *)object_alloc(mirror_class);
	
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
void mirror_free(t_mirror *x)
{
	dsp_free((t_pxobject *)x);
	
	/* Print message to Max window */
	object_post((t_object *)x, "Memory was freed");
}

/* The 'assist' method ********************************************************/
void mirror_assist(t_mirror *x, void *b, long msg, long arg, char *dst)
{
	/* Document inlet functions */
	if (msg == ASSIST_INLET) {
		switch (arg) {
			case INPUT1: sprintf(dst, "(signal) Input"); break;
		}
	}
	
	/* Document outlet functions */
	else if (msg == ASSIST_OUTLET) {
		switch (arg) {
			case OUTPUT1: sprintf(dst, "(signal) Output"); break;
		}
	}
}

/******************************************************************************/
