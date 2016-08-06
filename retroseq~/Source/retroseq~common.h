#ifndef retroseq_common_h
#define retroseq_common_h

/* Header files required by Max and Pd ****************************************/
#ifdef TARGET_IS_MAX
#include "ext.h"
#include "z_dsp.h"
#include "ext_obex.h"
#elif TARGET_IS_PD
#include "m_pd.h"
#endif

#include "math.h"

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
typedef struct _retroseq {
#ifdef TARGET_IS_MAX
	t_pxobject obj;
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
} t_retroseq;

/* The arguments/inlets/outlets/vectors indexes *******************************/
enum ARGUMENTS { A_FREQUENCY, A_TABLE_SIZE, A_WAVEFORM, A_HARMONICS };
enum INLETS { I_FREQUENCY, NUM_INLETS };
enum OUTLETS { O_OUTPUT, NUM_OUTLETS };
enum DSP { PERFORM, OBJECT, FREQUENCY, OUTPUT, VECTOR_SIZE, NEXT };

/* The class pointer **********************************************************/
static t_class *retroseq_class;

/* Function prototypes ********************************************************/
void *common_new(t_retroseq *x, short argc, t_atom *argv);
void retroseq_free(t_retroseq *x);

void retroseq_dsp(t_retroseq *x, t_signal **sp, short *count);
t_int *retroseq_perform(t_int *w);

void retroseq_build_sine(t_retroseq *x);
void retroseq_build_sawtooth(t_retroseq *x);
void retroseq_build_triangle(t_retroseq *x);
void retroseq_build_square(t_retroseq *x);
void retroseq_build_pulse(t_retroseq *x);
void retroseq_build_list(t_retroseq *x, t_symbol *msg, short argc, t_atom *argv);
void retroseq_build_waveform(t_retroseq *x);
void retroseq_fadetime(t_retroseq *x, t_symbol *msg, short argc, t_atom *argv);
void retroseq_fadetype(t_retroseq *x, t_symbol *msg, short argc, t_atom *argv);

/******************************************************************************/

#endif /* retroseq_common_h */
