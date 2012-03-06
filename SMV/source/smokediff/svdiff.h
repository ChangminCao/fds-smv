// $Date$ 
// $Revision$
// $Author$

#ifndef SVDIFF_H_DEFINED
#define SVDIFF_H_DEFINED
#include "histogram.h"

//************************** pre-processing directives ****************************************

#ifdef INMAIN
#define EXTERN
#else
#define EXTERN extern
#endif

#include "string_util.h"


#ifdef WIN32
#define STDCALL extern void _stdcall
#else
#define STDCALL extern void
#endif

#ifdef X64
#define STRUCTSTAT struct __stat64
#define STAT _stat64
#else
#define STRUCTSTAT struct stat
#define STAT stat
#endif

#ifdef X64
#undef BIT64
#define BIT64
#endif

#ifdef pp_LINUX64
#undef BIT64
#define BIT64
#endif

#ifdef BIT64
#define FILE_SIZE unsigned long long
#else
#define FILE_SIZE unsigned int
#endif

//************************** data structures ****************************************

typedef struct {
  int ibar, jbar, kbar;
  float xbar0, xbar, ybar0, ybar, zbar0, zbar;
  float dx, dy, dz;
  float *xplt, *yplt, *zplt;
} mesh;

typedef struct _boundary {
  char *file;
  int version;
  struct _boundary *boundary2;
  FILE_SIZE filesize;
  int npatches;
  int *pi1, *pi2, *pj1, *pj2, *pk1, *pk2, *patchdir;
  int *patch2index, *patchsize, *qoffset;
  char keyword[255];
  int boundarytype;
  histogramdata *histogram;
  mesh *boundarymesh;
  flowlabels label;
} boundary;

typedef struct _slice {
  char *file;
  int is1, is2, js1, js2, ks1, ks2;
  float xmin, xmax, ymin, ymax, zmin, zmax;
  FILE_SIZE filesize;
  int factor[3];
  int version;
  int volslice;
  struct _slice *slice2;
  char keyword[255];
  int slicetype;
  mesh *slicemesh;
  histogramdata *histogram;
  flowlabels label;
} slice;

typedef struct _plot3d {
  char keyword[255];
  char *file;
  float time;
  struct _plot3d *plot3d2;
  float xmin, xmax, ymin, ymax, zmin, zmax;
  histogramdata *histogram[5];
  mesh *plot3dmesh;
  flowlabels labels[5];
} plot3d;

typedef struct {
  slice *sliceinfo;
  mesh *meshinfo;
  plot3d *plot3dinfo;
  boundary *boundaryinfo;
  char *dir;
  int endian;
  int nmeshes;
  int nsliceinfo, nplot3dinfo, nboundary_files;
} casedata;

//************************** headers ****************************************

int getendian(void);
int getmaxrevision(void);
void version(void);
void usage(void);
int mesh_match(mesh *mesh1, mesh *mesh2);
int readsmv(FILE *streamsmv, FILE *stream_out, casedata *smvcase);
void setup_boundary(FILE *stream_out);
void setup_slice(FILE *stream_out);
void setup_plot3d(FILE *stream_out);
plot3d *getplot3d(plot3d *plot3din, casedata *case2);
slice *getslice(slice *slicein, casedata *case2);
boundary *getboundary(boundary *boundaryin, casedata *case2);
void diff_boundaryes(FILE *stream_out);
void diff_slices(FILE *stream_out);
void diff_plot3ds(FILE *stream_out);
int similar_grid(mesh *mesh1, mesh *mesh2, int *factor);
int exact_grid(mesh *mesh1, mesh *mesh2, int *factor);
int getpatchindex(int in1, boundary *boundaryin, boundary *boundaryout);

