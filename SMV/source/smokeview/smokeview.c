// $Date$ 
// $Revision$
// $Author$

// svn revision character string
char smokeview_revision[]="$Revision$";

#include "options.h"
#include <stdio.h>  
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <sys/types.h>
#include <sys/stat.h>
#ifdef pp_OSX
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include "string_util.h"
#include "smokeviewvars.h"

#ifdef WIN32
#include <direct.h>
#endif

/* ----------------------- unsetClipPlanes ----------------------------- */

void unsetClipPlanes(void){
  if(xyz_clipplane==2){
    glDisable(GL_CLIP_PLANE0);
    glDisable(GL_CLIP_PLANE1);
    glDisable(GL_CLIP_PLANE2);
    glDisable(GL_CLIP_PLANE3);
    glDisable(GL_CLIP_PLANE4);
    glDisable(GL_CLIP_PLANE5);
  }
}

/* ----------------------- setClipPlanes ----------------------------- */

void setClipPlanes(int mode){
  static GLdouble clipplane_x[4], clipplane_y[4], clipplane_z[4];
  static GLdouble clipplane_X[4], clipplane_Y[4], clipplane_Z[4];

  if(mode==0&&xyz_clipplane==2)return;
  if(mode==1&&xyz_clipplane!=2)return;
  if(xyz_clipplane==0){
    glDisable(GL_CLIP_PLANE0);
    glDisable(GL_CLIP_PLANE1);
    glDisable(GL_CLIP_PLANE2);
    glDisable(GL_CLIP_PLANE3);
    glDisable(GL_CLIP_PLANE4);
    glDisable(GL_CLIP_PLANE5);
    return;
  }

  if(clip_x==1){
    clipplane_x[0]=1.0;
    clipplane_x[1]=0.0;
    clipplane_x[2]=0.0;
    clipplane_x[3]=-NORMALIZE_X(clip_x_val);
    glClipPlane(GL_CLIP_PLANE0,clipplane_x);
    glEnable(GL_CLIP_PLANE0);
  }
  else{
    glDisable(GL_CLIP_PLANE0);
  }

  if(clip_X==1){
    clipplane_X[0]=-1.0;
    clipplane_X[1]=0.0;
    clipplane_X[2]=0.0;
    clipplane_X[3]=NORMALIZE_X(clip_X_val);
    glClipPlane(GL_CLIP_PLANE3,clipplane_X);
    glEnable(GL_CLIP_PLANE3);
  }
  else{
    glDisable(GL_CLIP_PLANE3);
  }

  if(clip_y==1){
    clipplane_y[0]=0.0;
    clipplane_y[1]=1.0;
    clipplane_y[2]=0.0;
    clipplane_y[3]=-NORMALIZE_Y(clip_y_val);
    glClipPlane(GL_CLIP_PLANE1,clipplane_y);
    glEnable(GL_CLIP_PLANE1);
  }
  else{
    glDisable(GL_CLIP_PLANE1);
  }

  if(clip_Y==1){
    clipplane_Y[0]=0.0;
    clipplane_Y[1]=-1.0;
    clipplane_Y[2]=0.0;
    clipplane_Y[3]=NORMALIZE_Y(clip_Y_val);
    glClipPlane(GL_CLIP_PLANE4,clipplane_Y);
    glEnable(GL_CLIP_PLANE4);
  }
  else{
    glDisable(GL_CLIP_PLANE4);
  }

  if(clip_z==1){
    clipplane_z[0]=0.0;
    clipplane_z[1]=0.0;
    clipplane_z[2]=1.0;
    clipplane_z[3]=-NORMALIZE_Z(clip_z_val);
    glClipPlane(GL_CLIP_PLANE2,clipplane_z);
    glEnable(GL_CLIP_PLANE2);
  }
  else{
    glDisable(GL_CLIP_PLANE2);
  }

  if(clip_Z==1){
    clipplane_Z[0]=0.0;
    clipplane_Z[1]=0.0;
    clipplane_Z[2]=-1.0;
    clipplane_Z[3]=NORMALIZE_X(clip_Z_val);
    glClipPlane(GL_CLIP_PLANE5,clipplane_Z);
    glEnable(GL_CLIP_PLANE5);
  }
  else{
    glDisable(GL_CLIP_PLANE5);
  }
}

