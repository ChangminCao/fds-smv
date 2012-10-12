// $Date$ 
// $Revision$
// $Author$

#ifndef OPTIONS_H_DEFINED
#define OPTIONS_H_DEFINED
// build Smokeview as a standard release unless the pp_BETA directive is defined

#define pp_release
// comment following line when building an official release
#define pp_UNOFFICIAL_RELEASE

//*** uncomment the following two lines to force all versions to be beta
//#undef pp_BETA
//#define pp_BETA

//VVVVVVVVVVVVVVVVVVVVVVVVVVVVV  define smokeview title VVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVV
#ifdef pp_BETA
#define PROGVERSION "Test"
#undef pp_release
#endif
#ifdef pp_release
#ifdef pp_UNOFFICIAL_RELEASE
#define PROGVERSION "Unofficial release"
#else
#define PROGVERSION "6.0.6"
#endif
#endif

//VVVVVVVVVVVVVVVVVVVVVVVVVVVVV  turn on options available on all platforms VVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVV

//#define pp_HIDEMULTI
//#define pp_GEOMPRINT

#define pp_TERRAIN2GEOM
#define pp_GPU

#ifdef pp_GPU
#define pp_CULL
#define pp_GPUTHROTTLE
#endif
#define pp_DRAWISO
#define EGZ
#define USE_ZLIB
#define pp_THREAD
#define pp_SLICECONTOURS
#define pp_LANG
#define pp_SHOOTER
#define pp_FREEZE_VOLSMOKE
#define pp_LIGHT

#define _CRT_SECURE_NO_DEPRECATE

//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

//VVVVVVVVVVVVVVVVVVVVVVVVVVVVV  turn on options that are being tested VVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVV

#ifdef pp_BETA
// #define pp_BENCHMARK
//#define pp_GPUDEPTH
#define pp_OPEN
#define pp_SHOWLIGHT
//#define pp_MERGESMOKE
#endif

//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

#ifdef _DEBUG  // comment the following line when debugging REALLY large cases (to avoid memory checks)
#define pp_MEMDEBUG
#endif

//VVVVVVVVVVVVVVVVVVVVVVVVVVVVV  turn on windows only options VVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVV

#ifdef WIN32
#define pp_memstatus
#define pp_COMPRESS
#define pp_noappend
#include "pragmas.h"
#endif
//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^


// VVVVVVVVVVVVVVVVVVVVVVVVV  set platform defines VVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVV

#ifdef pp_LINUX64
#define pp_LINUX
#endif

#ifdef pp_OSX64
#define pp_OSX
#endif

//*** turn on BIT64 if compiled on a 64 bit platform

#ifdef X64
#undef BIT64
#define BIT64
#endif

#ifdef pp_LINUX64
#undef BIT64
#define BIT64
#endif

#ifdef pp_OSX64
#undef pp_OSX
#define pp_OSX
#undef BIT64
#define BIT64
#endif

#ifdef BIT64
#define FILE_SIZE unsigned long long
#else
#define FILE_SIZE unsigned int
#endif

#ifdef X64
#define STRUCTSTAT struct __stat64
#define STAT _stat64
#else
#define STRUCTSTAT struct stat
#define STAT stat
#endif

#define LINT long int
#ifdef X64
#undef LINT
#ifdef WIN32
#define LINT __int64
#else
#define LINT long long int
#endif
#endif

#ifndef pp_OSX
#define pp_JPEG
#endif
#ifdef pp_OSX
#undef pp_LANG
#endif

// VVVVVVVVVVVVVVVVVVVVVVVVV  set defines used by various headers VVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVV
#ifdef CPP
#define CCC "C"
#else
#define CCC
#endif

#ifdef INMAIN
#define SVEXTERN
#define SVDECL(var,val)  var=val
#else
#define SVEXTERN extern CCC
#define SVDECL(var,val)  var
#endif

#ifdef CPP
#define EXTERNCPP extern "C"
#else
#define EXTERNCPP
#endif

#include "lint.h"
#endif