# Summary

This repository is a collection of Max/MSP and Pd externals developed from the explanations in the book *"Designing Audio Objects for Max/MSP and Pd"* by Eric Lyon.

The externals developed are:  
- [**bed**](bed) is a max-only external that provides non-real-time buffer (or array) operations.  
- [**cleaner~**](cleaner~) has basic controls to do adaptive noise reduction in the frequency domain.  
- [**dynstoch~**](dynstoch~) implements dynamic stochastic synthesis as formulated by Iannis Xenakis.  
- [**mirror~**](mirror~) simply copies audio from the input directly to the output without any modifications.  
- [**moogvcf~**](moogvcf~) is a port of the Csound unit generator generator "moogvcf".  
- [**multy~**](multy~) multiplies the two input signals and sends the result to the output.  
- [**multy64~**](multy64~) is the same as [multy~](multy~) but implements the 64-bit dsp and perform routines for the Max/MSP version of the external.  
- [**oscil~**](oscil~) is a wavetable oscilator that allows specification of waveform by additive synthesis and by name of common band-limited waveforms.  
- [**oscil_attributes~**](oscil_attributes~) is the same as [oscil~](oscil~) but implements attributes for the Max/MSP version of the external.  
- [**retroseq~**](retroseq~) is a sample-accurate sequencer that sends a signal representing the frequency.  
- [**scrubber~**](scrubber~) is an implementation of the phase vocoder algorithm.  
- [**vdelay~**](vdelay~) provides variable delay with feedback and allows delays shorter than the signal vector size.  
- [**vpdelay~**](vpdelay~) is the same as [vdelay~](vdelay~) but implemented using pointers instead of array indexes.  

Additional utility externals were written to provide functionality available in built-in Max/MSP externals that do not exist "natively" in Pd:  
- [**cartopol~**](cartopol~) converts real and imaginary parts to magnitude and phase.  
- [**poltocar~**](poltocar~) converts magnitude and phase to real and imaginary parts.  
- [**windowvec~**](windowvec~) multiplies the incoming signal with a Hann window (raised cosine wave).  

The externals have been built and tested in OS X El Capitan 10.11.6 using Xcode 8.2 for Max 7.3.1 and Pd-0.46-7.  

# Build
You can run ``ruby build.rb`` in the terminal to build all the externals in this repository. The ``build.rb`` script creates a folder called ``./_externals_/`` with subfolders for the Max/MSP and Pd externals. A copy of all the Max/MSP and Pd patches showing the usage of the externals will be copied as well.

# License
Please see the [LICENSE.md](LICENSE.md) file for details.