/* ------------------ snifferrors ------------------------ */

void _Sniff_Errors(char *whereat){
  int error;
  char *glu_error;
  while((error=glGetError())!=GL_NO_ERROR){
    glu_error=(char *)gluErrorString((unsigned int)error);
    fprintf(stderr,"*** Error: OpenGL error:%s, where:%s %i\n",
      glu_error,whereat,snifferrornumber);
      snifferrornumber++;
  }
}

/* ------------------ updateLights ------------------------ */

void updateLights(int pos){
  GLfloat ambientlight2[4], diffuselight2[4];
  int i;

  if(visLIGHT0==1&&visLIGHT1==1){
    for(i=0;i<3;i++){
      ambientlight2[i]=ambientlight[i]/2.0;
      diffuselight2[i]=diffuselight[i]/2.0;
    }
    ambientlight2[3]=1.0;
    diffuselight2[3]=1.0;
    glLightfv(GL_LIGHT0,GL_DIFFUSE,diffuselight2);
    glLightfv(GL_LIGHT0,GL_AMBIENT,ambientlight2);
    if(pos==1)glLightfv(GL_LIGHT0,GL_POSITION,light_position0);
    glEnable(GL_LIGHT0);

    glLightfv(GL_LIGHT1,GL_DIFFUSE,diffuselight2);
    glLightfv(GL_LIGHT1,GL_AMBIENT,ambientlight2);
    if(pos==1)glLightfv(GL_LIGHT1,GL_POSITION,light_position1);
    glEnable(GL_LIGHT1);

  }
  if(visLIGHT0==1&&visLIGHT1==0){
    diffuselight[3]=1.0;
    ambientlight[3]=1.0;
    glLightfv(GL_LIGHT0,GL_DIFFUSE,diffuselight);
    glLightfv(GL_LIGHT0,GL_AMBIENT,ambientlight);
    if(pos==1)glLightfv(GL_LIGHT0,GL_POSITION,light_position0);
    glEnable(GL_LIGHT0);

    glDisable(GL_LIGHT1);

  }
  if(visLIGHT0==0&&visLIGHT1==1){
    diffuselight[3]=1.0;
    ambientlight[3]=1.0;
    glEnable(GL_LIGHT0);
    glLightfv(GL_LIGHT0,GL_DIFFUSE,diffuselight);
    glLightfv(GL_LIGHT0,GL_AMBIENT,ambientlight);
    if(pos==1)glLightfv(GL_LIGHT0,GL_POSITION,light_position1);
    glDisable(GL_LIGHT1);
  }
  if(visLIGHT0==0&&visLIGHT1==0){
    glDisable(GL_LIGHT0);
    glDisable(GL_LIGHT1);
  }


  UpdateLIGHTS=0;

}

/* ------------------ antialias ------------------------ */

void antialias(int flag){
  if(antialiasflag==1){
    if(flag==1){
      glEnable(GL_LINE_SMOOTH);
      glEnable(GL_BLEND);
      glEnable(GL_POINT_SMOOTH);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      glHint(GL_LINE_SMOOTH_HINT,GL_DONT_CARE);
    }
    if(flag==0){
      glDisable(GL_LINE_SMOOTH);
      glDisable(GL_POINT_SMOOTH);
      glDisable(GL_BLEND);
    }
  }
}

/* ------------------ transparenton ------------------------ */

void transparenton(void){
  glEnable(GL_BLEND);
  glDepthMask(GL_FALSE);
  glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
}

/* ------------------ transparentoff ------------------------ */

void transparentoff(void){
  glDepthMask(GL_TRUE);
  glDisable(GL_BLEND);
}

/* ------------------ smv2quat ------------------------ */

