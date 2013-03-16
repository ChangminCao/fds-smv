// $Date$ 
// $Revision$
// $Author$

#include "options.h"
#define INMAIN
#include "zlib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "svzip.h"
#include "MALLOC.h"

//dummy change to bump version number to 1.4.5
//dummy change to force revision update

// svn revision character string
char main_revision[]="$Revision$";


/* ------------------ main ------------------------ */

int main(int argc, char **argv){

//  Bytef *source,*sourcecheck,*dest;
//  int sourceLen, destLen;
//  int returncode;

  char *arg;

  char *filebase;
  int filelen;
  char smvfile[1024];
  char smzlogfile[1024];
  char smvfilebase[1024];
  char *ext;
  char inifile[1024];
  char inifilebase[1024];
  char *prog;
  int i;
  int endian_fds;
  int endian_info;
  int redirect=0;

  set_outstream(stdout);
  initMALLOC();
  GLOBdoit_lighting=0;
  GLOBdoit_smoke3d=1;
  GLOBdoit_boundary=1;
  GLOBdoit_slice=1;
  GLOBdoit_volslice=1;
#ifdef pp_PLOT3D
  GLOBdoit_plot3d=1;
#else
  GLOBdoit_plot3d=0;
#endif
#ifdef pp_PART2
  GLOBdoit_particle=0;
#endif

#ifdef pp_KDTEST
  test_kd();
  exit(0);
#endif
  strcpy(GLOBpp,"%");
  strcpy(GLOBx,"X");
  GLOBfirst_initsphere=1;
  GLOBfirst_slice=1;
  GLOBfirst_patch=1;
  GLOBfirst_plot3d=1;
  GLOBfirst_part2iso=1;
  GLOBfirst_part2iso_smvopen=1;
#ifdef pp_THREAD
  mt_nthreads=2;
#endif
  GLOBframeskip=-1;
  GLOBno_chop=0;
  GLOBautozip=0;
  GLOBmake_demo=0;
  GLOBendf=0;
  GLOBsyst=0;
  GLOBendianfile=NULL;
  GLOBdestdir=NULL;
  GLOBsourcedir=NULL;
  endianswitch=-1;
  GLOBoverwrite_b=0;
  GLOBoverwrite_s=0;
  GLOBget_bounds=0;
  GLOBget_slice_bounds=0;
  GLOBget_plot3d_bounds=0;
  GLOBget_boundary_bounds=0;
#ifdef pp_PART
  GLOBget_part_bounds=0;
  GLOBpartfile2iso=0;
#endif
  GLOBoverwrite_slice=0;
  GLOBoverwrite_volslice=0;
  GLOBoverwrite_plot3d=0;
  endian_info=0;
  GLOBcleanfiles=0;
  GLOBsmoke3dzipstep=1;
  GLOBboundzipstep=1;
  GLOBslicezipstep=1;
  GLOBfilesremoved=0;

  npatchinfo=0;
  nsmoke3dinfo=0;
#ifdef pp_PART
  npartinfo=0;
  npartclassinfo=0;
  partinfo=NULL;
  partclassinfo=NULL;
  maxpart5propinfo=0;
  npart5propinfo=0;
#endif
  nsliceinfo=0;
  sliceinfo=NULL;
  nmeshes=0;

  patchinfo=NULL;
  smoke3dinfo=NULL;

  prog=argv[0];
  filebase=NULL;
  if(argc==1){
    version();
    return 1;
  }

  for(i=1;i<argc;i++){
    int lenarg;
    int lenarg2;
    char *arg2;

    arg=argv[i];
    lenarg=strlen(arg);
    if(arg[0]=='-'&&lenarg>1){
      switch(arg[1]){
      case 'a':
        GLOBautozip=1;
        break;
      case 'b':
        if(strcmp(arg,"-bounds")==0){
          GLOBget_bounds=1;
          GLOBget_slice_bounds=1;
#ifdef pp_PLOT3D
          GLOBget_plot3d_bounds=1;
#endif
          GLOBget_boundary_bounds=1;
#ifdef pp_PART
          GLOBget_part_bounds=1;
#endif
        }
        else if(strcmp(arg,"-bb")==0){
          GLOBget_boundary_bounds=1;
        }
        else if(strcmp(arg,"-bs")==0){
          GLOBget_slice_bounds=1;
        }
#ifdef pp_PLOT3D
        else if(strcmp(arg,"-bp")==0){
          GLOBget_plot3d_bounds=1;
        }
#endif
#ifdef pp_PART2
        else if(strcmp(arg,"-bP")==0){
          GLOBget_part_bounds=1;
        }
#endif
        else{
          GLOBoverwrite_b=1;
        }
        break;
#ifdef pp_PART2
      case 'y':
        if(strcmp(arg,"-yP")==0){
          GLOBdoit_particle=1;
        }
        break;
#endif
      case 'l':
        GLOBdoit_lighting=1;
        break;
      case 'n':
        if(strcmp(arg,"-n3")==0){
          GLOBdoit_smoke3d=0;
        }
        else if(strcmp(arg,"-nb")==0){
          GLOBdoit_boundary=0;
        }
#ifdef pp_PLOT3D
        else if(strcmp(arg,"-np")==0){
          GLOBdoit_plot3d=0;
        }
#endif
#ifdef pp_PART2
        else if(strcmp(arg,"-nP")==0){
          GLOBdoit_particle=0;
        }
#endif
        else if(strcmp(arg,"-ns")==0){
          GLOBdoit_slice=0;
        }
        else if(strcmp(arg,"-nvs")==0){
          GLOBdoit_volslice=0;
        }
        else if(strcmp(arg,"-no_chop")==0){
          GLOBno_chop=1;
        }
        break;
      case '2':
        GLOBoverwrite_slice=1;
        break;
      case '3':
        GLOBoverwrite_volslice=1;
        GLOBoverwrite_s=1;
        break;
#ifdef  pp_PART2
      case 'P':
        GLOBoverwrite_part=1;
        break;
#endif
      case 'p':
        if(strcmp(arg,"-part2iso")==0){
          GLOBpartfile2iso=1;
        }
#ifdef pp_PLOT3D
        else{
          GLOBoverwrite_plot3d=1;
        }
#endif
        break;
      case 'f':
        GLOBoverwrite_b=1;
        GLOBoverwrite_s=1;
        GLOBoverwrite_slice=1;
        GLOBoverwrite_volslice=1;
#ifdef pp_PLOT3D
        GLOBoverwrite_plot3d=1;
#endif
#ifdef pp_PART2
        GLOBoverwrite_part=1;
#endif
        break;
      case 'c':
        GLOBcleanfiles=1;
        break;
      case 'e':
        endian_info=1;
        break;
      case 'r':
        redirect=1;
        break;
      case 's':
        if(i+1>=argc)break;
        if(lenarg==2){
            lenarg2=strlen(argv[i+1]);
            NewMemory((void **)&GLOBsourcedir,lenarg2+2);
            strcpy(GLOBsourcedir,argv[i+1]);
            if(GLOBsourcedir[lenarg2-1]!=dirseparator[0]){
              strcat(GLOBsourcedir,dirseparator);
            }
            if(getfileinfo(GLOBsourcedir,NULL,NULL)!=0){
              fprintf(stderr,"*** Warning: The source directory specified, %s, does not exist or cannot be accessed\n",GLOBsourcedir);
              return 1;
            }
           i++;
        }
        else if(strcmp(arg,"-skip")==0){
          GLOBframeskip=-1;
          arg2=argv[i+1];
          sscanf(arg2,"%i",&GLOBframeskip);
          if(GLOBframeskip>0){
            GLOBslicezipstep=GLOBframeskip;
            GLOBsmoke3dzipstep=GLOBframeskip;
            GLOBboundzipstep=GLOBframeskip;
          }
          i++;
        }
        break;
      case 'd':
        if(strcmp(arg,"-demo")==0){
          GLOBautozip=1;
          GLOBmake_demo=1;
          break;
        }
        if(i+1<argc){
          lenarg2=strlen(argv[i+1]);
          NewMemory((void **)&GLOBdestdir,lenarg2+2);
          strcpy(GLOBdestdir,argv[i+1]);
          if(GLOBdestdir[lenarg2-1]!=dirseparator[0]){
            strcat(GLOBdestdir,dirseparator);
          }
 //         if(getfileinfo(GLOBdestdir,NULL,NULL)!=0){
 //           fprintf(stderr,"*** Warning: The destination directory %s does not exist or cannot be accessed\n",GLOBdestdir);
 //           return 1;
 //         }
          i++;
        }
        break;
#ifdef pp_THREAD
      case 't':
        mt_compress=1;
        if(i+1<argc){
          arg2=argv[i+1];
          sscanf(arg2,"%i",&mt_nthreads);
          if(mt_nthreads<1)mt_nthreads=1;
          if(mt_nthreads>NTHREADS_MAX)mt_nthreads=NTHREADS_MAX;
          i++;
        }
        break;
#endif
      case 'h':
        usage(prog);
        return 1;
      case 'v':
        version();
        return 1;
      default:
        usage(prog);
        return 1;
      }
    }
    else{
      if(filebase==NULL){
        filebase=argv[i];
      }
    }
  }

#ifdef pp_THREAD
  if(GLOBcleanfiles==1)mt_nthreads=1;
#endif

  // construct smv filename
  
  if(filebase==NULL){
    usage(prog);
    return 1;
  }
#ifdef pp_THREAD
  init_pthread_mutexes();
#endif
  filelen=strlen(filebase);
  if(filelen>4){
    ext=filebase+filelen-4;
    if(strcmp(ext,".smv")==0){
      ext[0]=0;
      filelen=strlen(filebase);
    }
  }
  if(GLOBsourcedir==NULL){
    strcpy(smvfile,filebase);
    strcpy(smzlogfile,filebase);
  }
  else{
    strcpy(smvfile,GLOBsourcedir);
    strcat(smvfile,filebase);
    strcpy(smzlogfile,GLOBsourcedir);
    strcat(smzlogfile,filebase);
  }
  strcpy(smvfilebase,filebase);
  if(GLOBpartfile2iso==1||GLOBcleanfiles==1){
    strcpy(GLOBsmvisofile,smvfile);
    strcat(GLOBsmvisofile,".isosmv");
  }

  strcat(smvfile,".smv");
  if(redirect==1){
    if(GLOBsourcedir==NULL){
      strcpy(smzlogfile,filebase);
    }
    else{
      strcpy(smzlogfile,GLOBsourcedir);
      strcat(smzlogfile,filebase);
    }
    strcat(smzlogfile,".smzlog");
    SMZLOG_STREAM=fopen(smzlogfile,"w");
    if(SMZLOG_STREAM!=NULL){
      set_outstream(SMZLOG_STREAM);
    }
  }
  
  // construct ini file name

  strcpy(inifile,smvfile);
  inifile[strlen(inifile)-4]=0;
  strcat(inifile,".ini");
  strcpy(inifilebase,filebase);
  strcat(inifilebase,".ini");

  strcpy(GLOBendianfilebase,"");

  // make sure smv file name exists

  if(getfileinfo(smvfile,NULL,NULL)!=0){
    fprintf(stderr,"*** Error: The file %s does not exist\n",smvfile);
    return 1;
  }

  // make sure smv file can be opened

  if(readsmv(smvfile)!=0)return 1;

#ifdef pp_PLOT3D
  if(nplot3dinfo>0){
    plot3dinfo[0].dup=0;
    for(i=1;i<nplot3dinfo;i++){
      plot3d *plot3di; 

      plot3di = plot3dinfo + i;

      plot3di->dup=0;
      plot3ddup(plot3di,i);
    }
  }
#endif
  if(npatchinfo>0){
    patchinfo[0].dup=0;
    for(i=1;i<npatchinfo;i++){
      patch *patchi; 

      patchi = patchinfo + i;

      patchi->dup=0;
      patchdup(patchi,i);
    }
  }
  if(nsliceinfo>0){
    sliceinfo[0].dup=0;
    for(i=1;i<nsliceinfo;i++){
      slice *slicei; 

      slicei = sliceinfo + i;

      slicei->dup=0;
      slicedup(slicei,i);
    }
  }

  if(getendian()==1){
      fprintf(alt_stdout,"Smokezip running on a big endian computer.\n");
  }
  else{
      fprintf(alt_stdout,"Smokezip running on a little endian computer.\n");
  }
  if(GLOBendf==0&&GLOBsyst==0){
    fprintf(stderr,"Warning: casename.end file is missing.  Endianness of\n");
    fprintf(stderr,"         FDS boundary file data is unknown.\n");
    if(getendian()==1){
      fprintf(stderr,"         Assuming FDS boundary data is big endian - \n");
    }
    if(getendian()==0){
      fprintf(stderr,"         Assuming FDS boundary data is little endian - \n");
    }
    fprintf(stderr,"         or equivalently assuming FDS and Smokezip are\n");
    fprintf(stderr,"         being run on the same type of computer\n");
    endianswitch=0;
  }
  else{
    endian_fds=getendian()+endianswitch;
    if(endian_fds==2)endian_fds=0;
    if(endian_fds==1){
      fprintf(alt_stdout,"FDS was run on a big endian computer. \n\n");
    }
    else{
      fprintf(alt_stdout,"FDS was run on a little endian computer.\n\n");
    }
  }
  if(endian_info==1)return 0;

  readini(inifile);

#ifdef pp_THREAD
  mt_compress_all();
#else
  compress_all(NULL);
#endif

  if(GLOBcleanfiles==0&&GLOBdestdir!=NULL){
    fprintf(alt_stdout,"Copying .smv, .ini and .end files to %s directory\n",GLOBdestdir);
    filecopy(GLOBdestdir,smvfile,smvfilebase);
    filecopy(GLOBdestdir,inifile,inifilebase);
    filecopy(GLOBdestdir,GLOBendianfile,GLOBendianfilebase);
  }
  if(GLOBcleanfiles==1&&GLOBfilesremoved==0){
    fprintf(alt_stdout,"No compressed files were removed\n");
  }
  if(GLOBmake_demo==1){
    makesvd(GLOBdestdir,smvfile);
  }
  return 0;
}

