/* Common header files ********************************************************/
#include "retroseq~common.h"

/* The argument parsing utility functions *************************************/
void parse_float_arg(float *variable,
                     float minimum_value,
                     float default_value,
                     float maximum_value,
                     int arg_index,
                     short argc,
                     t_atom *argv)
{
    *variable = default_value;

    if (argc > arg_index) { *variable = atom_getfloatarg(arg_index, argc, argv); }

    if (*variable < minimum_value) {
        *variable = minimum_value;
    }
    else if (*variable > maximum_value) {
        *variable = maximum_value;
    }
}

void parse_int_arg(long *variable,
                   long minimum_value,
                   long default_value,
                   long maximum_value,
                   int arg_index,
                   short argc,
                   t_atom *argv)
{
    *variable = default_value;

    if (argc > arg_index) { *variable = atom_getintarg(arg_index, argc, argv); }

    if (*variable < minimum_value) {
        *variable = minimum_value;
    }
    else if (*variable > maximum_value) {
        *variable = maximum_value;
    }
}

void parse_symbol_arg(t_symbol **variable,
                      t_symbol *default_value,
                      int arg_index,
                      short argc,
                      t_atom *argv)
{
    *variable = default_value;

#ifdef TARGET_IS_MAX
    if (argc > arg_index) { *variable = atom_getsymarg(arg_index, argc, argv); }
#elif TARGET_IS_PD
    if (argc > arg_index) { *variable = atom_getsymbolarg(arg_index, argc, argv); }
#endif
}

/* The 'new' and 'delete' pointers utility functions **************************/
void *new_memory(long nbytes)
{
#ifdef TARGET_IS_MAX
    t_ptr pointer = sysmem_newptr(nbytes);
#elif TARGET_IS_PD
    void *pointer = getbytes(nbytes);
#endif

    if (pointer == NULL) {
        error("retroseq~ • Cannot allocate memory for this object");
        return NULL;
    }
    return pointer;
}

void free_memory(void *ptr, long nbytes)
{
#ifdef TARGET_IS_MAX
    sysmem_freeptr(ptr);
#elif TARGET_IS_PD
    freebytes(ptr, nbytes);
#endif
}

/******************************************************************************/