#ifdef pp_noappend
#define FORTgetsliceparms getsliceparms
#define FORTclosefortranfile closefortranfile
#define FORTopenslice openslice
#define FORTgetsliceframe getsliceframe
#define FORToutsliceframe outsliceframe
#define FORToutsliceheader outsliceheader
#define FORTgetplot3dq getplot3dq
#define FORTplot3dout plot3dout
#define FORTgetboundaryheader1 getboundaryheader1
#define FORTgetboundaryheader2 getboundaryheader2
#define FORTopenboundary openboundary
#define FORTgetpatchdata getpatchdata
#define FORToutboundaryheader outboundaryheader
#define FORToutpatchframe outpatchframe
#define FORTendianout endianout
#define FORTget_file_unit get_file_unit
#else
#define FORTgetsliceparms getsliceparms_
#define FORTclosefortranfile closefortranfile_
#define FORTopenslice openslice_
#define FORTgetsliceframe getsliceframe_
#define FORToutsliceframe outsliceframe_
#define FORToutsliceheader outsliceheader_
#define FORTgetplot3dq getplot3dq_
#define FORTplot3dout plot3dout_
#define FORTgetboundaryheader1 getboundaryheader1_
#define FORTgetboundaryheader2 getboundaryheader2_
#define FORTopenboundary openboundary_
#define FORTgetpatchdata getpatchdata_
#define FORToutboundaryheader outboundaryheader_
#define FORToutpatchframe outpatchframe_
#define FORTendianout endianout_
#define FORTget_file_unit get_file_unit_
#endif
STDCALL FORTget_file_unit(int *file_unit, int *file_unit_start);
STDCALL FORToutpatchframe(int *lunit, int *npatch,
                          int *pi1, int *pi2, int *pj1, int *pj2, int *pk1, int *pk2,
                          float *patchtime, float *pqq, int *error);
STDCALL FORToutboundaryheader(char *outfile, int *unit3, int *npatches,
                              int *pi1, int *pi2, int *pj1, int *pj2, int *pk1, int *pk2,
                              int *patchdir, int *error1, FILE_SIZE len);
STDCALL FORTgetpatchdata(int *lunit, int *npatch,int *pi1,int *pi2,int *pj1,int *pj2,int *pk1,int *pk2,
                         float *patch_times,float *pqq, int *npqq, int *error);
STDCALL FORTopenboundary(char *boundaryfilename, int *boundaryunitnumber, 
                         int *endian, int *version, int *error, FILE_SIZE len);
STDCALL FORTgetboundaryheader1(char *boundaryfilename, int *boundaryunitnumber, 
                               int *endian, int *npatch, int *error, FILE_SIZE lenfile);
STDCALL FORTgetboundaryheader2(int *boundaryunitnumber, int *version, int *npatches,
                               int *pi1, int *pi2, int *pj1, int *pj2, int *pk1, int *pk2, int *patchdir);
STDCALL FORTgetsliceframe(int *lu11,
                          int *is1,int *is2,int *js1,int *js2,int *ks1,int *ks2,
                          float *time,float *qframe,int *slicetest, int *error);
STDCALL FORTgetsliceparms(char *file,int *endian,
                          int *is1,int *is2,int *js1,int *js2,int *ks1, int *ks2,
                          int *ni, int *nj, int *nk,
                          int *slice3d, int *error,FILE_SIZE lenfile);
STDCALL FORTopenslice(char *slicefilename, int *unit, int *endian, 
                      int *is1, int *is2, int *js1, int *js2, int *ks1, int *ks2,
                      int *error, FILE_SIZE lenfile);
STDCALL FORTclosefortranfile(int *unit);
STDCALL FORToutsliceframe(int *unit3,
                          int *is1a,int *is1b,int *js1a,int *js1b,int *ks1a,int *ks1b,
                          float *time1,float *qframeout, int *error);
STDCALL FORToutsliceheader(int *file_unit,char *outfile,int *unit3,
                             int *is1a,int *is2a,int *js1a,int *js2a,int *ks1a,int *ks2a,
                             int *error1,int len);
STDCALL FORTgetplot3dq(char *qfilename, int *nx, int *ny, int *nz, float *qq, int *error, int *endian, int *isotest, int len);
STDCALL FORTplot3dout(char *outfile,int *nx,int *ny,int *nz,float *qout,int *error3,int lenout);
STDCALL FORTendianout(char *endianfilename,int lenout);

//************************** global variables ****************************************

EXTERN char pp[2];
EXTERN casedata *caseinfo;
EXTERN char *sourcedir1, *sourcedir2, *destdir;
EXTERN int test_mode;
EXTERN char type_label[1024];

#endif