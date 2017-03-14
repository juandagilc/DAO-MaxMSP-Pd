#ifndef vpdelay_common_h
#define vpdelay_common_h

/* Header files required by Max and Pd ****************************************/
#ifdef TARGET_IS_MAX
#include "ext.h"
#include "z_dsp.h"
#include "ext_obex.h"
#elif TARGET_IS_PD
#include "m_pd.h"
#endif

#include <math.h>

/* Borrowed macros ************************************************************/
#define IS_DENORM(v)  ((((*(unsigned long *)&(v))&0x7f800000)==0)&&((v)!=0.f))
#define FIX_DENORM(v) ((v)=IS_DENORM(v)?0.f:(v))

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
typedef struct _vpdelay {
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
	
	float *write_ptr;
	float *read_ptr;
    float *end_ptr;
	
	short delay_connected;
	short feedback_connected;
} t_vpdelay;

/* The arguments/inlets/outlets/vectors indexes *******************************/
enum ARGUMENTS { A_MAX_DELAY, A_DELAY, A_FEEDBACK };
enum INLETS { I_INPUT, I_DELAY, I_FEEDBACK, NUM_INLETS};
enum OUTLETS { O_OUTPUT, NUM_OUTLETS };
enum DSP { PERFORM, OBJECT, INPUT1, DELAY, FEEDBACK, OUTPUT1, VECTOR_SIZE, NEXT };

/* The class pointer **********************************************************/
static t_class *vpdelay_class;

/* The function prototypes ****************************************************/
void *vpdelay_common_new(t_vpdelay *x, short argc, t_atom *argv);
void vpdelay_free(t_vpdelay *x);

void vpdelay_dsp(t_vpdelay *x, t_signal **sp, short *count);
t_int *vpdelay_perform(t_int *w);

/******************************************************************************/

#endif /* vpdelay_common_h */
