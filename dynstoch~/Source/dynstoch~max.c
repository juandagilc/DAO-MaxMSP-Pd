/* Common header files ********************************************************/
#include "dynstoch~common.h"

/* Function prototypes ********************************************************/
void *dynstoch_new(t_symbol *s, short argc, t_atom *argv);

t_max_err a_ampdev_set(t_dynstoch *x, void *attr, long ac, t_atom *av);
t_max_err a_durdev_set(t_dynstoch *x, void *attr, long ac, t_atom *av);
t_max_err a_setfreq_set(t_dynstoch *x, void *attr, long ac, t_atom *av);
t_max_err a_freqrange_set(t_dynstoch *x, void *attr, long ac, t_atom *av);
void dynstoch_float(t_dynstoch *x, double farg);
void dynstoch_assist(t_dynstoch *x, void *b, long msg, long arg, char *dst);

/* The 'initialization' routine ***********************************************/
int C74_EXPORT main()
{
    /* Initialize the class */
    dynstoch_class = class_new("dynstoch~",
                               (method)dynstoch_new,
                               (method)dynstoch_free,
                               sizeof(t_dynstoch), 0, A_GIMME, 0);

    /* Bind the DSP method, which is called when the DACs are turned on */
    class_addmethod(dynstoch_class, (method)dynstoch_dsp, "dsp", A_CANT, 0);

    /* Bind the float method, which is called when floats are sent to inlets */
    class_addmethod(dynstoch_class, (method)dynstoch_float, "float", A_FLOAT, 0);

    /* Bind the assist method, which is called on mouse-overs to inlets and outlets */
    class_addmethod(dynstoch_class, (method)dynstoch_assist, "assist", A_CANT, 0);

    /* Bind the object-specific methods */
    // handled using attributes

    /* Add standard Max methods to the class */
    class_dspinit(dynstoch_class);

    /* Bind the attributes */
    CLASS_ATTR_FLOAT(dynstoch_class, "ampdev", 0, t_dynstoch, a_ampdev);
    CLASS_ATTR_LABEL(dynstoch_class, "ampdev", 0, "Amplitude deviation");
    CLASS_ATTR_ORDER(dynstoch_class, "ampdev", 0, "1");
    CLASS_ATTR_ACCESSORS(dynstoch_class, "ampdev", NULL, a_ampdev_set);

    CLASS_ATTR_FLOAT(dynstoch_class, "durdev", 0, t_dynstoch, a_durdev);
    CLASS_ATTR_LABEL(dynstoch_class, "durdev", 0, "Duration deviation");
    CLASS_ATTR_ORDER(dynstoch_class, "durdev", 0, "2");
    CLASS_ATTR_ACCESSORS(dynstoch_class, "durdev", NULL, a_durdev_set);

    CLASS_ATTR_FLOAT(dynstoch_class, "setfreq", 0, t_dynstoch, a_newfreq);
    CLASS_ATTR_LABEL(dynstoch_class, "setfreq", 0, "Frequency");
    CLASS_ATTR_ORDER(dynstoch_class, "setfreq", 0, "3");
    CLASS_ATTR_ACCESSORS(dynstoch_class, "setfreq", NULL, a_setfreq_set);

    CLASS_ATTR_FLOAT_ARRAY(dynstoch_class, "freqrange", 0, t_dynstoch, a_freqrange, 2);
    CLASS_ATTR_LABEL(dynstoch_class, "freqrange", 0, "Frequency range");
    CLASS_ATTR_ORDER(dynstoch_class, "freqrange", 0, "4");
    CLASS_ATTR_ACCESSORS(dynstoch_class, "freqrange", NULL, a_freqrange_set);

    /* Register the class with Max */
    class_register(CLASS_BOX, dynstoch_class);

    /* Print message to Max window */
    object_post(NULL, "dynstoch~ • External was loaded");

    /* Return with no error */
    return 0;
}

/* The 'new instance' routine *************************************************/
void *dynstoch_new(t_symbol *s, short argc, t_atom *argv)
{
    /* Instantiate a new object */
    t_dynstoch *x = (t_dynstoch *)object_alloc(dynstoch_class);

    return dynstoch_common_new(x, argc, argv);
}

/******************************************************************************/






/* The 'setter' and 'getter' methods for attributes ***************************/
t_max_err a_ampdev_set(t_dynstoch *x, void *attr, long ac, t_atom *av)
{
    if (ac && av) {
        x->a_ampdev = atom_getfloat(av);
        dynstoch_ampdev(x, x->a_ampdev);
    }

    return MAX_ERR_NONE;
}

t_max_err a_durdev_set(t_dynstoch *x, void *attr, long ac, t_atom *av)
{
    if (ac && av) {
        x->a_durdev = atom_getfloat(av);
        dynstoch_durdev(x, x->a_durdev);
    }

    return MAX_ERR_NONE;
}

t_max_err a_setfreq_set(t_dynstoch *x, void *attr, long ac, t_atom *av)
{
    if (ac && av) {
        x->a_newfreq = atom_getfloat(av);
        dynstoch_setfreq(x, x->a_newfreq);
    }

    return MAX_ERR_NONE;
}

t_max_err a_freqrange_set(t_dynstoch *x, void *attr, long ac, t_atom *av)
{
    if (ac && av) {
        atom_getfloat_array(ac, av, 2, x->a_freqrange);
        dynstoch_freqrange(x, x->a_freqrange[0], x->a_freqrange[1]);
    }

    return MAX_ERR_NONE;
}

/* The 'float' method *********************************************************/
void dynstoch_float(t_dynstoch *x, double farg)
{
    // nothing
}

/* The 'assist' method ********************************************************/
void dynstoch_assist(t_dynstoch *x, void *b, long msg, long arg, char *dst)
{
    /* Document inlet functions */
    if (msg == ASSIST_INLET) {
        switch (arg) {
            case I_INPUT: sprintf(dst, "(messages) Object's messages"); break;
        }
    }

    /* Document outlet functions */
    else if (msg == ASSIST_OUTLET) {
        switch (arg) {
            case O_OUTPUT: sprintf(dst, "(signal) Output"); break;
            case O_FREQUENCY: sprintf(dst, "(signal) Frequency"); break;
        }
    }
}

/******************************************************************************/
