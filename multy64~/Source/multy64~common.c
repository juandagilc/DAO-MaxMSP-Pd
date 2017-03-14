/* Common header files ********************************************************/
#include "multy64~common.h"

/* The common 'new instance' routine ******************************************/
void *multy64_common_new(t_multy64 *x, short argc, t_atom *argv)
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

    /* Create signal outlets */
    outlet_new(&x->obj, gensym("signal"));

#endif

    /* Print message to Max window */
    post("multy64~ • Object was created");

    /* Return a pointer to the new object */
    return x;
}

/* The 'free instance' routine ************************************************/
void multy64_free(t_multy64 *x)
{
#ifdef TARGET_IS_MAX
    /* Remove the object from the DSP chain */
    dsp_free((t_pxobject *)x);
#endif

    /* Print message to Max window */
    post("multy64~ • Memory was freed");
}

/******************************************************************************/






/* The 'DSP' method ***********************************************************/
void multy64_dsp(t_multy64 *x, t_signal **sp, short *count)
{
    /* Attach the object to the DSP chain */
    dsp_add(multy64_perform, NEXT-1, x,
            sp[0]->s_vec,
            sp[1]->s_vec,
            sp[2]->s_vec,
            sp[0]->s_n);

    /* Print message to Max window */
    post("multy64~ • Executing 32-bit perform routine");
}

/* The 'perform' routine ******************************************************/
t_int *multy64_perform(t_int *w)
{
    /* Copy the object pointer */
    // t_multy64 *x = (t_multy64 *)w[OBJECT];

    /* Copy signal pointers */
    t_float *input1 = (t_float *)w[INPUT1];
    t_float *input2 = (t_float *)w[INPUT2];
    t_float *output = (t_float *)w[OUTPUT];

    /* Copy the signal vector size */
    t_int n = w[VECTOR_SIZE];

    /* Perform the DSP loop */
    while (n--) {
        *output++ = *input1++ * *input2++;
    }

    /* Return the next address in the DSP chain */
    return w + NEXT;
}

/******************************************************************************/






#ifdef TARGET_IS_MAX
/* The 'DSP64' method *********************************************************/
void multy64_dsp64(t_multy64 *x, t_object *dsp64,
                   short *count,
                   double samplerate,
                   long maxvectorsize,
                   long flags)
{
    /* Attach the object to the DSP chain */
    //dsp_add64(dsp64, (t_object *)x, (t_perfroutine64)multy64_perform64, 0, NULL);
    object_method(dsp64, gensym("dsp_add64"), x, multy64_perform64, 0, NULL);

    /* Print message to Max window */
    post("multy64~ • Executing 64-bit perform routine");
}

/* The 'perform64' routine ****************************************************/
void multy64_perform64(t_multy64 *x, t_object *dsp64,
                       double **inputs, long numinputs,
                       double **outputs, long numoutputs,
                       long vectorsize,
                       long flags,
                       void *userparams)
{
    /* Copy signal pointers */
    t_double *input1 = inputs[0];
    t_double *input2 = inputs[1];
    t_double *output = outputs[0];

    /* Copy the signal vector size */
    long n = vectorsize;

    /* Perform the DSP loop */
    while (n--) {
        *output++ = *input1++ * *input2++;
    }
}
#endif

/******************************************************************************/
