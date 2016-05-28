/* Common header files ********************************************************/
#include "vdelay~common.h"

/* The 'DSP/perform' arguments list *******************************************/
enum DSP {
	PERFORM, OBJECT,
	INPUT, DELAY, FEEDBACK, OUTPUT,
	VECTOR_SIZE, NEXT
};

/* The 'DSP' method ***********************************************************/
void vdelay_dsp(t_vdelay *x, t_signal **sp, short *count)
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
		
		#ifdef TARGET_IS_PD
			long old_bytes = x->delay_bytes;
		#endif
		x->delay_bytes = x->delay_length * sizeof(float);
		#ifdef TARGET_IS_MAX
			x->delay_line = (float *)sysmem_resizeptr((void *)x->delay_line, x->delay_bytes);
		#elif TARGET_IS_PD
			x->delay_line = (float *)resizebytes((void *)x, old_bytes, x->delay_bytes);
		#endif
		if (x->delay_line == NULL) {
			post("vdelay~ • Cannot reallocate memory for this object");
			return;
		}
		
		for (int ii = 0; ii < x->delay_length; ii++) {
			x->delay_line[ii] = 0.0;
		}
		
		x->write_idx = 0;
		x->read_idx = 0;
	}
	
	/* Attach the object to the DSP chain */
	dsp_add(vdelay_perform, NEXT-1, x,
			sp[0]->s_vec,
			sp[1]->s_vec,
			sp[2]->s_vec,
			sp[3]->s_vec,
			sp[3]->s_n);
	
	/* Print message to Max window */
	post("vdelay~ • Executing 32-bit perform routine");
}

/* The 'perform' routine ******************************************************/
t_int *vdelay_perform(t_int *w)
{
	/* Copy the object pointer */
	t_vdelay *x = (t_vdelay *)w[OBJECT];
	
	/* Copy signal pointers */
	t_float *input = (t_float *)w[INPUT];
	t_float *delay = (t_float *)w[DELAY];
	t_float *feedback = (t_float *)w[FEEDBACK];
	t_float *output = (t_float *)w[OUTPUT];
	
	/* Copy the signal vector size */
	t_int n = w[VECTOR_SIZE];
	
	/* Load state variables */
	float delay_float = x->delay;
	float feedback_float = x->feedback;
	float fsms = x->fs * 1e-3;
	long delay_length = x->delay_length;
	float *delay_line = x->delay_line;
	long write_idx = x->write_idx;
	long read_idx = x->read_idx;
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
	
	while (n--) {
		if (delay_connected) {
			delay_time = *delay++ * fsms;
		} else {
			delay_time = delay_float * fsms;
		}
		
		if (feedback_connected) {
			fb = *feedback++;
		} else {
			fb = feedback_float;
		}
		
		idelay = trunc(delay_time);
		fraction = delay_time - idelay;
		
		if (idelay < 0) {
			idelay = 0;
		}
		else if (idelay > delay_length) {
			idelay = delay_length - 1;
		}
		
		read_idx = write_idx - idelay;
		while (read_idx < 0) {
			read_idx += delay_length;
		}
		
		if (read_idx == write_idx) {
			out_sample = *input++;
		} else {
			samp1 = delay_line[ (read_idx+0)              ];
			samp2 = delay_line[ (read_idx+1)%delay_length ];
			out_sample = samp1 + fraction * (samp2 - samp1);
			
			feed_sample = (*input++) + (out_sample * fb);
			if (fabs(feed_sample) < 1e-6) {
				feed_sample = 0.0;
			}
			delay_line[write_idx] = feed_sample;
		}
		
		*output++ = out_sample;
		
		write_idx++;
		if (write_idx >= delay_length) {
			write_idx -= delay_length;
		}
	}
	
	/* Update state variables */
	x->write_idx = write_idx;
	
	/* Return the next address in the DSP chain */
	return w + NEXT;
}

/******************************************************************************/