void camera2quat(camera *ca, float *quat, float *rotation){
  if(ca->quat_defined==1){
    quat[0]=ca->quaternion[0];
    quat[1]=ca->quaternion[1];
    quat[2]=ca->quaternion[2];
    quat[3]=ca->quaternion[3];
  }
  else{
    float quat_temp[4];
    float azimuth, elevation,axis[3];

    azimuth = ca->az_elev[0]*DEG2RAD;
    elevation = (ca->az_elev[1])*DEG2RAD;

    axis[0]=1.0;
    axis[1]=0.0;
    axis[2]=0.0;

    angleaxis2quat(elevation,axis,quat_temp);

    axis[0]=0.0;
    axis[1]=0.0;
    axis[2]=1.0;

    angleaxis2quat(azimuth,axis,quat);

    mult_quat(quat_temp,quat,quat);
  }

  if(rotation!=NULL)quat2rot(quat,rotation);
}


/* ------------------ ResetView ------------------------ */

void ResetView(int option){
  int rotation_type_save;

  in_external=0;
  switch (option){
  case RESTORE_EXTERIOR_VIEW_ZOOM:
    break;
  case RESTORE_EXTERIOR_VIEW:
    in_external=1;
    rotation_type_save = camera_current->rotation_type;
    copy_camera(camera_current,camera_external);
    camera_current->rotation_type=rotation_type_save;
    if(camera_current->projection_type==1){
      camera_current->eye[1]=camera_current->isometric_y;
    }
    break;
  case RESTORE_INTERIOR_VIEW:
    rotation_type_save = camera_current->rotation_type;
    copy_camera(camera_current,camera_internal);
    camera_current->rotation_type=rotation_type_save;
    break;
  case RESTORE_SAVED_VIEW:
    copy_camera(camera_current,camera_save);
    break;
  case 3:
  case 4:
    ASSERT(FFALSE);
    break;
  default:
    ASSERT(FFALSE);
    break;
  }
  if(rotation_type==ROTATION_3AXIS){
    float azimuth, elevation,axis[3];
    float quat_temp[4];
    float x, y, z;
   
    azimuth = camera_current->az_elev[0]*DEG2RAD;
    elevation = camera_current->az_elev[1]*DEG2RAD;

    x = cos(azimuth);
    y = sin(azimuth);
    z = cos(elevation);

    axis[0]=0.0;
    axis[1]=0.0;
    axis[2]=1.0;

    angleaxis2quat(azimuth,axis,quat_temp);

    axis[0]=x;
    axis[1]=y;
    axis[2]=0.0;

    angleaxis2quat(acos(z),axis,quat_general);

    mult_quat(quat_temp,quat_general,quat_general);

    quat2rot(quat_general,quat_rotation);
  }
  if(option==RESTORE_EXTERIOR_VIEW_ZOOM)camera_current->zoom=zooms[zoomindex];
  zoom=camera_current->zoom;
  update_glui_zoom();
}

/* ------------------ parse_commandline ------------------------ */

