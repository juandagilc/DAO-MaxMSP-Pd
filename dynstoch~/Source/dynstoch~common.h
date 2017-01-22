#ifndef dynstoch_common_h
#define dynstoch_common_h

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
typedef struct _dynstoch {
#ifdef TARGET_IS_MAX
    t_pxobject obj;
#elif TARGET_IS_PD
    t_object obj;
    t_float x_f;
#endif

    
} t_dynstoch;

/* The arguments/inlets/outlets/vectors indexes *******************************/
enum ARGUMENTS { NONE };
enum INLETS { I_INPUT, NUM_INLETS };
enum OUTLETS { O_OUTPUT, NUM_OUTLETS };
enum DSP { PERFORM, OBJECT,
           INPUT, OUTPUT,
           VECTOR_SIZE, NEXT };

/* The class pointer **********************************************************/
static t_class *dynstoch_class;

/* Function prototypes ********************************************************/
void *common_new(t_dynstoch *x, short argc, t_atom *argv);
void dynstoch_free(t_dynstoch *x);

void dynstoch_dsp(t_dynstoch *x, t_signal **sp, short *count);
t_int *dynstoch_perform(t_int *w);

/******************************************************************************/

#endif /* dynstoch_common_h */
