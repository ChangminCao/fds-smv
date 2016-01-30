
#include "options.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <math.h>
#include GLUT_H

#include "update.h"
#include "smokeviewvars.h"
#include "IOvolsmoke.h"

int set_slice_bound_min(const char *slice_type, int set, float value) {
    for(int i = 0; i < nslice2; i++) {
        printf("setting %s min bound ", slice_type);
        if(set) {printf("ON");} else {printf("OFF");}
        printf(" with value of %f\n", value);
        if(!strcmp(slice_type, slicebounds[i].datalabel)) {
            slicebounds[i].setvalmin=set;
            slicebounds[i].valmin=value;
        }
    }
    updateslicebounds();
    Slice_CB(6); // TODO: remove constant
}

int set_slice_bound_max(const char *slice_type, int set, float value) {
    for(int i = 0; i < nslice2; i++) {
        printf("setting %s max bound ", slice_type);
        if(set) {printf("ON");} else {printf("OFF");}
        printf(" with value of %f\n", value);
        if(!strcmp(slice_type, slicebounds[i].datalabel)) {
            slicebounds[i].setvalmax=set;
            slicebounds[i].valmax=value;
        }
    }
    updateslicebounds();
    Slice_CB(6); // TODO: remove constant
}

/* ------------------ loadsmvall ------------------------ */
// Loads a SMV file into smokeview. This should be completely independent from
// the setup of smokeview, and should be able to be run multiple times (or not
// at all). This is based on setup_case from startup.c. Currently it features
// initialisation of some GUI elements that have yet to be factored out.
int loadsmvall(const char *input_filename) {
  printf("about to load %s\n", input_filename);
  int return_code;
  // fdsprefix and input_filename_ext are global and defined in smokeviewvars.h
  // TODO: move these into the model information namespace
  parse_smv_filepath(input_filename, fdsprefix, input_filename_ext);
  printf("fdsprefix: %s\ninput_filename_ext: %s\n", fdsprefix,
         input_filename_ext);
  return_code=loadsmv(fdsprefix, input_filename_ext);
  if(return_code==0&&update_bounds==1)return_code=Update_Bounds();
  if(return_code!=0)return 1;
  // if(convert_ini==1){
    // readini(ini_from);
  // }
}

// This function takes a filepath to an smv file an finds the casename
// and the extension, which are returned in the 2nd and 3rd arguments (the
// 2nd and 3rd aguments a pre-existing strings).
int parse_smv_filepath(char *smv_filepath, char *fdsprefix,
                       char *input_filename_ext) {
  int len_casename;
  strcpy(input_filename_ext,"");
  len_casename = (int) strlen(smv_filepath);
  printf("len_casename %d\n", len_casename);
  if(len_casename>4){
    char *c_ext;

    c_ext=strrchr(smv_filepath,'.');
    if(c_ext!=NULL){
      printf("c_ext: %s\n", c_ext);
      STRCPY(input_filename_ext,c_ext);
      to_lower(input_filename_ext);

      if(c_ext!=NULL&&
        (strcmp(input_filename_ext,".smv")==0||
         strcmp(input_filename_ext,".svd")==0||
         strcmp(input_filename_ext,".smt")==0)
         ){
        printf("comp done\n");
        // c_ext[0]=0;
        printf("1\n");
        STRCPY(fdsprefix,smv_filepath);
        printf("fdsprefixlen1: %d\n", strlen(fdsprefix));
        fdsprefix[strlen(fdsprefix)-4] = 0;
        printf("fdsprefixlen2: %d\n", strlen(fdsprefix));
        printf("2\n");
        strcpy(movie_name, fdsprefix);
        printf("3\n");
        strcpy(render_file_base, fdsprefix);
        FREEMEMORY(trainer_filename);
        printf("4\n");
        NewMemory((void **)&trainer_filename,(unsigned int)(len_casename+7));
        STRCPY(trainer_filename,smv_filepath);
        printf("5\n");
        STRCAT(trainer_filename,".svd");
        FREEMEMORY(test_filename);
        printf("6\n");
        NewMemory((void **)&test_filename,(unsigned int)(len_casename+7));
        STRCPY(test_filename,smv_filepath);
        STRCAT(test_filename,".smt");
        printf("7\n");
      }
    }
  }
  return 0;
}

