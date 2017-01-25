#ifndef multy64_common_h
#define multy64_common_h

/* Header files required by Max and Pd ****************************************/
#ifdef TARGET_IS_MAX
#include "ext.h"
#include "z_dsp.h"
#include "ext_obex.h"
#elif TARGET_IS_PD
#include "m_pd.h"
#include "math.h"
#endif

/* The object structure *******************************************************/
typedef struct _multy64 {
#ifdef TARGET_IS_MAX
    t_pxobject obj;
#elif TARGET_IS_PD
    t_object obj;
    t_float x_f;
#endif
} t_multy64;

/* The arguments/inlets/outlets/vectors indexes *******************************/
enum ARGUMENTS { NONE };
enum INLETS { I_REAL, I_IMAG, NUM_INLETS };
enum OUTLETS { O_MAGN, O_PHASE, NUM_OUTLETS };
enum DSP { PERFORM, OBJECT,
           INPUT_REAL, INPUT_IMAG, OUTPUT_MAGN, OUTPUT_PHASE,
           VECTOR_SIZE, NEXT };

/* The class pointer **********************************************************/
static t_class *multy64_class;

/* Function prototypes ********************************************************/
void *common_new(t_multy64 *x, short argc, t_atom *argv);
void multy64_free(t_multy64 *x);

void multy64_dsp(t_multy64 *x, t_signal **sp, short *count);
t_int *multy64_perform(t_int *w);

/******************************************************************************/

#endif /* multy64_common_h */
