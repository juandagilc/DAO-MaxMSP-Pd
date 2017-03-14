#ifndef windowvec_common_h
#define windowvec_common_h

/* Header files required by Max and Pd ****************************************/
#ifdef TARGET_IS_MAX
#include "ext.h"
#include "z_dsp.h"
#include "ext_obex.h"
#elif TARGET_IS_PD
#include "m_pd.h"
#endif

#include <math.h>
#include <stdlib.h>

/* The global variables *******************************************************/
#define TWOPI 6.2831853071796

/* The object structure *******************************************************/
typedef struct _windowvec {
#ifdef TARGET_IS_MAX
    t_pxobject obj;
#elif TARGET_IS_PD
    t_object obj;
    t_float x_f;
#endif

    float fs;

    float* window;
    int vecsize;
} t_windowvec;

/* The arguments/inlets/outlets/vectors indexes *******************************/
enum ARGUMENTS { NONE };
enum INLETS { I_INPUT, NUM_INLETS };
enum OUTLETS { O_OUTPUT, NUM_OUTLETS };
enum DSP { PERFORM, OBJECT,
           INPUT1, OUTPUT1,
           VECTOR_SIZE, NEXT };

/* The class pointer **********************************************************/
static t_class *windowvec_class;

/* Function prototypes ********************************************************/
void *windowvec_common_new(t_windowvec *x, short argc, t_atom *argv);
void windowvec_free(t_windowvec *x);

void windowvec_dsp(t_windowvec *x, t_signal **sp, short *count);
t_int *windowvec_perform(t_int *w);

/******************************************************************************/

#endif /* windowvec_common_h */