/* ------------------ loadsmv ------------------------ */
int loadsmv(char *input_filename, char *input_filename_ext){
  int return_code;
  char *input_file;
  if(input_filename==NULL){
      printf("ERROR: input_filename is NULL\n");
  }
  if(input_filename_ext==NULL){
      printf("ERROR: input_filename_ext is NULL\n");
  }
  fflush(stdout);
  printf("loading smv: %s\n", input_filename);
  printf("ext: %s\n", input_filename_ext);
  fflush(stdout);
  /*
  warning: the following line was commented out!! (perhaps it broke something)
     this line is necessary in order to define smv_filename and trainer_filename
  */
 // parse_commandlines(argc, argv);
  return_code=-1;
  if(strcmp(input_filename_ext,".svd")==0||demo_option==1){
    trainer_mode=1;
    trainer_active=1;
    if(strcmp(input_filename_ext,".svd")==0){
      input_file=trainer_filename;
    }
    else if(strcmp(input_filename_ext,".smt")==0){
      input_file=test_filename;
    }
    else{
      input_file=smv_filename;
    }
    return_code=readsmv(input_file,iso_filename);
    if(return_code==0){
      show_glui_trainer();
      show_glui_alert();
    }
  }
  else{
    input_file=smv_filename;
    return_code=readsmv(input_file,iso_filename);
  }
  switch(return_code){
    case 1:
      fprintf(stderr,"*** Error: Smokeview file, %s, not found\n",input_file);
      return 1;
    case 2:
      fprintf(stderr,"*** Error: problem reading Smokeview file, %s\n",input_file);
      return 2;
    case 0:
      readsmv_dynamic(input_file);
      break;
    default:
      ASSERT(FFALSE);
  }

  /* initialize units */

  InitUnits();
  init_unit_defs();
  set_unit_vis();

  CheckMemory;
  readini(NULL);
  readboundini();
  if(use_graphics==0)return 0;
#ifdef pp_LANG
  init_lang();
#endif

  if(sb_atstart==1)smooth_blockages();

  if(ntours==0)setup_tour();
  glui_colorbar_setup(mainwindow_id);
  glui_motion_setup(mainwindow_id);
  glui_bounds_setup(mainwindow_id);
  glui_shooter_setup(mainwindow_id);
  glui_geometry_setup(mainwindow_id);
  glui_clip_setup(mainwindow_id);
  // glui_console_setup(mainwindow_id);
  glui_wui_setup(mainwindow_id);
  glui_labels_setup(mainwindow_id);
  glui_device_setup(mainwindow_id);
  glui_tour_setup(mainwindow_id);
  glui_alert_setup(mainwindow_id);
  glui_stereo_setup(mainwindow_id);
  glui_3dsmoke_setup(mainwindow_id);

  if(UpdateLIGHTS==1)updateLights(light_position0,light_position1);

  glutReshapeWindow(screenWidth,screenHeight);

  glutSetWindow(mainwindow_id);
  glutShowWindow();
  glutSetWindowTitle(fdsprefix);
  Init();
  glui_trainer_setup(mainwindow_id);
  glutDetachMenu(GLUT_RIGHT_BUTTON);
  InitMenus(LOAD);
  glutAttachMenu(GLUT_RIGHT_BUTTON);
  if(trainer_mode==1){
    show_glui_trainer();
    show_glui_alert();
  }
  return 0;
}

/* ------------------ loadfile ------------------------ */

int loadfile(const char *filename) {
  int i;
  int errorcode;

  FREEMEMORY(loaded_file);
  PRINTF("loading file %s\n\n",filename);
  if(filename!=NULL&&strlen(filename)>0){
    NewMemory((void **)&loaded_file,strlen(filename)+1);
    strcpy(loaded_file,filename);
  }
  for(i=0;i<nsliceinfo;i++){
    slicedata *sd;

    sd = sliceinfo + i;
    if(strcmp(sd->file,filename)==0){
      if(i<nsliceinfo-nfedinfo){
        readslice(sd->file,i,LOAD,&errorcode);
      }
      else{
        readfed(i,LOAD,FED_SLICE,&errorcode);
      }
      return errorcode;
    }
  }
  for(i=0;i<npatchinfo;i++){
    patchdata *patchi;

    patchi = patchinfo + i;
    if(strcmp(patchi->file,filename)==0){
      readpatch(i,LOAD,&errorcode);
      return errorcode;
    }
  }
  npartframes_max=get_min_partframes();
  for(i=0;i<npartinfo;i++){
    partdata *parti;

    parti = partinfo + i;
    if(strcmp(parti->file,filename)==0){
      readpart(parti->file,i,LOAD,&errorcode);
      return errorcode;
    }
  }
  for(i=0;i<nisoinfo;i++){
    isodata *isoi;

    isoi = isoinfo + i;
    if(strcmp(isoi->file,filename)==0){
      readiso(isoi->file,i,LOAD,NULL,&errorcode);
      return errorcode;
    }
  }
  for(i=0;i<nsmoke3dinfo;i++){
    smoke3ddata *smoke3di;

    smoke3di = smoke3dinfo + i;
    if(strcmp(smoke3di->file,filename)==0){
      readsmoke3d(i,LOAD,&errorcode);
      return errorcode;
    }
  }
  for(i=0;i<nzoneinfo;i++){
    zonedata *zonei;

    zonei = zoneinfo + i;
    if(strcmp(zonei->file,filename)==0){
      readzone(i,LOAD,&errorcode);
      return errorcode;
    }
  }
  for(i=0;i<nplot3dinfo;i++){
    plot3ddata *plot3di;

    plot3di = plot3dinfo + i;
    if(strcmp(plot3di->file,filename)==0){
      ReadPlot3dFile=1;
      readplot3d(plot3di->file,i,LOAD,&errorcode);
      update_menu();
      return errorcode;
    }
  }

  fprintf(stderr,"*** Error: file %s failed to load\n",filename);
  return 1;
}

/* ------------------ loadinifile ------------------------ */

void loadinifile(const char *filepath){
  PRINTF("loading ini file %s\n\n",filepath);
  windowresized=0;
  char f[1048];
  strcpy(f,filepath);
  readini(f);
}

/* ------------------ loadvfile ------------------------ */

int loadvfile(const char *filepath){
  int i;
  FREEMEMORY(loaded_file);
  PRINTF("loading vector slice file %s\n\n",filepath);
  for(i=0;i<nvsliceinfo;i++){
    slicedata *val;
    vslicedata *vslicei;

    vslicei = vsliceinfo + i;
    val = sliceinfo + vslicei->ival;
    if(val==NULL)continue;
    if(strcmp(val->reg_file,filepath)==0){
      LoadVSliceMenu(i);
      if(filepath!=NULL&&strlen(filepath)>0){
        NewMemory((void **)&loaded_file,strlen(filepath)+1);
        strcpy(loaded_file,filepath);
      }
      return 0;
    }
  }
  fprintf(stderr,"*** Error: Vector slice file %s was not loaded\n",filepath);
  return 1;
}

/* ------------------ loadboundaryfile ------------------------ */

