/* Common header files ********************************************************/
#include "oscil_attributes~common.h"

/* Function prototypes ********************************************************/
void *oscil_attributes_new(t_symbol *s, short argc, t_atom *argv);

void oscil_attributes_float(t_oscil_attributes *x, double farg);
t_max_err a_frequency_set(t_oscil_attributes *x, void *attr, long ac, t_atom *av);
t_max_err a_crossfade_type_set(t_oscil_attributes *x, void *attr, long ac, t_atom *av);
t_max_err a_crossfade_time_set(t_oscil_attributes *x, void *attr, long ac, t_atom *av);
t_max_err a_waveform_set(t_oscil_attributes *x, void *attr, long ac, t_atom *av);
t_max_err a_harmonics_set(t_oscil_attributes *x, void *attr, long ac, t_atom *av);
t_max_err a_amplitudes_get(t_oscil_attributes *x, void *attr, long *ac, t_atom **av);
t_max_err a_amplitudes_set(t_oscil_attributes *x, void *attr, long ac, t_atom *av);
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
	
    /* Bind the attributes */
    CLASS_ATTR_FLOAT(oscil_attributes_class, "frequency", 0, t_oscil_attributes, a_frequency);
    CLASS_ATTR_LABEL(oscil_attributes_class, "frequency", 0, "Frequency");
    CLASS_ATTR_ORDER(oscil_attributes_class, "frequency", 0, "1");
    CLASS_ATTR_ACCESSORS(oscil_attributes_class, "frequency", NULL, a_frequency_set);

    CLASS_ATTR_LONG(oscil_attributes_class, "fadetype", 0, t_oscil_attributes, a_crossfade_type);
    CLASS_ATTR_LABEL(oscil_attributes_class, "fadetype", 0, "Crossfade type");
    CLASS_ATTR_ORDER(oscil_attributes_class, "fadetype", 0, "2");
    CLASS_ATTR_ENUMINDEX(oscil_attributes_class, "fadetype", 0,
                         "\"No Fade\" \"Linear\" \"Equal power\"");
    CLASS_ATTR_ACCESSORS(oscil_attributes_class, "fadetype", NULL, a_crossfade_type_set);

    CLASS_ATTR_FLOAT(oscil_attributes_class, "fadetime", 0, t_oscil_attributes, a_crossfade_time);
    CLASS_ATTR_LABEL(oscil_attributes_class, "fadetime", 0, "Crossfade time");
    CLASS_ATTR_ORDER(oscil_attributes_class, "fadetime", 0, "3");
    CLASS_ATTR_ACCESSORS(oscil_attributes_class, "fadetime", NULL, a_crossfade_time_set);

    CLASS_ATTR_SYM(oscil_attributes_class, "waveform", 0, t_oscil_attributes, a_waveform);
    CLASS_ATTR_LABEL(oscil_attributes_class, "waveform", 0, "Waveform");
    CLASS_ATTR_ORDER(oscil_attributes_class, "waveform", 0, "4");
    CLASS_ATTR_ENUM(oscil_attributes_class, "waveform", 0, "sine triangle sawtooth square pulse additive");
    CLASS_ATTR_ACCESSORS(oscil_attributes_class, "waveform", NULL, a_waveform_set);

    CLASS_ATTR_LONG(oscil_attributes_class, "harmonics", 0, t_oscil_attributes, a_harmonics);
    CLASS_ATTR_LABEL(oscil_attributes_class, "harmonics", 0, "Harmonics");
    CLASS_ATTR_ORDER(oscil_attributes_class, "harmonics", 0, "5");
    CLASS_ATTR_ACCESSORS(oscil_attributes_class, "harmonics", NULL, a_harmonics_set);

    CLASS_ATTR_FLOAT_ARRAY(oscil_attributes_class, "amplitudes", 0, t_oscil_attributes, a_amplitudes, MAXIMUM_HARMONICS);
    CLASS_ATTR_LABEL(oscil_attributes_class, "amplitudes", 0, "Amplitudes");
    CLASS_ATTR_ORDER(oscil_attributes_class, "amplitudes", 0, "6");
    CLASS_ATTR_ACCESSORS(oscil_attributes_class, "amplitudes", a_amplitudes_get, a_amplitudes_set);

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
    
	return oscil_attributes_common_new(x, argc, argv);
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

/* The 'setter' and 'getter' methods for attributes ***************************/
t_max_err a_frequency_set(t_oscil_attributes *x, void *attr, long ac, t_atom *av)
{
    if (ac && av) {
        x->a_frequency = atom_getfloat(av);
        x->frequency = x->a_frequency;
    }

    return MAX_ERR_NONE;
}

t_max_err a_crossfade_type_set(t_oscil_attributes *x, void *attr, long ac, t_atom *av)
{
    if (ac && av) {
        x->a_crossfade_type = atom_getfloat(av);
        x->crossfade_type = x->a_crossfade_type;
    }

    return MAX_ERR_NONE;
}

t_max_err a_crossfade_time_set(t_oscil_attributes *x, void *attr, long ac, t_atom *av)
{
    if (ac && av) {
        x->a_crossfade_time = atom_getfloat(av);
        x->crossfade_time = x->a_crossfade_time;
    }

    oscil_attributes_fadetime(x, NULL, ac, av);

    return MAX_ERR_NONE;
}

t_max_err a_waveform_set(t_oscil_attributes *x, void *attr, long ac, t_atom *av)
{
    if (ac && av) {
        x->a_waveform = atom_getsym(av);
        x->waveform = x->a_waveform;

        oscil_attributes_build_wavetable(x);
    }

    return MAX_ERR_NONE;
}

t_max_err a_harmonics_set(t_oscil_attributes *x, void *attr, long ac, t_atom *av)
{
    if (ac && av) {
        x->a_harmonics = atom_getfloat(av);
        x->harmonics = x->a_harmonics;

        oscil_attributes_build_wavetable(x);
    }

    return MAX_ERR_NONE;
}

t_max_err a_amplitudes_get(t_oscil_attributes *x, void *attr, long *ac, t_atom **av)
{
    if (!((*ac) && (*av))) {
        *ac = MAXIMUM_HARMONICS;
        if (!(*av = (t_atom *)sysmem_newptr(sizeof(t_atom) * (*ac)))) {
            *ac = 0;
            return MAX_ERR_OUT_OF_MEM;
        }
    }

    for (int ii = 0; ii < MAXIMUM_HARMONICS; ii++) {
        atom_setfloat(*av + ii, x->a_amplitudes[ii]);
    }

    return MAX_ERR_NONE;
}

t_max_err a_amplitudes_set(t_oscil_attributes *x, void *attr, long ac, t_atom *av)
{
    if (ac && av) {
        for (int ii = 0; ii < MAXIMUM_HARMONICS; ii++) {
            x->a_amplitudes[ii] = atom_getfloatarg(ii, ac, av);
        }
    }

    t_atom *rv = NULL;
    object_method_sym((t_object *)x, gensym("waveform"), gensym("additive"), rv);

    return MAX_ERR_NONE;
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
