/* Common header files ********************************************************/
#include "moogvcf~common.h"

/* The common 'new instance' routine ******************************************/
void *moogvcf_common_new(t_moogvcf *x, short argc, t_atom *argv)
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

    /* Print message to Max window */
    post("moogvcf~ • Object was created");

    /* Return a pointer to the new object */
    return x;
}

/* The 'free instance' routine ************************************************/
void moogvcf_free(t_moogvcf *x)
{
#ifdef TARGET_IS_MAX
    /* Remove the object from the DSP chain */
    dsp_free((t_pxobject *)x);
#endif

    /* Free allocated dynamic memory */
    // nothing

    /* Print message to Max window */
    post("moogvcf~ • Memory was freed");
}

/******************************************************************************/






/* The 'DSP' method ***********************************************************/
void moogvcf_dsp(t_moogvcf *x, t_signal **sp, short *count)
{
    if (sp[0]->s_sr == 0) {
        error("moogvcf~ • Sampling rate is equal to zero!");
        return;
    }

    /* Initialize state variables */
    x->onedsr = 1 / sp[0]->s_sr;
    x->xnm1 = 0.0;
    x->y1nm1 = 0.0;
    x->y2nm1 = 0.0;
    x->y3nm1 = 0.0;
    x->y1n = 0.0;
    x->y2n = 0.0;
    x->y3n = 0.0;
    x->y4n = 0.0;

    /* Attach the object to the DSP chain */
    dsp_add(moogvcf_perform, NEXT-1, x,
            sp[0]->s_vec,
            sp[1]->s_vec,
            sp[2]->s_vec,
            sp[3]->s_vec,
            sp[0]->s_n);

    /* Print message to Max window */
    post("moogvcf~ • Executing 32-bit perform routine");
}

/* The 'perform' routine ******************************************************/
t_int *moogvcf_perform(t_int *w)
{
    /* Copy the object pointer */
    t_moogvcf *x = (t_moogvcf *)w[OBJECT];

    /* Copy signal pointers */
    t_float *input = (t_float *)w[INPUT1];
    t_float *frequency = (t_float *)w[FREQUENCY];
    t_float *resonance = (t_float *)w[RESONANCE];
    t_float *output = (t_float *)w[OUTPUT1];

    /* Copy the signal vector size */
    t_int n = w[VECTOR_SIZE];

    /* Load state variables */
    double onedsr = x->onedsr;
    double xnm1 = x->xnm1;
    double y1nm1 = x->y1nm1;
    double y2nm1 = x->y2nm1;
    double y3nm1 = x->y3nm1;
    double y1n = x->y1n;
    double y2n = x->y2n;
    double y3n = x->y3n;
    double y4n = x->y4n;

    /* Perform the DSP loop */
    double freq_factor = 1.78179 * onedsr;
    double frequency_normalized = 0.0;
    double kp = 0.0;
    double pp1d2 = 0.0;
    double scale = 0.0;

    double k = 0.0;
    double xn = 0.0;

    for (int ii = 0; ii < n; ii++) {
        // normalized frequency from 0 to nyquist
        frequency_normalized = *frequency * freq_factor;

        // empirical tunning
        kp = - 1.0
             + 3.6 * frequency_normalized
             - 1.6 * frequency_normalized * frequency_normalized;

        // timesaver
        pp1d2 = (kp + 1.0) * 0.5;

        // scaling factor
        scale = exp((1.0 - pp1d2) * 1.386249);

        // inverted feedback for corner peaking
        k = *resonance++ * scale;
        xn = *input++ - (k * y4n);

        // four cascade onepole filters (bilinear transform)
        y1n = (xn  + xnm1 ) * pp1d2 - (kp * y1n);
        y2n = (y1n + y1nm1) * pp1d2 - (kp * y2n);
        y3n = (y2n + y2nm1) * pp1d2 - (kp * y3n);
        y4n = (y3n + y3nm1) * pp1d2 - (kp * y4n);

        // update coefficients
        xnm1 = xn;
        y1nm1 = y1n;
        y2nm1 = y2n;
        y3nm1 = y3n;

        // clipper band limited sigmoid
        *output++ = y4n - (y4n * y4n * y4n) / 6.0;
    }

    /* Update state variables */
    x->xnm1 = xnm1;
    x->y1nm1 = y1nm1;
    x->y2nm1 = y2nm1;
    x->y3nm1 = y3nm1;
    x->y1n = y1n;
    x->y2n = y2n;
    x->y3n = y3n;
    x->y4n = y4n;
    
    /* Return the next address in the DSP chain */
    return w + NEXT;
}

/******************************************************************************/
