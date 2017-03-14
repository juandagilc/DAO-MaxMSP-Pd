/* Common header files ********************************************************/
#include "poltocar~common.h"

/* Function prototypes ********************************************************/
void *poltocar_new(t_symbol *s, short argc, t_atom *argv);

void poltocar_float(t_poltocar *x, double farg);
void poltocar_assist(t_poltocar *x, void *b, long msg, long arg, char *dst);

/* The 'initialization' routine ***********************************************/
int C74_EXPORT main()
{
    /* Initialize the class */
    poltocar_class = class_new("poltocar~",
                               (method)poltocar_new,
                               (method)poltocar_free,
                               sizeof(t_poltocar), 0, A_GIMME, 0);

    /* Bind the DSP method, which is called when the DACs are turned on */
    class_addmethod(poltocar_class, (method)poltocar_dsp, "dsp", A_CANT, 0);

    /* Bind the float method, which is called when floats are sent to inlets */
    class_addmethod(poltocar_class, (method)poltocar_float, "float", A_FLOAT, 0);

    /* Bind the assist method, which is called on mouse-overs to inlets and outlets */
    class_addmethod(poltocar_class, (method)poltocar_assist, "assist", A_CANT, 0);

    /* Add standard Max methods to the class */
    class_dspinit(poltocar_class);

    /* Register the class with Max */
    class_register(CLASS_BOX, poltocar_class);

    /* Print message to Max window */
    object_post(NULL, "poltocar~ • External was loaded");

    /* Return with no error */
    return 0;
}

/* The 'new instance' routine *************************************************/
void *poltocar_new(t_symbol *s, short argc, t_atom *argv)
{
    /* Instantiate a new object */
    t_poltocar *x = (t_poltocar *)object_alloc(poltocar_class);

    return poltocar_common_new(x, argc, argv);
}

/******************************************************************************/






/* The 'float' method *********************************************************/
void poltocar_float(t_poltocar *x, double farg)
{
    // nothing
}

/* The 'assist' method ********************************************************/
void poltocar_assist(t_poltocar *x, void *b, long msg, long arg, char *dst)
{
    /* Document inlet functions */
    if (msg == ASSIST_INLET) {
        switch (arg) {
            case I_MAGN: sprintf(dst, "(signal) Magnitude"); break;
            case I_PHASE: sprintf(dst, "(signal) Phase"); break;
        }
    }

    /* Document outlet functions */
    else if (msg == ASSIST_OUTLET) {
        switch (arg) {
            case O_REAL: sprintf(dst, "(signal) Real part"); break;
            case O_IMAG: sprintf(dst, "(signal) Imaginary part"); break;
        }
    }
}

/******************************************************************************/