void loadboundaryfile(const char *filepath){
  int i;
  int errorcode;
  int count=0;

  FREEMEMORY(loaded_file);
  PRINTF("loading boundary files of type: %s\n\n",filepath);

  for(i=0;i<npatchinfo;i++){
    patchdata *patchi;

    patchi = patchinfo + i;
    if(strcmp(patchi->label.longlabel,filepath)==0){
      LOCK_COMPRESS
      readpatch(i,LOAD,&errorcode);
      if(filepath!=NULL&&strlen(filepath)>0){
        FREEMEMORY(loaded_file);
        NewMemory((void **)&loaded_file,strlen(filepath)+1);
        strcpy(loaded_file,filepath);
      }
      count++;
      UNLOCK_COMPRESS
    }
  }
  if(count==0)fprintf(stderr,"*** Error: Boundary files of type %s failed to"
                             "load\n",filepath);
  force_redisplay=1;
  updatemenu=1;
  Update_Framenumber(0);

}

/* ------------------ label ------------------------ */

void label(const char *label) {
    PRINTF("*******************************\n");
    PRINTF("*** %s ***\n",label);
    PRINTF("*******************************\n");
}

/*
  Specify offset clip in pixels.
*/
void renderclip(int flag, int left, int right, int bottom, int top) {
    clip_rendered_scene = flag;
    render_clip_left    = left;
    render_clip_right   = right;
    render_clip_bottom  = bottom;
    render_clip_top     = top;
}

/* ------------------ render ------------------------ */

void render(const char *filename) {
    //runluascript=0;
	Display_CB();
    //runluascript=1;
    //strcpy(render_file_base,filename);
    printf("basename(c): %s\n", filename);
	RenderFrameLua(VIEW_CENTER, filename);
}


// construct filepath for image to be renderd
char* form_filename(int view_mode, char *renderfile_name, char *renderfile_dir,
                   char *renderfile_path, int woffset, int hoffset, int screenH,
                   char *basename) {
    
    // char renderfile_ext[4]; // does not include the '.'
    char suffix[20];
    char* renderfile_ext;
    char* view_suffix;
    
    // determine the extension to be used, and set renderfile_ext to it
    switch(renderfiletype) {
        case 0:
            renderfile_ext = ext_png;
            break;
        case 1:
            renderfile_ext = ext_jpg;
            break;
        default:
            renderfiletype = 2;
            renderfile_ext = ext_png;
            break;
    }
    
    // if the basename has not been specified, use a predefined method to
    // determine the filename
    if(basename == NULL) {
        printf("basename is null\n");
        
        
        switch(view_mode) {
            case VIEW_LEFT:
                if(showstereo==STEREO_LR){
                    view_suffix = "_L";
                }
                break;
            case VIEW_RIGHT:
                if(showstereo==STEREO_LR){
                    view_suffix = "_R";
                }
                break;
            case VIEW_CENTER:
                view_suffix = "";
                break;
            default:
                ASSERT(FFALSE);
                break;
        }
        
        if(can_write_to_dir(renderfile_dir)==0){
            printf("Creating directory: %s\n", renderfile_dir);
            
// #if defined(WIN32)
//             CreateDirectory (renderfile_dir, NULL);
// #elif defined(_MINGW32_)
//             CreateDirectory (renderfile_dir, NULL);
// #else
//             mkdir(renderfile_dir, 0700);
// #endif
            // TODO: ensure this can be made cross-platform
            if (strlen(renderfile_dir)>0) {
                printf("making dir: %s", renderfile_dir);
                mkdir(renderfile_dir);
            }
        }
        if(showstereo==STEREO_LR&&(view_mode==VIEW_LEFT||view_mode==VIEW_RIGHT)){
          hoffset=screenHeight/4;
          screenH = screenHeight/2;
          if(view_mode==VIEW_RIGHT)woffset=screenWidth;
        }

        snprintf(renderfile_name, 1024,
                  "%s%s%s",
                  fdsprefix, view_suffix, renderfile_ext);
        printf("directory is: %s\n", renderfile_dir);
        printf("filename is: %s\n", renderfile_name);
    } else {
        snprintf(renderfile_name, 1024, "%s%s", basename, renderfile_ext);
    }
    return renderfile_name;
}

// This is function fulfills the exact same purpose as the original RenderFrame
// function, except that it takes a second argument, basename. This could be
// be used as a drop in replacement as long as all existing calls are modified
// to use basename = NULL.
  /* ------------------ RenderFrameLua ------------------------ */
// The second argument to RenderFrameLua is the name that should be given to the
// rendered file. If basename == NULL, then a default filename is formed based
// on the chosen frame and rendering options.
void RenderFrameLua(int view_mode, char *basename) {
  char renderfile_name[1024]; // the name the file (including extension)
  char renderfile_dir[1024]; // the directory into which the image will be rendered
  char renderfile_path[2048]; // the full path of the rendered image
  int woffset=0,hoffset=0;
  int screenH;

  if(script_dir_path != NULL){
      strcpy(renderfile_dir, script_dir_path);
  } else {
      strcpy(renderfile_dir, ".");
  }

#ifdef WIN32
  SetThreadExecutionState(ES_DISPLAY_REQUIRED); // reset display idle timer to prevent screen saver from activating
#endif

  screenH = screenHeight;
  if(view_mode==VIEW_LEFT&&showstereo==STEREO_RB)return;
  // construct filename for image to be rendered
  form_filename(view_mode, renderfile_name, renderfile_dir, renderfile_path,
                woffset, hoffset, screenH, basename);
                
  printf("renderfile_name: %s\n", renderfile_name);
  // render image
  SVimage2file(renderfile_dir,renderfile_name,renderfiletype,woffset,screenWidth,hoffset,screenH);
  if(RenderTime==1&&output_slicedata==1){
    output_Slicedata();
  }
}


/* ------------------ settourkeyframe ------------------------ */

