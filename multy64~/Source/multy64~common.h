#ifndef multy64_common_h
#define multy64_common_h

/* Header files required by Max and Pd ****************************************/
#ifdef TARGET_IS_MAX
#include "ext.h"
#include "ext_obex.h"
#include "z_dsp.h"
#include "z_sampletype.h"
#elif TARGET_IS_PD
#include "m_pd.h"
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
enum INLETS { I_INPUT1, I_INPUT2, NUM_INLETS };
enum OUTLETS { O_OUTPUT, NUM_OUTLETS };
enum DSP { PERFORM, OBJECT,
           INPUT1, INPUT2, OUTPUT,
           VECTOR_SIZE, NEXT };

/* The class pointer **********************************************************/
static t_class *multy64_class;

/* Function prototypes ********************************************************/
void *multy64_common_new(t_multy64 *x, short argc, t_atom *argv);
void multy64_free(t_multy64 *x);

void multy64_dsp(t_multy64 *x, t_signal **sp, short *count);
t_int *multy64_perform(t_int *w);

#ifdef TARGET_IS_MAX
void multy64_dsp64(t_multy64 *x, t_object *dsp64,
                   short *count,
                   double samplerate,
                   long maxvectorsize,
                   long flags);
void multy64_perform64(t_multy64 *x, t_object *dsp64,
                       double **ins, long numins,
                       double **outs, long numouts,
                       long vectorsize,
                       long flags,
                       void *userparams);
#endif

/******************************************************************************/

#endif /* multy64_common_h */
