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
	t_float *delay_time = (t_float *)w[DELAY];
	t_float *feedback = (t_float *)w[FEEDBACK];
	t_float *output = (t_float *)w[OUTPUT];
	
	/* Copy the signal vector size */
	t_int n = w[VECTOR_SIZE];
	
	/* Load state variables */
	float fsms = x->fs * 1e-3;
	long delay_length = x->delay_length;
	float *delay_line = x->delay_line;
	long write_idx = x->write_idx;
	long read_idx = x->read_idx;
	
	/* Perform the DSP loop */
	long idelay;
	float out_sample;
	
	while (n--) {
		idelay = floorf(*delay_time++ * fsms);
		
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
		
		out_sample = delay_line[read_idx];
		delay_line[write_idx] = *input++ + out_sample * *feedback++;
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
