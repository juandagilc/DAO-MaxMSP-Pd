/* Common header files ********************************************************/
#include "oscil_attributes~common.h"

/* Function prototypes ********************************************************/
void *oscil_attributes_new(t_symbol *s, short argc, t_atom *argv);

void oscil_attributes_float(t_oscil_attributes *x, double farg);
void oscil_attributes_assist(t_oscil_attributes *x, void *b, long msg, long arg, char *dst);

/* The 'initialization' routine ***********************************************/
int C74_EXPORT main()
{
	/* Initialize the class */
	oscil_attributes_class = class_new("oscil_attributes~",
                                       (method)oscil_attributes_new,
                                       (method)oscil_attributes_free,
                                       sizeof(t_oscil_attributes), 0, A_GIMME, 0);
	
	/* Bind the DSP method, which is called when the DACs are turned on */
	class_addmethod(oscil_attributes_class, (method)oscil_attributes_dsp, "dsp", A_CANT, 0);
	
	/* Bind the float method, which is called when floats are sent to inlets */
	class_addmethod(oscil_attributes_class, (method)oscil_attributes_float, "float", A_FLOAT, 0);
	
	/* Bind the assist method, which is called on mouse-overs to inlets and outlets */
	class_addmethod(oscil_attributes_class, (method)oscil_attributes_assist, "assist", A_CANT, 0);
    
    /* Bind the object-specific methods */
    class_addmethod(oscil_attributes_class, (method)oscil_attributes_build_sine, "sine", 0);
    class_addmethod(oscil_attributes_class, (method)oscil_attributes_build_triangle, "triangle", 0);
    class_addmethod(oscil_attributes_class, (method)oscil_attributes_build_sawtooth, "sawtooth", 0);
    class_addmethod(oscil_attributes_class, (method)oscil_attributes_build_square, "square", 0);
    class_addmethod(oscil_attributes_class, (method)oscil_attributes_build_pulse, "pulse", 0);
    class_addmethod(oscil_attributes_class, (method)oscil_attributes_build_list, "list", A_GIMME, 0);
    class_addmethod(oscil_attributes_class, (method)oscil_attributes_fadetime, "fadetime", A_GIMME, 0);
    class_addmethod(oscil_attributes_class, (method)oscil_attributes_fadetype, "fadetype", A_GIMME, 0);
	
	/* Add standard Max methods to the class */
	class_dspinit(oscil_attributes_class);
	
	/* Register the class with Max */
	class_register(CLASS_BOX, oscil_attributes_class);
	
	/* Print message to Max window */
	object_post(NULL, "oscil_attributes~ • External was loaded");
	
	/* Return with no error */
	return 0;
}

/* The 'new instance' routine *************************************************/
void *oscil_attributes_new(t_symbol *s, short argc, t_atom *argv)
{
	/* Instantiate a new object */
	t_oscil_attributes *x = (t_oscil_attributes *)object_alloc(oscil_attributes_class);
    
	return common_new(x, argc, argv);
}

/* The 'float' method *********************************************************/
void oscil_attributes_float(t_oscil_attributes *x, double farg)
{
    long inlet = ((t_pxobject *)x)->z_in;
    
    switch (inlet) {
        case 0:
            if (farg < MINIMUM_FREQUENCY) {
                farg = MINIMUM_FREQUENCY;
                object_warn((t_object *)x, "Invalid argument: Frequency set to %d[ms]", (int)farg);
            }
            else if (farg > MAXIMUM_FREQUENCY) {
                farg = MAXIMUM_FREQUENCY;
                object_warn((t_object *)x, "Invalid argument: Frequency set to %d[ms]", (int)farg);
            }
            x->frequency = farg;
            break;
    }
    
    /* Print message to Max window */
    object_post((t_object *)x, "Receiving floats");
}

/* The 'assist' method ********************************************************/
void oscil_attributes_assist(t_oscil_attributes *x, void *b, long msg, long arg, char *dst)
{
    /* Document inlet functions */
    if (msg == ASSIST_INLET) {
        switch (arg) {
            case I_FREQUENCY: sprintf(dst, "(signal/float) Frequency"); break;
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
