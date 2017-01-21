/* Common header files ********************************************************/
#include "moogvcf~common.h"

/* The common 'new instance' routine ******************************************/
void *common_new(t_moogvcf *x, short argc, t_atom *argv)
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
    // nothing - Pd gives one by default

    /* Create signal outlets */
    outlet_new(&x->obj, gensym("signal"));

#endif

    /* Initialize some state variables */
    x->fs = sys_getsr();

    x->window = NULL;
    x->vecsize = 128;

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
    if (x->window != NULL) {
        free(x->window);
    }

    /* Print message to Max window */
    post("moogvcf~ • Memory was freed");
}

/******************************************************************************/






/* The 'DSP' method ***********************************************************/
void moogvcf_dsp(t_moogvcf *x, t_signal **sp, short *count)
{
    if (x->vecsize != sp[0]->s_n) {
        x->vecsize = sp[0]->s_n;

        int bytesize = x->vecsize * sizeof(float);
        if (x->window == NULL) {
            x->window = (float *)malloc(bytesize);
        } else {
            x->window = (float *)realloc(x->window, bytesize);
        }

        for (int ii = 0; ii < x->vecsize; ii++) {
            x->window[ii] = -0.5 * cos(TWOPI * ii / (float)x->vecsize) + 0.5;
        }
    }

    /* Attach the object to the DSP chain */
    dsp_add(moogvcf_perform, NEXT-1, x,
            sp[0]->s_vec,
            sp[1]->s_vec,
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
    t_float *input = (t_float *)w[INPUT];
    t_float *output = (t_float *)w[OUTPUT];

    /* Copy the signal vector size */
    t_int n = w[VECTOR_SIZE];

    /* Load state variables */
    float *window = x->window;
    float vecsize = x->vecsize;

    /* Perform the DSP loop */
    for (int ii = 0; ii < n; ii++) {
        output[ii] = input[ii] * window[ii];
    }
    
    /* Return the next address in the DSP chain */
    return w + NEXT;
}

/******************************************************************************/
