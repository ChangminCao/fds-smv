// $Date$ 
// $Revision$
// $Author$

#include "options.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "zlib.h"
#include "egz_stdio.h"
#include "svzip.h"
#include "lightsmoke.h"
#include "MALLOC.h"

// svn revision character string
char CNV3dsmoke_revision[]="$Revision$";

/* ------------------ convert_3dsmoke ------------------------ */

void convert_3dsmoke(smoke3d *smoke3di, int *thread_index){
  unsigned char *full_alphabuffer=NULL, *compressed_alphabuffer=NULL;
  FILE *smoke3dstream=NULL,*smoke3dsizestream=NULL;
  EGZ_FILE *SMOKE3DFILE=NULL;
  char smoke3dfile_svz[1024], smoke3dsizefile_svz[1024];
  int nxyz[9];
  int nx, ny, nz;
  int version;
  int buffersize;
  unsigned int sizebefore, sizeafter;
  int count;
  float time;
  int nchars[2];
  int nfull_file,nfull_data;
  int ncompressed_rle;
  uLongf ncompressed_zlib;
  int returncode;
  int percent_done;
  int percent_next=10;
  long data_loc;
  char *smoke3dfile;
  float time_max;
  radiancedata radianceinfo;
  unsigned char *radiance, *opacity;

  smoke3dfile=smoke3di->file;
  smoke3di->compressed=0;

//int compress (Bytef *dest,   uLongf *destLen, const Bytef *source, uLong sourceLen);
//int uncompress (Bytef *dest,   uLongf *destLen, const Bytef *source, uLong sourceLen);

  if(GLOBcleanfiles==0){
#ifdef pp_THREAD
    int fileindex;

    fileindex = smoke3di + 1 - smoke3dinfo;
    sprintf(threadinfo[*thread_index].label,"s3d %i",fileindex);
#else
    printf("Compressing 3D smokefile %s\n",smoke3dfile);
#endif
  }

  if(getfileinfo(smoke3dfile,NULL,NULL)!=0){
    printf("  %s does not exist\n",smoke3dfile);
    return;
  }
  SMOKE3DFILE=EGZ_FOPEN(smoke3dfile,"rb",0,2);
  if(SMOKE3DFILE==NULL){
    printf("  %s could not be opened\n",smoke3dfile);
    return;
  }

  // name 3d smoke flie

  if(GLOBdestdir!=NULL){
    strcpy(smoke3dfile_svz,GLOBdestdir);
    strcat(smoke3dfile_svz,smoke3di->filebase);
  }
  else{
    strcpy(smoke3dfile_svz,smoke3di->file);
  }
  strcat(smoke3dfile_svz,".svz");

  // name size file

  if(GLOBdestdir!=NULL){
    strcpy(smoke3dsizefile_svz,GLOBdestdir);
    strcat(smoke3dsizefile_svz,smoke3di->filebase);
  }
  else{
    strcpy(smoke3dsizefile_svz,smoke3di->file);
  }
  strcat(smoke3dsizefile_svz,".szz");

  // remove files if clean option is set

  if(GLOBcleanfiles==1){
    smoke3dstream=fopen(smoke3dfile_svz,"rb");
    if(smoke3dstream!=NULL){
      fclose(smoke3dstream);
      printf("  Removing %s.\n",smoke3dfile_svz);
      UNLINK(smoke3dfile_svz);
      LOCK_COMPRESS;
      GLOBfilesremoved++;
      UNLOCK_COMPRESS;
    }
    smoke3dsizestream=fopen(smoke3dsizefile_svz,"r");
    if(smoke3dsizestream!=NULL){
      fclose(smoke3dsizestream);
      printf("  Removing %s.\n",smoke3dsizefile_svz);
      UNLINK(smoke3dsizefile_svz);
      LOCK_COMPRESS;
      GLOBfilesremoved++;
      UNLOCK_COMPRESS;
    }
    return;
  }

  if(GLOBoverwrite_s==0){
    smoke3dstream=fopen(smoke3dfile_svz,"rb");
    if(smoke3dstream!=NULL){
      fclose(smoke3dstream);
      printf("  %s exists.\n",smoke3dfile_svz);
      printf("     Use the -f option to overwrite smokezip compressed files\n");
      return;
    }
  }

  smoke3dsizestream=fopen(smoke3dsizefile_svz,"w");
  smoke3dstream=fopen(smoke3dfile_svz,"wb");
  if(smoke3dstream==NULL||smoke3dsizestream==NULL
    ){
    if(smoke3dstream==NULL){
      printf("  3dsmoke file, %s, could not be opened for output\n",smoke3dfile_svz);
    }
    if(smoke3dsizestream==NULL){
      printf("  3dsmoke size file, %s, could not be opened for output\n",smoke3dsizefile_svz);
    }
    if(smoke3dsizestream!=NULL)fclose(smoke3dsizestream);
    if(smoke3dstream!=NULL)fclose(smoke3dstream);
    EGZ_FCLOSE(SMOKE3DFILE);
    return;
  }

  EGZ_FREAD(nxyz,4,8,SMOKE3DFILE);

  nxyz[0] = 1;
  version = nxyz[1];
  if(version==1){
    printf("  already compressed\n");
    EGZ_FCLOSE(SMOKE3DFILE);
    fclose(smoke3dstream);
    fclose(smoke3dsizestream);
    return;
  }

  version=1;

  nxyz[1]=version;
  fwrite(nxyz,4,8,smoke3dstream);
  fprintf(smoke3dsizestream,"%i\n",version);

  nx = nxyz[3]-nxyz[2]+1;
  ny = nxyz[5]-nxyz[4]+1;
  nz = nxyz[7]-nxyz[6]+1;
  buffersize=2*(1.01*nx*ny*nz+600);
  smoke3di->nx=nx;
  smoke3di->ny=ny;
  smoke3di->nz=nz;
  smoke3di->ncompressed_lighting_zlib=buffersize;

  NewMemory((void **)&full_alphabuffer,buffersize);
  NewMemory((void **)&compressed_alphabuffer,buffersize);

  {
    float xyzbar0[3], xyzbar[3], dxyz[3];
    int ijkbar[3];
    
    opacity=full_alphabuffer;
    radiance = opacity + nx*ny*nz;

    ijkbar[0]=nx;
    ijkbar[1]=ny;
    ijkbar[2]=nz;
    xyzbar0[0]=smoke3di->smokemesh->xbar0;
    xyzbar0[1]=smoke3di->smokemesh->ybar0;
    xyzbar0[2]=smoke3di->smokemesh->zbar0;
    xyzbar[0]=smoke3di->smokemesh->xbar;
    xyzbar[1]=smoke3di->smokemesh->ybar;
    xyzbar[2]=smoke3di->smokemesh->zbar;
    dxyz[0]=smoke3di->smokemesh->dx;
    dxyz[1]=smoke3di->smokemesh->dy;
    dxyz[2]=smoke3di->smokemesh->dz;
    setup_radiancemap(&radianceinfo,ijkbar,xyzbar0,xyzbar,dxyz,radiance,opacity);
  }

  count=-1;
  sizebefore=8;
  sizeafter=8;
  time_max=-1000000.0;
  for(;;){
    int nlight_data;
    
    EGZ_FREAD(&time,4,1,SMOKE3DFILE);
    if(EGZ_FEOF(SMOKE3DFILE)!=0)break;

    EGZ_FREAD(nchars,4,2,SMOKE3DFILE);
    nfull_file=nchars[0];
    ncompressed_rle=nchars[1];

    // read compressed frame

    EGZ_FREAD(compressed_alphabuffer,ncompressed_rle,1,SMOKE3DFILE);

    if(time<time_max)continue;
    count++;

    sizebefore+=12+ncompressed_rle;

    if(count%GLOBsmoke3dzipstep!=0)continue;
    time_max=time;

    // uncompress frame data (from RLE format)

    nfull_data=irle(compressed_alphabuffer, ncompressed_rle, full_alphabuffer);
    CheckMemory;
    if(nfull_file!=nfull_data){
      printf("  ***warning frame size expected=%i frame size found=%i\n",nfull_file,nfull_data);
    }

    if(GLOBdoit_lighting==1&&smoke3di->is_soot==1){
      build_radiancemap(&radianceinfo);
      nfull_data+=nx*ny*nz;
    }

    // compress frame data (into ZLIB format)

    ncompressed_zlib=buffersize;
    returncode=compress(compressed_alphabuffer, &ncompressed_zlib, full_alphabuffer, nfull_data);
    CheckMemory;
    if(returncode!=0){
      printf("  ***warning zlib compressor failed - frame %f\n",time);
    }

    data_loc=EGZ_FTELL(SMOKE3DFILE);
    percent_done=100.0*(float)data_loc/(float)smoke3di->filesize;
#ifdef pp_THREAD
    threadinfo[*thread_index].stat=percent_done;
    if(percent_done>percent_next){
      LOCK_PRINT;
      print_thread_stats();
      UNLOCK_PRINT;
      percent_next+=10;
    }
#else
    if(percent_done>percent_next){
      printf(" %i%s",percent_next,GLOBpp);
      fflush(stdout);
      percent_next+=10;
    }
#endif

    // write out new entries in the size (sz) file

    nchars[0]=nfull_data;
    if(GLOBdoit_lighting==1&&smoke3di->is_soot==1){
      nchars[1]=-ncompressed_zlib;
    }
    else{
      nchars[1]=ncompressed_zlib;
    }
    fwrite(&time,4,1,smoke3dstream);
    fwrite(nchars,4,2,smoke3dstream);
    if(ncompressed_zlib>0)fwrite(compressed_alphabuffer,1,ncompressed_zlib,smoke3dstream);
    sizeafter+=12+ncompressed_zlib;

// time, nframeboth, ncompressed_rle, ncompressed_zlib, nlightdata
    nlight_data=nfull_data-nfull_file;
    if(GLOBdoit_lighting==1&&smoke3di->is_soot==1)nlight_data=-nlight_data;
    fprintf(smoke3dsizestream,"%f %i %i %i %i\n",time,nfull_data,ncompressed_rle,(int)ncompressed_zlib,nlight_data);
  }
#ifdef pp_THREAD
  {
    char before_label[256],after_label[256];
  
    getfilesizelabel(sizebefore,before_label);
    getfilesizelabel(sizeafter,after_label);
    smoke3di->compressed=1;
    sprintf(smoke3di->summary,"compressed from %s to %s (%4.1f%s reduction)",before_label,after_label,(float)sizebefore/(float)sizeafter,GLOBx);
    fflush(stdout);
    threadinfo[*thread_index].stat=-1;
  }
#else
  printf(" 100%s completed\n",GLOBpp);
  printf("  records=%i, ",count);
  {
    char before_label[256],after_label[256];
  
    getfilesizelabel(sizebefore,before_label);
    getfilesizelabel(sizeafter,after_label);

    printf("Sizes: original=%s, ",before_label);

    printf("compressed=%s (%4.1f%s reduction)\n\n",after_label,(float)sizebefore/(float)sizeafter,GLOBx);
    fflush(stdout);
  }
#endif

  // close files and free buffers

  EGZ_FCLOSE(SMOKE3DFILE);

  fclose(smoke3dstream);
  fclose(smoke3dsizestream);
  FREEMEMORY(full_alphabuffer);
  FREEMEMORY(compressed_alphabuffer);
}

/* ------------------ convert_smoke3ds ------------------------ */

void *compress_smoke3ds(void *arg){
  int i;
  smoke3d *smoke3di;
  int *thread_index;

  thread_index = (int *)arg;

  for(i=0;i<nsmoke3dinfo;i++){
    smoke3di = smoke3dinfo + i;
    if(GLOBautozip==1&&smoke3di->autozip==0)continue;
    LOCK_SMOKE;
    if(smoke3di->inuse==1){
      UNLOCK_SMOKE;
      continue;
    }
    smoke3di->inuse=1;
    UNLOCK_SMOKE;
    convert_3dsmoke(smoke3di,thread_index);
    CheckMemory;
  }
  return NULL;
}