/* The common 'new instance' routine ******************************************/
void *retroseq_common_new(t_retroseq *x, short argc, t_atom *argv)
{
#ifdef TARGET_IS_MAX
    /* Create non-signal outlets */
    x->shuffle_durs_outlet = listout((t_pxobject *)x);
    x->shuffle_freqs_outlet = listout((t_pxobject *)x);
    x->bang_outlet = bangout((t_pxobject *)x);
    x->adsr_outlet = listout((t_pxobject *)x);

    /* Create inlets */
    dsp_setup((t_pxobject *)x, NUM_INLETS);

    /* Create signal outlets */
    outlet_new((t_object *)x, "signal");

    /* Avoid sharing memory among audio vectors */
    x->obj.z_misc |= Z_NO_INPLACE;

    /* Initialize clocks */
    x->bang_clock = clock_new(x, (method)retroseq_send_bang);
    x->adsr_clock = clock_new(x, (method)retroseq_send_adsr);
#elif TARGET_IS_PD
    /* Create inlets */
    //nothing

    /* Create signal outlets */
    outlet_new(&x->obj, gensym("signal"));

    /* Create non-signal outlets */
    x->adsr_outlet = outlet_new(&x->obj, gensym("list"));
    x->bang_outlet = outlet_new(&x->obj, gensym("bang"));
    x->shuffle_freqs_outlet = outlet_new(&x->obj, gensym("list"));
    x->shuffle_durs_outlet = outlet_new(&x->obj, gensym("list"));

    /* Initialize clocks */
    x->bang_clock = clock_new(x, (t_method)retroseq_send_bang);
    x->adsr_clock = clock_new(x, (t_method)retroseq_send_adsr);
#endif

    /* Parse passed arguments */
    //nothing

    /* Initialize some state variables */
    x->fs = sys_getsr();

    x->max_sequence_bytes = MAXIMUM_SEQUENCE_LENGTH * sizeof(float);

    x->note_sequence = (float *)new_memory(x->max_sequence_bytes);
    x->note_sequence_length = DEFAULT_SEQUENCE_LENGTH;
    x->note_sequence[0] = F0;
    x->note_sequence[1] = F1;
    x->note_sequence[2] = F2;

    x->duration_sequence = (float *)new_memory(x->max_sequence_bytes);
    x->duration_sequence_length = DEFAULT_SEQUENCE_LENGTH;
    x->duration_sequence[0] = D0;
    x->duration_sequence[1] = D1;
    x->duration_sequence[2] = D2;

    srand((unsigned int)clock());
    x->shuffle_list = (t_atom *)new_memory(MAXIMUM_SEQUENCE_LENGTH * sizeof(t_atom));

    x->tempo_bpm = DEFAULT_TEMPO_BPM;
    x->elastic_sustain = 0;
    x->sustain_amplitude = DEFAULT_SUSTAIN_AMPLITUDE;

    x->adsr_bytes = 4 * sizeof(float);
    x->adsr = (float *)new_memory(x->adsr_bytes);
    x->adsr[0] = DEFAULT_ATACK_DURATION;
    x->adsr[1] = DEFAULT_DECAY_DURATION;
    x->adsr[2] = DEFAULT_SUSTAIN_DURATION;
    x->adsr[3] = DEFAULT_RELEASE_DURATION;

    x->adsr_out_bytes = 10 * sizeof(float);
    x->adsr_out = (float *)new_memory(x->adsr_out_bytes);
    x->adsr_list_bytes = 10 * sizeof(t_atom);
    x->adsr_list = (t_atom *)new_memory(x->adsr_list_bytes);

    /* Print message to Max window */
    post("retroseq~ • Object was created");

    /* Return a pointer to the new object */
    return x;
}

/* The 'free instance' routine ************************************************/
void retroseq_free(t_retroseq *x)
{
#ifdef TARGET_IS_MAX
    /* Remove the object from the DSP chain */
    dsp_free((t_pxobject *)x);
#endif

    /* Free allocated dynamic memory */
    clock_free(x->bang_clock);
    clock_free(x->adsr_clock);

    free_memory(x->note_sequence, x->max_sequence_bytes);
    free_memory(x->duration_sequence, x->max_sequence_bytes);

    free_memory(x->shuffle_list, MAXIMUM_SEQUENCE_LENGTH * sizeof(t_atom));

    free_memory(x->adsr, x->adsr_bytes);
    free_memory(x->adsr_out, x->adsr_out_bytes);
    free_memory(x->adsr_list, x->adsr_list_bytes);

    /* Print message to Max window */
    post("retroseq~ • Memory was freed");
}

/* The object-specific methods ************************************************/
void send_sequence_as_list(int the_length,
                           float *the_sequence,
                           t_atom *the_list,
                           void *the_outlet)
{
    for (int ii = 0; ii < the_length; ii++) {
#ifdef TARGET_IS_MAX
        atom_setfloat(the_list + ii, the_sequence[ii]);
#elif TARGET_IS_PD
        SETFLOAT(the_list + ii, the_sequence[ii]);
#endif
    }
    outlet_list(the_outlet, NULL, the_length, the_list);
}

void retroseq_list(t_retroseq *x, t_symbol *msg, short argc, t_atom *argv)
{
    if (argc < 2) {
        error("retroseq~ • The sequence must have at least two members");
        return;
    }
    if (argc % 2) {
        error("retroseq~ • The sequence must have odd number of members");
        return;
    }

    if (argc > 2 * MAXIMUM_SEQUENCE_LENGTH) {
        argc = 2 * MAXIMUM_SEQUENCE_LENGTH;
    }

    for (int ii = 0, jj = 0; jj < argc; ii++, jj += 2) {
        x->note_sequence[ii] = atom_getfloat(argv + jj);
        x->duration_sequence[ii] = atom_getfloat(argv + jj+1);
    }

    x->note_sequence_length = argc / 2;
    x->note_counter = x->note_sequence_length - 1;
    x->duration_sequence_length = argc / 2;
    x->duration_counter = x->duration_sequence_length - 1;

    send_sequence_as_list(x->note_sequence_length,
                          x->note_sequence,
                          x->shuffle_list,
                          x->shuffle_freqs_outlet);

    send_sequence_as_list(x->duration_sequence_length,
                          x->duration_sequence,
                          x->shuffle_list,
                          x->shuffle_durs_outlet);
}

