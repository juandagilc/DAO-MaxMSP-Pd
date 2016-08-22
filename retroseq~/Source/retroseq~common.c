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
void *common_new(t_retroseq *x, short argc, t_atom *argv)
{
#ifdef TARGET_IS_MAX
	/* Create signal inlets */
	dsp_setup((t_pxobject *)x, NUM_INLETS);
	
	/* Create signal outlets */
	outlet_new((t_object *)x, "signal");
	
	/* Avoid sharing memory among audio vectors */
	x->obj.z_misc |= Z_NO_INPLACE;
#elif TARGET_IS_PD
	/* Create signal inlets */
	//nothing
	
	/* Create signal outlets */
	outlet_new(&x->obj, gensym("signal"));
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
    free_memory(x->note_sequence, x->max_sequence_bytes);
    free_memory(x->duration_sequence, x->max_sequence_bytes);
	
	/* Print message to Max window */
	post("retroseq~ • Memory was freed");
}

/* The object-specific methods ************************************************/
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

    for (int ii = 0, jj = 0; ii < argc; ii++, jj+=2) {
        x->note_sequence[ii] = atom_getfloat(argv + jj);
        x->duration_sequence[ii] = atom_getfloat(argv + jj+1);
    }

    x->note_sequence_length = argc / 2;
    x->duration_sequence_length = argc / 2;
    x->note_counter = x->note_sequence_length - 1;
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
}

void retroseq_tempo(t_retroseq *x, t_symbol *msg, short argc, t_atom *argv)
{
    float old_tempo_bpm;
    old_tempo_bpm = x->tempo_bpm;

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
    
    x->tempo_bpm = new_tempo_bpm;
    x->duration_factor = (DEFAULT_TEMPO_BPM / new_tempo_bpm) * (x->fs / 1000.0);
    x->sample_counter *= old_tempo_bpm / new_tempo_bpm;
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
    x->tempo_bpm = DEFAULT_TEMPO_BPM;
    x->duration_factor = (DEFAULT_TEMPO_BPM / x->tempo_bpm) * (x->fs / 1000.0);
    x->sample_counter = x->duration_sequence[0] * (x->fs / 1000.0);

    x->current_note_value = x->note_sequence[0];
    x->note_counter = 0;

    x->current_duration_value = x->duration_sequence[0];
    x->duration_counter = 0;

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
    
	/* Perform the DSP loop */
    while (n--)
    {
        if (sample_counter-- == 0) {
            if (++note_counter >= note_sequence_length) {
                note_counter = 0;
            }
            if (++duration_counter >= duration_sequence_length) {
                duration_counter = 0;
            }

            current_duration_value = duration_sequence[duration_counter];
            current_note_value = note_sequence[note_counter];

            sample_counter = current_duration_value * duration_factor;
        }
        *output++ = current_note_value;
    }
    
    /* Update state variables */
    x->current_note_value = current_note_value;
    x->note_counter = note_counter;

    x->current_duration_value = current_duration_value;
    x->duration_counter = duration_counter;

    x->sample_counter = sample_counter;
    
    /* Return the next address in the DSP chain */
	return w + NEXT;
}

/******************************************************************************/
