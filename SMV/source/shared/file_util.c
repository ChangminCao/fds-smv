// $Date$ 
// $Revision$
// $Author$

// svn revision character string
char file_util_revision[]="$Revision$";

#include "options.h"
#include <stdio.h> 
#include <string.h>
#include <sys/stat.h>
#include <ctype.h>
#include <stdlib.h>
#include <math.h>
#ifdef WIN32
#include <io.h>
#include <direct.h>
#include <dirent_win.h>
#else
#include <dirent.h>
#endif
#include "MALLOC.h"
#include "string_util.h"
#include "file_util.h"
#include "smv_endian.h"

/* ------------------ filecopy ------------------------ */

#define FILE_BUFFER 1000000
void filecopy(char *destdir, char *file_in, char *file_out){
  char buffer[FILE_BUFFER];
  FILE *streamin;
  FILE *streamout;
  char *full_file_out=NULL;
  size_t chars_in;

  if(destdir==NULL||file_in==NULL)return;
  streamin=fopen(file_in,"rb");
  if(streamin==NULL)return;

  full_file_out=NULL;
  NewMemory((void **)&full_file_out,strlen(file_out)+strlen(destdir)+1+1);
  strcpy(full_file_out,destdir);
  if(destdir[strlen(destdir)-1]!=*dirseparator){
    strcat(full_file_out,dirseparator);
  }
  strcat(full_file_out,file_out);

  streamout=fopen(full_file_out,"rb");
  if(streamout!=NULL){
    fprintf(stderr,"*** Warning: will not overwrite %s%s\n",destdir,file_in);
    fclose(streamout);
    fclose(streamin);
    return;
  }
  streamout=fopen(full_file_out,"wb");
  if(streamout==NULL){
    fclose(streamin);
    return;
  }
  for(;;){
    int end_of_file;
       
    end_of_file=0;
    chars_in=fread(buffer,1,FILE_BUFFER,streamin);
    if(chars_in!=FILE_BUFFER)end_of_file=1;
    if(chars_in>0)fwrite(buffer,chars_in,1,streamout);
    if(end_of_file==1)break;
  }
  fclose(streamin);
  fclose(streamout);
}

/* ------------------ copyfile ------------------------ */

void copyfile2(char *destfile, char *sourcefile, int mode){
  char buffer[FILE_BUFFER];
  FILE *streamin, *streamout;
  size_t chars_in;

  streamin=fopen(sourcefile,"rb");
  if(streamin==NULL)return;

  if(mode==0){
    streamout=fopen(destfile,"wb");
  }
  else{
    streamout=fopen(destfile,"ab");
  }
  if(streamout==NULL){
    fclose(streamin);
    return;
  }
  printf("  Copying %s to %s\n",sourcefile,destfile);
  for(;;){
    int end_of_file;
       
    end_of_file=0;
    chars_in=fread(buffer,1,FILE_BUFFER,streamin);
    if(chars_in!=FILE_BUFFER)end_of_file=1;
    if(chars_in>0)fwrite(buffer,chars_in,1,streamout);
    if(end_of_file==1)break;
  }
  fclose(streamin);
  fclose(streamout);
}

/* ------------------ get_smokezippath ------------------------ */

char *get_smokezippath(char *progdir){
  STRUCTSTAT statbuffer;
  char *zip_path;

  if(progdir!=NULL){
    NewMemory((void **)&zip_path,strlen(progdir)+20);
    strcpy(zip_path,progdir);
  }
  else{
    NewMemory((void **)&zip_path,2+20);
    strcpy(zip_path,".");
    strcat(zip_path,dirseparator);
  }

  strcat(zip_path,"smokezip");
#ifdef WIN32
  strcat(zip_path,"_win");
#endif
#ifdef pp_LINUX
  strcat(zip_path,"_linux");
#endif
#ifdef pp_OSX
  strcat(zip_path,"_osx");
#endif
#ifdef BIT64
  strcat(zip_path,"_64");
#else
  strcat(zip_path,"_32");
#endif
#ifdef WIN32
  strcat(zip_path,".exe");
#endif
  if(STAT(zip_path,&statbuffer)==0)return zip_path;
  FREEMEMORY(zip_path);
  return NULL;
}

/* ------------------ setdir ------------------------ */