void settourkeyframe(float keyframe_time) {
  keyframe *keyj,*minkey=NULL;
  tourdata *touri;
  float minkeytime=1000000000.0;

  if(selected_tour==NULL)return;
  touri=selected_tour;
  for(keyj=(touri->first_frame).next;keyj->next!=NULL;keyj=keyj->next){
    float diff_time;

    if(keyj==(touri->first_frame).next){
      minkey=keyj;
      minkeytime=ABS(keyframe_time-keyj->nodeval.time);
      continue;
    }
    diff_time=ABS(keyframe_time-keyj->nodeval.time);
    if(diff_time<minkeytime){
      minkey=keyj;
      minkeytime=diff_time;
    }
  }
  if(minkey!=NULL){
    new_select(minkey);
    set_glui_keyframe();
    update_tourcontrols();
  }
}

/* ------------------ gsliceview ------------------------ */

void gsliceview(int data, int show_triangles, int show_triangulation,
                int show_normal) {
  vis_gslice_data = data;
  show_gslice_triangles = show_triangles;
  show_gslice_triangulation = show_triangulation;
  show_gslice_normal = show_normal;
  update_gslice = 1;
}

/* ------------------ gslicepos ------------------------ */

void gslicepos(float x, float y, float z) {
  gslice_xyz[0] = x;
  gslice_xyz[1] = y;
  gslice_xyz[2] = z;
  update_gslice = 1;
}

/* ------------------ gsliceorien ------------------------ */

void gsliceorien(float az, float elev) {
  gslice_normal_azelev[0] = az;
  gslice_normal_azelev[1] = elev;
  update_gslice = 1;
}

/* ------------------ settourview ------------------------ */

void settourview(int edittourArg, int mode, int show_tourlocusArg,
                 float tour_global_tensionArg) {
  edittour=edittourArg;
  show_tourlocus=show_tourlocusArg;
  tour_global_tension_flag=1;
  tour_global_tension=tour_global_tensionArg;
  switch(mode){
    case 0:
      viewtourfrompath=0;
      keyframe_snap=0;
      break;
    case 1:
      viewtourfrompath=1;
      keyframe_snap=0;
      break;
    case 2:
      viewtourfrompath=0;
      keyframe_snap=1;
      break;
    default:
      viewtourfrompath=0;
      keyframe_snap=0;
      break;
  }
  update_tour_state();
}

int getframe() {
    int framenumber = itimes;
    return framenumber;
}

float gettime() {
    return global_times[itimes];
}
/* ------------------ settime ------------------------ */

void settime(float timeval) {
  printf("t: %f\n", timeval);
  int i,imin;
  float valmin;

  PRINTF("setting time to %f\n\n",timeval);
  if(global_times!=NULL&&nglobal_times>0){
    if(timeval<global_times[0])timeval=global_times[0];
    if(timeval>global_times[nglobal_times-1]-0.0001){
      float dt;

      dt = timeval-global_times[nglobal_times-1]-0.0001;
#ifdef pp_SETTIME
      if(nglobal_times>1&&dt>global_times[1]-global_times[0]){
        fprintf(stderr,"*** Error: data not available at time requested\n");
        fprintf(stderr,"           time: %f s, min time: %f, max time: %f s\n",
          timeval,global_times[0],global_times[nglobal_times-1]);
        if(loaded_file!=NULL)fprintf(stderr,"           loaded file: %s\n",
                                     loaded_file);
        if(script_labelstring!=NULL)fprintf(stderr,"                 "
                                            "label: %s\n",script_labelstring);
      }
#endif
      timeval=global_times[nglobal_times-1]-0.0001;
    }
    valmin=ABS(global_times[0]-timeval);
    imin=0;
    for(i=1;i<nglobal_times-1;i++){
      float val;

      val = ABS(global_times[i]-timeval);
      if(val<valmin){
        valmin=val;
        imin=i;
      }
    }
    itimes=imin;
    script_itime=imin;
    stept=0;
    force_redisplay=1;
    Update_Framenumber(0);
    UpdateTimeLabels();
  }
  PRINTF("script: time set to %f s (i.e. frame number: %d)\n\n",
         global_times[itimes], itimes);
}

void settimebarvisibility(int setting) {
  visTimebar = setting;
  if(visTimebar==0)PRINTF("Time bar hidden\n");
  if(visTimebar==1)PRINTF("Time bar visible\n");
}

int gettimebarvisibility() {
    return visTimebar;
}

void toggletimebarvisibility() {
  visTimebar = 1 - visTimebar;
  if(visTimebar==0)PRINTF("Time bar hidden\n");
  if(visTimebar==1)PRINTF("Time bar visible\n");
}

void setframe(int framenumber) {
  itimes=framenumber;
  script_itime=itimes;
  stept=0;
  force_redisplay=1;
  Update_Framenumber(0);
  UpdateTimeLabels();
}

/* ------------------ loadvolsmoke ------------------------ */
/*
  Load files needed to view volume rendered smoke. One may either load files for
  all meshes or for one particular mesh. Use meshnumber = -1 for all meshes.
*/
void loadvolsmoke(int meshnumber) {
  int imesh;

  imesh = meshnumber;
  if(imesh==-1){
    read_vol_mesh=VOL_READALL;
    read_volsmoke_allframes_allmeshes2(NULL);
  }
  else if(imesh>=0&&imesh<nmeshes){
    mesh *meshi;
    volrenderdata *vr;

    meshi = meshinfo + imesh;
    vr = &meshi->volrenderinfo;
    read_volsmoke_allframes(vr);
  }
}

