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

    x->sequence_bytes = MAXIMUM_SEQUENCE_LENGTH * sizeof(float);
    x->sequence = (float *)new_memory(x->sequence_bytes);
    x->sequence_length = DEFAULT_SEQUENCE_LENGTH;

    x->sequence[0] = F0;
    x->sequence[1] = F1;
    x->sequence[2] = F2;

    x->note_duration_ms = DEFAULT_NOTE_DURATION_MS;
    x->note_duration_samples = x->note_duration_ms / 1000.0 * x->fs;
    
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
    free_memory(x->sequence, x->sequence_bytes);
	
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

    if (argc > MAXIMUM_SEQUENCE_LENGTH) {
        argc = MAXIMUM_SEQUENCE_LENGTH;
    }

    for (int ii = 0; ii < argc; ii++) {
        x->sequence[ii] = atom_getfloat(argv + ii);
    }

    x->sequence_length = argc;
    x->note_counter = x->sequence_length - 1;
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

	/* Adjust to changes in the sampling rate */
	if (x->fs != sp[0]->s_sr) {
		x->fs = sp[0]->s_sr;
		x->note_duration_samples = x->note_duration_ms / 1000.0 * x->fs;
	}

    /* Initialize the remaining state variables */
    x->sample_counter = x->note_duration_samples;
    x->note_counter = 0;

    x->current_note_value = x->sequence[0];
	
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
    float *sequence = x->sequence;
    int sequence_length = x->sequence_length;
    int note_duration_samples = x->note_duration_samples;

    int sample_counter = x->sample_counter;
    int note_counter = x->note_counter;

    float current_note_value = x->current_note_value;
    
	/* Perform the DSP loop */
    while (n--)
    {
        if (sample_counter-- == 0) {
            note_counter++;
            if (note_counter >= sequence_length) {
                note_counter = 0;
            }
            sample_counter = note_duration_samples;
            current_note_value = sequence[note_counter];
        }
        *output++ = current_note_value;
    }
    
    /* Update state variables */
    x->sample_counter = sample_counter;
    x->note_counter = note_counter;
    x->current_note_value = current_note_value;
    
    /* Return the next address in the DSP chain */
	return w + NEXT;
}

/******************************************************************************/
