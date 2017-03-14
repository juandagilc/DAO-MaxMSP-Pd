/* Common header files ********************************************************/
#include "cleaner~common.h"

/* The common 'new instance' routine ******************************************/
void *cleaner_common_new(t_cleaner *x, short argc, t_atom *argv)
{
#ifdef TARGET_IS_MAX
    /* Create inlets */
    dsp_setup((t_pxobject *)x, NUM_INLETS);

    /* Create signal outlets */
    outlet_new((t_object *)x, "signal");

    /* Avoid sharing memory among audio vectors */
    x->obj.z_misc |= Z_NO_INPLACE;

#elif TARGET_IS_PD
    /* Create inlets */
    inlet_new(&x->obj, &x->obj.ob_pd, gensym("signal"), gensym("signal"));
    inlet_new(&x->obj, &x->obj.ob_pd, gensym("signal"), gensym("signal"));

    /* Create signal outlets */
    outlet_new(&x->obj, gensym("signal"));

#endif

    /* Initialize input arguments */
    float threshold_value = DEFAULT_THRESHOLD;
    float attenuation_value = DEFAULT_ATTENUATION;

    /* Parse passed arguments */
    if (argc > A_THRESHOLD) {
        threshold_value = atom_getfloatarg(A_THRESHOLD, argc, argv);
    }
    if (argc > A_ATTENUATION) {
        attenuation_value = atom_getfloatarg(A_ATTENUATION, argc, argv);
    }

    /* Check validity of passed arguments */
    if (threshold_value < MINIMUM_THRESHOLD) {
        threshold_value = MINIMUM_THRESHOLD;
        post("cleaner~ • Invalid argument: Minimum threshold value set to %.4f", threshold_value);
    }
    else if (threshold_value > MAXIMUM_THRESHOLD) {
        threshold_value = MAXIMUM_THRESHOLD;
        post("cleaner~ • Invalid argument: Maximum threshold value set to %.4f", threshold_value);
    }
    if (attenuation_value < MINIMUM_ATTENUATION) {
        attenuation_value = MINIMUM_ATTENUATION;
        post("cleaner~ • Invalid argument: Minimum attenuation value set to %.4f", attenuation_value);
    }
    else if (attenuation_value > MAXIMUM_ATTENUATION) {
        attenuation_value = MAXIMUM_ATTENUATION;
        post("cleaner~ • Invalid argument: Maximum attenuation value set to %.4f", attenuation_value);
    }

    /* Initialize some state variables */
    x->fs = sys_getsr();
    x->threshold_value = threshold_value;
    x->attenuation_value = attenuation_value;

    /* Print message to Max window */
    post("cleaner~ • Object was created");

    /* Return a pointer to the new object */
    return x;
}

/* The 'free instance' routine ************************************************/
void cleaner_free(t_cleaner *x)
{
#ifdef TARGET_IS_MAX
    /* Remove the object from the DSP chain */
    dsp_free((t_pxobject *)x);
#endif

    /* Print message to Max window */
    post("cleaner~ • Memory was freed");
}

/******************************************************************************/






/* The 'DSP' method ***********************************************************/
void cleaner_dsp(t_cleaner *x, t_signal **sp, short *count)
{
    /* Store signal connection states of inlets */
#ifdef TARGET_IS_MAX
    x->threshold_connected = count[I_THRESHOLD];
    x->attenuation_connected = count[I_ATTENUATION];
#elif TARGET_IS_PD
    x->threshold_connected = 1;
    x->attenuation_connected = 1;
#endif

    /* Adjust to changes in the sampling rate */
    if (x->fs != sp[0]->s_sr) {
        x->fs = sp[0]->s_sr;
    }

    /* Attach the object to the DSP chain */
    dsp_add(cleaner_perform, NEXT-1, x,
            sp[0]->s_vec,
            sp[1]->s_vec,
            sp[2]->s_vec,
            sp[3]->s_vec,
            sp[0]->s_n);

    /* Print message to Max window */
    post("cleaner~ • Executing 32-bit perform routine");
}

/* The 'perform' routine ******************************************************/
t_int *cleaner_perform(t_int *w)
{
    /* Copy the object pointer */
    t_cleaner *x = (t_cleaner *)w[OBJECT];

    /* Copy signal pointers */
    t_float *input = (t_float *)w[INPUT1];
    t_float *threshold = (t_float *)w[THRESHOLD];
    t_float *attenuation = (t_float *)w[ATTENUATION];
    t_float *output = (t_float *)w[OUTPUT1];

    /* Copy the signal vector size */
    t_int n = w[VECTOR_SIZE];

    /* Load state variables */
    // nothing

    /* Perform the DSP loop */
    float maxamp = 0.0;
    float threshold_value;
    float attenuation_value;

    for (int ii = 0; ii < n; ii++) {
        if (maxamp < input[ii]) {
            maxamp = input[ii];
        }
    }

    if (x->threshold_connected) {
        threshold_value = (*threshold) * maxamp;
    } else {
        threshold_value = x->threshold_value * maxamp;
    }

    if (x->attenuation_connected) {
        attenuation_value = (*attenuation);
    } else {
        attenuation_value = x->attenuation_value;
    }

    for (int ii = 0; ii < n; ii++) {
        if (input[ii] < threshold_value) {
            input[ii] *= attenuation_value;
        }
        output[ii] = input[ii];
    }

    /* Update state variables */
    // nothing
    
    /* Return the next address in the DSP chain */
    return w + NEXT;
}

/******************************************************************************/
