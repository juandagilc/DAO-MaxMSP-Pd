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

/* The global variables *******************************************************/
#define MINIMUM_MAX_DELAY_TIME 00000.0
#define DEFAULT_MAX_DELAY_TIME 01000.0
#define MAXIMUM_MAX_DELAY_TIME 10000.0

#define MINIMUM_DELAY_TIME 000.0
#define DEFAULT_DELAY_TIME 100.0
#define MAXIMUM_DELAY_TIME MAXIMUM_MAX_DELAY_TIME

#define MINIMUM_FEEDBACK 0.0000
#define DEFAULT_FEEDBACK 0.3000
#define MAXIMUM_FEEDBACK 0.9999

/* The object structure *******************************************************/
typedef struct _vdelay {
#ifdef TARGET_IS_MAX
	t_pxobject obj;
#elif TARGET_IS_PD
	t_object obj;
	t_float x_f;
#endif

	float max_delay_time;
	float delay_time;
	float feedback;
	
	float fs;
	
	long delay_length;
	long delay_bytes;
	float *delay_line;
	
	long write_idx;
	long read_idx;
	
	short delay_time_connected;
	short feedback_connected;
} t_vdelay;

/* The class pointer **********************************************************/
static t_class *vdelay_class;

/* Function prototypes ********************************************************/
void vdelay_dsp(t_vdelay *x, t_signal **sp, short *count);
t_int *vdelay_perform(t_int *w);

/******************************************************************************/

#endif /* vdelay_common_h */