char *setdir(char *argdir){
  int lendir;
  char *dir;

  lendir=strlen(argdir);
  NewMemory((void **)&dir,lendir+2);
  strcpy(dir,argdir);
  if(dir[lendir-1]!=dirseparator[0]){
    strcat(dir,dirseparator);
  }
  return dir;
}

/* ------------------ fullfile ------------------------ */

void fullfile(char *file_out, char *dir, char *file){
  char *file2;

  trim(file);
  file2=trim_front(file);
  strcpy(file_out,"");
  if(dir!=NULL)strcat(file_out,dir);
  strcat(file_out,file2);
}

/* ------------------ filecat ------------------------ */

int filecat(char *file_in1, char *file_in2, char *file_out){
  char buffer[FILE_BUFFER];
  FILE *stream_in1, *stream_in2, *stream_out;
  int chars_in;

  if(file_in1==NULL||file_in2==NULL)return -1;
  if(file_out==NULL)return -2;

  stream_in1=fopen(file_in1,"r");
  if(stream_in1==NULL)return -1;

  stream_in2=fopen(file_in2,"r");
  if(stream_in2==NULL){
    fclose(stream_in1);
    return -1;
  }

  stream_out=fopen(file_out,"w");
  if(stream_out==NULL){
    fclose(stream_in1);
    fclose(stream_in2);
    return -2;
  }

  for(;;){
    int end_of_file;
       
    end_of_file=0;
    chars_in=fread(buffer,1,FILE_BUFFER,stream_in1);
    if(chars_in!=FILE_BUFFER)end_of_file=1;
    if(chars_in>0)fwrite(buffer,chars_in,1,stream_out);
    if(end_of_file==1)break;
  }
  fclose(stream_in1);

  for(;;){
    int end_of_file;
       
    end_of_file=0;
    chars_in=fread(buffer,1,FILE_BUFFER,stream_in2);
    if(chars_in!=FILE_BUFFER)end_of_file=1;
    if(chars_in>0)fwrite(buffer,chars_in,1,stream_out);
    if(end_of_file==1)break;
  }
  fclose(stream_in2);
  fclose(stream_out);
  return 0;

}

/* ------------------ make_outfile ------------------------ */

void make_outfile(char *outfile, char *destdir, char *file1, char *ext){
  char filename_buffer[1024], *file1_noext;

  trim(file1);
  strcpy(filename_buffer,trim_front(file1));
  file1_noext=strstr(filename_buffer,ext);
  strcpy(outfile,"");
  if(file1_noext==NULL)return;
  file1_noext[0]='\0';
  if(destdir!=NULL){
    strcpy(outfile,destdir);
  }
  strcat(outfile,filename_buffer);
  strcat(outfile,"_diff");
  strcat(outfile,ext);
}

/* ------------------ can_write_to_dir ------------------------ */

int can_write_to_dir(char *dir){
  /*! \fn int can_write_to_dir(char *dir)
      \brief returns 1 if the directory can be written to, 0 otherwise
  */
  char *full_name;
  char file_name[256], *file_name_ptr;
  FILE *stream;
  int len;

  file_name_ptr=randstr(file_name,20);
  if(file_name_ptr==NULL)return 0;

  len = 20 + 1 + 1;
  if(dir!=NULL)len+=strlen(dir);

  NewMemory((void **)&full_name,len);

  strcpy(full_name,"");
  if(dir!=NULL&&strcmp(dir,".")!=0&&strlen(dir)>0){
    strcat(full_name,dir);
    strcat(full_name,dirseparator);
  }

  strcat(full_name,file_name_ptr);
  
  stream=fopen(full_name,"wb");
  if(stream==NULL){
    unlink(full_name);
    FREEMEMORY(full_name);
    return 0;
  }
  else{
    fclose(stream);
    unlink(full_name);
    FREEMEMORY(full_name);
  }
  return 1;
}

/* ------------------ is_file_newer ------------------------ */

int is_file_newer(char *file1, char *file2){
  /*! \fn int is_file_newer(char *file1, char *file2)
      \brief returns 1 if file1 is newer than file2, 0 otherwise
  */
  STRUCTSTAT statbuff1, statbuff2;
  int statfile1, statfile2;

  if(file1==NULL||file2==NULL)return -1;

  statfile1=STAT(file1,&statbuff1);
  statfile2=STAT(file2,&statbuff2);
  if(statfile1!=0||statfile2!=0)return -1;

  if(statbuff1.st_mtime>statbuff2.st_mtime)return 1;
  return 0;
}

  /* ------------------ getfileinfo ------------------------ */

