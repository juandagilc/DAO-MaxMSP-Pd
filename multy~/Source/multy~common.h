#ifndef multy_common_h
#define multy_common_h

/* Header files required by Max and Pd ****************************************/
#ifdef TARGET_IS_MAX
#include "ext.h"
#include "z_dsp.h"
#include "ext_obex.h"
#elif TARGET_IS_PD
#include "m_pd.h"
#endif

/* The global variables *******************************************************/
// None

/* The object structure *******************************************************/
typedef struct _multy {
#ifdef TARGET_IS_MAX
	t_pxobject obj;
#elif TARGET_IS_PD
	t_object obj;
	t_float x_f;
#endif
} t_multy;

/* The class pointer **********************************************************/
static t_class *multy_class;

/* Function prototypes ********************************************************/
void multy_dsp(t_multy *x, t_signal **sp, short *count);
t_int *multy_perform(t_int *w);

/******************************************************************************/

#endif /* multy_common_h */
