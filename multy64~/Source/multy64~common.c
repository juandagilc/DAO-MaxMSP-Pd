/* Common header files ********************************************************/
#include "multy64~common.h"

/* The common 'new instance' routine ******************************************/
void *common_new(t_multy64 *x, short argc, t_atom *argv)
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
            sp[3]->s_vec,
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
    t_float *input_real = (t_float *)w[INPUT_REAL];
    t_float *input_imag = (t_float *)w[INPUT_IMAG];
    t_float *output_magn = (t_float *)w[OUTPUT_MAGN];
    t_float *output_phase = (t_float *)w[OUTPUT_PHASE];

    /* Copy the signal vector size */
    t_int n = w[VECTOR_SIZE];

    /* Perform the DSP loop */
    int framesize;
    #ifdef TARGET_IS_MAX
        framesize = (int)n;
    #elif TARGET_IS_PD
        framesize = (int)(n / 2) + 1;
    #endif

    float local_real;
    float local_imag;

    for (int ii = 0; ii < framesize; ii++) {
        local_real = input_real[ii];
        local_imag = (ii == 0 || ii == framesize-1) ? 0.0 : input_imag[ii];

        output_magn[ii] = hypotf(local_real, local_imag);
        output_phase[ii] = -atan2(local_imag, local_real);
    }

    #ifdef TARGET_IS_PD
        for (int ii = framesize; ii < n; ii++) {
            output_magn[ii] = 0.0;
            output_phase[ii] = 0.0;
        }
    #endif

    /* Return the next address in the DSP chain */
    return w + NEXT;
}

/******************************************************************************/
