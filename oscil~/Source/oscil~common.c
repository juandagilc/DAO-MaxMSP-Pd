/* Common header files ********************************************************/
#include "oscil~common.h"

/* The argument parsing functions *********************************************/
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

/* The 'new' and 'delete' pointers functions **********************************/

void *new_memory(long nbytes)
{
#ifdef TARGET_IS_MAX
    t_ptr pointer = sysmem_newptr(nbytes);
#elif TARGET_IS_PD
    void *pointer = getbytes(nbytes);
#endif
    
    if (pointer == NULL) {
        error("oscil~ • Cannot allocate memory for this object");
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

/* The common 'new instance' routine ******************************************/
void *oscil_common_new(t_oscil *x, short argc, t_atom *argv)
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
	inlet_new(&x->obj, &x->obj.ob_pd, gensym("signal"), gensym("signal"));
	
	/* Create signal outlets */
	outlet_new(&x->obj, gensym("signal"));
#endif
    
    /* Parse passed arguments */
    parse_float_arg(&x->frequency, MINIMUM_FREQUENCY, DEFAULT_FREQUENCY, MAXIMUM_FREQUENCY, A_FREQUENCY, argc, argv);
    parse_int_arg(&x->table_size, MINIMUM_TABLE_SIZE, DEFAULT_TABLE_SIZE, MAXIMUM_TABLE_SIZE, A_TABLE_SIZE, argc, argv);
    parse_symbol_arg(&x->waveform, gensym(DEFAULT_WAVEFORM), A_WAVEFORM, argc, argv);
    parse_int_arg(&x->harmonics, MINIMUM_HARMONICS, DEFAULT_HARMONICS, MAXIMUM_HARMONICS, A_HARMONICS, argc, argv);
    
	/* Initialize state variables */
    x->wavetable_bytes = x->table_size * sizeof(float);
    x->wavetable = (float *)new_memory(x->wavetable_bytes);
    x->wavetable_old = (float *)new_memory(x->wavetable_bytes);
    
    x->amplitudes_bytes = MAXIMUM_HARMONICS * sizeof(float);
    x->amplitudes = (float *)new_memory(x->amplitudes_bytes);
    
    x->fs = sys_getsr();
    
    x->phase = 0;
    x->increment = (float)x->table_size / x->fs;
    x->dirty = 0;
    
    x->crossfade_type = POWER_CROSSFADE;
    x->crossfade_time = DEFAULT_CROSSFADE;
    x->crossfade_samples = x->crossfade_time * x->fs / 1000.0;
    x->crossfade_countdown = 0;
    x->crossfade_in_progress = 0;
    x->just_turned_on = 1;
    
    x->twopi = 8.0 * atan(1.0);
    x->piOtwo = 2.0 * atan(1.0);
    
    /* Build wavetable */
    if (x->waveform == gensym("sine")) {
        x->waveform = gensym("");
        oscil_build_sine(x);
    } else if (x->waveform == gensym("triangle")) {
        x->waveform = gensym("");
        oscil_build_triangle(x);
    } else if (x->waveform == gensym("sawtooth")) {
        x->waveform = gensym("");
        oscil_build_sawtooth(x);
    } else if (x->waveform == gensym("square")) {
        x->waveform = gensym("");
        oscil_build_square(x);
    } else if (x->waveform == gensym("pulse")) {
        x->waveform = gensym("");
        oscil_build_pulse(x);
    } else {
        x->waveform = gensym("");
        oscil_build_sine(x);
        
        error("oscil~ • Invalid argument: Waveform set to %s", x->waveform->s_name);
    }
    
	/* Print message to Max window */
	post("oscil~ • Object was created");
	
	/* Return a pointer to the new object */
	return x;
}

/* The object-specific methods ************************************************/
void oscil_build_sine(t_oscil *x)
{
    if (x->waveform == gensym("sine")) { return; }
    
    x->harmonics_bl = x->harmonics;
    
    for (int ii = 0; ii < x->harmonics_bl; ii++) {
        x->amplitudes[ii] = 0.0;
    }
    x->amplitudes[1] = 1.0;
    
    oscil_build_waveform(x);
    x->waveform = gensym("sine");
}

void oscil_build_triangle(t_oscil *x)
{
    if (x->waveform == gensym("triangle")) { return; }
    
    x->harmonics_bl = x->harmonics;
    
    float sign = 1.0;
    for (int ii = 1; ii < x->harmonics_bl; ii += 2) {
        x->amplitudes[ii+0] = sign / ( (float)ii * (float)ii );
        x->amplitudes[ii+1] = 0.0;
        sign *= -1.0;
    }
    
    oscil_build_waveform(x);
    x->waveform = gensym("triangle");
}

void oscil_build_sawtooth(t_oscil *x)
{
    if (x->waveform == gensym("sawtooth")) { return; }
    
    x->harmonics_bl = x->harmonics;
    
    float sign = 1.0;
    for (int ii = 1; ii < x->harmonics_bl; ii++) {
        x->amplitudes[ii] = sign / (float)ii;
        sign *= -1.0;
    }
    
    oscil_build_waveform(x);
    x->waveform = gensym("sawtooth");
}
void oscil_build_square(t_oscil *x)
{
    if (x->waveform == gensym("square")) { return; }
    
    x->harmonics_bl = x->harmonics;
    
    for (int ii = 1; ii < x->harmonics_bl; ii += 2) {
        x->amplitudes[ii+0] = 1.0 / (float)ii;
        x->amplitudes[ii+1] = 0.0;
    }
    
    oscil_build_waveform(x);
    x->waveform = gensym("square");
}
void oscil_build_pulse(t_oscil *x)
{
    if (x->waveform == gensym("pulse")) { return; }
    
    x->harmonics_bl = x->harmonics;
    
    for (int ii = 1; ii < x->harmonics_bl; ii++) {
        x->amplitudes[ii] = 1.0;
    }
    
    oscil_build_waveform(x);
    x->waveform = gensym("pulse");
}

void oscil_build_list(t_oscil *x, t_symbol *msg, short argc, t_atom *argv)
{
    x->harmonics_bl = 0;
    
    if (argc > MAXIMUM_HARMONICS) {
        argc = MAXIMUM_HARMONICS;
    }
    
    for (int ii = 0; ii < argc; ii++) {
        x->amplitudes[ii] = atom_getfloat(argv + ii);
        x->harmonics_bl++;
    }
    
    oscil_build_waveform(x);
    x->waveform = gensym("list");
}

void oscil_build_waveform(t_oscil *x)
{
    /* Load state variables */
    long table_size = x->table_size;
    long harmonics_bl = x->harmonics_bl;
    
    float *wavetable = x->wavetable;
    float *wavetable_old = x->wavetable_old;
    float *amplitudes = x->amplitudes;
    
    float twopi = x->twopi;
    
    /* Check for state of crossfade */
    if (x->crossfade_in_progress) {
        return;
    }
    
    /* Save a backup of the current wavetable */
    for (int ii = 0; ii < table_size; ii++) {
        wavetable_old[ii] = wavetable[ii];
    }
    
    x->dirty = 1;
    
        /* Initialize crossfade and wavetable */
        if (x->crossfade_type != NO_CROSSFADE && !x->just_turned_on) {
            x->crossfade_countdown = x->crossfade_samples;
            x->crossfade_in_progress = 1;
        } else {
            x->crossfade_countdown = 0;
            
            for (int ii = 0; ii < x->table_size; ii++) {
                x->wavetable[ii] = 0.0;
            }
        }
    
        /* Initialize (clear) wavetable with DC component */
        for (int ii = 0; ii < table_size; ii++) {
            wavetable[ii] = amplitudes[0];
        }
        
        /* Build the wavetable using additive synthesis */
        for (int jj = 1; jj < harmonics_bl; jj++) {
            if (amplitudes[jj]) {
                for (int ii = 0; ii < table_size; ii++) {
                    wavetable[ii] += amplitudes[jj] * sin( twopi * (float)ii * (float)jj / (float)table_size );
                }
            }
        }
        
        /* Normalize wavetable to a peak value of 1.0 */
        float max = 0.0;
        for (int ii = 0; ii < table_size; ii++) {
            if (max < fabs(wavetable[ii])) {
                max = fabs(wavetable[ii]);
            }
        }
        if (max != 0.0) {
            float rescale = 1.0/max;
            for (int ii = 0; ii < table_size; ii++) {
                wavetable[ii] *= rescale;
            }
        }
    
    x->dirty = 0;
    x->just_turned_on = 0;
}

void oscil_fadetime(t_oscil *x, t_symbol *msg, short argc, t_atom *argv)
{
    float crossfade_ms = atom_getfloat(argv);
    
    if (crossfade_ms < MINIMUM_CROSSFADE) {
        crossfade_ms = MINIMUM_CROSSFADE;
    }
    else if (crossfade_ms > MAXIMUM_CROSSFADE) {
        crossfade_ms = MAXIMUM_CROSSFADE;
    }
    
    x->crossfade_time = crossfade_ms;
    x->crossfade_samples = x->crossfade_time * x->fs / 1000;
}

void oscil_fadetype(t_oscil *x, t_symbol *msg, short argc, t_atom *argv)
{
    float crossfade_type = (short)atom_getfloat(argv);
    
    if (crossfade_type < NO_CROSSFADE) {
        crossfade_type = NO_CROSSFADE;
    }
    else if (crossfade_type > POWER_CROSSFADE) {
        crossfade_type = POWER_CROSSFADE;
    }
    
    x->crossfade_type = (short)crossfade_type;
}

/* The 'free instance' routine ************************************************/
void oscil_free(t_oscil *x)
{
#ifdef TARGET_IS_MAX
	/* Remove the object from the DSP chain */
	dsp_free((t_pxobject *)x);
#endif
    
    /* Free allocated dynamic memory */
    free_memory(x->wavetable, x->wavetable_bytes);
    free_memory(x->wavetable_old, x->wavetable_bytes);
    free_memory(x->amplitudes, x->amplitudes_bytes);
	
	/* Print message to Max window */
	post("oscil~ • Memory was freed");
}

/* The 'DSP' method ***********************************************************/
void oscil_dsp(t_oscil *x, t_signal **sp, short *count)
{
	/* Store signal connection states of inlets */
	#ifdef TARGET_IS_MAX
		x->frequency_connected = count[0];
	#elif TARGET_IS_PD
		x->frequency_connected = 1;
	#endif

	/* Adjust to changes in the sampling rate */
	if (x->fs != sp[0]->s_sr) {
		x->fs = sp[0]->s_sr;
		
		x->increment = (float)x->table_size / x->fs;
        x->crossfade_samples = x->crossfade_time * x->fs / 1000.0;
	}
    x->phase = 0;
    x->just_turned_on = 1;
	
	/* Attach the object to the DSP chain */
	dsp_add(oscil_perform, NEXT-1, x,
			sp[0]->s_vec,
			sp[1]->s_vec,
			sp[0]->s_n);
	
	/* Print message to Max window */
	post("oscil~ • Executing 32-bit perform routine");
}

/* The 'perform' routine ******************************************************/
t_int *oscil_perform(t_int *w)
{
	/* Copy the object pointer */
	t_oscil *x = (t_oscil *)w[OBJECT];
	
	/* Copy signal pointers */
    t_float *frequency_signal = (t_float *)w[FREQUENCY];
    t_float *output = (t_float *)w[OUTPUT];
	
	/* Copy the signal vector size */
	t_int n = w[VECTOR_SIZE];
	
	/* Load state variables */
    float frequency = x->frequency;
    long table_size = x->table_size;
    
    float *wavetable = x->wavetable;
    float *wavetable_old = x->wavetable_old;
    
    float phase = x->phase;
    float increment = x->increment;
    
    short crossfade_type = x->crossfade_type;
    long crossfade_samples = x->crossfade_samples;
    long crossfade_countdown = x->crossfade_countdown;
    
    float piOtwo = x->piOtwo;
    
	/* Perform the DSP loop */
    float sample_increment;
    long iphase;
    
    float interp;
    float samp1;
    float samp2;
    
    float old_sample;
    float new_sample;
    float out_sample;
    float fraction;
    
    while (n--)
    {
        if (x->frequency_connected) {
            sample_increment = increment * *frequency_signal++;
        } else {
            sample_increment = increment * frequency;
        }
        
        iphase = floor(phase);
        interp = phase - iphase;
        
        samp1 = wavetable_old[ (iphase+0) ];
        samp2 = wavetable_old[ (iphase+1)%table_size ];
        old_sample = samp1 + interp * (samp2 - samp1);
        
        samp1 = wavetable[ (iphase+0) ];
        samp2 = wavetable[ (iphase+1)%table_size ];
        new_sample = samp1 + interp * (samp2 - samp1);
        
        if (x->dirty) {
            out_sample = old_sample;
        } else if (crossfade_countdown > 0) {
            fraction = (float)crossfade_countdown / (float)crossfade_samples;
            
            if (crossfade_type == POWER_CROSSFADE) {
                fraction *= piOtwo;
                out_sample = sin(fraction) * old_sample + cos(fraction) * new_sample;
            } else if (crossfade_type == LINEAR_CROSSFADE) {
                out_sample = fraction * old_sample + (1-fraction) * new_sample;
            } else {
                out_sample = old_sample;
            }
            
            crossfade_countdown--;
            
        } else {
            out_sample = new_sample;
        }
        
        *output++ = out_sample;
        
        phase += sample_increment;
        while (phase >= table_size)
            phase -= table_size;
        while (phase < 0)
            phase += table_size;
    }
	
    if (crossfade_countdown == 0) {
        x->crossfade_in_progress = 0;
    }
    
    /* Update state variables */
    x->phase = phase;
    x->crossfade_countdown = crossfade_countdown;
    
    /* Return the next address in the DSP chain */
	return w + NEXT;
}

/******************************************************************************/
