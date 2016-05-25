/* Common header files ********************************************************/
#include "multy~common.h"

/* The 'DSP/perform' arguments list *******************************************/
enum DSP {PERFORM, OBJECT, INPUT1_VECTOR, INPUT2_VECTOR, OUTPUT_VECTOR, VECTOR_SIZE, NEXT};

/* The 'DSP' method ***********************************************************/
void multy_dsp(t_multy *x, t_signal **sp, short *count)
{
	/* Attach the object to the DSP chain */
	dsp_add(multy_perform, NEXT-1, x, sp[0]->s_vec, sp[1]->s_vec, sp[2]->s_vec, sp[0]->s_n);
	
	/* Print message to Max window */
	post("multy~ • Executing 32-bit perform routine");
}

/* The 'perform' routine ******************************************************/
t_int *multy_perform(t_int *w)
{
	/* Copy the object pointer */
	t_multy *x = (t_multy *)w[OBJECT];
	
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