/* ------------------ loadvolsmokeframe ------------------------ */
/*
  As with loadvolsmoke, but for a single frame indicated my framenumber. Flag is
  set to 1 when calling from a script. Reason unkown.
*/
void loadvolsmokeframe(int meshnumber, int framenumber, int flag) {
  int framenum, index;
  int first = 1;
  int i;

  index = meshnumber;
  framenum = framenumber;
  if(index > nmeshes - 1)index = -1;
  for(i = 0; i < nmeshes; i++){
    if(index == i || index < 0){
      mesh *meshi;
      volrenderdata *vr;

      meshi = meshinfo + i;
      vr = &meshi->volrenderinfo;
      free_volsmoke_frame(vr, framenum);
      read_volsmoke_frame(vr, framenum, &first);
      if(vr->times_defined == 0){
        vr->times_defined = 1;
        get_volsmoke_all_times(vr);
      }
      vr->loaded = 1;
      vr->display = 1;
    }
  }
  plotstate = getplotstate(DYNAMIC_PLOTS);
  stept = 1;
  Update_Times();
  force_redisplay = 1;
  Update_Framenumber(framenum);
  i = framenum;
  itimes = i;
  script_itime = i;
  stept = 1;
  force_redisplay = 1;
  Update_Framenumber(0);
  UpdateTimeLabels();
  // TODO: replace with a call to render()
  keyboard('r', FROM_SMOKEVIEW);
  RenderOnceNow = 0;
  if(flag == 1)script_render = 1;// called when only rendering a single frame
}

/* ------------------ load3dsmoke ------------------------ */

void load3dsmoke(const char *smoke_type){
  int i;
  int errorcode;
  int count=0;

  FREEMEMORY(loaded_file);
  PRINTF("script: loading smoke3d files of type: %s\n\n",smoke_type);

  for(i=0;i<nsmoke3dinfo;i++){
    smoke3ddata *smoke3di;

    smoke3di = smoke3dinfo + i;
    if(match_upper(smoke3di->label.longlabel,smoke_type)==1){
      readsmoke3d(i,LOAD,&errorcode);
      if(smoke_type!=NULL&&strlen(smoke_type)>0){
        FREEMEMORY(loaded_file);
        NewMemory((void **)&loaded_file,strlen(smoke_type)+1);
        strcpy(loaded_file,smoke_type);
      }
      count++;
    }
  }
  if(count==0)fprintf(stderr,"*** Error: Smoke3d files of type %s failed to "
                      "load\n",smoke_type);
  force_redisplay=1;
  updatemenu=1;

}

void rendertype(const char *type) {
    if(STRCMP(type, "JPG")==0){
      update_render_type(JPEG);
    }
    else{
      update_render_type(PNG);
    }
}

int get_rendertype() {
    return renderfiletype;
}

void set_movietype(const char *type) {
    if(STRCMP(type, "WMV") == 0){
      update_movie_type(WMV);
    }
    if(STRCMP(type, "MP4") == 0){
      update_movie_type(MP4);
    }
    else{
      update_movie_type(AVI);
    }
}

int get_movietype() {
    return moviefiletype;
}

void makemovie(const char *name, const char *base, float framerate) {
    strcpy(movie_name, name);
    strcpy(render_file_base, base);
    movie_framerate=framerate;
    Render_CB(MAKE_MOVIE);
}

/* ------------------ script_loadtour ------------------------ */

int loadtour(const char *tourname) {
  int i;
  int count=0;
  int errorcode = 0;
  PRINTF("loading tour %s\n\n", tourname);

  for(i=0;i<ntours;i++){
    tourdata *touri;

    touri = tourinfo + i;
    if(strcmp(touri->label,tourname)==0){
      TourMenu(i);
      viewtourfrompath=0;
      TourMenu(MENU_TOUR_VIEWFROMROUTE);
      count++;
      break;
    }
  }

  if(count==0) {
      fprintf(stderr,"*** Error: The tour %s failed to load\n",
                      tourname);
      errorcode = 1;
  }
  force_redisplay=1;
  updatemenu=1;
  return errorcode;
}

/* ------------------ loadparticles ------------------------ */

void loadparticles(const char *name){
  int i;
  int errorcode;
  int count=0;

  FREEMEMORY(loaded_file);

  PRINTF("script: loading particles files\n\n");

  npartframes_max=get_min_partframes();
  for(i=0;i<npartinfo;i++){
    partdata *parti;

    parti = partinfo + i;
    if(parti->evac==1)continue;
    if(parti->version==1){
      readpart(parti->file,i,UNLOAD,&errorcode);
      count++;
    }
  }
  for(i=0;i<npartinfo;i++){
    partdata *parti;

    parti = partinfo + i;
    if(parti->evac==1)continue;
    if(parti->version==1){
      readpart(parti->file,i,LOAD,&errorcode);
      if(name!=NULL&&strlen(name)>0){
        FREEMEMORY(loaded_file);
        NewMemory((void **)&loaded_file,strlen(name)+1);
        strcpy(loaded_file,name);
      }
      count++;
    }
  }
  if(count==0)fprintf(stderr,"*** Error: Particles files failed to load\n");
  force_redisplay=1;
  Update_Framenumber(0);
  updatemenu=1;
}
/* ------------------ partclasscolor ------------------------ */

void partclasscolor(const char *color){
  int i;
  int count=0;

  for(i=0;i<npart5prop;i++){
	part5prop *propi;

	propi = part5propinfo + i;
	if(propi->particle_property==0)continue;
	if(strcmp(propi->label->longlabel,color)==0){
	  ParticlePropShowMenu(i);
	  count++;
	}
  }
  if(count==0)fprintf(stderr,"*** Error: particle class color: %s failed to be set\n",color);
}

/* ------------------ partclasstype ------------------------ */

void partclasstype(const char *part_type){
  int i;
  int count=0;

  for(i=0;i<npart5prop;i++){
    part5prop *propi;
    int j;

    propi = part5propinfo + i;
    if(propi->display==0)continue;
    for(j=0;j<npartclassinfo;j++){
      part5class *partclassj;

      if(propi->class_present[j]==0)continue;
      partclassj = partclassinfo + j;
      if(partclassj->kind==HUMANS)continue;
      if(strcmp(partclassj->name,part_type)==0){
        ParticlePropShowMenu(-10-j);
        count++;
      }
    }
  }
  if(count==0)fprintf(stderr,"*** Error: particle class type %s failed to be "
                             "set\n",part_type);
}

