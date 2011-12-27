// $Date$ 
// $Revision$
// $Author$

#ifndef STRING_UTIL_H_DEFINED
#define STRING_UTIL_H_DEFINED
#include <time.h>

#ifdef IN_STRING_UTIL
#define STREXTERN
#define STRDECL(var,val)  var=val
#else
#define STREXTERN extern CCC
#define STRDECL(var,val)  var
#endif


EXTERNCPP int match(char *buffer, const char *key);
EXTERNCPP int match_upper(char *buffer, const char *key);
EXTERNCPP int randint(int min, int max);
EXTERNCPP void fparsecsv(char *buffer, float *vals, int *valids, int ncols, int *ntokens);
EXTERNCPP void parsecsv(char *buffer, char **tokens, int ncols, int *ntokens);
EXTERNCPP void stripquotes(char *buffer);
EXTERNCPP void stripcommas(char *buffer);
EXTERNCPP int getrowcols(FILE *stream, int *nrows, int *ncols);

EXTERNCPP void remove_comment(char *buffer);
EXTERNCPP void trim(char *line);
EXTERNCPP void trim_commas(char *line);
EXTERNCPP char *trim_front(char *line);
EXTERNCPP void trimzeros(char *line);
EXTERNCPP void trimmzeros(char *line);
EXTERNCPP char *Strstr(char *c, char *key);
EXTERNCPP char *STRSTR(char *c, const char *key);
EXTERNCPP void scalestring(const char *stringfrom, char *stringto, const float *scale, float range);
EXTERNCPP void scalefloat2string(float floatfrom, char *stringto, const float *scale, float range);
EXTERNCPP void num2string(char *string, float tval,float range);
EXTERNCPP char *trim_string(char *buffer);
EXTERNCPP int STRCMP(const char *s1, const char *s2);
EXTERNCPP char *get_chid(char *file, char *buffer);
#ifdef pp_GPU
EXTERNCPP int log_base2(float xx);
#endif
EXTERNCPP void array2string(float *vals, int nvals, char *string);
EXTERNCPP float frexp10(float x, int *exp10);
EXTERNCPP int getrevision(char *svn);
EXTERNCPP char *getstring(char *buffer);
EXTERNCPP char *time2timelabel(float time, float dt, char *timelabel);
EXTERNCPP char *randstr(char* str, int length);


#ifdef WIN32
STREXTERN char STRDECL(dirseparator[],"\\");
#else
STREXTERN char STRDECL(dirseparator[],"/");
#endif
#endif