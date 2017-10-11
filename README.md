# Summary
This repository is a collection of Max/MSP and Pd externals developed from the explanations in the book *"Designing Audio Objects for Max/MSP and Pd"* by Eric Lyon.  

The externals developed are:  
- [**bed**](bed) is a max-only<sup>1</sup> external that provides non-real-time buffer/array operations.  
- [**cleaner~**](cleaner~) has basic controls to do adaptive noise reduction in the frequency domain.  
- [**dynstoch~**](dynstoch~) implements dynamic stochastic synthesis as formulated by Iannis Xenakis.  
- [**mirror~**](mirror~) simply copies audio from the input directly to the output without any modifications.  
- [**moogvcf~**](moogvcf~) is a port of the Csound unit generator generator "moogvcf".  
- [**multy~**](multy~) multiplies the two input signals and sends the result to the output.  
- [**multy64~**](multy64~) is the same as [multy~](multy~) but implements the 64-bit version of the dsp and perform routines of the Max/MSP external.  
- [**oscil~**](oscil~) is a wavetable oscilator that allows specification of waveform by additive synthesis and by name of common band-limited waveforms.  
- [**oscil_attributes~**](oscil_attributes~) is the same as [oscil~](oscil~) but implements attributes for the Max/MSP version of the external.  
- [**retroseq~**](retroseq~) is a sample-accurate sequencer that sends a signal representing frequency values.  
- [**scrubber~**](scrubber~) is an implementation of the phase vocoder algorithm.  
- [**vdelay~**](vdelay~) provides variable delay with feedback and allows delays shorter than the signal vector size.  
- [**vpdelay~**](vpdelay~) is the same as [vdelay~](vdelay~) but implemented using pointers instead of array indexes.  

Additional utility externals were written to provide functionality available as built-in Max/MSP externals that do not exist "natively" in Pd:  
- [**cartopol~**](cartopol~) converts real and imaginary parts to magnitude and phase.  
- [**poltocar~**](poltocar~) converts magnitude and phase to real and imaginary parts.  
- [**windowvec~**](windowvec~) multiplies the incoming signal with a Hann window (raised cosine wave).  

# Clone
To clone this repository, run the following commands in the terminal:  
```
git clone https://github.com/juandagilc/DAO-MaxMSP-Pd.git
cd DAO-MaxMSP-Pd/
git submodule update --init
```

# Build
The table below shows details about the tools that have been used to build and test the externals of this repository. For more information about the steps followed to setup the Xcode projects, Visual Studio projects, and Makefiles, see the [SETUP.md](SETUP.md) file.  

| Operating Systems                                                    | Built with                   | Tested on                                              |
|:--------------------------------------------------------------------:|:----------------------------:|:------------------------------------------------------:|
| macOS Sierra 10.12.6                                                 | Xcode 9.0                    | Max 7.3.4 (32/64-bit<sup>2</sup>) & Pd-0.48-0 (32-bit) |
| Windows 10                                                           | Visual Studio Community 2015 | Max 7.3.4 (32/64-bit<sup>2</sup>) & Pd-0.48-0 (32-bit) |
| Linux (Ubuntu MATE 16.04 LTS 32-bit)                                 | GNU Make 4.1<sup>3</sup>     | Pd-0.48-0 (32-bit)                                     |
| Linux (Debian GNU/Linux 7.4 (wheezy) on the BeagleBone Black + Bela) | GNU Make 3.81<sup>3</sup>    | libpd 0.8.3 (armv7l) BeagleBone Black + Bela Cape      |

To build all the externals in this repository at once you can run ``ruby build.rb`` from the *Terminal* on macOS or Linux, or from the *Developer Command Prompt for VS2015* on Windows (on macOS Ruby comes preinstalled, on Linux you can install it using ``sudo apt-get install ruby-full``, and on Windows you can install it from https://rubyinstaller.org). The ``build.rb`` script creates a folder called ``_EXTERNALS_`` in the root of the repository with subfolders for Max/MSP and Pd files. The optional argument ``BBB`` can be passed to the script to cross-compile the externals for the [*BeagleBone Black + Bela Cape*](http://bela.io) from a host operating system (cross-compilation was tested only on macOS using the [Linaro toolchain](https://github.com/BelaPlatform/Bela/wiki/Compiling-Bela-projects-in-Eclipse)). Max/MSP and Pd patches showing the usage of the externals will be copied to the ``_EXTERNALS_`` folder as well.  

# License
Please see the [LICENSE.md](LICENSE.md) file for details.

********************************************************************************

<sup>1</sup> "Max-only" means that the external does not process audio in real-time, but it is implemented for Max/MSP and Pd.  
<sup>2</sup> However, the only external that implements 64-bit dsp and perform routines is [multy64~](multy64~).  
<sup>3</sup> The Makefiles can be used to build Pd externals for macOS, Windows, and Linux, using make.  