int getfileinfo(char *filename, char *source_dir, FILE_SIZE *filesize){
  STRUCTSTAT statbuffer;
  int statfile;
  char buffer[1024];

  if(source_dir==NULL){
    strcpy(buffer,filename);
  }
  else{
    strcpy(buffer,source_dir);
    strcat(buffer,filename);
  }
  if(filesize!=NULL)*filesize=0;
  statfile=STAT(buffer,&statbuffer);
  if(statfile!=0)return statfile;
  if(filesize!=NULL)*filesize=statbuffer.st_size;
  return statfile;
}

/* ------------------ get_filesize ------------------------ */

FILE_SIZE get_filesize(const char *filename){
  STRUCTSTAT statbuffer;
  int statfile;
  FILE_SIZE return_val;

  return_val=0;
  if(filename==NULL)return return_val;
  statfile=STAT(filename,&statbuffer);
  if(statfile!=0)return return_val;
  return_val = statbuffer.st_size;
  return return_val;
}

  /* ------------------ file_exists ------------------------ */

int file_exists(char *filename){
  /*! \fn int file_exists(char *filename)
      \brief returns 1 if the file filename exists, 0 otherwise
  */
#ifdef WIN32
  if(filename==NULL||_access(filename,0)==-1){
    return 0;
  }
  else{
    return 1;
  }
#else
  STRUCTSTAT statbuffer;

  if(STAT(filename,&statbuffer)==0){
    return 1;
  }
  else{
    return 0;
  }
#endif
}

/* ------------------ get_filelist ------------------------ */

void free_filelist(filelistdata *filelist, int *nfilelist) {
  int i;

  for(i=0;i<*nfilelist;i++){
    FREEMEMORY(filelist[i].file);
  }
  FREEMEMORY(filelist);
  *nfilelist=0;
}

  /* ------------------ get_filelist ------------------------ */

int get_nfilelist(const char *path, char *key) {
  struct dirent *entry;
  DIR *dp;
  int maxfiles=0;
 
  dp = opendir(path);
  if (dp == NULL) {
    perror("opendir");
    return 0;
  }
  while( (entry = readdir(dp)) ){
    if((entry->d_type==DT_DIR&&entry->d_name[0]!='.')||(entry->d_type==DT_REG&&match_wild(entry->d_name,key)==1)){
      maxfiles++;
      continue;
    }
  }
  closedir(dp);
  return maxfiles;
}

 /* ------------------ get_filelist ------------------------ */

int get_filelist(const char *path, char *key, int maxfiles, filelistdata **filelist) {
  struct dirent *entry;
  DIR *dp;
  int nfiles=0;
  filelistdata *flist;

  // DT_DIR - is a diretory
  // DT_REG - is a regular file
 
  dp = opendir(path);
  if (dp == NULL) {
    perror("opendir");
    *filelist=NULL;
    return 0;
  }
  if(maxfiles==0){
    closedir(dp);
    *filelist=NULL;
    return 0;
  }
  NewMemory((void **)&flist,maxfiles*sizeof(filelistdata));
  while( (entry = readdir(dp))&&nfiles<maxfiles ){
    if(entry->d_type==DT_DIR&&entry->d_name[0]!='.'){
      char *file;
      filelistdata *flisti;

      flisti = flist + nfiles;
      NewMemory((void **)&file,strlen(entry->d_name)+1);
      strcpy(file,entry->d_name);
      flisti->file=file;
      flisti->type=1;
      nfiles++;
    }
  }
  rewinddir(dp);
  while( (entry = readdir(dp))&&nfiles<maxfiles ){
    if(entry->d_type==DT_REG&&match_wild(entry->d_name,key)==1){
      char *file;
      filelistdata *flisti;

      flisti = flist + nfiles;
      NewMemory((void **)&file,strlen(entry->d_name)+1);
      strcpy(file,entry->d_name);
      flisti->file=file;
      flisti->type=0;
      nfiles++;
    }
  }
  *filelist=flist;
  closedir(dp);
  return nfiles;
}

/* ------------------ getfilesizelabel ------------------------ */

