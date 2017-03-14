/* Common header files ********************************************************/
#include "poltocar~common.h"

/* The common 'new instance' routine ******************************************/
void *poltocar_common_new(t_poltocar *x, short argc, t_atom *argv)
{
#ifdef TARGET_IS_MAX
    /* Create inlets */
    dsp_setup((t_pxobject *)x, NUM_INLETS);

    /* Create signal outlets */
    outlet_new((t_object *)x, "signal");
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
    post("poltocar~ • Object was created");

    /* Return a pointer to the new object */
    return x;
}

/* The 'free instance' routine ************************************************/
void poltocar_free(t_poltocar *x)
{
#ifdef TARGET_IS_MAX
    /* Remove the object from the DSP chain */
    dsp_free((t_pxobject *)x);
#endif

    /* Print message to Max window */
    post("poltocar~ • Memory was freed");
}

/******************************************************************************/






/* The 'DSP' method ***********************************************************/
void poltocar_dsp(t_poltocar *x, t_signal **sp, short *count)
{
    /* Attach the object to the DSP chain */
    dsp_add(poltocar_perform, NEXT-1, x,
            sp[0]->s_vec,
            sp[1]->s_vec,
            sp[2]->s_vec,
            sp[3]->s_vec,
            sp[0]->s_n);

    /* Print message to Max window */
    post("poltocar~ • Executing 32-bit perform routine");
}

/* The 'perform' routine ******************************************************/
t_int *poltocar_perform(t_int *w)
{
    /* Copy the object pointer */
    // t_poltocar *x = (t_poltocar *)w[OBJECT];

    /* Copy signal pointers */
    t_float *input_magn = (t_float *)w[INPUT_MAGN];
    t_float *input_phase = (t_float *)w[INPUT_PHASE];
    t_float *output_real = (t_float *)w[OUTPUT_REAL];
    t_float *output_imag = (t_float *)w[OUTPUT_IMAG];

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
        local_real = input_magn[ii] * cos(input_phase[ii]);
        local_imag = -input_magn[ii] * sin(input_phase[ii]);

        output_real[ii] = local_real;
        output_imag[ii] = (ii == 0 || ii == framesize-1) ? 0.0 : local_imag;
    }

    #ifdef TARGET_IS_PD
        for (int ii = framesize; ii < n; ii++) {
            output_real[ii] = 0.0;
            output_imag[ii] = 0.0;
        }
    #endif

    /* Return the next address in the DSP chain */
    return w + NEXT;
}

/******************************************************************************/
