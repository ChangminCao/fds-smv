// $Date$ 
// $Revision$
// $Author$

#ifndef EGZ_H_DEFINED
#define EGZ_H_DEFINED

#ifdef EGZ

#ifdef USE_ZLIB

#ifdef WIN32

#define _WINDOWS

/* #define ZLIB_DLL */
#endif   /* end WIN32 */

#ifndef DEF_ZLIB
#define DEF_ZLIB
#include "zlib.h"
#endif
#endif   /* end USE_ZLIB */

typedef struct {
  int endianswitch;
  int compression;
#ifdef USE_ZLIB
  gzFile *stream;
  FILE *stream2;
#else
  FILE *stream;
#endif
} EGZ_FILE;
int EGZ_FCLOSE(EGZ_FILE *egz_stream);
EGZ_FILE *EGZ_FOPEN(const char *file, const char *mode, int compress, int endian);
size_t EGZ_FREAD( void *buffer, size_t size, size_t count, const EGZ_FILE *stream );
size_t EGZ_FWRITE( void *buffer, size_t size, size_t count, const EGZ_FILE *egz_stream );
int EGZ_FEOF( const EGZ_FILE *egz_stream );
int EGZ_FSEEK( const EGZ_FILE *stream, long offset, int origin );
long EGZ_FTELL( const EGZ_FILE *stream );
void EGZ_REWIND(const EGZ_FILE *egz_stream);


#else
#define EGZ_FCLOSE fclose
#define EGZ_FOPEN fopen
#define EGZ_FREAD fread
#define EGZ_FWRITE fwrite
#define EGZ_FTELL ftell
#define EGZ_FEOF feof
#define EGZ_FSEEK fseek
#define EGZ_FILE FILE
#define EGZ_REWIND rewind 
#endif
#endif
