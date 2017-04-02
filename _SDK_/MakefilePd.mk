# Makefile to build Pd externals -----------------------------------------------

# Set paths and variables ------------------------------------------------------

EXTERNAL := $(shell basename $(abspath $(THISDIR)..))
SDK := $(THISDIR)../../_SDK_/pd-includes
SOURCE := $(THISDIR)../Source
PRODUCTS := $(THISDIR)../Products
SRCFILES := $(SOURCE)/$**pd.c $(SOURCE)/$**common.c
PDWIN := "C:/Program Files (x86)/pd/bin"

# Select target ----------------------------------------------------------------

ifeq ($(OS), Windows_NT)
	TARGET := pd_win
else
	UNAME_S := $(shell uname -s)
	ifeq ($(UNAME_S), Darwin)
		TARGET := pd_darwin
	endif
	ifeq ($(UNAME_S), Linux)
		TARGET := pd_linux
	endif
endif

ifeq ($(BBB), 1)
	PREFIX := /usr/local/linaro/arm-linux-gnueabihf/bin/arm-linux-gnueabihf-
	TARGET := pd_linux
endif

current: $(TARGET)

# OS X -------------------------------------------------------------------------

pd_darwin : $(PRODUCTS)/$(EXTERNAL).pd_darwin

DARWINCFLAGS = -DPD -O3 -Wall -W -Wshadow -Wstrict-prototypes -Wno-unused -Wno-parentheses -Wno-switch
DARWINOFLAGS = -undefined dynamic_lookup
DARWINARCH = -arch i386
DARWININCLUDE = -I$(SDK)

$(PRODUCTS)/%.pd_darwin : $(SRCFILES)
	@mkdir -p $(PRODUCTS)
	@gcc $(DARWINCFLAGS) $(DARWINARCH) $(DARWININCLUDE) -c $(SRCFILES) -DTARGET_IS_PD=1
	@gcc $(DARWINOFLAGS) $(DARWINARCH) -o $(PRODUCTS)/$*.pd_darwin *.o
	@rm -f *.o

# LINUX ------------------------------------------------------------------------

pd_linux : $(PRODUCTS)/$(EXTERNAL).pd_linux

LINUXCFLAGS = -DPD -DUNIX -DICECAST -O2 -funroll-loops -fomit-frame-pointer -Wall -W -Wshadow -Wstrict-prototypes -fno-strict-aliasing -Wno-unused -Wno-parentheses -Wno-switch -fPIC -std=c11
LINUXLDFLAGS = --export-dynamic -shared
LINUXSTRIPFLAGS = --strip-unneeded
LINUXINCLUDE = -I$(SDK)

$(PRODUCTS)/%.pd_linux : $(SRCFILES)
	@mkdir -p $(PRODUCTS)
	@$(PREFIX)gcc $(LINUXCFLAGS) $(LINUXINCLUDE) -c $(SRCFILES) -DTARGET_IS_PD=1
	@$(PREFIX)ld $(LINUXLDFLAGS) -o $(PRODUCTS)/$*.pd_linux *.o -lc
	@$(PREFIX)strip $(LINUXSTRIPFLAGS) $(PRODUCTS)/$*.pd_linux
	@rm -f *.o

# WINDOWS (with MinGW) ---------------------------------------------------------

pd_win : $(PRODUCTS)/$(EXTERNAL).dll

WINCFLAGS = -DPD -DNT -Werror -Wno-unused -mms-bitfields -Wno-parentheses -Wno-switch -O6 -funroll-loops -fomit-frame-pointer -fno-strict-aliasing
WINLDFLAGS = -shared
WINSTRIPFLAGS = --strip-unneeded
WININCLUDE = -I$(SDK)

$(PRODUCTS)/%.dll : $(SRCFILES)
	@mkdir -p $(PRODUCTS)
	@gcc $(WINCFLAGS) $(WININCLUDE) -c $(SRCFILES) -DTARGET_IS_PD=1
	@gcc $(WINLDFLAGS) -o $(PRODUCTS)/$*.dll *.o $(PDWIN)/pd.dll
	@strip $(WINSTRIPFLAGS) $(PRODUCTS)/$*.dll
	@rm -f *.o

# ------------------------------------------------------------------------------

clean:
	@rm -f $(PRODUCTS)/*.pd_darwin
	@rm -f $(PRODUCTS)/*.pd_linux
	@rm -f $(PRODUCTS)/*.dll

#-------------------------------------------------------------------------------
