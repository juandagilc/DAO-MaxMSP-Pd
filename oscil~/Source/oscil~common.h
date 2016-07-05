#ifndef oscil_common_h
#define oscil_common_h

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

/* The object structure *******************************************************/
typedef struct _oscil {
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
    long amplitudes_bytes;
    float *amplitudes;
    
    float fs;
    
    float phase;
    float increment;
    
    float twopi;
    float piOtwo;
} t_oscil;

/* The arguments/inlets/outlets/vectors indexes *******************************/
enum ARGUMENTS { A_FREQUENCY, A_TABLE_SIZE, A_WAVEFORM, A_HARMONICS };
enum INLETS { I_FREQUENCY, NUM_INLETS };
enum OUTLETS { O_OUTPUT, NUM_OUTLETS };
enum DSP { PERFORM, OBJECT, FREQUENCY, OUTPUT, VECTOR_SIZE, NEXT };

/* The class pointer **********************************************************/
static t_class *oscil_class;

/* Function prototypes ********************************************************/
void *common_new(t_oscil *x, short argc, t_atom *argv);
void oscil_free(t_oscil *x);

void oscil_dsp(t_oscil *x, t_signal **sp, short *count);
t_int *oscil_perform(t_int *w);

void oscil_build_sine(t_oscil *x);
void oscil_build_sawtooth(t_oscil *x);
void oscil_build_triangle(t_oscil *x);
void oscil_build_square(t_oscil *x);
void oscil_build_pulse(t_oscil *x);
void oscil_build_waveform(t_oscil *x);

/******************************************************************************/

#endif /* oscil_common_h */