void getfilesizelabel(int size, char *sizelabel){
  int leftsize,rightsize;

#define sizeGB   1000000000
#define size100MB 100000000
#define size10MB   10000000
#define sizeMB     1000000
#define size100KB    100000
#define size10KB      10000

  if(size>=sizeGB){
    size/=size10MB;
    leftsize=size/100;
    rightsize=size-100*leftsize;
    sprintf(sizelabel,"%i.%02i GB",leftsize,rightsize);
  }
  else if(size>=size100MB&&size<sizeGB){
    size/=sizeMB;
    leftsize=size;
    sprintf(sizelabel,"%i MB",leftsize);
  }
  else if(size>=size10MB&&size<size100MB){
    size/=size100KB;
    leftsize=size/10;
    rightsize=size-10*leftsize;
    sprintf(sizelabel,"%i.%i MB",leftsize,rightsize);
  }
  else if(size>=sizeMB&&size<size10MB){
    size/=size10KB;
    leftsize=size/100;
    rightsize=size-100*leftsize;
    sprintf(sizelabel,"%i.%02i MB",leftsize,rightsize);
  }
  else if(size>=size100KB&&size<sizeMB){
    size/=1000;
    leftsize=size;
    sprintf(sizelabel,"%i KB",leftsize);
  }
  else{
    size/=10;
    leftsize=size/100;
    rightsize=size-100*leftsize;
    sprintf(sizelabel,"%i.%02i KB",leftsize,rightsize);
  }
}

/* ------------------ rootdir ------------------------ */

char *getprogdir(char *progname, char **svpath){
  /*! \fn char *getprogdir(char *progname, char **svpath)
      \brief returns the directory containing the file progname
  */
  char *progpath, *lastsep, *smokeviewpath2;
#ifdef WIN32
  char cdirsep='\\';
#else
  char cdirsep='/';
#endif

  lastsep=strrchr(progname,cdirsep);
  if(lastsep==NULL){
    char *dir;

    dir = which(progname);
    if(dir==NULL){
      NewMemory((void **)&progpath,(unsigned int)3);
      strcpy(progpath,".");
      strcat(progpath,dirseparator);
    }
    else{
      int lendir;

      lendir=strlen(dir);
      NewMemory((void **)&progpath,(unsigned int)(lendir+2));
      strcpy(progpath,dir);
      if(progpath[lendir-1]!=cdirsep)strcat(progpath,dirseparator);
    }
    NewMemory((void **)&smokeviewpath2,(unsigned int)(strlen(progpath)+strlen(progname)+1));
    strcpy(smokeviewpath2,progpath);
  }
  else{
    int lendir;

    lendir=lastsep-progname+1;
    NewMemory((void **)&progpath,(unsigned int)(lendir+1));
    strncpy(progpath,progname,lendir);
    progpath[lendir]=0;
    NewMemory((void **)&smokeviewpath2,(unsigned int)(strlen(progname)+1));
    strcpy(smokeviewpath2,"");;
  }
  strcat(smokeviewpath2,progname);
  *svpath=smokeviewpath2;
  return progpath;
}

/* ------------------ lastname ------------------------ */

char *lastname(char *argi){
  /*! \fn char *lastname(char *argi)
      \brief returns the file name contained in the full path name argi
  */
  char *lastdirsep;
  char *dir, *filename, cwdpath[1000];

#ifdef WIN32
#define CHDIR _chdir
#define GETCWD _getcwd
#define SEP '\\'
#else
#define CHDIR chdir
#define GETCWD getcwd
#define SEP '/'
#endif

  filename=argi;
  lastdirsep=strrchr(argi,SEP);
  if(lastdirsep!=NULL){
    dir=argi;
    filename=lastdirsep+1;
    lastdirsep[0]=0;
    GETCWD(cwdpath,1000);
    if(strcmp(cwdpath,dir)!=0){
      CHDIR(dir);
    }
  }
  return filename;
}

/* ------------------ get_zonefilename ------------------------ */

char *get_zonefilename(char *bufptr){
  char *full_name, *last_name, *filename;
  STRUCTSTAT statbuffer;

  full_name=bufptr;
  if(STAT(full_name,&statbuffer)!=0)full_name=NULL;

  last_name=lastname(bufptr);
  if(STAT(last_name,&statbuffer)!=0)last_name=NULL;

  if(last_name!=NULL&&full_name!=NULL){
    if(strcmp(last_name,full_name)==0){
      last_name=NULL;
    }
  }

  if(last_name!=NULL&&full_name!=NULL){
    filename=last_name;
  }
  else if(last_name==NULL&&full_name!=NULL){
    filename=full_name;
  }
  else if(last_name!=NULL&&full_name==NULL){
    filename=last_name;
  }
  else{
    filename=NULL;
  }
  return filename;
}

