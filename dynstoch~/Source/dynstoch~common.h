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

#include <stdlib.h>

/* The object structure *******************************************************/
typedef struct _dynstoch {
#ifdef TARGET_IS_MAX
    t_pxobject obj;
#elif TARGET_IS_PD
    t_object obj;
    t_float x_f;
#endif

    double onedsr;
    double xnm1;
    double y1nm1;
    double y2nm1;
    double y3nm1;
    double y1n;
    double y2n;
    double y3n;
    double y4n;
} t_dynstoch;

/* The arguments/inlets/outlets/vectors indexes *******************************/
enum ARGUMENTS { NONE };
enum INLETS { I_INPUT, I_FREQUENCY, I_RESONANCE, NUM_INLETS };
enum OUTLETS { O_OUTPUT, NUM_OUTLETS };
enum DSP { PERFORM, OBJECT,
           INPUT, FREQUENCY, RESONANCE, OUTPUT,
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