void retroseq_freqlist(t_retroseq *x, t_symbol *msg, short argc, t_atom *argv)
{
    if (argc < 2) {
        error("retroseq~ • The sequence must have at least two members");
        return;
    }

    if (argc > MAXIMUM_SEQUENCE_LENGTH) {
        argc = MAXIMUM_SEQUENCE_LENGTH;
    }

    for (int ii = 0; ii < argc; ii++) {
        x->note_sequence[ii] = atom_getfloat(argv + ii);
    }

    x->note_sequence_length = argc;
    x->note_counter = x->note_sequence_length - 1;

    send_sequence_as_list(x->note_sequence_length,
                          x->note_sequence,
                          x->shuffle_list,
                          x->shuffle_freqs_outlet);
}

void retroseq_durlist(t_retroseq *x, t_symbol *msg, short argc, t_atom *argv)
{
    if (argc < 2) {
        error("retroseq~ • The sequence must have at least two members");
        return;
    }

    if (argc > MAXIMUM_SEQUENCE_LENGTH) {
        argc = MAXIMUM_SEQUENCE_LENGTH;
    }

    for (int ii = 0; ii < argc; ii++) {
        x->duration_sequence[ii] = atom_getfloat(argv + ii);
    }

    x->duration_sequence_length = argc;
    x->duration_counter = x->duration_sequence_length - 1;

    send_sequence_as_list(x->duration_sequence_length,
                          x->duration_sequence,
                          x->shuffle_list,
                          x->shuffle_durs_outlet);
}

void retroseq_permute(float *sequence, int length)
{
    while (length > 0) {
        int random_position = rand() % length;

        float temp = sequence[random_position];
        sequence[random_position] = sequence[length - 1];
        sequence[length - 1] = temp;

        length--;
    }
}

void retroseq_shuffle_freqs(t_retroseq *x)
{
    retroseq_permute(x->note_sequence, x->note_sequence_length);

    send_sequence_as_list(x->note_sequence_length,
                          x->note_sequence,
                          x->shuffle_list,
                          x->shuffle_freqs_outlet);
}

void retroseq_shuffle_durs(t_retroseq *x)
{
    retroseq_permute(x->duration_sequence, x->duration_sequence_length);

    send_sequence_as_list(x->duration_sequence_length,
                          x->duration_sequence,
                          x->shuffle_list,
                          x->shuffle_durs_outlet);
}

void retroseq_shuffle(t_retroseq *x)
{
    retroseq_shuffle_freqs(x);
    retroseq_shuffle_durs(x);
}

void retroseq_set_tempo(t_retroseq *x, t_symbol *msg, short argc, t_atom *argv)
{
    float new_tempo_bpm;
    if (argc == 1) {
        new_tempo_bpm = atom_getfloat(argv);
    } else {
        return;
    }

    if (new_tempo_bpm <= 0) {
        error("retroseq~ • Tempo must be greater than zero");
        return;
    }

    float old_tempo_bpm = x->tempo_bpm;
    x->tempo_bpm = new_tempo_bpm;

    x->duration_factor = (60.0 / new_tempo_bpm) * (x->fs / 1000.0);
    x->sample_counter *= old_tempo_bpm / new_tempo_bpm;
}

void retroseq_set_elastic_sustain(t_retroseq *x, t_symbol *msg, short argc, t_atom *argv)
{
    if (argc == 1) {
        x->elastic_sustain = atom_getfloat(argv);
    }
}