/* ------------------ script_plot3dprops ------------------------ */

void plot3dprops(int variable_index, int showvector, int vector_length_index,
                 int display_type, float vector_length) {
  int i, p_index;

  p_index = variable_index;
  if(p_index<1)p_index=1;
  if(p_index>5)p_index=5;

  visVector = showvector;
  if(visVector!=1)visVector=0;

  plotn = p_index;
  if(plotn<1){
    plotn=numplot3dvars;
  }
  if(plotn>numplot3dvars){
    plotn=1;
  }
  updateallplotslices();
  if(visiso==1)updatesurface();
  updateplot3dlistindex();

  vecfactor=1.0;
  if(vector_length>=0.0)vecfactor=vector_length;
  update_vector_widgets();

  PRINTF("vecfactor=%f\n",vecfactor);

  contour_type=CLAMP(display_type,0,2);
  update_plot3d_display();

  if(visVector==1&&ReadPlot3dFile==1){
    mesh *gbsave,*gbi;

    gbsave=current_mesh;
    for(i=0;i<nmeshes;i++){
      gbi = meshinfo + i;
      if(gbi->plot3dfilenum==-1)continue;
      update_current_mesh(gbi);
      updateplotslice(X_SLICE);
      updateplotslice(Y_SLICE);
      updateplotslice(Z_SLICE);
    }
    update_current_mesh(gbsave);
  }
}

// TODO: this function uses 5 int script values, but the documentation only
// lists three. Find out what is going on here.
// /* ------------------ script_showplot3ddata ------------------------ */
//
// void script_showplot3ddata(int meshnumber, int plane_orientation, int display,
//                            float position) {
//   mesh *meshi;
//   int imesh, dir, showhide;
//   float val;
//   int isolevel;
//
//   imesh = scripti->ival-1;
//   if(imesh<0||imesh>nmeshes-1)return;
//
//   meshi = meshinfo + imesh;
//   update_current_mesh(meshi);
//
//   dir = scripti->ival2;
//   if(dir<1)dir=1;
//   if(dir>4)dir=4;
//
//   plotn=scripti->ival3;
//
//   showhide = scripti->ival4;
//   val = scripti->fval;
//
//   switch(dir){
//     case 1:
//       visx_all=showhide;
//       iplotx_all=get_index(val,1,plotx_all,nplotx_all);
//       next_xindex(1,0);
//       next_xindex(-1,0);
//       break;
//     case 2:
//       visy_all=showhide;
//       iploty_all=get_index(val,2,ploty_all,nploty_all);
//       next_yindex(1,0);
//       next_yindex(-1,0);
//       break;
//     case 3:
//       visz_all=showhide;
//       iplotz_all=get_index(val,3,plotz_all,nplotz_all);
//       next_zindex(1,0);
//       next_zindex(-1,0);
//       break;
//     case 4:
//       isolevel=scripti->ival5;
//       plotiso[plotn-1]=isolevel;
//       updateshowstep(showhide,ISO);
//       updatesurface();
//       updatemenu=1;
//       break;
//     default:
//       ASSERT(FFALSE);
//       break;
//   }
//   updateplotslice(dir);
//
// }


/* ------------------ loadplot3d ------------------------ */

void loadplot3d(int meshnumber, float time_local){
  int i;
  int blocknum;
  int count=0;

  time_local = time_local;
  blocknum = meshnumber-1;

  for(i=0;i<nplot3dinfo;i++){
    plot3ddata *plot3di;

    plot3di = plot3dinfo + i;
    if(plot3di->blocknumber==blocknum&&ABS(plot3di->time-time_local)<0.5){
      count++;
      LoadPlot3dMenu(i);
    }
  }
  UpdateRGBColors(COLORBAR_INDEX_NONE);
  set_labels_controls();
  if(count==0)fprintf(stderr,"*** Error: Plot3d file failed to load\n");

  //update_menu();
}

/* ------------------ loadiso ------------------------ */

void loadiso(const char *type) {
  int i;
  int count=0;

  FREEMEMORY(loaded_file);
  PRINTF("loading isosurface files of type: %s\n\n",type);

  update_readiso_geom_wrapup = UPDATE_ISO_START_ALL;
  for(i = 0; i<nisoinfo; i++){
    int errorcode;
    isodata *isoi;

    isoi = isoinfo + i;
    if(STRCMP(isoi->surface_label.longlabel,type)==0){
      readiso(isoi->file,i,LOAD,NULL,&errorcode);
      if(type != NULL&&strlen(type)>0){
        FREEMEMORY(loaded_file);
        NewMemory((void **)&loaded_file,strlen(type)+1);
        strcpy(loaded_file,type);
      }
      count++;
    }
  }
  if(update_readiso_geom_wrapup == UPDATE_ISO_ALL_NOW)readiso_geom_wrapup();
  update_readiso_geom_wrapup = UPDATE_ISO_OFF;
  if(count == 0)fprintf(stderr, "*** Error: Isosurface files of type %s failed "
                                "to load\n", type);
  force_redisplay=1;
  updatemenu=1;
}

/* ------------------ loadslice ------------------------ */