/* ------------------ mt_compress_all ------------------------ */
#ifdef pp_THREAD
void mt_compress_all(void){
  int i;
  pthread_t *thread_ids;
  int *index;

  NewMemory((void **)&thread_ids,mt_nthreads*sizeof(pthread_t));
  NewMemory((void **)&index,mt_nthreads*sizeof(int));
  NewMemory((void **)&threadinfo,mt_nthreads*sizeof(threaddata));

  for(i=0;i<mt_nthreads;i++){
    index[i]=i;
    pthread_create(&thread_ids[i],NULL,compress_all,&index[i]);
    threadinfo[i].stat=-1;
  }

  for(i=0;i<mt_nthreads;i++){
    pthread_join(thread_ids[i],NULL);
  }

  print_summary();
  FREEMEMORY(thread_ids);
  FREEMEMORY(index);
  FREEMEMORY(threadinfo);
}
#endif

/* ------------------ compress_all ------------------------ */

void *compress_all(void *arg){
  int *thread_index;

  thread_index=(int *)(arg);
  if(GLOBdoit_boundary==1)compress_patches(thread_index);
  if(GLOBdoit_slice==1)compress_slices(thread_index);
  if(GLOBdoit_volslice==1)compress_volslices(thread_index);
  if(GLOBdoit_smoke3d==1)compress_smoke3ds(thread_index);
#ifdef pp_PLOT3D
  if(GLOBdoit_plot3d==1)compress_plot3ds(thread_index);
#endif
  convert_parts2iso(thread_index);
#ifdef pp_PART2
  if(GLOBdoit_particle)compress_parts(NULL);
#endif
  return NULL;
}
       
