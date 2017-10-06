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

#include <time.h>
#include <stdlib.h>

/* The global variables *******************************************************/
#define MAXIMUM_SEQUENCE_LENGTH 1024
#define DEFAULT_SEQUENCE_LENGTH 3

#define F0 440
#define F1 550
#define F2 660

#define D0 250
#define D1 125
#define D2 125

#define DEFAULT_TEMPO_BPM 15

#define DEFAULT_SUSTAIN_AMPLITUDE 0.7
#define DEFAULT_ATACK_DURATION 20
#define DEFAULT_DECAY_DURATION 50
#define DEFAULT_SUSTAIN_DURATION 100
#define DEFAULT_RELEASE_DURATION 50

/* The object structure *******************************************************/
typedef struct _retroseq {
#ifdef TARGET_IS_MAX
    t_pxobject obj;
#elif TARGET_IS_PD
    t_object obj;
    t_float x_f;
#endif

    float fs;

    int max_sequence_bytes;

    float *note_sequence;
    int note_sequence_length;
    float current_note_value;
    int note_counter;

    float *duration_sequence;
    int duration_sequence_length;
    float current_duration_value;
    int duration_counter;

    void *shuffle_freqs_outlet;
    void *shuffle_durs_outlet;
    t_atom *shuffle_list;

    float tempo_bpm;
    float duration_factor;
    int sample_counter;

    void *bang_outlet;
    void *bang_clock;

    void *adsr_outlet;
    void *adsr_clock;

    short elastic_sustain;
    float sustain_amplitude;

    int adsr_bytes;
    float *adsr;
    int adsr_out_bytes;
    float *adsr_out;
    int adsr_list_bytes;
    t_atom *adsr_list;

    short manual_override;
    short trigger_sent;
    short play_backwards;
} t_retroseq;

/* The arguments/inlets/outlets/vectors indexes *******************************/
enum ARGUMENTS { A_NONE };
enum INLETS { NUM_INLETS };
enum OUTLETS { O_OUTPUT, O_ADSR, O_BANG, O_SHUFFLE_F, O_SHUFFLE_D, NUM_OUTLETS };
enum DSP { PERFORM,
           OBJECT, OUTPUT, VECTOR_SIZE,
           NEXT };

/* The class pointer **********************************************************/
static t_class *retroseq_class;

/* Function prototypes ********************************************************/
void *retroseq_common_new(t_retroseq *x, short argc, t_atom *argv);
void retroseq_free(t_retroseq *x);

void retroseq_dsp(t_retroseq *x, t_signal **sp, short *count);
t_int *retroseq_perform(t_int *w);

/* The object-specific prototypes *********************************************/
void retroseq_list(t_retroseq *x, t_symbol *msg, short argc, t_atom *argv);
void retroseq_freqlist(t_retroseq *x, t_symbol *msg, short argc, t_atom *argv);
void retroseq_durlist(t_retroseq *x, t_symbol *msg, short argc, t_atom *argv);

void retroseq_shuffle_freqs(t_retroseq *x);
void retroseq_shuffle_durs(t_retroseq *x);
void retroseq_shuffle(t_retroseq *x);

void retroseq_set_tempo(t_retroseq *x, t_symbol *msg, short argc, t_atom *argv);
void retroseq_set_elastic_sustain(t_retroseq *x, t_symbol *msg, short argc, t_atom *argv);
void retroseq_set_sustain_amplitude(t_retroseq *x, t_symbol *msg, short argc, t_atom *argv);
void retroseq_set_adsr(t_retroseq *x, t_symbol *msg, short argc, t_atom *argv);

void retroseq_send_adsr(t_retroseq *x);
void retroseq_send_bang(t_retroseq *x);

void retroseq_manual_override(t_retroseq *x, t_symbol *msg, short argc, t_atom *argv);
void retroseq_trigger_sent(t_retroseq *x);
void retroseq_play_backwards(t_retroseq *x, t_symbol *msg, short argc, t_atom *argv);

/******************************************************************************/

#endif /* retroseq_common_h */
