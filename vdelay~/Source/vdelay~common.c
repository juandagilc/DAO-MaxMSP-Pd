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
	t_float *in1 = (t_float *)w[INPUT1_VECTOR];
	t_float *in2 = (t_float *)w[INPUT2_VECTOR];
	t_float *out = (t_float *)w[OUTPUT_VECTOR];
	
	/* Copy the signal vector size */
	t_int n = w[VECTOR_SIZE];
	
	/* Perform the DSP loop */
	while (n--) {
		*out++ = *in1++ * *in2++;
	}
	
	/* Return the next address in the DSP chain */
	return w + NEXT;
}

/******************************************************************************/
