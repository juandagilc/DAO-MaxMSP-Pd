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
#define MAXIMUM_SEQUENCE_LENGTH 1024
#define DEFAULT_SEQUENCE_LENGTH 3

#define F0 440
#define F1 550
#define F2 660

#define DEFAULT_NOTE_DURATION_MS 250

/* The object structure *******************************************************/
typedef struct _retroseq {
#ifdef TARGET_IS_MAX
	t_pxobject obj;
#elif TARGET_IS_PD
	t_object obj;
	t_float x_f;
#endif
    float fs;

    int sequence_bytes;
    float *sequence;
    int sequence_length;

    float note_duration_ms;
    int note_duration_samples;
    int sample_counter;
    int note_counter;
} t_retroseq;

/* The arguments/inlets/outlets/vectors indexes *******************************/
enum ARGUMENTS { A_NONE };
enum INLETS { NUM_INLETS };
enum OUTLETS { O_OUTPUT, NUM_OUTLETS };
enum DSP { PERFORM,
           OBJECT, OUTPUT, VECTOR_SIZE,
           NEXT };

/* The class pointer **********************************************************/
static t_class *retroseq_class;

/* Function prototypes ********************************************************/
void *common_new(t_retroseq *x, short argc, t_atom *argv);
void retroseq_free(t_retroseq *x);

void retroseq_dsp(t_retroseq *x, t_signal **sp, short *count);
t_int *retroseq_perform(t_int *w);

/* The object-specific prototypes *********************************************/
//nothing

/******************************************************************************/

#endif /* retroseq_common_h */