void parse_commandline(int argc, char **argv){
  int i, len_casename;
  int iarg;
  size_t len_memory;
  char *argi;
  char SMVFILENAME[1024];
  int smv_parse;

  CheckMemory;
  partscale=a_partscale;
  zonescale=a_zonescale;

  if(argc==1){
  //  usage(argv);
    exit(1);
  }
  if(strncmp(argv[1],"-ini",3)==0){
    init_camera_list();
    InitOpenGL();
    UpdateRGBColors(COLORBAR_INDEX_NONE);
    writeini(GLOBAL_INI);
    exit(0);
  }

  if(strncmp(argv[1],"-ng_ini",6)==0){
    init_camera_list();
    use_graphics=0;
    UpdateRGBColors(COLORBAR_INDEX_NONE);
    writeini(GLOBAL_INI);
    exit(0);
  }
  strcpy(SMVFILENAME,"");
  smv_parse=0;
  for(iarg=1;iarg<argc;iarg++){
    argi=argv[iarg];
    if(strncmp(argi,"-",1)==0){
      if(
        strncmp(argi,"-points",7)==0||
        strncmp(argi,"-frames",7)==0||
#ifdef pp_LANG        
        strncmp(argi,"-lang",5)==0||
#endif        
        strncmp(argi,"-script",7)==0||
        strncmp(argi,"-startframe",11)==0||
        strncmp(argi,"-skipframe",10)==0||
        strncmp(argi,"-bindir",7)==0
        ){
        iarg++;
      }

      if(smv_parse==0)continue;
      if(smv_parse==1)break;
    }
    if(smv_parse==1)strcat(SMVFILENAME," ");
    smv_parse=1;
    strcat(SMVFILENAME,argi);
  }

  argi=SMVFILENAME;
#ifndef pp_OSX
  argi=lastname(argi);
#endif
  len_casename = (int) strlen(argi);
  CheckMemory;
  FREEMEMORY(fdsprefix);
  len_memory=len_casename+strlen(part_ext)+100;
  NewMemory((void **)&fdsprefix,(unsigned int)len_memory);
  STRCPY(fdsprefix,argi);
  FREEMEMORY(trainer_filename);
  FREEMEMORY(test_filename);
  FREEMEMORY(smoothblockage_filename);

  strcpy(input_filename_ext,"");

  if(len_casename>4){
    char *c_ext;

    c_ext=strrchr(argi,'.');
    if(c_ext!=NULL){
      STRCPY(input_filename_ext,c_ext);
      to_lower(input_filename_ext);

      if(c_ext!=NULL&&
        (strcmp(input_filename_ext,".smv")==0||
         strcmp(input_filename_ext,".svd")==0||
         strcmp(input_filename_ext,".smt")==0)
         ){
        c_ext[0]=0;
        STRCPY(fdsprefix,argi);
        FREEMEMORY(trainer_filename);
        NewMemory((void **)&trainer_filename,(unsigned int)(len_casename+7));
        STRCPY(trainer_filename,argi);
        STRCAT(trainer_filename,".svd");
        FREEMEMORY(test_filename);
        NewMemory((void **)&test_filename,(unsigned int)(len_casename+7));
        STRCPY(test_filename,argi);
        STRCAT(test_filename,".smt");
      }
    }
  }

  FREEMEMORY(log_filename);
  NewMemory((void **)&log_filename,len_casename+7+1);
  STRCPY(log_filename,fdsprefix);
  STRCAT(log_filename,".smvlog");

  FREEMEMORY(caseini_filename);
  NewMemory((void **)&caseini_filename,len_casename+strlen(ini_ext)+1);
  STRCPY(caseini_filename,fdsprefix);
  STRCAT(caseini_filename,ini_ext);

  FREEMEMORY(boundini_filename);
  NewMemory((void **)&boundini_filename,len_casename+5+1);
  STRCPY(boundini_filename,fdsprefix);
  STRCAT(boundini_filename,".bini");

  if(smv_filename==NULL){
    STRUCTSTAT statbuffer;

    NewMemory((void **)&smv_filename,(unsigned int)(len_casename+6));
    STRCPY(smv_filename,fdsprefix);
    STRCAT(smv_filename,".smv");
    {
      char scriptbuffer[1024];

      STRCPY(scriptbuffer,fdsprefix);
      STRCAT(scriptbuffer,".ssf");
      if(default_script==NULL&&STAT(scriptbuffer,&statbuffer)==0){
        default_script = insert_scriptfile(scriptbuffer);
      }
    }
  }
  if(smv_filename!=NULL){
    STRUCTSTAT statbuffer;

    FREEMEMORY(fds_filein);
    NewMemory((void **)&fds_filein,strlen(fdsprefix)+6);
    STRCPY(fds_filein,fdsprefix);
    STRCAT(fds_filein,".fds");
    if(STAT(fds_filein,&statbuffer)!=0){
      FREEMEMORY(fds_filein);
    }
  }
  if(fed_filename==NULL){
    STRCPY(fed_filename_base,fdsprefix);
    STRCAT(fed_filename_base,".fed_smv");
    fed_filename=get_filename(smokeviewtempdir,fed_filename_base,tempdir_flag);
  }
  if(stop_filename==NULL){
    NewMemory((void **)&stop_filename,(unsigned int)(len_casename+6));
    STRCPY(stop_filename,fdsprefix);
    STRCAT(stop_filename,".stop");
  }
  if(sliceinfo_filename==NULL){
    NewMemory((void **)&sliceinfo_filename,strlen(fdsprefix)+11+1);
    STRCPY(sliceinfo_filename,fdsprefix);
    STRCAT(sliceinfo_filename,"_slice.info");
  }

  // if smokezip created part2iso files then concatenate .smv entries found in the .isosmv file 
  // to the end of the .smv file creating a new .smv file.  Then read in that .smv file.

  {
    FILE *stream_iso=NULL;

    NewMemory((void **)&iso_filename,len_casename+7+1);
    STRCPY(iso_filename,fdsprefix);
    STRCAT(iso_filename,".isosmv");
    stream_iso=fopen(iso_filename,"r");
    if(stream_iso!=NULL){
      fclose(stream_iso);
    }
    else{
      FREEMEMORY(iso_filename);
    }
  }

  if(trainer_filename==NULL){
    NewMemory((void **)&trainer_filename,(unsigned int)(len_casename+6));
    STRCPY(trainer_filename,fdsprefix);
    STRCAT(trainer_filename,".svd");
  }
  if(test_filename==NULL){
    NewMemory((void **)&test_filename,(unsigned int)(len_casename+6));
    STRCPY(test_filename,fdsprefix);
    STRCAT(test_filename,".svd");
  }
  if(smoothblockage_filename==NULL){
    char filename_base[1024];

    STRCPY(filename_base,fdsprefix);
    STRCAT(filename_base,".sb");
    smoothblockage_filename=get_filename(smokeviewtempdir,filename_base,tempdir_flag);
  }

  for (i=1;i<argc;i++){
    if(strncmp(argv[i],"-",1)!=0)continue;
    if(strncmp(argv[i],"-ini",3)==0){
      writeini(GLOBAL_INI);
    }
    else if(strncmp(argv[i],"-update_bounds",14)==0){
      use_graphics=0;
      update_bounds=1;
    }
    else if(strncmp(argv[i],"-nogpu",6)==0){
      disable_gpu=1;
    }
    else if(strncmp(argv[i],"-demo",5)==0){
       demo_option=1;
    }
    else if(strncmp(argv[i],"-stereo",7)==0){
      stereoactive=1;
      showstereo=1;
      printf("stereo option activated\n");
    }
#ifdef pp_LANG
    else if(strncmp(argv[i],"-lang",5)==0){
      ++i;
      if(i<argc){
        int langlen;
        char *lang;

        FREEMEMORY(tr_name);
        lang=argv[i];
        langlen=strlen(lang);
        NewMemory((void **)&tr_name,langlen+48+1);
        strcpy(tr_name,lang);
        show_lang_menu=1;
      }
    }
#endif    
    else if(strncmp(argv[i],"-isotest",8)==0){
      isotest=1;
    }
#ifdef _DEBUG
    else if(strncmp(argv[i],"-tempdir",8)==0){
      tempdir_flag=1;
    }
#endif
    else if(strncmp(argv[i],"-h",2)==0){
      usage(argv);
      exit(0);
    }
    else if(strncmp(argv[i],"-noblank",8)==0){
      arg_iblank=1;
      use_iblank=0;
    }
    else if(strncmp(argv[i],"-blank",6)==0){
      arg_iblank=1;
      use_iblank=1;
    }
    else if(
      strncmp(argv[i],"-volrender",10)!=0&&(strncmp(argv[i],"-version",8)==0||strncmp(argv[i],"-v",2)==0)
      ){
      display_version_info();
      exit(0);
    }
    else if(
      strncmp(argv[i],"-redirect",9)==0
      ){
        LOGSTREAM=freopen(log_filename,"w",stdout);
    }
    else if(strncmp(argv[i],"-runscript",10)==0){
      from_commandline=1;
      runscript=1;
    }
    else if(strncmp(argv[i],"-skipframe",10)==0){
      from_commandline=1;
      ++i;
      if(i<argc){
        sscanf(argv[i],"%i",&skipframe0);
      }
    }
    else if(strncmp(argv[i],"-startframe",11)==0){
      from_commandline=1;
      ++i;
      if(i<argc){
        sscanf(argv[i],"%i",&startframe0);
      }
    }
    else if(strncmp(argv[i],"-volrender",10)==0){
      from_commandline=1;
      make_volrender_script=1;
    }
    else if(strncmp(argv[i],"-script",7)==0){
      from_commandline=1;
      ++i;
      if(i<argc){
        char scriptbuffer[256];
        scriptfiledata *sfd;

        strcpy(scriptbuffer,argv[i]);
        sfd = insert_scriptfile(scriptbuffer);
        if(sfd!=NULL)default_script=sfd;
        runscript=1;
      }
    }
    else if(strncmp(argv[i],"-noexit",7)==0){
      noexit=1;
    }
    else if(strncmp(argv[i],"-bindir",7)==0){
      ++i;
      if(i<argc){
        int len2;

        len2 = strlen(argv[i]);
        NewMemory((void **)&smokeview_bindir,len2+2);
        strcpy(smokeview_bindir,argv[i]);
        if(smokeview_bindir[len2-1]!=dirseparator[0])strcat(smokeview_bindir,dirseparator);
      }
    }
    else if(strncmp(argv[i],"-build",6)==0){
      showbuild=1;
      usage(argv);
      exit(0);
    }
    else {
      fprintf(stderr,"*** Error: unknown option: %s\n",argv[i]);
      usage(argv);
      exit(1);
    }
  }
  if(make_volrender_script==1){
    scriptfiledata *sfd;
    FILE *script_stream;

    NewMemory((void **)&volrender_scriptname,(unsigned int)(len_casename+14+1));
    STRCPY(volrender_scriptname,fdsprefix);
    STRCAT(volrender_scriptname,"_volrender.ssf");

    sfd = insert_scriptfile(volrender_scriptname);
    if(sfd!=NULL)default_script=sfd;
    if(file_exists(volrender_scriptname)==1){
      runscript=1;
    }
    else{
      script_stream=fopen(volrender_scriptname,"w");
      if(script_stream!=NULL){
        fprintf(script_stream,"RENDERDIR\n");
        fprintf(script_stream," .\n");
        fprintf(script_stream,"VOLSMOKERENDERALL\n");
        fprintf(script_stream," 1 0\n");
        fprintf(script_stream," %s\n",fdsprefix);
        runscript=1;
        fclose(script_stream);
      } 
    }
  }
}

