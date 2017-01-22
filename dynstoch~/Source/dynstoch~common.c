/* Common header files ********************************************************/
#include "dynstoch~common.h"

/* The common 'new instance' routine ******************************************/
void *common_new(t_dynstoch *x, short argc, t_atom *argv)
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
    // inlet_new(&x->obj, &x->obj.ob_pd, gensym("signal"), gensym("signal"));

    /* Create signal outlets */
    outlet_new(&x->obj, gensym("signal"));

#endif

    /* Print message to Max window */
    post("dynstoch~ • Object was created");

    /* Return a pointer to the new object */
    return x;
}

/* The 'free instance' routine ************************************************/
void dynstoch_free(t_dynstoch *x)
{
#ifdef TARGET_IS_MAX
    /* Remove the object from the DSP chain */
    dsp_free((t_pxobject *)x);
#endif

    /* Free allocated dynamic memory */
    // nothing

    /* Print message to Max window */
    post("dynstoch~ • Memory was freed");
}

/******************************************************************************/






/* The 'DSP' method ***********************************************************/
void dynstoch_dsp(t_dynstoch *x, t_signal **sp, short *count)
{
    if (sp[0]->s_sr == 0) {
        error("dynstoch~ • Sampling rate is equal to zero!");
        return;
    }

    /* Initialize state variables */
    // nothing

    /* Attach the object to the DSP chain */
    dsp_add(dynstoch_perform, NEXT-1, x,
            sp[0]->s_vec,
            sp[1]->s_vec,
            sp[0]->s_n);

    /* Print message to Max window */
    post("dynstoch~ • Executing 32-bit perform routine");
}

/* The 'perform' routine ******************************************************/
t_int *dynstoch_perform(t_int *w)
{
    /* Copy the object pointer */
    t_dynstoch *x = (t_dynstoch *)w[OBJECT];

    /* Copy signal pointers */
    t_float *input = (t_float *)w[INPUT];
    t_float *output = (t_float *)w[OUTPUT];

    /* Copy the signal vector size */
    t_int n = w[VECTOR_SIZE];

    /* Load state variables */
    // nothing

    /* Perform the DSP loop */
    for (int ii = 0; ii < n; ii++) {
        *output++ = *input++;
    }

    /* Update state variables */
    // nothing
    
    /* Return the next address in the DSP chain */
    return w + NEXT;
}

/******************************************************************************/
