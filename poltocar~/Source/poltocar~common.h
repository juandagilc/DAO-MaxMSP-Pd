#ifndef poltocar_common_h
#define poltocar_common_h

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
typedef struct _poltocar {
#ifdef TARGET_IS_MAX
    t_pxobject obj;
#elif TARGET_IS_PD
    t_object obj;
    t_float x_f;
#endif
} t_poltocar;

/* The arguments/inlets/outlets/vectors indexes *******************************/
enum ARGUMENTS { NONE };
enum INLETS { I_MAGN, I_PHASE, NUM_INLETS };
enum OUTLETS { O_REAL, O_IMAG, NUM_OUTLETS };
enum DSP { PERFORM, OBJECT,
           INPUT_MAGN, INPUT_PHASE, OUTPUT_REAL, OUTPUT_IMAG,
           VECTOR_SIZE, NEXT };

/* The class pointer **********************************************************/
static t_class *poltocar_class;

/* Function prototypes ********************************************************/
void *poltocar_common_new(t_poltocar *x, short argc, t_atom *argv);
void poltocar_free(t_poltocar *x);

void poltocar_dsp(t_poltocar *x, t_signal **sp, short *count);
t_int *poltocar_perform(t_int *w);

/******************************************************************************/

#endif /* poltocar_common_h */