/* ------------------ version ------------------------ */

void display_version_info(void){
    int svn_num;
    char version[256];

    getPROGversion(version);
    svn_num=getmaxrevision();    // get svn revision number
    printf("\n");
    printf("%s\n\n",TITLERELEASE);
    printf("Version: %s\n",version);
#ifdef BIT64
    printf("Smokeview (64 bit) Revision Number: %i\n",svn_num);
#else
    printf("Smokeview (32 bit) Revision Number: %i\n",svn_num);
#endif
#ifdef WIN32
#ifdef X64
    printf("Platform: WIN64 ");
#else
    printf("Platform: WIN32 ");
#endif
#ifdef pp_INTEL
    printf(" (Intel C/C++)\n");
#else
#ifdef WIN32
    printf(" (MSVS C/C++)\n");
#endif
#endif
#endif
#ifndef pp_OSX64
#ifdef pp_OSX
    printf("Platform: OSX\n");
#endif
#endif
#ifdef pp_OSX64
    printf("Platform: OSX64\n");
#endif
#ifndef pp_LINUX64
#ifdef pp_LINUX
    printf("Platform: LINUX\n");
#endif
#endif
#ifdef pp_LINUX64
    printf("Platform: LINUX64\n");
#endif
    printf("Build Date: %s\n",__DATE__);
    if(revision_fds>0){
      printf("FDS Revision Number: %i\n",revision_fds);
    }
    if(smokeviewpath!=NULL){
      printf("Smokeview path: %s\n",smokeviewpath);
    }
    if(smokezippath!=NULL){
      printf("Smokezip path: %s\n",smokezippath);
    }
    if(texturedir!=NULL){
      printf("Texture directory path: %s\n",texturedir);
    }
}