void loadslice(const char *type, int axis, float distance){
  int i;
  int count=0;

  PRINTF("loading slice files of type: %s\n\n",type);

  for(i=0;i<nmultisliceinfo;i++){
    multislicedata *mslicei;
    slicedata *slicei;
    int j;
    float delta_orig;

    mslicei = multisliceinfo + i;
    if(mslicei->nslices<=0)continue;
    slicei = sliceinfo + mslicei->islices[0];
    printf("slice name: %s, axis: %d, position: %f\n", slicei->label.longlabel,
           slicei->idir, slicei->position_orig);
    if(match_upper(slicei->label.longlabel,type)==0)continue;
    if(slicei->idir!=axis)continue;
    delta_orig = slicei->position_orig - distance;
    if(delta_orig<0.0)delta_orig = -delta_orig;
    if(delta_orig>slicei->delta_orig)continue;

    for(j=0;j<mslicei->nslices;j++){
      LoadSliceMenu(mslicei->islices[j]);
      count++;
    }
    break;
  }
  if(count==0)fprintf(stderr,"*** Error: Slice files of type %s failed to "
                             "load\n",type);
}

/* ------------------ loadvslice ------------------------ */

void loadvslice(const char *type, int axis, float distance){
  int i;
  float delta_orig;
  int count=0;

  PRINTF("loading vector slice files of type: %s\n\n",type);

  for(i=0;i<nmultivsliceinfo;i++){
    multivslicedata *mvslicei;
    int j;
    slicedata *slicei;

    mvslicei = multivsliceinfo + i;
    if(mvslicei->nvslices<=0)continue;
    slicei = sliceinfo + mvslicei->ivslices[0];
    if(match_upper(slicei->label.longlabel,type)==0)continue;
    if(slicei->idir!=axis)continue;
    delta_orig = slicei->position_orig - distance;
    if(delta_orig<0.0)delta_orig = -delta_orig;
    if(delta_orig>slicei->delta_orig)continue;

    for(j=0;j<mvslicei->nvslices;j++){
      LoadVSliceMenu(mvslicei->ivslices[j]);
      count++;
    }
    break;
  }
  if(count==0)fprintf(stderr,"*** Error: Vector slice files of type %s failed "
                             "to load\n",type);
}

/* ------------------ unloadslice ------------------------ */

void unloadslice(int value){
  int errorcode,i;

  updatemenu=1;
  glutPostRedisplay();
  if(value>=0){
    readslice("",value,UNLOAD,&errorcode);
  }
  else{
    if(value==UNLOAD_ALL){
      for(i=0;i<nsliceinfo;i++){
        readslice("",i,UNLOAD,&errorcode);
      }
    }
    else if(value==UNLOAD_LAST){
      int unload_index;

      unload_index=last_slice_loadstack();
      if(unload_index>=0&&unload_index<nsliceinfo){
        readslice("",unload_index,UNLOAD,&errorcode);
      }
    }
  }
}

/* ------------------ unloadall ------------------------ */

void unloadall() {
  int errorcode;
  int i;
  int ii;

   // leaving code here commented in case I later decide to unload terrain files
   // for(i=0;i<nterraininfo;i++){
   //   readterrain("",i,UNLOAD,&errorcode);
   // }
    if(hrr_csv_filename!=NULL){
      readhrr(UNLOAD, &errorcode);
    }
#ifdef pp_DEVICE
    if(devc_csv_filename!=NULL){
      read_device_data(devc_csv_filename,CSV_FDS,UNLOAD);
    }
    if(exp_csv_filename!=NULL){
      read_device_data(exp_csv_filename,CSV_EXP,UNLOAD);
    }
#endif
    if(nvolrenderinfo>0){
      LoadVolSmoke3DMenu(UNLOAD_ALL);
    }
    for(i=0;i<nsliceinfo;i++){
      readslice("",i,UNLOAD,&errorcode);
    }
    for(i=0;i<nplot3dinfo;i++){
      readplot3d("",i,UNLOAD,&errorcode);
    }
    for(i=0;i<npatchinfo;i++){
      readpatch(i,UNLOAD,&errorcode);
    }
    for(i=0;i<npartinfo;i++){
      readpart("",i,UNLOAD,&errorcode);
    }
    for(i=0;i<nisoinfo;i++){
      readiso("",i,UNLOAD,NULL,&errorcode);
    }
    for(i=0;i<nzoneinfo;i++){
      readzone(i,UNLOAD,&errorcode);
    }
    for(i=0;i<nsmoke3dinfo;i++){
      readsmoke3d(i,UNLOAD,&errorcode);
    }
    if(nvolrenderinfo>0){
      UnLoadVolSmoke3DMenu(UNLOAD_ALL);
    }
}

void unloadtour() {
    TourMenu(MENU_TOUR_MANUAL);
}

/* ------------------ exit_smokeview ------------------------ */

void exit_smokeview() {
	PRINTF("exiting...\n");
	exit(0);
}

/* ------------------ setviewpoint ------------------------ */

int setviewpoint(const char *viewpoint){
  camera *ca;
  int count=0;
  int errorcode = 0;
  PRINTF("setting viewpoint to %s\n\n",viewpoint);
  for(ca=camera_list_first.next;ca->next!=NULL;ca=ca->next){
    if(strcmp(viewpoint,ca->name)==0){
      ResetMenu(ca->view_id);
      count++;
      fprintf(stderr, "Setting viewpoint set to %s\n", ca->name);
      break;
    }
  }
  if(count==0){
      errorcode = 1;
      fprintf(stderr,"*** Error: The viewpoint %s was not found\n",viewpoint);
  }
  fprintf(stderr, "Viewpoint set to %s\n", camera_current->name);
  return errorcode;
}

void set_clipping_mode(int mode) {
    clip_mode=mode;
    updatefacelists=1;
}

void set_sceneclip_x(int clipMin, float min, int clipMax, float max) {
    clipinfo.clip_xmin=clipMin;
    clipinfo.xmin = min;

    clipinfo.clip_xmax=clipMax;
    clipinfo.xmax = max;
    updatefacelists=1;
}

void set_sceneclip_x_min(int flag, float value) {
    clipinfo.clip_xmin = flag;
    clipinfo.xmin = value;
    updatefacelists = 1;
}

