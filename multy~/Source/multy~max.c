/* Common header files ********************************************************/
#include "multy~common.h"

/* Function prototypes ********************************************************/
void *multy_new(void);
void multy_free(t_multy *x);
void multy_assist(t_multy *x, void *b, long msg, long arg, char *dst);

/* The 'initialization' routine ***********************************************/
int C74_EXPORT main()
{
	/* Initialize the class */
	multy_class = class_new("multy~",
							 (method)multy_new,
							 (method)multy_free,
							 sizeof(t_multy), 0, 0);
	
	/* Bind the DSP method, which is called when the DACs are turned on */
	class_addmethod(multy_class, (method)multy_dsp, "dsp", A_CANT, 0);
	
	/* Bind the assist method, which is called on mouse-overs to inlets and outlets */
	class_addmethod(multy_class, (method)multy_assist, "assist", A_CANT, 0);
	
	/* Add standard Max methods to the class */
	class_dspinit(multy_class);
	
	/* Register the class with Max */
	class_register(CLASS_BOX, multy_class);
	
	/* Print message to Max window */
	object_post(NULL, "multy~ • External was loaded");
	
	/* Return with no error */
	return 0;
}

/* The inlets/outlets indexes *************************************************/
enum INLETS {INPUT1, INPUT2, NUM_INLETS};
enum OUTLETS {OUTPUT, NUM_OUTLETS};

/* The 'new instance' routine *************************************************/
void *multy_new(void)
{
	/* Instantiate a new object */
	t_multy *x = (t_multy *)object_alloc(multy_class);
	
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
void multy_free(t_multy *x)
{
	dsp_free((t_pxobject *)x);
	
	/* Print message to Max window */
	object_post((t_object *)x, "Memory was freed");
}

/* The 'assist' method ********************************************************/
void multy_assist(t_multy *x, void *b, long msg, long arg, char *dst)
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
