/* Common header files ********************************************************/
#include "retroseq~common.h"

/* Function prototypes ********************************************************/
void *retroseq_new(t_symbol *s, short argc, t_atom *argv);

void retroseq_float(t_retroseq *x, double farg);
void retroseq_assist(t_retroseq *x, void *b, long msg, long arg, char *dst);

/* The 'initialization' routine ***********************************************/
int C74_EXPORT main()
{
	/* Initialize the class */
	retroseq_class = class_new("retroseq~",
							(method)retroseq_new,
							(method)retroseq_free,
							sizeof(t_retroseq), 0, A_GIMME, 0);
	
	/* Bind the DSP method, which is called when the DACs are turned on */
	class_addmethod(retroseq_class, (method)retroseq_dsp, "dsp", A_CANT, 0);
	
	/* Bind the float method, which is called when floats are sent to inlets */
	class_addmethod(retroseq_class, (method)retroseq_float, "float", A_FLOAT, 0);
	
	/* Bind the assist method, which is called on mouse-overs to inlets and outlets */
	class_addmethod(retroseq_class, (method)retroseq_assist, "assist", A_CANT, 0);
    
    /* Bind the object-specific methods */
    class_addmethod(retroseq_class, (method)retroseq_list, "list", A_GIMME, 0);
    class_addmethod(retroseq_class, (method)retroseq_freqlist, "freqlist", A_GIMME, 0);
    class_addmethod(retroseq_class, (method)retroseq_durlist, "durlist", A_GIMME, 0);
    class_addmethod(retroseq_class, (method)retroseq_tempo, "tempo", A_GIMME, 0);
	
	/* Add standard Max methods to the class */
	class_dspinit(retroseq_class);
	
	/* Register the class with Max */
	class_register(CLASS_BOX, retroseq_class);
	
	/* Print message to Max window */
	object_post(NULL, "retroseq~ • External was loaded");
	
	/* Return with no error */
	return 0;
}

/* The 'new instance' routine *************************************************/
void *retroseq_new(t_symbol *s, short argc, t_atom *argv)
{
	/* Instantiate a new object */
	t_retroseq *x = (t_retroseq *)object_alloc(retroseq_class);
    
	return common_new(x, argc, argv);
}

/******************************************************************************/






/* The 'float' method *********************************************************/
void retroseq_float(t_retroseq *x, double farg)
{
    //nothing
    
    /* Print message to Max window */
    object_post((t_object *)x, "Receiving floats");
}

/* The 'assist' method ********************************************************/
void retroseq_assist(t_retroseq *x, void *b, long msg, long arg, char *dst)
{
    /* Document inlet functions */
    if (msg == ASSIST_INLET) {
        switch (arg) {
            //nothing
        }
    }
    
    /* Document outlet functions */
    else if (msg == ASSIST_OUTLET) {
        switch (arg) {
            case O_OUTPUT: sprintf(dst, "(signal) Output"); break;
        }
    }
}

/******************************************************************************/
