// $Date$ 
// $Revision$
// $Author$

#include "options.h"
#ifdef WIN32
#include <direct.h>
#endif
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "ASSERT.h"
#include "smokeviewvars.h"
#include "MALLOC.h"

// svn revision character string
char compress3dc_revision[]="$Revision$";

void compress_onoff(int flag);
void compress_svzip2(void);
unsigned int rle(unsigned char *buffer_in, int nchars_in, unsigned char *buffer_out);

#ifndef pp_noappend
#define CCsmoke3dheader smoke3dheader_
#define CCsmoke3dtofile smoke3dtofile_
#else
#define CCsmoke3dheader smoke3dheader
#define CCsmoke3dtofile smoke3dtofile
#endif

// this code is only used by FDS - needs to be updated if FDS does the ZLIB compression of 3d smoke files

/* ------------------ CCsmoke3dheader ------------------------ */

void CCsmoke3dheader(char *file,int *is1, int *is2, int *js1, int *js2, int *ks1, int *ks2){
  FILE *binfile,*textfile;
  int nxyz[8], VERSION=0;
  char textfilename[1024];

  nxyz[0]=1;
  nxyz[1]=VERSION;
  nxyz[2]=*is1;
  nxyz[3]=*is2;
  nxyz[4]=*js1;
  nxyz[5]=*js2;
  nxyz[6]=*ks1;
  nxyz[7]=*ks2;



  binfile=fopen(file,"wb");
  if(binfile==NULL){
    printf("*** warning:  unable to write to %s\n",file);
    return;
  }
  fwrite(nxyz,  4,1,binfile);
  fwrite(nxyz+1,4,7,binfile);
  fclose(binfile);

  strcpy(textfilename,file);
  strcat(textfilename,".sz");
  textfile=fopen(textfilename,"w");
  if(textfile!=NULL){
    fprintf(textfile,"%i\n",VERSION);
    fclose(textfile);
  }
  else{
    printf("*** warning:  unable to write to %s\n",textfilename);
  }
}

// this code is only used by FDS - needs to be updated if FDS does the ZLIB compression of 3d smoke files

/* ------------------ CCsmoke3dtofile ------------------------ */

void CCsmoke3dtofile(char *file, float *time, float *dx, int *type, float *xyz, int *nx, int *ny, int *nz){

  FILE *binfile,*textfile;
  unsigned char *buffer_in, *buffer_out;
  int nchars_in, nchars_out;
  int i;
  double xtype;
  int nchars[2];
  char textfilename[1024];
  int nxyz;
  double factor;

#define SOOT 1
#define FIRE 2
#define WATER 3

  nxyz=(*nx)*(*ny)*(*nz);
  if(nxyz<1)return;

  NewMemory((void **)&buffer_in,nxyz);
  if(buffer_in==NULL)return;

  NewMemory((void **)&buffer_out,nxyz);
  if(buffer_out==NULL){
    free(buffer_in);
    return;
  }

  binfile=fopen(file,"ab");
  if(binfile==NULL){
    free(buffer_in);
    free(buffer_out);
    return;
  }

  nchars_in=nxyz;
  switch (*type){
  case SOOT:
    /*xtype = 9.5;*/          // from john widmann
    xtype = 7.6;          
    factor = -xtype*(*dx)/1000.0;
    for(i=0;i<nxyz;i++){
      if(*xyz<0.0)*xyz=0.0;
      buffer_in[i]=254*(1.0-exp( factor*(*xyz++)) );
    }

    break;
  case FIRE:
    for(i=0;i<nxyz;i++){
      if(*xyz<0.0)*xyz=0.0;
      if(*xyz>1200.0)*xyz=1200.0;
      buffer_in[i]=254*(*xyz/1200.0);
      xyz++;
    }

    break;
  case WATER:
    factor=1.0/(0.1*0.5*(*dx));
    for(i=0;i<nxyz;i++){
      *xyz=*xyz-0.003;
      if(*xyz<0.0)*xyz=0.0;
      buffer_in[i]=254*(1.0-pow(0.5,*xyz*factor));
      xyz++;
    }

    break;
  default:
    ASSERT(0);
  }

  nchars_out=rle(buffer_in,nchars_in,buffer_out);

  nchars[0]=nchars_in;
  nchars[1]=nchars_out;

  fwrite(time,4,1,binfile);
  fwrite(nchars,4,2,binfile);
  if(nchars_out>0)fwrite(buffer_out,1,nchars_out,binfile);

  FREEMEMORY(buffer_in);
  FREEMEMORY(buffer_out);
  fclose(binfile);

  strcpy(textfilename,file);
  strcat(textfilename,".sz");
  textfile=fopen(textfilename,"a");
  if(textfile!=NULL){
    fprintf(textfile,"%f %i %i\n",*time,nchars_in,nchars_out);
    fclose(textfile);
  }
}

/* ------------------ compreess_svzip2 ------------------------ */

void compress_svzip2(void){
  char shellcommand[1024];

  printf("Compressing...\n");
  compress_onoff(0);

  writeini(LOCAL_INI);

// surround smokezip path name with "'s so that the system call can handle imbedded blanks

  strcpy(shellcommand,"\"");
  strcat(shellcommand,smokezippath);
  strcat(shellcommand,"\" ");
  if(overwrite_all==1){
    strcat(shellcommand," -f ");
  }
  if(erase_all==1){
    strcat(shellcommand," -c ");
  }
  if(compress_autoloaded==1){
    strcat(shellcommand," -auto ");
  }
  strcat(shellcommand," ");
  strcat(shellcommand,smvfilename);

  printf("Executing shell command: %s\n",shellcommand);
  system(shellcommand);
  updatesmoke3dmenulabels();
  updatepatchmenulabels();
  compress_onoff(1);
  updatemenu=1;
  printf("Compression completed\n");
}
