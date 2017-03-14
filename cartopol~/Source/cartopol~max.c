/* Common header files ********************************************************/
#include "cartopol~common.h"

/* Function prototypes ********************************************************/
void *cartopol_new(t_symbol *s, short argc, t_atom *argv);

void cartopol_float(t_cartopol *x, double farg);
void cartopol_assist(t_cartopol *x, void *b, long msg, long arg, char *dst);

/* The 'initialization' routine ***********************************************/
int C74_EXPORT main()
{
    /* Initialize the class */
    cartopol_class = class_new("cartopol~",
                               (method)cartopol_new,
                               (method)cartopol_free,
                               sizeof(t_cartopol), 0, A_GIMME, 0);

    /* Bind the DSP method, which is called when the DACs are turned on */
    class_addmethod(cartopol_class, (method)cartopol_dsp, "dsp", A_CANT, 0);

    /* Bind the float method, which is called when floats are sent to inlets */
    class_addmethod(cartopol_class, (method)cartopol_float, "float", A_FLOAT, 0);

    /* Bind the assist method, which is called on mouse-overs to inlets and outlets */
    class_addmethod(cartopol_class, (method)cartopol_assist, "assist", A_CANT, 0);

    /* Add standard Max methods to the class */
    class_dspinit(cartopol_class);

    /* Register the class with Max */
    class_register(CLASS_BOX, cartopol_class);

    /* Print message to Max window */
    object_post(NULL, "cartopol~ • External was loaded");

    /* Return with no error */
    return 0;
}

/* The 'new instance' routine *************************************************/
void *cartopol_new(t_symbol *s, short argc, t_atom *argv)
{
    /* Instantiate a new object */
    t_cartopol *x = (t_cartopol *)object_alloc(cartopol_class);

    return cartopol_common_new(x, argc, argv);
}

/******************************************************************************/






/* The 'float' method *********************************************************/
void cartopol_float(t_cartopol *x, double farg)
{
    // nothing
}

/* The 'assist' method ********************************************************/
void cartopol_assist(t_cartopol *x, void *b, long msg, long arg, char *dst)
{
    /* Document inlet functions */
    if (msg == ASSIST_INLET) {
        switch (arg) {
            case I_REAL: sprintf(dst, "(signal) Real part"); break;
            case I_IMAG: sprintf(dst, "(signal) Imaginary part"); break;
        }
    }

    /* Document outlet functions */
    else if (msg == ASSIST_OUTLET) {
        switch (arg) {
            case O_MAGN: sprintf(dst, "(signal) Magnitude"); break;
            case O_PHASE: sprintf(dst, "(signal) Phase"); break;
        }
    }
}

/******************************************************************************/