/* ------------------ usage ------------------------ */

void usage(char **argv){
  printf("%s\n",TITLERELEASE);
  printf("%s\n\n",_("Visualize fire/smoke flow simulations."));
  printf("Usage: %s [options] casename",argv[0]);
  printf("%s\n\n",_("where "));
  printf("%s\n",_(" casename       - project id (file names without the extension)"));
  printf("%s\n",_(" -bindir dir    - specify location of smokeview bin directory"));
  printf("%s\n",_(" -build         - show directives used in this build of Smokeview"));
  printf("%s\n",_(" -demo          - use demonstrator mode of Smokeview"));
  printf("%s\n",_(" -help          - display this message"));
  printf("%s\n",_(" -ini           - output default smokeview parameters to smokeview.ini"));
  printf("%s\n",_(" -ng_ini        - No graphics version of -ini."));
  printf("%s\n",_(" -runscript     - run the script file casename.ssf"));
  printf("%s\n",_(" -script scriptfile - run the script file scriptfile"));
  printf("%s\n",_(" -skipframe n   - render every n frames"));
  printf("%s\n",_(" -startframe n  - start rendering at frame n"));
  printf("%s\n",_(" -stereo        - activate stereo mode"));
  printf("%s\n",_(" -tempdir       - forces output files to be written to the temporary directory"));
  printf("%s\n",_(" -update_bounds - calculate boundary file bounds and save to casename.bini"));
  printf("%s\n",_(" -version       - display version information"));
  printf("%s\n",_(" -volrender     - generate images of volume rendered smoke and fire"));

  if(showbuild==1){
    char label[1024],*labelptr;

    labelptr=label+2;
    strcpy(label,"");
#ifdef pp_COMPRESS
    strcat(label,", pp_COMPRESS");
#endif
#ifdef pp_CULL
    strcat(label,", pp_CULL");
#endif
#ifdef _DEBUG
    strcat(label," _DEBUG");
#endif
#ifdef pp_DRAWISO
    strcat(label,", pp_DRAWISO");
#endif
#ifdef EGZ
    strcat(label,", EGZ");
#endif
#ifdef pp_GPU
    strcat(label,", pp_GPU");
#endif
#ifdef pp_GPUDEPTH
    strcat(label,", pp_GPUDEPTH");
#endif
#ifdef ISO_DEBUG
    strcat(label,", ISO_DEBUG");
#endif
#ifdef pp_LIGHT
    strcat(label,", pp_LIGHT");
#endif
#ifdef pp_LINUX64
    strcat(label,", pp_LINUX64");
#endif
#ifdef pp_memstatus
    strcat(label,", pp_memstatus");
#endif
#ifdef pp_MESSAGE
    strcat(label,", pp_MESSAGE");
#endif
#ifdef pp_OSX64
    strcat(label,", pp_OSX64");
#endif
#ifdef pp_noappend
    strcat(label,", pp_noappend");
#endif
#ifdef pp_OSX
    strcat(label,", pp_OSX");
#endif
#ifdef pp_release
    strcat(label," pp_release");
#endif
#ifdef pp_SHOOTER
    strcat(label,", pp_SHOOTER");
#endif
#ifdef pp_SHOWLIGHT
    strcat(label,", pp_SHOWLIGHT");
#endif
#ifdef pp_SLICECONTOURS
    strcat(label,", pp_SLICECONTOURS");
#endif
#ifdef pp_SMOKETEST
    strcat(label,", pp_SMOKETEST");
#endif
#ifdef pp_THREAD
    strcat(label,", pp_THREAD");
#endif
#ifdef X64
    strcat(label,", X64");
#endif
#ifdef USE_ZLIB
    strcat(label,", USE_ZLIB");
#endif
#ifdef WIN32
    strcat(label,", WIN32");
#endif
    printf("  \n");
    printf("%s\n\n",_("  Smokeview was built using the following pre-processing directives:"));
    printf("%s \n",labelptr);
  }
}
