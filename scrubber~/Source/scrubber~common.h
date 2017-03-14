#ifndef scrubber_common_h
#define scrubber_common_h

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
#include <string.h>

/* The global variables *******************************************************/
#define MINIMUM_DURATION 000.0
#define DEFAULT_DURATION 5000.0
#define MAXIMUM_DURATION 10000.0

#define SCRUBBER_EMPTY 0
#define SCRUBBER_FULL 1

#define PI 3.1415926535898
#define TWOPI 6.2831853071796

/* The object structure *******************************************************/
typedef struct _scrubber {
#ifdef TARGET_IS_MAX
    t_pxobject obj;
#elif TARGET_IS_PD
    t_object obj;
    t_float x_f;
#endif

    float fs;
    
    float** amplitudes;
    float** phasediffs;
    float* last_phase_in;
    float* last_phase_out;

    float duration_ms;
    float overlap;
    long fftsize;
    long framecount;
    long old_framecount;

    long recording_frame;
    float playback_frame;
    float last_position;

    short acquire_sample;
    short buffer_status;
} t_scrubber;

/* The arguments/inlets/outlets/vectors indexes *******************************/
enum ARGUMENTS { A_DURATION };
enum INLETS { I_REAL, I_IMAG, I_SPEED, I_POSITION, NUM_INLETS };
enum OUTLETS { O_REAL, O_IMAG, NUM_OUTLETS };
enum DSP { PERFORM, OBJECT,
           INPUT_REAL, INPUT_IMAG, SPEED, POSITION, OUTPUT_REAL, OUTPUT_IMAG, SYNC,
           VECTOR_SIZE, NEXT };

/* The class pointer **********************************************************/
static t_class *scrubber_class;

/* Function prototypes ********************************************************/
void *scrubber_common_new(t_scrubber *x, short argc, t_atom *argv);
void scrubber_free(t_scrubber *x);

void scrubber_dsp(t_scrubber *x, t_signal **sp, short *count);
t_int *scrubber_perform(t_int *w);

/* The object-specific prototypes *********************************************/
void scrubber_init_memory(t_scrubber *x);
void scrubber_sample(t_scrubber *x);
void scrubber_overlap(t_scrubber *x, t_symbol *msg, short argc, t_atom *argv);
void scrubber_resize(t_scrubber *x, t_symbol *msg, short argc, t_atom *argv);

/******************************************************************************/

#endif /* scrubber_common_h */
