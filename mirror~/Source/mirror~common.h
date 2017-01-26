#ifndef mirror_common_h
#define mirror_common_h

/* Header files required by Max and Pd ****************************************/
#ifdef TARGET_IS_MAX
#include "ext.h"
#include "z_dsp.h"
#include "ext_obex.h"
#elif TARGET_IS_PD
#include "m_pd.h"
#endif

/* The global variables *******************************************************/
#define GAIN 0.1

/* The object structure *******************************************************/
typedef struct _mirror {
#ifdef TARGET_IS_MAX
	t_pxobject obj;
#elif TARGET_IS_PD
	t_object obj;
	t_float x_f;
#endif
} t_mirror;

/* The class pointer **********************************************************/
static t_class *mirror_class;

/* Function prototypes ********************************************************/
void mirror_dsp(t_mirror *x, t_signal **sp, short *count);
t_int *mirror_perform(t_int *w);

/******************************************************************************/

#endif /* mirror_common_h */
