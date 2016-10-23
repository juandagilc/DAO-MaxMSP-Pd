/* Common header files ********************************************************/
#include "bed_common.h"

/* Function prototypes ********************************************************/
void *bed_new(t_symbol *s, short argc, t_atom *argv);

void bed_float(t_bed *x, double farg);
void bed_assist(t_bed *x, void *b, long msg, long arg, char *dst);

/* The 'initialization' routine ***********************************************/
int C74_EXPORT main()
{
    /* Initialize the class */
    bed_class = class_new("bed",
                          (method)bed_new,
                          (method)bed_free,
                          sizeof(t_bed), 0, A_GIMME, 0);

    /* Bind the DSP method, which is called when the DACs are turned on */
    class_addmethod(bed_class, (method)bed_dsp, "dsp", A_CANT, 0);

    /* Bind the float method, which is called when floats are sent to inlets */
    class_addmethod(bed_class, (method)bed_float, "float", A_FLOAT, 0);

    /* Bind the assist method, which is called on mouse-overs to inlets and outlets */
    class_addmethod(bed_class, (method)bed_assist, "assist", A_CANT, 0);

    /* Bind the object-specific methods */
    class_addmethod(bed_class, (method)bed_list, "list", A_GIMME, 0);
    class_addmethod(bed_class, (method)bed_freqlist, "freqlist", A_GIMME, 0);
    class_addmethod(bed_class, (method)bed_durlist, "durlist", A_GIMME, 0);

    class_addmethod(bed_class, (method)bed_shuffle_freqs, "shuffle_freqs", 0);
    class_addmethod(bed_class, (method)bed_shuffle_durs, "shuffle_durs", 0);
    class_addmethod(bed_class, (method)bed_shuffle, "shuffle", 0);

    class_addmethod(bed_class, (method)bed_set_tempo, "tempo", A_GIMME, 0);
    class_addmethod(bed_class, (method)bed_set_elastic_sustain, "elastic_sustain", A_GIMME, 0);
    class_addmethod(bed_class, (method)bed_set_sustain_amplitude, "sustain_amplitude", A_GIMME, 0);
    class_addmethod(bed_class, (method)bed_set_adsr, "adsr", A_GIMME, 0);

    class_addmethod(bed_class, (method)bed_manual_override, "manual_override", A_LONG, 0);
    class_addmethod(bed_class, (method)bed_trigger_sent, "bang", 0);
    class_addmethod(bed_class, (method)bed_play_backwards, "play_backwards", A_LONG, 0);

    /* Add standard Max methods to the class */
    class_dspinit(bed_class);

    /* Register the class with Max */
    class_register(CLASS_BOX, bed_class);

    /* Print message to Max window */
    object_post(NULL, "bed • External was loaded");

    /* Return with no error */
    return 0;
}

/* The 'new instance' routine *************************************************/
void *bed_new(t_symbol *s, short argc, t_atom *argv)
{
    /* Instantiate a new object */
    t_bed *x = (t_bed *)object_alloc(bed_class);

    return common_new(x, argc, argv);
}

/******************************************************************************/






/* The 'float' method *********************************************************/
void bed_float(t_bed *x, double farg)
{
    //nothing

    /* Print message to Max window */
    object_post((t_object *)x, "Receiving floats");
}

/* The 'assist' method ********************************************************/
void bed_assist(t_bed *x, void *b, long msg, long arg, char *dst)
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
        switch (arg) {
            case O_ADSR: sprintf(dst, "(list) ADSR envelope"); break;
        }
        switch (arg) {
            case O_BANG: sprintf(dst, "(bang) When sequence (re)starts"); break;
        }
        switch (arg) {
            case O_SHUFFLE_F: sprintf(dst, "(list) Permuted frequency sequence"); break;
        }
        switch (arg) {
            case O_SHUFFLE_D: sprintf(dst, "(list) Permuted duration sequence"); break;
        }
    }
}

/******************************************************************************/