void retroseq_set_sustain_amplitude(t_retroseq *x, t_symbol *msg, short argc, t_atom *argv)
{
    if (argc == 1) {
        short state = (short)atom_getfloat(argv);
        x->sustain_amplitude = state;
    }

    if (x->sustain_amplitude < 0) {
        x->sustain_amplitude = 0;
    } else if (x->sustain_amplitude > 1) {
        x->sustain_amplitude = 1;
    }
}

void retroseq_set_adsr(t_retroseq *x, t_symbol *msg, short argc, t_atom *argv)
{
    if (argc != 4) {
        error("retroseq~ • The envelope must have four members");
        return;
    }

    for (int ii = 0; ii < argc; ii++) {
        x->adsr[ii] = atom_getfloat(argv + ii);
        if (x->adsr[ii] < 1.0) {
            x->adsr[ii] = 1.0;
        }
    }
}

void retroseq_send_adsr(t_retroseq *x)
{
    short elastic_sustain = x->elastic_sustain;

    float *adsr = x->adsr;
    float *adsr_out = x->adsr_out;
    t_atom *adsr_list = x->adsr_list;

    float note_duration_ms = x->duration_sequence[x->duration_counter]
                             * (60.0 / x->tempo_bpm);

    adsr_out[0] = 0.0;
    adsr_out[1] = 0.0;
    adsr_out[2] = 1.0;
    adsr_out[3] = adsr[0];
    adsr_out[4] = x->sustain_amplitude;
    adsr_out[5] = adsr[1];
    adsr_out[6] = x->sustain_amplitude;

    adsr_out[8] = 0.0;
    adsr_out[9] = adsr[3];

    if (x->manual_override) {
        adsr_out[7] = adsr[2];
    } else {
        if (elastic_sustain) {
            adsr_out[7] = note_duration_ms - (adsr[0] + adsr[1] + adsr[3]);
            if (adsr_out[7] < 1.0) {
                adsr_out[7] = 1.0;
            }
        } else {
            adsr_out[7] = adsr[2];
        }

        float duration_sum = adsr_out[3] + adsr_out[5] + adsr_out[7] + adsr_out[9];
        if (duration_sum > note_duration_ms) {
            float rescale = note_duration_ms / duration_sum;
            adsr_out[3] *= rescale;
            adsr_out[5] *= rescale;
            adsr_out[7] *= rescale;
            adsr_out[9] *= rescale;
        }
    }

    for (int ii = 0; ii < 10; ii++) {
#ifdef TARGET_IS_MAX
        atom_setfloat(adsr_list + ii, adsr_out[ii]);
#elif TARGET_IS_PD
        SETFLOAT(adsr_list + ii, adsr_out[ii]);
#endif
    }
    outlet_list(x->adsr_outlet, NULL, 10, adsr_list);
}

void retroseq_send_bang(t_retroseq *x)
{
    outlet_bang(x->bang_outlet);
}

void retroseq_manual_override(t_retroseq *x, t_symbol *msg, short argc, t_atom *argv)
{
    if (argc == 1) {
        short state = (short)atom_getfloat(argv);
        x->manual_override = state;
    }
}

void retroseq_trigger_sent(t_retroseq *x)
{
    x->trigger_sent = 1;
}

void retroseq_play_backwards(t_retroseq *x, t_symbol *msg, short argc, t_atom *argv)
{
    if (argc == 1) {
        short state = (short)atom_getfloat(argv);

        if (x->play_backwards != state) {
            x->play_backwards = (short)state;

            float *sequence;
            int length;
            int position;

            sequence = x->note_sequence;
            length = x->note_sequence_length;
            position = 0;
            while (position < length) {
                float temp = sequence[position];
                sequence[position] = sequence[length - 1];
                sequence[length - 1] = temp;

                position++;
                length--;
            }
            send_sequence_as_list(x->note_sequence_length,
                                  x->note_sequence,
                                  x->shuffle_list,
                                  x->shuffle_freqs_outlet);

            sequence = x->duration_sequence;
            length = x->duration_sequence_length;
            position = 0;
            while (position < length) {
                float temp = sequence[position];
                sequence[position] = sequence[length - 1];
                sequence[length - 1] = temp;

                position++;
                length--;
            }
            send_sequence_as_list(x->duration_sequence_length,
                                  x->duration_sequence,
                                  x->shuffle_list,
                                  x->shuffle_durs_outlet);
        }
    }
}

