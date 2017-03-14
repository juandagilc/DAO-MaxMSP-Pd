#ifndef dynstoch_common_h
#define dynstoch_common_h

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
#include <time.h>

/* The global variables *******************************************************/
#define NUM_POINTS 12

#define INITIAL_FREQ 440.0
#define MINIMUM_FREQ 100.0
#define MAXIMUM_FREQ 800.0

#define MINIMUM_AMP_DEV 0.0
#define DEFAULT_AMP_DEV 0.001
#define MAXIMUM_AMP_DEV 2.0

#define MINIMUM_DUR_DEV 0.0
#define DEFAULT_DUR_DEV 0.001

/* The object structure *******************************************************/
typedef struct _dynstoch {
#ifdef TARGET_IS_MAX
    t_pxobject obj;
    float a_ampdev;
    float a_durdev;
    float a_freqrange[2];
    float a_newfreq;
#elif TARGET_IS_PD
    t_object obj;
    t_float x_f;
#endif

    int num_points;
    float *amplitudes;
    float *durations;

    float sr;
    float freq;

    float min_freq;
    float max_freq;
    float min_duration;
    float max_duration;

    float amplitude_deviation;
    float duration_deviation;

    int current_segment;
    int remaining_samples;

    int total_length;
    short first_time;
} t_dynstoch;

/* The arguments/inlets/outlets/vectors indexes *******************************/
enum ARGUMENTS { NONE };
enum INLETS { I_INPUT, NUM_INLETS };
enum OUTLETS { O_OUTPUT, O_FREQUENCY, NUM_OUTLETS };
enum DSP { PERFORM, OBJECT,
           INPUT1, OUTPUT1, FREQUENCY,
           VECTOR_SIZE, NEXT };

/* The class pointer **********************************************************/
static t_class *dynstoch_class;

/* Function prototypes ********************************************************/
void *dynstoch_common_new(t_dynstoch *x, short argc, t_atom *argv);
void dynstoch_free(t_dynstoch *x);

void dynstoch_dsp(t_dynstoch *x, t_signal **sp, short *count);
t_int *dynstoch_perform(t_int *w);

/* The object-specific methods ************************************************/
void dynstoch_ampdev(t_dynstoch *x, float ampdev);
void dynstoch_durdev(t_dynstoch *x, float durdev);
void dynstoch_setfreq(t_dynstoch *x, float new_freq);
void dynstoch_freqrange(t_dynstoch *x, float min_freq, float max_freq);

float dynstoch_rand(float min, float max);
void dynstoch_initwave(t_dynstoch *x);
void dynstoch_recalculate(t_dynstoch *x);

/******************************************************************************/

#endif /* dynstoch_common_h */
