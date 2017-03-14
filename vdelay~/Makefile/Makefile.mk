# Makefile to build Pd externals -----------------------------------------------

THISDIR := $(dir $(lastword $(MAKEFILE_LIST)))
include $(THISDIR)../../_SDK_/MakefilePd.mk

print-%  : ; @echo $* = $($*)

#-------------------------------------------------------------------------------
