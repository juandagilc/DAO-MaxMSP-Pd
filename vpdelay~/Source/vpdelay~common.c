/* Common header files ********************************************************/
#include "vpdelay~common.h"

/* The common 'new instance' routine ******************************************/
void *vpdelay_common_new(t_vpdelay *x, short argc, t_atom *argv)
{
#ifdef TARGET_IS_MAX
	/* Create signal inlets */
	dsp_setup((t_pxobject *)x, NUM_INLETS);
	
	/* Create signal outlets */
	outlet_new((t_object *)x, "signal");
	
	/* Avoid sharing memory among audio vectors */
	x->obj.z_misc |= Z_NO_INPLACE;
#elif TARGET_IS_PD
	/* Create signal inlets */
	inlet_new(&x->obj, &x->obj.ob_pd, gensym("signal"), gensym("signal"));
	inlet_new(&x->obj, &x->obj.ob_pd, gensym("signal"), gensym("signal"));
	
	/* Create signal outlets */
	outlet_new(&x->obj, gensym("signal"));
#endif

	/* Initialize input arguments */
	float max_delay = DEFAULT_MAX_DELAY;
	float delay = DEFAULT_DELAY;
	float feedback = DEFAULT_FEEDBACK;
	
	/* Parse arguments passed from object */
	if (argc > A_FEEDBACK) { feedback = atom_getfloatarg(A_FEEDBACK, argc, argv); }
	if (argc > A_DELAY) { delay = atom_getfloatarg(A_DELAY, argc, argv); }
	if (argc > A_MAX_DELAY) { max_delay = atom_getfloatarg(A_MAX_DELAY, argc, argv); }
	
	/* Check validity of passed arguments */
	if (max_delay < MINIMUM_MAX_DELAY) {
		max_delay = MINIMUM_MAX_DELAY;
		post("vpdelay~ • Invalid argument: Maximum delay time set to %.4f[ms]", max_delay);
	}
	else if (max_delay > MAXIMUM_MAX_DELAY) {
		max_delay = MAXIMUM_MAX_DELAY;
		post("vpdelay~ • Invalid argument: Maximum delay time set to %.4f[ms]", max_delay);
	}
	
	if (delay < MINIMUM_DELAY) {
		delay = MINIMUM_DELAY;
		post("vpdelay~ • Invalid argument: Delay time set to %.4f[ms]", delay);
	}
	else if (delay > MAXIMUM_DELAY) {
		delay = MAXIMUM_DELAY;
		post("vpdelay~ • Invalid argument: Delay time set to %.4f[ms]", delay);
	}
	
	if (feedback < MINIMUM_FEEDBACK) {
		feedback = MINIMUM_FEEDBACK;
		post("vpdelay~ • Invalid argument: Feedback factor set to %.4f", feedback);
	}
	else if (feedback > MAXIMUM_FEEDBACK) {
		feedback = MAXIMUM_FEEDBACK;
		post("vpdelay~ • Invalid argument: Feedback factor set to %.4f", feedback);
	}
	
	/* Initialize state variables */
	x->max_delay = max_delay;
	x->delay = delay;
	x->feedback = feedback;
	
	x->fs = sys_getsr();
	
	x->delay_length = (x->max_delay * 1e-3 * x->fs) + 1;
	x->delay_bytes = x->delay_length * sizeof(float);
	
#ifdef TARGET_IS_MAX
	x->delay_line = (float *)sysmem_newptr(x->delay_bytes);
#elif TARGET_IS_PD
	x->delay_line = (float *)getbytes(x->delay_bytes);
#endif
	
	if (x->delay_line == NULL) {
		post("vpdelay~ • Cannot allocate memory for this object");
		return NULL;
	}
	
	for (int ii = 0; ii < x->delay_length; ii++) {
		x->delay_line[ii] = 0.0;
	}
	
    x->write_ptr = x->delay_line;
    x->read_ptr = x->delay_line;
    x->end_ptr = x->delay_line + x->delay_length;
	
	/* Print message to Max window */
	post("vpdelay~ • Object was created");
	
	/* Return a pointer to the new object */
	return x;
}

/* The 'free instance' routine ************************************************/
void vpdelay_free(t_vpdelay *x)
{
#ifdef TARGET_IS_MAX
	/* Remove the object from the DSP chain */
	dsp_free((t_pxobject *)x);
	
	/* Free allocated dynamic memory */
	sysmem_freeptr(x->delay_line);
#elif TARGET_IS_PD
	/* Free allocated dynamic memory */
	freebytes(x->delay_line, x->delay_bytes);
#endif
	
	/* Print message to Max window */
	post("vpdelay~ • Memory was freed");
}

