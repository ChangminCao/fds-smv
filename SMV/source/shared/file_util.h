// $Date$ 
// $Revision$
// $Author$

#ifndef FILE_UTIL_H_DEFINED
#define FILE_UTIL_H_DEFINED

#include <time.h>

typedef struct {
  char *file;
  int type;
} filelistdata;

#ifdef BIT64
#ifdef X64
#define FSEEK(a,b,c) _fseeki64(a,b,c)
#else
#define FSEEK(a,b,c) fseeko(a,b,c)
#endif
#else
#define FSEEK(a,b,c) fseek(a,b,c)
#endif

EXTERNCPP void getfilesizelabel(int size, char *sizelabel);
EXTERNCPP void filecopy(char *destdir, char *file, char *filebase);
EXTERNCPP void copyfile(char *destfile, char *sourcefile);
EXTERNCPP char *get_smokezippath(char *progdir);
EXTERNCPP int filecat(char *file_in1, char *file_in2, char *file_out);
EXTERNCPP void make_outfile(char *outfile, char *destdir, char *file1, char *ext);
EXTERNCPP void fullfile(char *fileout, char *dir, char *file);
EXTERNCPP char *setdir(char *argdir);
EXTERNCPP int getfileinfo(char *filename, char *sourcedir, FILE_SIZE *filesize);
EXTERNCPP char *get_zonefilename(char *buffer);
EXTERNCPP int can_write_to_dir(char *dir);
EXTERNCPP int file_exists(char *filename);
EXTERNCPP int getfilesize(char *filename);

EXTERNCPP void free_filelist(filelistdata *filelist, int *nfilelist);
EXTERNCPP int get_nfilelist(const char *path, char *key) ;
EXTERNCPP int get_filelist(const char *path, char *key, int maxfiles, filelistdata **filelist);
EXTERNCPP char *which(char *progname);
EXTERNCPP FILE_SIZE get_filesize(const char *filename);
EXTERNCPP time_t file_modtime(char *filename);
EXTERNCPP int is_file_newer(char *file1, char *file2);
EXTERNCPP char *getprogdir(char *progname, char **svpath);

EXTERNCPP char *lastname(char *argi);

#ifndef STREXTERN
#ifdef WIN32
STREXTERN char STRDECL(dirseparator[],"\\");
#else
STREXTERN char STRDECL(dirseparator[],"/");
#endif
#endif
#endif
