#ifndef moogvcf_common_h
#define moogvcf_common_h

/* Header files required by Max and Pd ****************************************/
#ifdef TARGET_IS_MAX
#include "ext.h"
#include "z_dsp.h"
#include "ext_obex.h"
#elif TARGET_IS_PD
#include "m_pd.h"
#include "math.h"
#endif

#include <stdlib.h>

/* The global variables *******************************************************/
#define TWOPI 6.2831853071796

/* The object structure *******************************************************/
typedef struct _moogvcf {
#ifdef TARGET_IS_MAX
    t_pxobject obj;
#elif TARGET_IS_PD
    t_object obj;
    t_float x_f;
#endif

    float fs;

    float* window;
    int vecsize;
} t_moogvcf;

/* The arguments/inlets/outlets/vectors indexes *******************************/
enum ARGUMENTS { NONE };
enum INLETS { I_INPUT, NUM_INLETS };
enum OUTLETS { O_OUTPUT, NUM_OUTLETS };
enum DSP { PERFORM, OBJECT,
           INPUT, OUTPUT,
           VECTOR_SIZE, NEXT };

/* The class pointer **********************************************************/
static t_class *moogvcf_class;

/* Function prototypes ********************************************************/
void *common_new(t_moogvcf *x, short argc, t_atom *argv);
void moogvcf_free(t_moogvcf *x);

void moogvcf_dsp(t_moogvcf *x, t_signal **sp, short *count);
t_int *moogvcf_perform(t_int *w);

/******************************************************************************/

#endif /* moogvcf_common_h */