/* The 'DSP' method ***********************************************************/
void vpdelay_dsp(t_vpdelay *x, t_signal **sp, short *count)
{
	/* Store signal connection states of inlets */
	#ifdef TARGET_IS_MAX
		x->delay_connected = count[A_DELAY];
		x->feedback_connected = count[A_FEEDBACK];
	#elif TARGET_IS_PD
		x->delay_connected = 1;
		x->feedback_connected = 1;
	#endif

	/* Adjust to changes in the sampling rate */
	if (x->fs != sp[0]->s_sr) {
		x->fs = sp[0]->s_sr;
		
		x->delay_length = (x->max_delay * 1e-3 * x->fs) + 1;
		#ifdef TARGET_IS_MAX
			x->delay_bytes = x->delay_length * sizeof(float);
			x->delay_line = (float *)sysmem_resizeptr((void *)x->delay_line, x->delay_bytes);
		#elif TARGET_IS_PD
			long delay_bytes_old = x->delay_bytes;
			x->delay_bytes = x->delay_length * sizeof(float);
			x->delay_line = (float *)resizebytes((void *)x, delay_bytes_old, x->delay_bytes);
		#endif
		
		if (x->delay_line == NULL) {
			post("vpdelay~ • Cannot reallocate memory for this object");
			return;
		}
		
		for (int ii = 0; ii < x->delay_length; ii++) {
			x->delay_line[ii] = 0.0;
		}

        x->write_ptr = x->delay_line;
        x->read_ptr = x->delay_line;
        x->end_ptr = x->delay_line + x->delay_length;
	}
	
	/* Attach the object to the DSP chain */
	dsp_add(vpdelay_perform, NEXT-1, x,
			sp[0]->s_vec,
			sp[1]->s_vec,
			sp[2]->s_vec,
			sp[3]->s_vec,
			sp[0]->s_n);
	
	/* Print message to Max window */
	post("vpdelay~ • Executing 32-bit perform routine");
}

/* The 'perform' routine ******************************************************/
t_int *vpdelay_perform(t_int *w)
{
	/* Copy the object pointer */
	t_vpdelay *x = (t_vpdelay *)w[OBJECT];
	
	/* Copy signal pointers */
	t_float *input = (t_float *)w[INPUT1];
	t_float *delay = (t_float *)w[DELAY];
	t_float *feedback = (t_float *)w[FEEDBACK];
	t_float *output = (t_float *)w[OUTPUT1];
	
	/* Copy the signal vector size */
	t_int n = w[VECTOR_SIZE];
	
	/* Load state variables */
	float delay_float = x->delay;
	float feedback_float = x->feedback;
	float fsms = x->fs * 1e-3;
	long delay_length = x->delay_length;
	float *delay_line = x->delay_line;
    float *write_ptr = x->write_ptr;
    float *read_ptr = x->read_ptr;
    float *end_ptr = x->end_ptr;
	short delay_connected = x->delay_connected;
	short feedback_connected = x->feedback_connected;
	
	/* Perform the DSP loop */
	long delay_time;
	float fb;
	
	long idelay;
	float fraction;
	float samp1;
	float samp2;
	
	float feed_sample;
	float out_sample;

    // Calculations
    if (delay_connected) {
        delay_time = *delay * fsms;
    } else {
        delay_time = delay_float * fsms;
    }

    if (feedback_connected) {
        fb = *feedback;
    } else {
        fb = feedback_float;
    }

    idelay = (int)delay_time;
    fraction = delay_time - idelay;

    if (idelay < 0) {
        idelay = 0;
    }
    else if (idelay > delay_length) {
        idelay = delay_length - 1;
    }

    // Loop
	while (n--) {
        delay++;
        feedback++;
		
		read_ptr = write_ptr - idelay;
		while (read_ptr < delay_line) {
			read_ptr += delay_length;
		}
		
		if (read_ptr == write_ptr) {
			out_sample = *input++;
		} else {
			samp1 = *read_ptr++;
            if (read_ptr == end_ptr) {
                read_ptr = delay_line;
            }
			samp2 = *read_ptr;
			out_sample = samp1 + fraction * (samp2 - samp1);
			
			feed_sample = (*input++) + (out_sample * fb);
            FIX_DENORM(feed_sample);
			*write_ptr = feed_sample;
		}
		
		*output++ = out_sample;
		
		write_ptr++;
		if (write_ptr == end_ptr) {
			write_ptr = delay_line;
		}
	}
	
	/* Update state variables */
	x->write_ptr = write_ptr;
	
	/* Return the next address in the DSP chain */
	return w + NEXT;
}

/******************************************************************************/