/* ------------------ file_modtime ------------------------ */

time_t file_modtime(char *filename){
  /*! \fn time_t file_modtime(char *filename)
      \brief returns the modification time of the file named filename
  */
  STRUCTSTAT statbuffer;
  time_t return_val;
  int statfile;

  return_val=0;
  if(filename==NULL)return return_val;
  statfile=STAT(filename,&statbuffer);
  if(statfile!=0)return return_val;
  return_val = statbuffer.st_mtime;
  return return_val;
}

/* ------------------ which ------------------------ */

char *which(char *progname){
  /*! \fn char *which(char *progname)
      \brief returns the PATH directory containing the file progname
  */
  char *pathlistptr, fullpath[4096], pathlist[4096], prog[4096];
  char *dir,*returndir;
  const char *ext;
  char pathsep[2], dirsep[2];
  int lendir,lenprog;

#ifdef WIN32
  strcpy(pathsep,";");
  strcpy(dirsep,"\\");
#else
  strcpy(pathsep,":");
  strcpy(dirsep,"/");
#endif

  if(progname==NULL)return NULL;
  strcpy(prog,progname);
  progname=prog;

  pathlistptr=getenv("PATH");
  if(pathlistptr==NULL)return NULL;
  strcpy(pathlist,pathlistptr);
  
#ifdef WIN32
  lenprog=strlen(prog);
  ext=progname+lenprog-4;
  if(lenprog<=4||STRCMP(ext,".exe")!=0){
    strcat(progname,".exe");
  }
#endif
        
  dir=strtok(pathlist,pathsep);
  while(dir!=NULL){
    strcpy(fullpath,dir);
    strcat(fullpath,dirsep);
    strcat(fullpath,prog);
    if(file_exists(fullpath)==1){
      lendir=strlen(dir);
      if(lendir<=0)continue;
      NewMemory((void **)&returndir,(unsigned int)(lendir+2));
      strcpy(returndir,dir);
      strcat(returndir,dirsep);
#ifdef pp_BETA
      printf("Using %s in %s\n\n",prog,dir);
#endif
      return returndir;
    }
    dir=strtok(NULL,pathsep);
  }
#ifdef pp_BETA
  fprintf(stderr,"*** Error: %s not found in any path directory\n",prog);
#endif
  return NULL;
}

#define HEADER_SIZE 4
#define TRAILER_SIZE 4
#define FORTREAD(var,size) FSEEK(FORTSTREAM,HEADER_SIZE,SEEK_CUR);\
                           returncode=fread(var,4,size,FORTSTREAM);\
                           if(endianswitch==1)endian_switch(var,size);\
                           FSEEK(FORTSTREAM,TRAILER_SIZE,SEEK_CUR)

/* ------------------ get_slice_frame ------------------------ */

size_t get_slice_frame(char *file, int frame_index_local, int framesize, float *time_local, float *vals){
  int skip_local;
  FILE *FORTSTREAM;
  int returncode;
  int endianswitch=0;

  skip_local =         (HEADER_SIZE +30                     +TRAILER_SIZE); // long label
  skip_local +=        (HEADER_SIZE +30                     +TRAILER_SIZE); // short label
  skip_local +=        (HEADER_SIZE +30                     +TRAILER_SIZE); // unit label
  skip_local +=        (HEADER_SIZE +6*sizeof(int)          +TRAILER_SIZE); // is1, is2, js1, js2, ks1, ks2
  skip_local += frame_index_local*(HEADER_SIZE +sizeof(float)          +TRAILER_SIZE); // framenum time's
  skip_local += frame_index_local*(HEADER_SIZE +framesize*sizeof(float)+TRAILER_SIZE); // framenum slice data's
  FORTSTREAM=fopen(file,"r");
  if(FORTSTREAM==NULL)return 0;
  FSEEK(FORTSTREAM,skip_local,SEEK_SET); // skip from beginning of file
  FORTREAD(time_local,1);
  FORTREAD(vals,framesize);
  fclose(FORTSTREAM);
  return returncode;
}
