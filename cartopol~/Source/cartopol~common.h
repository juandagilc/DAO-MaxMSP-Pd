#ifndef cartopol_common_h
#define cartopol_common_h

/* Header files required by Max and Pd ****************************************/
#ifdef TARGET_IS_MAX
#include "ext.h"
#include "z_dsp.h"
#include "ext_obex.h"
#elif TARGET_IS_PD
#include "m_pd.h"
#endif

#include <math.h>

/* The object structure *******************************************************/
typedef struct _cartopol {
#ifdef TARGET_IS_MAX
    t_pxobject obj;
#elif TARGET_IS_PD
    t_object obj;
    t_float x_f;
#endif
} t_cartopol;

/* The arguments/inlets/outlets/vectors indexes *******************************/
enum ARGUMENTS { NONE };
enum INLETS { I_REAL, I_IMAG, NUM_INLETS };
enum OUTLETS { O_MAGN, O_PHASE, NUM_OUTLETS };
enum DSP { PERFORM, OBJECT,
           INPUT_REAL, INPUT_IMAG, OUTPUT_MAGN, OUTPUT_PHASE,
           VECTOR_SIZE, NEXT };

/* The class pointer **********************************************************/
static t_class *cartopol_class;

/* Function prototypes ********************************************************/
void *cartopol_common_new(t_cartopol *x, short argc, t_atom *argv);
void cartopol_free(t_cartopol *x);

void cartopol_dsp(t_cartopol *x, t_signal **sp, short *count);
t_int *cartopol_perform(t_int *w);

/******************************************************************************/

#endif /* cartopol_common_h */