/* ------------------ makesvd ------------------------ */

void makesvd(char *in_dir, char *smvfile){
  char *file_out=NULL,*svd;

  if(smvfile==NULL)return;
  svd=strrchr(smvfile,'.');
  if(svd==NULL)return;

  NewMemory((void **)&file_out,(svd-smvfile)+4+1);
  strcat(file_out,smvfile);
  strcpy(svd,".svd");

  if(in_dir==NULL){
    filecopy(".",smvfile,file_out);
  }
  else{
    filecopy(in_dir,smvfile,file_out);
  }

}
       
/* ------------------ usage ------------------------ */

void usage(char *prog){
  char smv_version[100];
  int svn_num;

  getPROGversion(smv_version);  // get Smokeview version (ie 5.x.z)
  svn_num=getmaxrevision();    // get svn revision number

  fprintf(alt_stdout,"\n");
  fprintf(alt_stdout,"  smokezip %s(%i) - %s\n\n",smv_version,svn_num,__DATE__);
  fprintf(alt_stdout,"  Compress FDS data files\n\n");
  fprintf(alt_stdout,"  %s [options] casename\n\n",prog);
  fprintf(alt_stdout,"  casename - Smokeview .smv file for case to be compressed\n\n");
  fprintf(alt_stdout,"options:\n");
  fprintf(alt_stdout,"  -c  - cleans or removes all compressed files\n");
#ifdef pp_THREAD
  fprintf(alt_stdout,"  -t nthread - Compress nthread files at a time (up to %i)\n",NTHREADS_MAX);
#endif
  fprintf(alt_stdout,"overwrite options:\n");
  fprintf(alt_stdout,"  -f  - overwrites all compressed files\n");
  fprintf(alt_stdout,"  -2  - overwrites 2d slice compressed files\n");
  fprintf(alt_stdout,"  -3  - overwrites 3d smoke files\n");
  fprintf(alt_stdout,"  -b  - overwrites boundary compressed files\n");
#ifdef pp_PLOT3D
  fprintf(alt_stdout,"  -p  - overwrites PLOT3D files\n");
#endif
#ifdef pp_PART2
  fprintf(alt_stdout,"  -P  - overwrites particle files\n");
#endif
#ifdef pp_PART
  fprintf(alt_stdout,"  -part2iso - generate isosurfaces from particle data\n");
#endif
  fprintf(alt_stdout,"bound options:\n");
  fprintf(alt_stdout,"  -bounds - estimate data bounds for all file types\n");
  fprintf(alt_stdout,"  -bb - estimate data bounds for boundary files\n");
  fprintf(alt_stdout,"  -bs - estimate data bounds for slice files\n");
  fprintf(alt_stdout,"  -no_chop - do not chop or truncate slice data.  Smokezip compresses\n");
  fprintf(alt_stdout,"        slice data truncating data above and below chop values\n");
  fprintf(alt_stdout,"        specified in the .ini file\n");
#ifdef pp_PLOT3D
  fprintf(alt_stdout,"  -bp - estimate data bounds for plot3d files\n");
#endif
#ifdef pp_PART2
  fprintf(alt_stdout,"  -bP - estimate data bounds for particle files\n");
#endif
  fprintf(alt_stdout,"compress options:\n");
  fprintf(alt_stdout,"  -n3 - do not compress 3d smoke files\n");
  fprintf(alt_stdout,"  -nb - do not compress boundary files\n");
#ifdef pp_PLOT3D
  fprintf(alt_stdout,"  -np - do not compress PLOT3D files\n");
#endif
  fprintf(alt_stdout,"  -ns - do not compress slice files\n");
#ifdef pp_PART2
  fprintf(alt_stdout,"  -nP - do not compress particle files\n");
  fprintf(alt_stdout,"  -yP - compress particle files\n");
#endif
  fprintf(alt_stdout,"output options:\n");
  fprintf(alt_stdout,"  -auto - compress only files that are auto-loaded by Smokeview\n");
  fprintf(alt_stdout,"  -d destdir - copies compressed files (and files needed by Smokeview\n");
  fprintf(alt_stdout,"        to view the case) to the directory destdir\n"); 
  fprintf(alt_stdout,"  -s GLOBsourcedir - specifies directory containing source files\n");
  fprintf(alt_stdout,"  -demo - Creates the files (compressed and .svd ) needed by the\n");
  fprintf(alt_stdout,"        Smokeview demonstrator mode.  Compresses files that are autoloaded, \n");
  fprintf(alt_stdout,"        uses (20.0,620.0) and (0.0,0.23) for temperature and oxygen bounds\n");
  fprintf(alt_stdout,"        and creates the .svd file which activates the Smokeview demonstrator\n");
  fprintf(alt_stdout,"        mode.\n");
  fprintf(alt_stdout,"  -skip skipval - skip frames when compressing files\n\n");
  fprintf(alt_stdout,"  -h  - display this message\n\n");
}
       
