#ifndef oscil_attributes_common_h
#define oscil_attributes_common_h

/* Header files required by Max and Pd ****************************************/
#ifdef TARGET_IS_MAX
#include "ext.h"
#include "z_dsp.h"
#include "ext_obex.h"
#elif TARGET_IS_PD
#include "m_pd.h"
#endif

#include <math.h>

/* The global variables *******************************************************/
#define MINIMUM_FREQUENCY 31.0
#define DEFAULT_FREQUENCY 300.0
#define MAXIMUM_FREQUENCY 8000.0

#define MINIMUM_TABLE_SIZE 4
#define DEFAULT_TABLE_SIZE 8192
#define MAXIMUM_TABLE_SIZE 1048576

#define DEFAULT_WAVEFORM "sine"

#define MINIMUM_HARMONICS 2
#define DEFAULT_HARMONICS 10
#define MAXIMUM_HARMONICS 1024

#define MINIMUM_CROSSFADE 0.0
#define DEFAULT_CROSSFADE 200.0
#define MAXIMUM_CROSSFADE 1000.0

#define NO_CROSSFADE 0
#define LINEAR_CROSSFADE 1
#define POWER_CROSSFADE 2

/* The object structure *******************************************************/
typedef struct _oscil_attributes {
#ifdef TARGET_IS_MAX
	t_pxobject obj;
    float a_frequency;
    long a_crossfade_type;
    float a_crossfade_time;
    t_symbol *a_waveform;
    long a_harmonics;
    t_float *a_amplitudes;
#elif TARGET_IS_PD
	t_object obj;
	t_float x_f;
#endif

    short frequency_connected;
    
    float frequency;
    long table_size;
    t_symbol *waveform;
    long harmonics;
    
    long harmonics_bl;
    
    long wavetable_bytes;
    float *wavetable;
    float *wavetable_old;
    long amplitudes_bytes;
    float *amplitudes;
    
    float fs;
    
    float phase;
    float increment;
    short dirty;
    
    short crossfade_type;
    float crossfade_time;
    long crossfade_samples;
    long crossfade_countdown;
    short crossfade_in_progress;
    short just_turned_on;
    
    float twopi;
    float piOtwo;
} t_oscil_attributes;

/* The arguments/inlets/outlets/vectors indexes *******************************/
enum ARGUMENTS { A_FREQUENCY, A_TABLE_SIZE, A_WAVEFORM, A_HARMONICS };
enum INLETS { I_FREQUENCY, NUM_INLETS };
enum OUTLETS { O_OUTPUT, NUM_OUTLETS };
enum DSP { PERFORM, OBJECT, FREQUENCY, OUTPUT, VECTOR_SIZE, NEXT };

/* The class pointer **********************************************************/
static t_class *oscil_attributes_class;

/* Function prototypes ********************************************************/
void *oscil_attributes_common_new(t_oscil_attributes *x, short argc, t_atom *argv);
void oscil_attributes_free(t_oscil_attributes *x);

void oscil_attributes_dsp(t_oscil_attributes *x, t_signal **sp, short *count);
t_int *oscil_attributes_perform(t_int *w);

void oscil_attributes_build_wavetable(t_oscil_attributes *x);
void oscil_attributes_build_sine(t_oscil_attributes *x);
void oscil_attributes_build_sawtooth(t_oscil_attributes *x);
void oscil_attributes_build_triangle(t_oscil_attributes *x);
void oscil_attributes_build_square(t_oscil_attributes *x);
void oscil_attributes_build_pulse(t_oscil_attributes *x);
void oscil_attributes_build_additive(t_oscil_attributes *x);
void oscil_attributes_build_list(t_oscil_attributes *x, t_symbol *msg, short argc, t_atom *argv);
void oscil_attributes_build_waveform(t_oscil_attributes *x);
void oscil_attributes_fadetime(t_oscil_attributes *x, t_symbol *msg, short argc, t_atom *argv);
void oscil_attributes_fadetype(t_oscil_attributes *x, t_symbol *msg, short argc, t_atom *argv);

/******************************************************************************/

#endif /* oscil_attributes_common_h */
