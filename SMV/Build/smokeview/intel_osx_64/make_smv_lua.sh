#!/bin/bash
source ../setopts.sh $*
LIBDIR=../LIBS/lib_osx_intel_64/
source ../test_libs.sh lua
LUA_SCRIPTING="LUA_SCRIPTING=true"

make -f ../Makefile clean
eval make -j 4 ${SMV_MAKE_OPTS} ${LUA_SCRIPTING} -f ../Makefile intel_osx_64
