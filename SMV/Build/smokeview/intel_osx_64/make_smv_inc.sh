#!/bin/bash
source ../scripts/setopts.sh $*
#source ../test_ifort.sh
LIBDIR=../LIBS/lib_osx_intel_64/
source ../scripts/test_libs.sh

eval make -j 4 ${SMV_MAKE_OPTS}-f ../Makefile intel_osx_64
