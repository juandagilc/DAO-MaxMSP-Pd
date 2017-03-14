/* Common header files ********************************************************/
#include "windowvec~common.h"

/* The common 'new instance' routine ******************************************/
void *windowvec_common_new(t_windowvec *x, short argc, t_atom *argv)
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
    post("windowvec~ • Object was created");

    /* Return a pointer to the new object */
    return x;
}

/* The 'free instance' routine ************************************************/
void windowvec_free(t_windowvec *x)
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
    post("windowvec~ • Memory was freed");
}

/******************************************************************************/






/* The 'DSP' method ***********************************************************/
void windowvec_dsp(t_windowvec *x, t_signal **sp, short *count)
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
    dsp_add(windowvec_perform, NEXT-1, x,
            sp[0]->s_vec,
            sp[1]->s_vec,
            sp[0]->s_n);

    /* Print message to Max window */
    post("windowvec~ • Executing 32-bit perform routine");
}

/* The 'perform' routine ******************************************************/
t_int *windowvec_perform(t_int *w)
{
    /* Copy the object pointer */
    t_windowvec *x = (t_windowvec *)w[OBJECT];

    /* Copy signal pointers */
    t_float *input = (t_float *)w[INPUT1];
    t_float *output = (t_float *)w[OUTPUT1];

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