/******************************************************************************/






/* The 'DSP' method ***********************************************************/
void retroseq_dsp(t_retroseq *x, t_signal **sp, short *count)
{
    /* Store signal connection states of inlets */
#ifdef TARGET_IS_MAX
    //nothing
#elif TARGET_IS_PD
    //nothing
#endif

    /* Initialize the remaining state variables */
    x->duration_factor = (60.0 / x->tempo_bpm) * (x->fs / 1000.0);
    x->sample_counter = 0;

    x->current_note_value = x->note_sequence[0];
    x->note_counter = x->note_sequence_length - 1;

    x->current_duration_value = x->duration_sequence[0];
    x->duration_counter = x->duration_sequence_length - 1;

    x->manual_override = 0;
    x->play_backwards = 0;

    /* Adjust to changes in the sampling rate */
    if (x->fs != sp[0]->s_sr) {
        x->duration_factor *= sp[0]->s_sr / x->fs;
        x->sample_counter *= x->fs / sp[0]->s_sr;

        x->fs = sp[0]->s_sr;
    }

    /* Attach the object to the DSP chain */
    dsp_add(retroseq_perform, NEXT-1, x,
            sp[0]->s_vec,
            sp[0]->s_n);

    /* Print message to Max window */
    post("retroseq~ • Executing 32-bit perform routine");
}

/* The 'perform' routine ******************************************************/
t_int *retroseq_perform(t_int *w)
{
    /* Copy the object pointer */
    t_retroseq *x = (t_retroseq *)w[OBJECT];

    /* Copy signal pointers */
    t_float *output = (t_float *)w[OUTPUT];

    /* Copy the signal vector size */
    t_int n = w[VECTOR_SIZE];

    /* Load state variables */
    float *note_sequence = x->note_sequence;
    int note_sequence_length = x->note_sequence_length;
    float current_note_value = x->current_note_value;
    int note_counter = x->note_counter;

    float *duration_sequence = x->duration_sequence;
    int duration_sequence_length = x->duration_sequence_length;
    float current_duration_value = x->current_duration_value;
    int duration_counter = x->duration_counter;

    float duration_factor = x->duration_factor;
    int sample_counter = x->sample_counter;

    short manual_override = x->manual_override;
    short trigger_sent = x->trigger_sent;

    /* Perform the DSP loop */
    if (manual_override) {
        while (n--)
        {
            if (trigger_sent) {
                trigger_sent = 0;

                if (++note_counter >= note_sequence_length) {
                    note_counter = 0;
                    clock_delay(x->bang_clock, 0);
                }

                current_note_value = note_sequence[note_counter];
                clock_delay(x->adsr_clock, 0);
            }
            
            *output++ = current_note_value;
        }
    }

    else {
        while (n--)
        {
            if (sample_counter-- <= 0) {
                if (++note_counter >= note_sequence_length) {
                    note_counter = 0;
                    clock_delay(x->bang_clock, 0);
                }

                if (++duration_counter >= duration_sequence_length) {
                    duration_counter = 0;
                }

                current_duration_value = duration_sequence[duration_counter];
                sample_counter = current_duration_value * duration_factor;

                current_note_value = note_sequence[note_counter];
                clock_delay(x->adsr_clock, 0);
            }
            
            *output++ = current_note_value;
        }
    }

    /* Update state variables */
    x->current_note_value = current_note_value;
    x->note_counter = note_counter;

    x->current_duration_value = current_duration_value;
    x->duration_counter = duration_counter;
    
    x->sample_counter = sample_counter;
    x->trigger_sent = trigger_sent;
    
    /* Return the next address in the DSP chain */
    return w + NEXT;
}

/******************************************************************************/