void set_sceneclip_x_max(int flag, float value) {
    clipinfo.clip_xmax = flag;
    clipinfo.xmax = value;
    updatefacelists = 1;
}

void set_sceneclip_y(int clipMin, float min, int clipMax, float max) {
    clipinfo.clip_ymin=clipMin;
    clipinfo.ymin = min;

    clipinfo.clip_ymax=clipMax;
    clipinfo.ymax = max;
    updatefacelists=1;
}

void set_sceneclip_y_min(int flag, float value) {
    clipinfo.clip_ymin = flag;
    clipinfo.ymin = value;
    updatefacelists = 1;
}

void set_sceneclip_y_max(int flag, float value) {
    clipinfo.clip_ymax = flag;
    clipinfo.ymax = value;
    updatefacelists = 1;
}

void set_sceneclip_z(int clipMin, float min, int clipMax, float max) {
    clipinfo.clip_zmin=clipMin;
    clipinfo.zmin = min;

    clipinfo.clip_zmax=clipMax;
    clipinfo.zmax = max;
    updatefacelists=1;
    update_clip_all();
}

void set_sceneclip_z_min(int flag, float value) {
    clipinfo.clip_zmin = flag;
    clipinfo.zmin = value;
    updatefacelists = 1;
}

void set_sceneclip_z_max(int flag, float value) {
    clipinfo.clip_zmax = flag;
    clipinfo.zmax = value;
    updatefacelists = 1;
}

/* ------------------ setrenderdir ------------------------ */

void setrenderdir(const char *dir) {
    printf("c_api: setting renderdir to: %s\n", dir);
	if(dir!=NULL&&strlen(dir)>0){
		script_dir_path=dir;
        if(can_write_to_dir(script_dir_path)==0){
          fprintf(stderr,"*** Error: Cannot write to the RENDERDIR directory: %s\n",script_dir_path);
        }
        PRINTF("script: setting render path to %s\n",script_dir_path);
   	} else {
        script_dir_path=NULL;
    }
  printf("c_api: renderdir set to: %s\n", script_dir_path);
}

/* ------------------ setcolorbarindex ------------------------ */
void setcolorbarindex(int chosen_index) {
	UpdateRGBColors(chosen_index);
}

/* ------------------ setcolorbarindex ------------------------ */
int getcolorbarindex() {
	return global_colorbar_index;
}

/* ------------------ setwindowsize ------------------------ */
void setwindowsize(int width, int height) {
  printf("Setting window size to %dx%d\n", width, height);
  glutReshapeWindow(width,height);
  ResizeWindow(width, height);
  Reshape_CB(width, height);

}

/* ------------------ setgridvisibility ------------------------ */

void setgridvisibility(int selection) {
	visGrid = selection;
	// selection may be one of:
	// - noGridnoProbe
	// - GridnoProbe
	// - GridProbe
	// - noGridProbe
	if(visGrid==GridProbe||visGrid==noGridProbe)visgridloc=1;
}

/* ------------------ setgridparms ------------------------ */

void setgridparms(int x_vis, int y_vis, int z_vis, int x_plot, int y_plot, int z_plot) {
	visx_all = x_vis;
	visy_all = y_vis;
	visz_all = z_vis;

	iplotx_all = x_plot;
	iploty_all = y_plot;
	iplotz_all = z_plot;

	if(iplotx_all>nplotx_all-1)iplotx_all=0;
	if(iploty_all>nploty_all-1)iploty_all=0;
	if(iplotz_all>nplotz_all-1)iplotz_all=0;
}

/* ------------------ setcolorbarflip ------------------------ */
void setcolorbarflip(int flip) {
	colorbarflip = flip;
	update_colorbarflip();
    UpdateRGBColors(COLORBAR_INDEX_NONE);
}

/* ------------------ getcolorbarflip ------------------------ */
int getcolorbarflip(int flip) {
    return colorbarflip;
}

// Camera API
// These function live-modify the current view by modifying "camera_current".
void camera_set_eyeview(int eyeview) {
  camera_current->rotation_type = eyeview;
}

void camera_set_rotation_index(int rotation_index) {
  camera_current->rotation_index = rotation_index;
}
/*
void camera_set_viewdir(float xcen, float ycen, float zcen) {
  viewpoint_set_xcen(xcen);
  viewpoint_set_ycen(ycen);
  viewpoint_set_zcen(zcen);
}
*/
// eyex
void camera_mod_eyex(float delta) {
  camera_current->eye[0] = camera_current->eye[0] + delta;
}
void camera_set_eyex(float eyex) {
  camera_current->eye[0] = eyex;
}

// eyey
void camera_mod_eyey(float delta) {
  camera_current->eye[1] = camera_current->eye[1] + delta;
}
void camera_set_eyey(float eyey) {
  camera_current->eye[1] = eyey;
}

// eyez
void camera_mod_eyez(float delta) {
  camera_current->eye[2] = camera_current->eye[2] + delta;
}
void camera_set_eyez(float eyez) {
  camera_current->eye[2] = eyez;
}

// azimuth
void camera_mod_az(float delta) {
  camera_current->az_elev[0] = camera_current->az_elev[0] + delta;
}
void camera_set_az(float az) {
  camera_current->az_elev[0] = az;
}

// elevation
void camera_mod_elev(float delta) {
  camera_current->az_elev[1] = camera_current->az_elev[1] + delta;
}
void camera_set_elev(float elev) {
  camera_current->az_elev[1] = elev;
}

// projection_type
void camera_toggle_projection_type() {
  camera_current->projection_type = 1 - camera_current->projection_type;
}
void camera_set_projection_type(int projection_type) {
  camera_current->projection_type = projection_type;
  // 1 is orthogonal
  // 0 is perspective
}
