#ifndef vdelay_common_h
#define vdelay_common_h

/* Header files required by Max and Pd ****************************************/
#ifdef TARGET_IS_MAX
#include "ext.h"
#include "z_dsp.h"
#include "ext_obex.h"
#elif TARGET_IS_PD
#include "m_pd.h"
#endif

#include <math.h>

/* The global variables *******************************************************/
#define MINIMUM_MAX_DELAY 0.0
#define DEFAULT_MAX_DELAY 1000.0
#define MAXIMUM_MAX_DELAY 10000.0

#define MINIMUM_DELAY 0.0
#define DEFAULT_DELAY 100.0
#define MAXIMUM_DELAY MAXIMUM_MAX_DELAY

#define MINIMUM_FEEDBACK 0.0
#define DEFAULT_FEEDBACK 0.3
#define MAXIMUM_FEEDBACK 0.9999

/* The object structure *******************************************************/
typedef struct _vdelay {
#ifdef TARGET_IS_MAX
	t_pxobject obj;
#elif TARGET_IS_PD
	t_object obj;
	t_float x_f;
#endif

	float max_delay;
	float delay;
	float feedback;
	
	float fs;
	
	long delay_length;
	long delay_bytes;
	float *delay_line;
	
	long write_idx;
	long read_idx;
	
	short delay_connected;
	short feedback_connected;
} t_vdelay;

/* The arguments/inlets/outlets/vectors indexes *******************************/
enum ARGUMENTS { A_MAX_DELAY, A_DELAY, A_FEEDBACK };
enum INLETS { I_INPUT, I_DELAY, I_FEEDBACK, NUM_INLETS};
enum OUTLETS { O_OUTPUT, NUM_OUTLETS };
enum DSP { PERFORM, OBJECT, INPUT1, DELAY, FEEDBACK, OUTPUT1, VECTOR_SIZE, NEXT };

/* The class pointer **********************************************************/
static t_class *vdelay_class;

/* Function prototypes ********************************************************/
void *vdelay_common_new(t_vdelay *x, short argc, t_atom *argv);
void vdelay_free(t_vdelay *x);

void vdelay_dsp(t_vdelay *x, t_signal **sp, short *count);
t_int *vdelay_perform(t_int *w);

/******************************************************************************/

#endif /* vdelay_common_h */