/* ------------------ usage ------------------------ */

void print_summary(void){
  int i;
  int nsum;

  fprintf(alt_stdout,"\n");
  nsum=0;
  for(i=0;i<nsliceinfo;i++){
    slice *slicei;

    slicei = sliceinfo + i;
    if(slicei->compressed==1)nsum++;
  }
  if(nsum>0){
    for(i=0;i<nsliceinfo;i++){
      slice *slicei;
      flowlabels *label;

      slicei = sliceinfo + i;
      if(slicei->compressed==0)continue;
      label=&slicei->label;
      fprintf(alt_stdout,"%s (%s)\n  %s\n",slicei->file,label->longlabel,slicei->summary);
      fprintf(alt_stdout,"  using: min=%f %s, max=%f %s \n\n",slicei->valmin,label->unit,slicei->valmax,label->unit);
    }
  }

  nsum=0;
  for(i=0;i<nsliceinfo;i++){
    slice *slicei;

    slicei = sliceinfo + i;
    if(slicei->vol_compressed==1)nsum++;
  }
  if(nsum>0){
    for(i=0;i<nsliceinfo;i++){
      slice *slicei;
      flowlabels *label;

      slicei = sliceinfo + i;
      if(slicei->vol_compressed==0)continue;
      label=&slicei->label;
      fprintf(alt_stdout,"%s (%s)\n  %s\n",slicei->file,label->longlabel,slicei->volsummary);
    }
  }

  nsum=0;
  for(i=0;i<nsmoke3dinfo;i++){
    smoke3d *smoke3di;

    smoke3di = smoke3dinfo + i;
    if(smoke3di->compressed==1)nsum++;
  }
  if(nsum>0){
    for(i=0;i<nsmoke3dinfo;i++){
      smoke3d *smoke3di;

      smoke3di = smoke3dinfo + i;
      if(smoke3di->compressed==0)continue;
      fprintf(alt_stdout,"%s\n  %s\n\n",smoke3di->file,smoke3di->summary);
    }
  }

  nsum=0;
  for(i=0;i<npatchinfo;i++){
    patch *patchi;

    patchi = patchinfo + i;
    if(patchi->compressed==1)nsum++;
  }
  if(nsum>0){
    for(i=0;i<npatchinfo;i++){
      patch *patchi;
      flowlabels *label;

      patchi = patchinfo + i;
      if(patchi->compressed==0)continue;
      label=&patchi->label;
      fprintf(alt_stdout,"%s (%s)\n  %s\n",patchi->file,label->longlabel,patchi->summary);
      fprintf(alt_stdout,"  using: min=%f %s, max=%f %s \n\n",patchi->valmin,label->unit,patchi->valmax,label->unit);
    }
  }

  nsum=0;
  for(i=0;i<npartinfo;i++){
    part *parti;

    parti = partinfo + i;
    if(parti->compressed2==1)nsum++;
  }
  if(nsum>0){
    for(i=0;i<npartinfo;i++){
      int j;
      part *parti;

      parti = partinfo + i;
      if(parti->compressed2==0)continue;
 
      fprintf(alt_stdout,"%s converted to:\n",parti->file);
      for(j=0;j<parti->nsummaries;j++){
        fprintf(alt_stdout,"  %s\n",parti->summaries[j]);
      }
      fprintf(alt_stdout,"\n");
    }
  }

}
