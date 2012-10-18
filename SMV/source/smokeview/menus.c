// $Date$ 
// $Revision$
// $Author$

// svn revision character string
char menu_revision[]="$Revision$";

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
#include "update.h"
#include "smokeviewvars.h"
#include "IOvolsmoke.h"

/* dummy change to bump revision number to 5.1.5 */

#ifdef WIN32
#include <direct.h>
#endif

#define MENU_TIMEVIEW -103
#define SAVE_VIEWPOINT -101
#define MENU_STARTUPVIEW -102
#define MENU_OUTLINEVIEW -104
#define MENU_SIZEPRESERVING -105
#define MENU_DUMMY -999

#define GRID_yz 1
#define GRID_xz 2
#define GRID_xy 3
#define GRID_showall 4
#define GRID_hideall 5
#define GRID_grid 7
#define GRID_probe 8

void add_scriptlist(char *file, int id);
void update_glui_render(void);
void PropMenu(int value);
void initcircle(unsigned int nsegs);
void UnLoadVolSmoke3DMenu(int value);
void LoadVolSmoke3DMenu(int value);
void update_script_step(void);

#ifdef WIN32

/* ------------------ OpenSMVFile ------------------------ */

void OpenSMVFile(char *filebuffer,int filebufferlength,int *openfile){
  char stringFilter[]="Smokeview Files (*.smv)\0*.smv\0\0\0";
  char strTitle[80]="Select Smokeview Case";
  int buffersize;
  char smv_directory[1024];
  OPENFILENAME fileinfo;

  *openfile=0;
  buffersize=sizeof(OPENFILENAME);

  STRCPY(filebuffer,"");
  fileinfo.lStructSize=(unsigned long)buffersize;
  fileinfo.hwndOwner=NULL;
  fileinfo.lpstrFilter=stringFilter;
  fileinfo.lpstrCustomFilter=NULL;
  fileinfo.lpstrFile=filebuffer;
  fileinfo.nMaxFile=(unsigned long)filebufferlength;
  fileinfo.lpstrFileTitle=NULL;
  fileinfo.nMaxFileTitle=80;
  fileinfo.lpstrInitialDir=NULL;
  fileinfo.lpstrTitle=strTitle;
  fileinfo.Flags=0;
  fileinfo.lpstrDefExt=NULL;

  if(GetOpenFileName(&fileinfo)){
    STRCPY(smv_directory,"");
    strncat(smv_directory,filebuffer,fileinfo.nFileOffset);
    if( _chdir( smv_directory )   ){
      printf( "Unable to locate the directory: %s\n", smv_directory );
    }
    else{
      *openfile=1;
    }
  }
}
#endif

/* ------------------ TrainerViewMenu ------------------------ */

void TrainerViewMenu(int value){
  switch (value) {
  case 1:   // realistic
    HideAllSlices();
    trainerload=1;
    ShowAllSmoke();
    trainerload_old=1;
    break;
  case 2:  // temperature slices
    HideAllSmoke();
    trainerload=2;
    ShowAllSlices("TEMPERATURE",NULL);
    trainerload_old=2;
    break;
  case 3:  //  oxygen slices
    HideAllSmoke();
    trainerload=3;
    ShowAllSlices("OXYGEN","OXYGEN VOLUME FRACTION");
    trainerload_old=3;
    break;
  case 998: // unload
    LoadUnloadMenu(UNLOADALL);
    trainerload=0;
    trainerload_old=0;
    break;
  default:
    ASSERT(FFALSE);
  }
  updatemenu=1;
  glutPostRedisplay();
}

/* ------------------ MainMenu ------------------------ */

void MainMenu(int value){

  if(value==3){
    if(scriptoutstream!=NULL){
      ScriptMenu(SCRIPT_STOP_RECORDING);
    }
    exit(0);
  }
  if(value==1){
    defaulttour();
  }
  if(value==997){
    trainer_mode=1-trainer_mode;
  }
  updatemenu=1;  
  glutPostRedisplay();  
}

/* ------------------ StaticVariableMenu ------------------------ */

void StaticVariableMenu(int value){
  mesh *meshi;

  meshi=current_mesh;
  plotn=value;
  plotstate=STATIC_PLOTS;
  visGrid=0;
  if(visiso==1){
    updateshowstep(1,ISO);
  }
  updatesurface();
  if(visx_all==1){
    updateshowstep(1,DIRX);
  }
  if(visy_all==1){
    updateshowstep(1,DIRY);
  }
  if(visz_all==1){
    updateshowstep(1,DIRZ);
  }
  if(visx_all==0&&visy_all==0&&visz_all==0){
    updateshowstep(1,DIRY);
  }
  updateallplotslices();
  updatemenu=1;  
  glutPostRedisplay();  
  updateplot3dlistindex();
}

/* ------------------ IsoVariableMenu ------------------------ */

void IsoVariableMenu(int value){
  mesh *meshi;
  meshi=current_mesh;
  if(ReadPlot3dFile==1){
    plotn=value;
    if(visx_all==1){
      updateshowstep(1,DIRX);
    }
    if(visy_all==1){
      updateshowstep(1,DIRY);
    }
    if(visz_all==1){
      updateshowstep(1,DIRZ);
    }
    updateshowstep(1,ISO);
    updatesurface();
    plotstate=STATIC_PLOTS;
    updateplotslice(1);
    updateplotslice(2);
    updateplotslice(3);
    updatemenu=1;  
    glutPostRedisplay();
    updateplot3dlistindex();
  }
}

/* ------------------ LabelMenu ------------------------ */

void LabelMenu(int value){
  updatemenu=1;  
  glutPostRedisplay();
  ASSERTFLAG(visColorbarLabels);
  ASSERTFLAG(visTimeLabels);
  ASSERTFLAG(visTitle0);
  ASSERTFLAG(visFramerate);
  ASSERTFLAG(visaxislabels);
  ASSERTFLAG(vis_slice_average);
  switch (value){
   case 0:
    visColorbarLabels=1-visColorbarLabels;
    break;
   case 1:
    visTimeLabels=1-visTimeLabels;
    break;
   case 2:
    visTitle0=1-visTitle0;
    break;
   case 3:
    visFramerate = 1 - visFramerate;
    break;
   case 4:
    vis_user_ticks=1;
    visColorbarLabels=1;
    visTimeLabels=1;
    visTitle0=1;
    visTitle1=1;
    visTitle2=1;
    visFramerate=1;
#ifdef pp_memstatus
    visAvailmemory=1;
#endif
    visaxislabels=1;
    visTimelabel=1;
    visFramelabel=1;
    visLabels=1;
    visBlocklabel=1;
    vis_slice_average=1;
    if(nticks>0)visTicks=1;
    visgridloc=1;
    visHRRlabel=1;
    show_hrrcutoff=1;
    visFramelabel=1;
    if(hrrinfo!=NULL){
      if(hrrinfo->display==0){
        hrrinfo->display=1;
        updatetimes();
      }
      hrrinfo->display=1;
    }
    break;
   case 5:
    vis_user_ticks=0;
    visColorbarLabels=0;
    visTimeLabels=0;
    visTitle0=0;
    visTitle1=0;
    visTitle2=0;
    visFramerate=0;
    visaxislabels=0;
    visLabels=0;
    visTimelabel=0;
    visFramelabel=0;
    visBlocklabel=0;
    visHRRlabel=0;
    show_hrrcutoff=0;
    if(hrrinfo!=NULL){
      if(hrrinfo->display==1){
        hrrinfo->display=0;
        updatetimes();
      }
      hrrinfo->display=0;
    }
    if(nticks>0)visTicks=0;
    visgridloc=0;
    vis_slice_average=0;
#ifdef pp_memstatus
    visAvailmemory=0;
#endif
    break;
   case 6:
    visaxislabels = 1 - visaxislabels;
    break;
   case 7:
     visLabels = 1 - visLabels;
     break;
   case 8:
     visTimelabel=1-visTimelabel;
     if(visTimelabel==1)visTimeLabels=1;
     break;
   case 9:
     visFramelabel=1-visFramelabel;
     if(visFramelabel==1)visTimeLabels=1;
     if(visFramelabel==1){
       visHRRlabel=0;
       if(hrrinfo!=NULL){
         hrrinfo->display=visHRRlabel;
         updatetimes();
       }
     }
     break;
   case 10:
     visBlocklabel=1-visBlocklabel;
     break;
#ifdef pp_memstatus
   case 11:
     visAvailmemory = 1 - visAvailmemory;
     break;
#endif
#ifdef pp_MEMDEBUG
   case 19:
     visUsagememory = 1 - visUsagememory;
#ifdef pp_memstatus
     if(visUsagememory==1)visAvailmemory=0;
#endif
     break;
#endif
   case 12:
     visTicks=1-visTicks;
     break;
   case 13:
     vishmsTimelabel = 1 - vishmsTimelabel;
     break;
   case 14:
     visgridloc = 1 - visgridloc;
     break;
   case 15:
     vis_slice_average = 1 - vis_slice_average;
     break;
   case 16:
     visHRRlabel=1-visHRRlabel;
     if(visHRRlabel==1)visTimeLabels=1;
     if(hrrinfo!=NULL){
       hrrinfo->display=visHRRlabel;
       updatetimes();
     }
     break;
   case 17:
     show_hrrcutoff=1-show_hrrcutoff;
     break;
   case 18:
     vis_user_ticks = 1 - vis_user_ticks;
     break;
   default:
     ASSERT(FFALSE);
     break;
  }
  updateshowtitles();
  set_labels_controls();

}

/* ------------------ updateshowtitles ------------------------ */

void updateshowtitles(void){
  ntitles=0;
  if(visTitle0==1)ntitles++;
  if(strlen(TITLE1)!=0&&visTitle1==1){
    ntitles++;
    showtitle1=1;
  }
  if(strlen(TITLE2)!=0&&visTitle2==1){
    ntitles++;
    showtitle2=1;
  }
  visTitle=0;
  if(visTitle0==1||showtitle1==1||showtitle2==1)visTitle=1;
}

/* ------------------ LightingMenu ------------------------ */

void LightingMenu(int value){
    if(value<0)return;
    ASSERTFLAG(visLIGHT0);
    ASSERTFLAG(visLIGHT1);
    switch (value){
      case 1: 
        visLIGHT0 = 1 - visLIGHT0; 
        break;
      case 2: 
        visLIGHT1 = 1 - visLIGHT1; 
        break;
      case 3: 
        visLIGHT1 = 1 - visLIGHT1; 
        visLIGHT0 = 1 - visLIGHT0; 
        break;
      case 4: 
        visLIGHT0=1; 
        visLIGHT1=1; 
        break;
      case 5: 
        visLIGHT0=0; 
        visLIGHT1=0; 
        break;
      default:
        ASSERT(FFALSE);
        break;
    }
    UpdateLIGHTS=1;
    updatemenu=1;  
    glutPostRedisplay();
}


/* ------------------ SmokeColorBarMenu ------------------------ */

void SmokeColorBarMenu(int value){
  if(value==-999)return;
  updatemenu=1;

  if(value<0)value=0;
  if(value>ncolorbars-1)value=ncolorbars-1;
  fire_colorbar_index=value;
  fire_colorbar = colorbarinfo + value;
  updatecolors(-1);
  if(FlowDir>0){
    keyboard('-',FROM_SMOKEVIEW);
    keyboard(' ',FROM_SMOKEVIEW);
  }
  else{
    keyboard(' ',FROM_SMOKEVIEW);
    keyboard('-',FROM_SMOKEVIEW);
  }
  glutPostRedisplay();
}

/* ------------------ ColorBarMenu ------------------------ */

void ColorBarMenu(int value){
  if(value==-999)return;
  updatemenu=1;
  glutPostRedisplay();
  if(value<0){
    switch (value){
    case -2:
      colorbarflip=1-colorbarflip;
      break;
    case -3:
      colorbarcycle++;
      if(colorbarcycle>=nrgb)colorbarcycle=0;
      break;
    case -4:
      colorbarcycle=0;
      show_extremedata=0;
      colorbarflip=0;
      contour_type=SHADED_CONTOURS;
      setbw=0;
      update_extreme();
      updatecolors(-1);
      break;
    case -5:
      viscolorbarpath=1-viscolorbarpath;
      break;
    case -7:
      show_extremedata=1-show_extremedata;
      update_extreme();
      updatecolors(-1);
      break;
    case -12:
     setbw=1-setbw;
     if(setbw==1){
       colorbartype_save=colorbartype;
       ColorBarMenu(bw_colorbar_index);
     }
     else{
       colorbartype=colorbartype_save;
       ColorBarMenu(colorbartype);
     }
     updatecolors(-1);
     set_labels_controls();
     break;
   case -13:
     use_transparency_data=1-use_transparency_data;
     updatecolors(-1);
     set_labels_controls();
     update_transparency();
     break;
   case -14:
     colorbarflip=1-colorbarflip;updatecolors(-1);
     break;
   case -15:
     colorbarcycle++;
     if(colorbarcycle>=nrgb)colorbarcycle=0;
     updatecolors(-1);
     break;
   case -16:
     colorbarcycle=0;
     background_flip=0;
     setbw=0;
     updatecolors(-1);
     break;
   case -17:
     contour_type=SHADED_CONTOURS;
     updatecolors(-1);
     break;
   case -18:
     contour_type=STEPPED_CONTOURS;
     updatecolors(-1);
     break;
   case -19:
     contour_type=LINE_CONTOURS;
     updatecolors(-1);
     break;
   default:
     ASSERT(FFALSE);
     break;
   }
  }
  if(value>=0){
    colorbartype=value;
    selectedcolorbar_index2=colorbartype;
    current_colorbar=colorbarinfo+colorbartype;
    update_colorbar_type();
    update_colorbar_list2();
    if(value==bw_colorbar_index){
      setbw=1;
    }
    else{
      setbw=0;
    }
  }
  if(value>-10){
    updatecolors(-1);
  }
}

/* ------------------ ShadeMenu ------------------------ */

void ShadeMenu(int value){
  ColorBarMenu(-10-value);
}
/* ------------------ Smoke3DShowMenu ------------------------ */

void Smoke3DShowMenu(int value){
  smoke3ddata *smoke3di;
  int i;

  updatemenu=1;  
  glutPostRedisplay();
  if(value<0){
    switch (value){
    case SHOW_ALL:
      plotstate=DYNAMIC_PLOTS;
      for(i=0;i<nsmoke3dinfo;i++){
        smoke3di = smoke3dinfo + i;
        if(smoke3di->loaded==1)smoke3di->display=1;
      }
      break;
    case HIDE_ALL:
      for(i=0;i<nsmoke3dinfo;i++){
        smoke3di = smoke3dinfo + i;
        if(smoke3di->loaded==1)smoke3di->display=0;
      }
      break;
    case HAVE_LIGHT:
      show_smoke_lighting = 1 - show_smoke_lighting;
      break;
    default:
      ASSERT(FFALSE);
    }
  }
  else{
    smoke3di = smoke3dinfo + value;
    if(plotstate!=DYNAMIC_PLOTS){
      plotstate=DYNAMIC_PLOTS;
      smoke3di->display=1;
    }
    else{
      smoke3di->display = 1 - smoke3di->display;
    }
  }

}

/* ------------------ IsoShowMenu ------------------------ */

void IsoShowMenu(int value){
  int i;
  int nisolevels, *showlevels;
  isodata *isoi;

  nisolevels=loaded_isomesh->nisolevels;
  showlevels=loaded_isomesh->showlevels;

  ASSERTFLAG(smoothtrinormal);
  ASSERTFLAG(showtrinormal);
  switch (value){
   case  4:
    smoothtrinormal=1-smoothtrinormal;
    break;
   case 5:
    showtrinormal = 1 - showtrinormal;
    break;
   case 1:
   case 2:
   case 3:
    if(value==1){
      showtrisurface=1-showtrisurface;
    }
    if(value==2){
      showtrioutline=1-showtrioutline;
    }
    if(value==3){
      showtripoints=1-showtripoints;
    }
    visAIso=showtrisurface*1+showtrioutline*2+showtripoints*4;
    if(visAIso!=0){
      plotstate=DYNAMIC_PLOTS;
    }
    update_glui_isotype();
    break;
   case 94:
    transparent_state=ALL_SOLID;
    if(loaded_isomesh==NULL)break;
    for(i=0;i<loaded_isomesh->nisolevels;i++){
      surfdata *surfi;

      surfi = surfinfo + nsurfinfo + 1 + i;
      surfi->transparent_level=1.0;
    }
    use_transparency_data=0;
    break;
   case 95:
    transparent_state=ALL_TRANSPARENT;
    if(loaded_isomesh==NULL)break;
    for(i=0;i<loaded_isomesh->nisolevels;i++){
      surfdata *surfi;

      surfi = surfinfo + nsurfinfo + 1 + i;
      surfi->transparent_level=transparent_level;
    }
    use_transparency_data=1;
    break;
   case 96:
    transparent_state=MIN_SOLID;
    if(loaded_isomesh==NULL)break;
    for(i=0;i<loaded_isomesh->nisolevels;i++){
      surfdata *surfi;

      surfi = surfinfo + nsurfinfo + 1 + i;
      surfi->transparent_level=transparent_level;
    }
    surfinfo[nsurfinfo+1].transparent_level=1.0;
    use_transparency_data=1;
    break;
   case 97:
    transparent_state=MAX_SOLID;
    if(loaded_isomesh==NULL)break;
    for(i=0;i<loaded_isomesh->nisolevels;i++){
      surfdata *surfi;

      surfi = surfinfo + nsurfinfo + 1 + i;
      surfi->transparent_level=transparent_level;
    }
    use_transparency_data=1;
    surfinfo[nsurfinfo+1+loaded_isomesh->nisolevels-1].transparent_level=1.0;
    break;
   case 98:
    showtrisurface=0;
    showtrioutline=0;
    showtripoints=0;
    visAIso=showtrisurface*1+showtrioutline*2+showtripoints*4;
    for(i=0;i<nisolevels;i++){
      showlevels[i]=0;
    }
    break;
   case 99:
    showtrisurface=1;
    showtrioutline=0;
    showtripoints=0;
    visAIso=showtrisurface*1+showtrioutline*2+showtripoints*4;
    for(i=0;i<nisolevels;i++){
      showlevels[i]=1;
    }
    break;
   default:
    if(value>99&&value<999&&value-100<nisolevels){
     ASSERTFLAG(showlevels[value-100]);
     showlevels[value-100] = 1 - showlevels[value-100];
    }
    else if(value>=1000&&value<=10000){      // we can only have 9900 isosurface files
     isoi = isoinfo + value - 1000;          // hope that is enough!!
     if(plotstate!=DYNAMIC_PLOTS){
       plotstate=DYNAMIC_PLOTS;
       isoi->display=1;
       iisotype=isoi->type;
     }
     else{
       if(isoi->type==iisotype){
         isoi->display = 1 - isoi->display;
         update_isotype();
       }
       else{
         isoi->display=1;
         iisotype=isoi->type;
       }
     }
     updateShow();
    }
    else if(value>=10001){
      if(value==10001){
        plotstate=DYNAMIC_PLOTS;
        for(i=0;i<nisoinfo;i++){
          isoinfo[i].display=1;
        }
      }
      else if(value==10002){
        for(i=0;i<nisoinfo;i++){
          isoinfo[i].display=0;
        }
      }
     updateShow();
    }
  }
  update_iso_showlevels();
  Update_Isotris(1);

  updatemenu=1;  
  glutPostRedisplay();
}

/* ------------------ ShowVSliceMenu ------------------------ */

void ShowVSliceMenu(int value){
  vslicedata *vd;
  slicedata *sd;
  int i;
  updatemenu=1;  
  glutPostRedisplay();
  if(value==SHOW_ALL){
    for(i=0;i<nvsliceinfo;i++){
      vd = vsliceinfo + i;
      if(vd->loaded==0)continue;
      vd->display=1;
    }
    show_all_slices=1;
    updatetimes();
    return;
  }
  if(value==HIDE_ALL){
    for(i=0;i<nvsliceinfo;i++){
      vd = vsliceinfo + i;
      if(vd->loaded==0)continue;
      vd->display=0;
    }
    show_all_slices=0;
    updatetimes();
    return;
  }
  if(value==-11){
    show_slice_in_obst=1-show_slice_in_obst;
    return;
  }
  if(value==-12){
    offset_slice=1-offset_slice;
    return;
  }
  vd = vsliceinfo + value;
  ASSERTFLAG(vd->display);
  if(islicetype==sliceinfo[vd->ival].type){
    if(plotstate!=DYNAMIC_PLOTS){
      plotstate=DYNAMIC_PLOTS;
      vd->display=1;
    }
    else{
      vd->display = 1 - vd->display;
    }
    if(vd->iu!=-1){
      sd=sliceinfo+vd->iu;
      sd->vloaded=vd->display;
    }
    if(vd->iv!=-1){
      sd=sliceinfo+vd->iv;
      sd->vloaded=vd->display;
    }
    if(vd->iw!=-1){
      sd=sliceinfo+vd->iw;
      sd->vloaded=vd->display;
    }
    if(vd->ival!=-1){
      sd=sliceinfo+vd->ival;
      sd->vloaded=vd->display;
    }
  }
  else{
    islicetype = sliceinfo[vd->ival].type;
    sd=sliceinfo+vd->ival;
    vd->display=1;
  }
  plotstate=getplotstate(DYNAMIC_PLOTS);
  updateShow();
}

/* ------------------ ShowHideSliceMenu ------------------------ */

void ShowHideSliceMenu(int value){
  int i;

  updatemenu=1;  
  glutPostRedisplay();
  if(value<0){
    switch (value){
    case SHOW_ALL:
      for(i=0;i<nsliceinfo;i++){
        sliceinfo[i].display=1;
      }
      show_all_slices=1;
      break;
    case HIDE_ALL:
      for(i=0;i<nsliceinfo;i++){
        sliceinfo[i].display=0;
      }
      show_all_slices=0;
      break;
    case -11:
      show_slice_in_obst=1-show_slice_in_obst;
      break;
    case -12:
      offset_slice=1-offset_slice;
      break;
    case -13:
      planar_terrain_slice=1-planar_terrain_slice;
      break;
    case -14:
      show_fed_area=1-show_fed_area;
      break;
    default:
      ASSERT(FFALSE);
    }
  }
  else{
    slicedata *sd;

    sd = sliceinfo + value;
    ASSERTFLAG(sd->display);
    if(islicetype==sd->type){
      if(plotstate!=DYNAMIC_PLOTS){
        plotstate=DYNAMIC_PLOTS;
        sd->display=1;
      }
      else{
        sd->display = 1 - sd->display;
      }
    }
    else{
      plotstate=DYNAMIC_PLOTS;
      islicetype = sd->type;
      sd->display=1;
    }
  }
  updateslicefilenum();
  plotstate=getplotstate(DYNAMIC_PLOTS);

  updateglui();
  updateslicelistindex(slicefilenum);
  updateShow();
}

/* ------------------ ShowMultiSliceMenu ------------------------ */

void ShowMultiSliceMenu(int value){
  multislicedata *mslicei;
  slicedata *sd;
  int mdisplay;
  int i;

  updatemenu=1;  
  glutPostRedisplay();
  switch (value){
  case SHOW_ALL:
  case HIDE_ALL:
    ShowHideSliceMenu(value);
    return;
  case -11:
    show_slice_in_obst=1-show_slice_in_obst;
    break;
  case -12:
    offset_slice=1-offset_slice;
    break;
  case -14:
    show_fed_area=1-show_fed_area;
  default:
    mslicei = multisliceinfo + value;
    mdisplay=0;
    if(islicetype==mslicei->type){
      if(plotstate!=DYNAMIC_PLOTS){
        plotstate=DYNAMIC_PLOTS;
        mdisplay=1;
      }
      else{
        mdisplay = 1 - mslicei->display;
      }
    }
    else{
      plotstate=DYNAMIC_PLOTS;
      islicetype=mslicei->type;
      mdisplay=1;
    }
    for(i=0;i<mslicei->nslices;i++){
      sd = sliceinfo + mslicei->islices[i];
      if(sd->loaded==0)continue;
      sd->display=mdisplay;
    }
    break;
  }
  updateslicefilenum();
  plotstate=getplotstate(DYNAMIC_PLOTS);

  updateglui();
  updateslicelistindex(slicefilenum);
  updateShow();
}

/* ------------------ ShowHideMenu ------------------------ */

void ShowHideMenu(int value){
  updatemenu=1;  
  glutPostRedisplay();
  switch (value){
#ifdef _DEBUG
  case 16:
    PrintMemoryInfo;
    break;
#endif
  case 15:
   background_flip = 1-background_flip;
   updatecolors(-1);
   set_labels_controls();
   break;
  case 13:
    if(plotstate==DYNAMIC_PLOTS){
      visEvac=1-visEvac;
    }
    else{
      plotstate=DYNAMIC_PLOTS;
      visEvac=1;
    }
    updatetimes();
    break;
  case 1:
    if(plotstate==DYNAMIC_PLOTS){
      visSmoke=1-visSmoke;
    }
    else{
      plotstate=DYNAMIC_PLOTS;
      visSmoke=1;
    }
    updatetimes();
    break;
  case 2:
    visTarg=1-visTarg;
    break;
  case 9:
    visSensor=1-visSensor;
    break;
  case 14:
    visSensorNorm=1-visSensorNorm;
    break;
  case 10:
    visHeat=1-visHeat;
    break;
  case 11:
    visSprink=1-visSprink;
    break;
  case 12:
    if(titlesafe_offset==0){
      titlesafe_offset=titlesafe_offsetBASE;
    }
    else{
      titlesafe_offset=0;
    }
    break;
  default:
    ASSERT(FFALSE);
    break;
  }
}

/* ------------------ ViewpointMenu ------------------------ */

void ViewpointMenu(int value){
  if(value==999)return;
  updatemenu=1;  
  glutPostRedisplay();
  switch (value){
  case TOGGLE_TITLE_SAFE:
    if(titlesafe_offset==0){
      titlesafe_offset=titlesafe_offsetBASE;
    }
    else{
      titlesafe_offset=0;
    }
    break;
  default:
    ASSERT(FFALSE);
    break;
  }
}

/* ------------------ DialogMenu ------------------------ */


void DialogMenu(int value){
  glutPostRedisplay();
  switch (value){
#ifdef pp_SHOOTER
  case 27:
    showshooter_dialog=1-showshooter_dialog;
    if(showshooter_dialog==1)show_glui_shooter();
    if(showshooter_dialog==0)hide_glui_shooter();
    break;
#endif
  case 25:
    showtrainer_dialog=1-showtrainer_dialog;
    if(showtrainer_dialog==1)show_glui_trainer();
    if(showtrainer_dialog==0)hide_glui_trainer();
    break;
  case 22:
    showdisplay_dialog=1-showdisplay_dialog;
    if(showdisplay_dialog==1)show_glui_display();
    if(showdisplay_dialog==0)hide_glui_display();
    break;
  case 28:
    showdevice_dialog=1-showdevice_dialog;
    if(showdevice_dialog==1)show_glui_device();
    if(showdevice_dialog==0)hide_glui_device();
    break;
  case 14:
    showbounds_dialog=1-showbounds_dialog;
    if(showbounds_dialog==1)show_glui_bounds();
    if(showbounds_dialog==0){
      show3dsmoke_dialog=0;
      showgluizip=0;
      hide_glui_bounds();
    }
    break;
  case 20:
    show3dsmoke_dialog=1-show3dsmoke_dialog;
    if(show3dsmoke_dialog==1){
      if(showbounds_dialog==0)DialogMenu(14);
      open_smokepanel();
    }
    else{
      close_smokepanel();
    }
    break;
  case 24:
    showgluizip=1-showgluizip;
    if(showgluizip==1){
      if(showbounds_dialog==0)DialogMenu(14);
      open_smokezippanel();
    }
    else{
      close_smokezippanel();
    }
    break;
  case 15:
    showmotion_dialog=1-showmotion_dialog;
    if(showmotion_dialog==1)show_glui_motion();
    if(showmotion_dialog==0)hide_glui_motion();
    break;
  case 21:
   showtour_dialog=1-showtour_dialog;
   if(showtour_dialog==1)show_glui_tour();
   if(showtour_dialog==0)hide_glui_tour();
   break;
  case 18:
    showclip_dialog=1-showclip_dialog;
    if(showclip_dialog==1)show_glui_clip();
    if(showclip_dialog==0)hide_glui_clip();
    break;
  case 19:
    showstereo_dialog=1-showstereo_dialog;
    if(showstereo_dialog==1)show_glui_stereo();
    if(showstereo_dialog==0)hide_glui_stereo();
    break;
  case 26:
    showwui_dialog=1-showwui_dialog;
    if(showwui_dialog==1)show_glui_wui();
    if(showwui_dialog==0)hide_glui_wui();
    break;
  case 23:
    showcolorbar_dialog=1-showcolorbar_dialog;
    if(showcolorbar_dialog==1){
      viscolorbarpath=1;
      show_glui_colorbar();
    }
    if(showcolorbar_dialog==0){
      viscolorbarpath=0;
      hide_glui_colorbar();
    }
    break;
  case 16:
    showedit_dialog=1-showedit_dialog;
    if(showedit_dialog==1){
      if(fds_filein!=NULL&&updategetobstlabels==1){
        CheckMemoryOff;
        getobstlabels(fds_filein);
        CheckMemoryOn;
        updategetobstlabels=0;
      }
      visBlocksSave=visBlocks;
      show_glui_edit();
      visBlocks=visBLOCKNormal;
    }
    if(showedit_dialog==0){
      hide_glui_edit();
      visBlocks=visBlocksSave;
    }
    update_trainer_outline();

    break;
  case -2:
    showdisplay_dialog=0;
    hide_glui_display();
    showbounds_dialog=0;
    show3dsmoke_dialog=0;
    showgluivol3dsmoke=0;
    showgluizip=0;
    hide_glui_bounds();
    showmotion_dialog=0;
    hide_glui_motion();
    showtour_dialog=0;
    hide_glui_tour();
    showclip_dialog=0;
    hide_glui_clip();
    showwui_dialog=0;
    hide_glui_wui();
    showstereo_dialog=0;
    hide_glui_stereo();
    showcolorbar_dialog=0;
    hide_glui_colorbar();
    if(showedit_dialog==1)DialogMenu(16);
    showtrainer_dialog=0;
    hide_glui_trainer();
    showdevice_dialog=0;
    hide_glui_device();
    break;
  default:
    ASSERT(FFALSE);
    break;
  }
  updatemenu=1;
}

/* ------------------ ZoomMenu ------------------------ */

void ZoomMenu(int value){
  if(value==999)return;
  updatemenu=1;  
  if(opengldefined==1){
    glutPostRedisplay();
  }
  zoomindex=value;
  if(zoomindex==-1){
    if(zoom<zooms[0]){
      zoom=zooms[0];
      zoomindex=0;
    }
    if(zoom>zooms[4]){
      zoom=zooms[4];
      zoomindex=4;
    }
    if(projection_type!=0){
      camera_current->projection_type=projection_type;
      ResetView(RESTORE_EXTERIOR_VIEW);
      update_projection_type();
    }
  }
  else if(zoomindex==-2){
    camera_current->projection_type=projection_type;
    update_projection_type();
    if(projection_type==0){
      update_camera_ypos(camera_current);
      //ResetView(RESTORE_EXTERIOR_VIEW);
    }
    else{
      camera_current->eye[1]=camera_current->isometric_y;
    }

  }
  else{
    if(zoomindex<0)zoomindex=2;
    if(zoomindex>4)zoomindex=2;
    zoom=zooms[zoomindex];
    if(projection_type!=0){
      ResetView(RESTORE_EXTERIOR_VIEW_ZOOM);
      camera_current->projection_type=projection_type;
      update_projection_type();
    }
  }
  camera_current->zoom=zoom;
  update_glui_zoom();
}

/* ------------------ ApertureMenu ------------------------ */

void ApertureMenu(int value){
  updatemenu=1;  
  if(opengldefined==1){
    glutPostRedisplay();
  }
  apertureindex=value;
  if(apertureindex<0)apertureindex=0;
  if(apertureindex>4)apertureindex=4;
  aperture=apertures[apertureindex];
}

/* ------------------ LanguageMenu ------------------------ */
#ifdef pp_LANG
void LanguageMenu(int value){
  updatemenu=1;
  if(opengldefined==1){
    glutPostRedisplay();
  }
  if(value==-1){
    strcpy(startup_lang_code,"en");
  }
  else{
    langlistdata *langi;

    if(value>=0&&value<nlanglistinfo){
      langi = langlistinfo + value;

      strcpy(startup_lang_code,langi->lang_code);
    }
  }
  init_translate(smokeview_bindir,startup_lang_code);
  update_glui_names();
}
#endif
/* ------------------ FontMenu ------------------------ */

void FontMenu(int value){
  updatemenu=1;  
  if(opengldefined==1){
    glutPostRedisplay();
  }
  switch (value){
  case SMALL_FONT:
    fontindex=SMALL_FONT;
    large_font=GLUT_BITMAP_HELVETICA_12;
    small_font=GLUT_BITMAP_HELVETICA_10;
    large_font_height=12;
    small_font_height=10;
    fontWoffset=0;
    fontHoffset=0;
    dwinH=dwinHbase;
    dwinH=1.2*dwinHbase;
    break;
  case LARGE_FONT:
    fontindex=LARGE_FONT;
    large_font=GLUT_BITMAP_HELVETICA_18;
    small_font=GLUT_BITMAP_HELVETICA_18;
    large_font_height=18;
    small_font_height=18;
    fontWoffset=0;
    fontHoffset=0;
    dwinH=1.2*dwinHbase;
    break;
  case SCALED_FONT:
    fontWoffset=0;
    fontHoffset=0;
    dwinH=dwinHbase;
    dwinH=1.2*dwinHbase;
    fontindex=SCALED_FONT; 
    break;
  default:
    ASSERT(FFALSE);
  }
  glui_update_fontindex();
  set_labels_controls();
}

/* ------------------ UnitsMenu ------------------------ */

void UnitsMenu(int value){
  int unitclass, unittype;
  int i;

  unitclass = value/1000;
  unittype = value - unitclass*1000;
  unitclasses[unitclass].active=unittype;
  if(value==-1){
    for(i=0;i<nunitclasses;i++){
      unitclasses[i].active=0;
    }
  }
  else if(value==-2){
    vishmsTimelabel = 1 - vishmsTimelabel;
    set_labels_controls();

  }
  updatemenu=1;  
  glutPostRedisplay();
}

/* ------------------ OptionMenu ------------------------ */

void OptionMenu(int value){
  if(value==999)return;
  updatemenu=1;  
  glutPostRedisplay();
  if(value==1){
    Labels_CB(17); // run the benchmark
  }
  if(value==2){
    trainer_mode=1;
    if(showtrainer_dialog==0){
      showtrainer_dialog=1;
      show_glui_trainer();
    }
    FontMenu(1);
  }
}

/* ------------------ Get_Next_View_Label ------------------------ */

void Get_Next_View_Label(char *label){
  camera *ca;
  int i;

  for(i=1;;i++){
    char view[256],*newview;

    sprintf(view,"view %i",i);
    newview=NULL;
    for(ca=camera_list_first.next;ca->next!=NULL;ca=ca->next){
      if(strcmp(view,ca->name)==0){
        newview=ca->name;
        break;
      }
    }
    if(newview==NULL){
      strcpy(label,view);
      return;
    }
  }
}

/* ------------------ ResetMenu ------------------------ */

void ResetMenu(int value){
  char view_label[256];

  if(value==MENU_DUMMY)return;
  switch (value){
  case MENU_SIZEPRESERVING:
    projection_type = 1 - projection_type;
    Motion_CB(PROJECTION);
    break;
  case MENU_OUTLINEVIEW:
    if(visBlocks==visBLOCKOutline){
      BlockageMenu(visBLOCKAsInput);
    }
    else{
      BlockageMenu(visBLOCKOutline);
    }
    break;
  case MENU_TIMEVIEW:
    updatetimes();
    break;
  case SAVE_VIEWPOINT:
    Get_Next_View_Label(view_label);
    add_list_view(view_label);
    break;
  case MENU_STARTUPVIEW:
    if(selected_view==-999)ResetMenu(SAVE_VIEWPOINT);
    set_startup_view();
    break;
  default:
    ASSERT(value>=0);
    if(value<100000){
      reset_glui_view(value);
      if(scriptoutstream!=NULL){
        fprintf(scriptoutstream,"SETVIEWPOINT\n");
        fprintf(scriptoutstream," %s\n",camera_current->name);
      }
    }
    break;
  }
  //updatezoommenu=1; // updating zoom causes a bug when restoring views from the menu
                      // kept commented code in for future reference
  updatemenu=1;  
  glutPostRedisplay();
}

/* ------------------ RenderState ------------------------ */

void RenderState(int onoff){
  if(onoff==1){
    saveW=screenWidth;
    saveH=screenHeight;
    RenderGif=1;
    if(renderW==0||renderH==0){
      ResizeWindow(screenWidth,screenHeight);
    }
    else{
      if(renderW>max_screenWidth){
        render_double=render_double_state;
        ResizeWindow(max_screenWidth,max_screenHeight);
      }
      else{
        ResizeWindow(renderW,renderH);
      }
    }
  }
  else{
    render_double=0;
    render_double_state=0;
    RenderGif=0;
    setScreenSize(&saveW,&saveH);
    ResizeWindow(screenWidth,screenHeight);
  }
}

/* ------------------ RenderMenu ------------------------ */

void RenderMenu(int value){
  slicedata *sd;
  int i,n;
  mesh *meshi;

  updatemenu=1;
  if(value>=10000)return;
  if(opengldefined==1){
    glutPostRedisplay();
  }
  switch (value){
  case Render320:
    render_double_menu=0;
    render_option=value;
    render_double_state=0;
    renderW=320;
    renderH=240;
    break;
  case Render640:
    render_double_menu=0;
    render_option=value;
    render_double_state=0;
    renderW=640;
    renderH=480;
    break;
  case RenderWindow:
    render_double_menu=0;
    render_option=value;
    render_double_state=0;
    renderW=0;
    renderH=0;
    break;
  case Render2Window:
    render_double_menu=1;
    render_option=value;
    renderW=2*screenWidth;
    renderH=2*screenHeight;
    render_double_state=2;
    break;
  case RenderOnce:
    render_from_menu=1;
    if(render_double_menu==1)render_double_state=1;
    if(render_double_state!=0){
      render_double=render_double_state;
      keyboard('R',FROM_SMOKEVIEW);
    }
    else{
      keyboard('r',FROM_SMOKEVIEW);
    }
     break;
  case RenderCancel:
    render_double_menu=0;
    render_double=0;
    render_double_state=0;
    RenderState(0);
    break;
  case RenderLABELframenumber:
    renderfilelabel=0;
    update_glui_filelabel(renderfilelabel);
    break;
  case RenderLABELtime:
    renderfilelabel=1;
    update_glui_filelabel(renderfilelabel);
    break;
  case RenderPNG:
     renderfiletype=0;
     updatemenu=1;  
     break;
#ifdef pp_JPEG
  case RenderJPEG:
     renderfiletype=1;
     updatemenu=1;  
     break;
#endif
  default:
    if(RenderTime==0&&touring==0)return;
    if(touring==1){
      rendertourcount=0;
      tourangle_global=0.0;
    }
    if(stept==0){
      keyboard('t',FROM_SMOKEVIEW);
    }
    RenderState(1);
    reset_itimes0();
    for(i=0;i<nsliceinfo;i++){
      sd=sliceinfo+i;
      sd->itime=0;
    }
    iframe=iframebeg;
    for(i=0;i<nmeshes;i++){
      meshi=meshinfo+i;
      meshi->patch_itime=0;
    }
    UpdateTimeLabels();
    RenderSkip=value;
    FlowDir=1;
    for(n=0;n<nglobal_times;n++){
      render_frame[n]=0;
    }
    if(scriptoutstream!=NULL){
      fprintf(scriptoutstream,"RENDERALL\n");
      fprintf(scriptoutstream," %i\n",RenderSkip);
      fprintf(scriptoutstream,"\n");
    }
    break;
  }
}

/* ------------------ EvacShowMenu ------------------------ */

void EvacShowMenu(int value){
  partdata *parti;
  int i;

  if(nevac==0)return;
  if(value==999)return;
  ASSERTFLAG(visEvac);
  if(value<0){
    value = -value;
    value--;
    parti = partinfo + value;
    parti->display = 1 - parti->display;
    updatemenu=1;  
    glutPostRedisplay();
    plotstate=getplotstate(DYNAMIC_PLOTS);
    return;
  }
  if(plotstate==DYNAMIC_PLOTS){
    switch (value){
    case 3:
      visEvac=1;
      for(i=0;i<npartinfo;i++){
        parti = partinfo + i;
        if(parti->loaded==0||parti->evac==0)continue;
        parti->display=1;
      }
      break;
    case 4:
      visEvac=0;
      for(i=0;i<npartinfo;i++){
        parti = partinfo + i;
        if(parti->loaded==0||parti->evac==0)continue;
        parti->display=0;
      }
      break;
      default:
        ASSERT(FFALSE);
        break;
    }
  }
  else{
    switch (value){
    case 3:
      visEvac=1;
      for(i=0;i<npartinfo;i++){
        parti = partinfo + i;
        if(parti->loaded==0||parti->evac==0)continue;
        parti->display=1;
      }
      break;
    default:
      ASSERT(FFALSE);
      break;
    }
  }
  updatemenu=1;  
  plotstate=getplotstate(DYNAMIC_PLOTS);
  glutPostRedisplay();

}

/* ------------------ ParticleShowMenu ------------------------ */

void ParticleShowMenu(int value){
  partdata *parti;
  int i;

  if(npartinfo==0)return;
  if(value==999)return;
  ASSERTFLAG(visSmoke);
  ASSERTFLAG(visEvac);
  ASSERTFLAG(visSprinkPart);
  ASSERTFLAG(visStaticSmoke);
  ASSERTFLAG(visSprinkPart);
  if(value<0){
    value = -value;
    value--;
    parti = partinfo + value;
    parti->display = 1 - parti->display;
    updatemenu=1;  
    glutPostRedisplay();
    plotstate=getplotstate(DYNAMIC_PLOTS);
    return;
  }
  if(plotstate==DYNAMIC_PLOTS){
    switch (value){
      case 1:
        if(visSmokePart==2){
          visSmokePart=0;
        }
        else{
          visSmokePart=2;
        }
        break;
      case 2: 
        visSprinkPart = 1 - visSprinkPart; 
        break;
      case 3: 
        visSprinkPart=1; 
        visSmokePart=2; 
        visStaticSmoke=1; 
        for(i=0;i<npartinfo;i++){
          parti = partinfo + i;
          if(parti->loaded==0||parti->evac==1)continue;
          parti->display=1;
        }
        break;
      case 5: 
        visStaticSmoke = 1 - visStaticSmoke; 
        break;
      case 4: 
        visSprinkPart=0; 
        visSmokePart=0; 
        visStaticSmoke=0;
        for(i=0;i<npartinfo;i++){
          parti = partinfo + i;
          if(parti->loaded==0||parti->evac==1)continue;
          parti->display=0;
        }
        break;
      default:
        ASSERT(FFALSE);
        break;
    }
    /*
    for(i=0;i<npartinfo;i++){
      parti = partinfo + i;
      if(parti->loaded==0||parti->evac==1)continue;
      parti->display_droplet=0;
      parti->display_smoke=0;
      if(visSmokePart!=0)parti->display_smoke=1;
      if(visSprinkPart==1)parti->display_droplet=1;
    }
    */
    if(visSprinkPart==1||visSmokePart!=0){
      visSmoke=1;
    }
    else{
      visSmoke=0;
    }
  }
  else{
  //  visSmokePart=0; 
  //  visSprinkPart=0;
    switch (value){
      case 1: 
        visSmokePart = 2; 
        break;
      case 2: 
        visSprinkPart = 1; 
        break;
      case 3: 
        visSprinkPart=1; 
        visSmokePart=2; 
        visStaticSmoke=1; 
        for(i=0;i<npartinfo;i++){
          parti = partinfo + i;
          if(parti->loaded==0)continue;
          parti->display=1;
        }
        break;
      case 5: 
        visStaticSmoke=1; 
        break;
      default:
        ASSERT(FFALSE);
        break;
    }
    /*
    for(i=0;i<npartinfo;i++){
      parti = partinfo + i;
      if(parti->loaded==0||parti->evac==1)continue;
      parti->display_droplet=0;
      parti->display_smoke=0;
      if(visSmokePart!=0)parti->display_smoke=1;
      if(visSprinkPart==1)parti->display_droplet=1;
    }
    */
    if(visSmokePart!=0||visSprinkPart==1){
      visSmoke=1;
    }
  }
  updatemenu=1;  
  plotstate=getplotstate(DYNAMIC_PLOTS);
  glutPostRedisplay();
}

/* ------------------ FrameRateMenu ------------------------ */

//void keyboard(unsigned char key, int x, int y);

void FrameRateMenu(int value){
  updateUpdateFrameRateMenu=0;
  realtime_flag=0;
  frameinterval=1;
  if(value > 0){
    switch (value){
    case 2001:
      if(nglobal_times>0){
        if(global_times!=NULL)frameinterval=1000.*(global_times[nglobal_times-1]-global_times[0])/nglobal_times;
      }
      realtime_flag=1;
      break;
    case 2002:
      if(global_times!=NULL)frameinterval=1000.*(global_times[nglobal_times-1]-global_times[0])/nglobal_times;
      frameinterval /= 2.0;
      realtime_flag=2;
      break;
    case 2004:
      if(global_times!=NULL)frameinterval=1000.*(global_times[nglobal_times-1]-global_times[0])/nglobal_times;
      frameinterval /= 4.0;
      realtime_flag=4;
      break;
    default:
      frameinterval = 1000./value;
      if(frameinterval<1.0){frameinterval = 0.0;}
      break;
    }
    if(global_times==NULL&&realtime_flag!=0)updateUpdateFrameRateMenu=1;
  }
  else{
    keyboard('t',FROM_SMOKEVIEW);
    RenderState(0);
    FlowDir=1;
  }
  frameratevalue=value;
  updatemenu=1;  
  if(opengldefined==1){
    glutPostRedisplay();
  }
  reset_gltime();
}

/* ------------------ IsoSurfaceTypeMenu ------------------------ */

void IsoSurfaceTypeMenu(int value){
  if(ReadPlot3dFile==1){
    switch (value){
    case 0:
      p3dsurfacesmooth=1;
      p3dsurfacetype=1;
      break;
    case 1:
      p3dsurfacesmooth=0;
      p3dsurfacetype=1;
      break;
    case 2:
      p3dsurfacetype=2;
      break;
    case 3:
      p3dsurfacetype=3;
      break;
    default:
      ASSERT(FFALSE);
      break;
    }
    update_glui_plot3dtype();
    updatemenu=1;  
    glutPostRedisplay();
  }
}

/* ------------------ IsoSurfaceMenu ------------------------ */

void IsoSurfaceMenu(int value){
  if(ReadPlot3dFile==1){
    updatemenu=1;  
    glutPostRedisplay();
    if(value==1){
      updateshowstep(0,ISO);
    }
    if(value==2){
      p3dsurfacesmooth = 1 - p3dsurfacesmooth;
    }
  }
}

/* ------------------ LevelMenu ------------------------ */

void LevelMenu(int value){
  if(ReadPlot3dFile==1){
    plotiso[plotn-1]=value;
    updateshowstep(1,ISO);
    updatesurface();
    updatemenu=1;  
    glutPostRedisplay();
  }
}

/* ------------------ HelpMenu ------------------------ */

void HelpMenu(int value){
  switch (value){
    case -1:
#ifdef pp_OSX
      system("open http://code.google.com/p/fds-smv/issues/");
#endif
#ifdef WIN32
      ShellExecute(NULL, "open", "http://code.google.com/p/fds-smv/issues/", NULL, NULL, SW_SHOWNORMAL);
#endif
      break;
    case -2:
#ifdef pp_OSX
      system("open http://code.google.com/p/fds-smv/downloads/");
#endif
#ifdef WIN32
      ShellExecute(NULL, "open", "http://code.google.com/p/fds-smv/downloads/", NULL, NULL, SW_SHOWNORMAL);
#endif
      break;
    case -3:
#ifdef pp_OSX
      system("open http://fire.nist.gov/fds/documentation.html");
#endif
#ifdef WIN32
      ShellExecute(NULL, "open", "http://fire.nist.gov/fds/documentation.html", NULL, NULL, SW_SHOWNORMAL);
#endif
      break;
    case -4:
#ifdef pp_OSX
      system("open http://fire.nist.gov/fds/");
#endif
#ifdef WIN32
      ShellExecute(NULL, "open", "http://fire.nist.gov/fds/", NULL, NULL, SW_SHOWNORMAL);
#endif
      break;
    case 999:
      break;
    default:
      ASSERT(0);
      break;
  }
}


/* ------------------ VectorSkipMenu ------------------------ */

void VectorSkipMenu(int value){
  if(value==-1)return; /* dummy label in menu */
  if(value==-2){       /* toggle vector visibility */
    visVector=1-visVector;
    if(vectorspresent==0)visVector=0;
    updatemenu=1;  
    glutPostRedisplay();
    return;
  }
  vectorskip=value;
  visVector=1;
  updatemenu=1;  
  glutPostRedisplay();
}

/* ------------------ TextureShowMenu ------------------------ */

void TextureShowMenu(int value){
  texture *texti;
  int i;
  int texturedisplay=0;
  int texture_flag=0;

  updatefacelists=1;
  if(value>=0){
    texti = textureinfo + value;
    texti->display = 1-texti->display;
    if(texti->display==1)texturedisplay=1;
    for(i=0;i<ntextures;i++){
      texti = textureinfo + i;
      if(texti->loaded==0||texti->used==0)continue;
      if(texti->display==0){
        showall_textures=0;
        texture_flag=1;
        break;
      }
    }
    if(texture_flag==0)showall_textures=1;
  }
  else{
    switch (value){
    case -1:
      for(i=0;i<ntextures;i++){
        texti = textureinfo + i;
        if(texti->loaded==0||texti->used==0)continue;
        texti->display=1;
        texturedisplay=1;
      }
      showall_textures=1;
      break;
    case -2:
      for(i=0;i<ntextures;i++){
        texti = textureinfo + i;
        if(texti->loaded==0||texti->used==0)continue;
        texti->display=0;
      }
      showall_textures=0;
      break;
    default:
      ASSERT(FFALSE);
      break;
    }
  }
  if(texturedisplay==1){
    if(value!=visBLOCKOutline&&value!=visBLOCKSolidOutline&&value!=visBLOCKHide){
      BlockageMenu(visBLOCKAsInput);
    }
  }
  updatemenu=1;
  glutPostRedisplay();

}

/* ------------------ Plot3DShowMenu ------------------------ */

void Plot3DShowMenu(int value){
  mesh *meshi;
  int i;

  meshi=current_mesh;
  switch (value){
    case 1:
      visz_all=1-visz_all;
      break;
    case 2:
      visy_all=1-visy_all;
      break;
    case 3:
      visx_all=1-visx_all;
      break;
    case 4:
      switch (contour_type){
        case SHADED_CONTOURS:
          contour_type=STEPPED_CONTOURS;
          break;
        case STEPPED_CONTOURS:
        case LINE_CONTOURS:
          contour_type=SHADED_CONTOURS;
          break;
        default:
          ASSERT(FFALSE);
          break;
      }
      break;
    case 5:
      visx_all=1;
      visy_all=1;
      visz_all=1;
      break;
    case 6:
      visx_all=0;
      visy_all=0;
      visz_all=0;
      plotstate=DYNAMIC_PLOTS;
      break;
   case 7:
      visVector=1-visVector;
      if(vectorspresent==0)visVector=0;
      break;
   case HIDEALL_PLOT3D:
     for(i=0;i<nplot3dinfo;i++){
       if(plot3dinfo[i].loaded==1)plot3dinfo[i].display=0;
     }
     break;
   case SHOWALL_PLOT3D:
     for(i=0;i<nplot3dinfo;i++){
       if(plot3dinfo[i].loaded==1)plot3dinfo[i].display=1;
     }
     break;
   default:
     if(value>=1000){
       if(plotstate==STATIC_PLOTS){
         plot3dinfo[value-1000].display=1-plot3dinfo[value-1000].display;
       }
       else{
         plot3dinfo[value-1000].display=1;
       }
     }
     break;
  }
  plotstate=getplotstate(STATIC_PLOTS);
  if(plotstate==STATIC_PLOTS&&visiso==1){
    updatesurface();
  }
  updatemenu=1;  
  glutPostRedisplay();
}


/* ------------------ GridSliceMenu ------------------------ */

void GridSliceMenu(int value){
  switch (value){
  case GRID_xy:
    visz_all=1-visz_all;
    if(visz_all==1&&visGrid==0)visGrid=1;
    break;
  case GRID_xz:
    visy_all=1-visy_all;
    if(visy_all==1&&visGrid==0)visGrid=1;
    break;
  case GRID_yz:
    visx_all=1-visx_all;
    if(visx_all==1&&visGrid==0)visGrid=1;
    break;
  case GRID_showall:
    visx_all=1;
    visy_all=1;
    visz_all=1;
    visGrid=1;
    break;
  case GRID_hideall:
    visx_all=0;
    visy_all=0;
    visz_all=0;
    break;
  case 999:
    break;
  case GRID_grid:
    switch (visGrid){
      case GridProbe:
        visGrid=noGridProbe;
        break;
      case GridnoProbe:
        visGrid=noGridnoProbe;
        break;
      case noGridProbe:
        visGrid=GridProbe;
        break;
      case noGridnoProbe:
        visGrid=GridnoProbe;
        break;
      default:
        ASSERT(0);
        break;
    }
    break;
  case GRID_probe:
    switch (visGrid){
      case GridProbe:
        visGrid=GridnoProbe;
        break;
      case GridnoProbe:
        visGrid=GridProbe;
        break;
      case noGridProbe:
        visGrid=noGridnoProbe;
        break;
      case noGridnoProbe:
        visGrid=noGridProbe;
        break;
      default:
        ASSERT(0);
        break;
    }
    break;
  default:
    ASSERT(FFALSE);
    break;
  }
  updatemenu=1;  
  glutPostRedisplay();
}

#ifdef pp_COMPRESS
/* ------------------ CompressMenu ------------------------ */

void CompressMenu(int value){
  if(value==999)return;
  switch (value){
  case 1:
    erase_all=1;
    overwrite_all=0;
    update_overwrite();
    compress_svzip();
    break;
  case 2:
    erase_all=0;
    overwrite_all=1-overwrite_all;
    update_overwrite();
    break;
  case 3:
    erase_all=0;
    compress_svzip();
    break;
  case 4:
    compress_autoloaded=1-compress_autoloaded;
    update_overwrite();
    break;
  default:
    ASSERT(FFALSE);
    break;
  }
  updatemenu=1;
}
#endif


/* ------------------ IniSubMenu ------------------------ */

void IniSubMenu(int value){
  if(value==-1){
    readini(0);
  }
  else{
    char *inifilename;

    inifilename = get_inifilename(value);
    if(inifilename==NULL||strlen(inifilename)==0)return;
    scriptinifilename2=scriptinifilename;
    strcpy(scriptinifilename,inifilename);
    windowresized=0;
    readini(2);
    scriptinifilename2=NULL;
  }
}

/* ------------------ SmokeviewiniMenu ------------------------ */

void SmokeviewiniMenu(int value){
  switch (value){
  case 1:
    readini(0);
    updatecolors(-1);
    updateshowtitles();
    break;
  case 2:
    writeini(GLOBAL_INI);
    break;
  case 3:
    writeini(LOCAL_INI);
    break;
  case 4:
    init_object_defs();
    break;
  case 999:
    break;
  default:
    ASSERT(FFALSE);
    break;
  }
  updatemenu=1;  
  glutPostRedisplay();
}

/* ------------------ PeriodicReloads ------------------------ */

void PeriodicReloads(int value){
  if(periodic_reloads!=0){
    LoadUnloadMenu(RELOADALL);
    glutTimerFunc((unsigned int)value,PeriodicReloads,value);
  }
}


/* ------------------ ScriptMenu2 ------------------------ */

void ScriptMenu2(int value){
  script_step=1;
  update_script_step();
  ScriptMenu(value);
}

/* ------------------ ScriptMenu ------------------------ */

void ScriptMenu(int value){
  int error_code;
  scriptfiledata *scriptfile;
  char newscriptfilename[1024];

  if(value==999)return;
  updatemenu=1;
  glutPostRedisplay();
  switch (value){
    case SCRIPT_FILE_LOADING:
      defer_file_loading = 1 - defer_file_loading;
      update_defer();
      break;
    case SCRIPT_STEP:
      script_step=1-script_step;
      break;
    case SCRIPT_CANCEL:
      current_script_command=NULL;
      runscript=0;
      script_step=0;
      glui_script_enable();
      break;
    case SCRIPT_CONTINUE:
      script_step=0;
      break;
    case SCRIPT_START_RECORDING2:
      defer_file_loading = 1;
      update_defer();
      ScriptMenu(SCRIPT_START_RECORDING);
      break;
    case SCRIPT_START_RECORDING:
      update_script_start();
      get_newscriptfilename(newscriptfilename);
      script_recording = insert_scriptfile(newscriptfilename);
      scriptoutstream=fopen(newscriptfilename,"w");
      if(scriptoutstream!=NULL){
        printf("Script recorder on\n");
        script_recording->recording=1;
        {
          char *renderdir;

          trim(script_renderdir);
          renderdir = trim_front(script_renderdir);
          if(strlen(renderdir)>0&&strcmp(renderdir,".")!=0){
            fprintf(scriptoutstream,"RENDERDIR\n");
            fprintf(scriptoutstream," %s\n",renderdir);
          }
          else{
            fprintf(scriptoutstream,"RENDERDIR\n");
            fprintf(scriptoutstream," .\n");
          }
        }
        fprintf(scriptoutstream,"XSCENECLIP\n");
        fprintf(scriptoutstream," %i %f %i %f\n",clip_x,clip_x_val,clip_X,clip_X_val);
        fprintf(scriptoutstream,"YSCENECLIP\n");
        fprintf(scriptoutstream," %i %f %i %f\n",clip_y,clip_y_val,clip_Y,clip_Y_val);
        fprintf(scriptoutstream,"ZSCENECLIP\n");
        fprintf(scriptoutstream," %i %f %i %f\n",clip_z,clip_z_val,clip_Z,clip_Z_val);
        fprintf(scriptoutstream,"SCENECLIP\n");
        fprintf(scriptoutstream," %i\n",xyz_clipplane);
      }
      else{
        script_recording->recording=0;
        script_recording=NULL;
        fprintf(stderr,"*** Error: The script file %s could not be opened for writing.",newscriptfilename);
      }
      break;
    case SCRIPT_STOP_RECORDING:
      if(script_recording!=NULL){
        script_recording->recording=0;
        add_scriptlist(script_recording->file,script_recording->id);
        script_recording=NULL;
      }
      if(scriptoutstream!=NULL){
        fclose(scriptoutstream);
        scriptoutstream=NULL;
        //writeini(SCRIPT_INI);
        printf("Script recorder off\n");
      }
      update_script_stop();
      break;
    default:
      for(scriptfile=first_scriptfile.next;scriptfile->next!=NULL;scriptfile=scriptfile->next){
        char *file;

        file=scriptfile->file;
        if(file==NULL)continue;
        if(scriptfile->id!=value)continue;
        error_code=compile_script(file);
        if(error_code==0){
          readini(1);
          start_script();
        }
        else{
          fprintf(stderr,"*** Error (fatal): unable to open script file");
          if(file!=NULL)fprintf(stderr,": %s",file);
          fprintf(stderr,"\n");
          if(from_commandline==1)exit(1);
        }
        break;
      }
      break;
  }
}

/* ------------------ ReLoadMenu ------------------------ */

void ReloadMenu(int value){
  int msecs;

  updatemenu=1;
  periodic_value=value;
  switch (value){
  case -1:
    periodic_reloads=0;
    break;
  case 0:
    LoadUnloadMenu(RELOADALL);
    break;
  default:
    periodic_reloads=1;
    msecs = value*60*1000;
    glutTimerFunc((unsigned int)msecs,PeriodicReloads,msecs);
    break;
  }
}


/* ------------------ AboutMenu ------------------------ */

void AboutMenu(int value){
}

/* ------------------ LoadUnloadMenu ------------------------ */

void LoadUnloadMenu(int value){
  int errorcode;
  int i;
  int ii;

  if(value==999)return;
  glutSetCursor(GLUT_CURSOR_WAIT);
  if(value==UNLOADALL){
   // leaving code here commented in case I later decide to unload terrain files
   // for(i=0;i<nterraininfo;i++){
   //   readterrain("",i,UNLOAD,&errorcode);
   // }
    if(scriptoutstream!=NULL){
      fprintf(scriptoutstream,"UNLOADALL\n");
    }
    if(hrr_csvfilename!=NULL){
      readhrr(UNLOAD, &errorcode);
    }
#ifdef pp_DEVICE
    if(devc_csvfilename!=NULL){
      read_device_data(devc_csvfilename,CSV_FDS,UNLOAD);
    }
    if(exp_csvfilename!=NULL){
      read_device_data(exp_csvfilename,CSV_EXP,UNLOAD);
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
      readiso("",i,UNLOAD,&errorcode);
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
    updatemenu=1;  
    glutPostRedisplay();
  }
  if(value==RELOADALL){
    LOCK_COMPRESS
    readsmv_dynamic(smvfilename);
    if(hrr_csvfilename!=NULL){
      readhrr(LOAD, &errorcode);
    }
#ifdef pp_DEVICE
    if(devc_csvfilename!=NULL||exp_csvfilename!=NULL){
      read_device_data(NULL,CSV_FDS,UNLOAD);
      if(devc_csvfilename!=NULL)read_device_data(devc_csvfilename,CSV_FDS,LOAD);
      read_device_data(NULL,CSV_EXP,UNLOAD);
      if(exp_csvfilename!=NULL)read_device_data(exp_csvfilename,CSV_EXP,LOAD);
    }
#endif
    islicetype_save=islicetype;
    for(i=0;i<nsliceinfo;i++){
      sliceinfo[i].reload=1;
    }
    for(i=0;i<nterraininfo;i++){
      if(terraininfo[i].loaded==1){
        readterrain(terraininfo[i].file,i,LOAD,&errorcode);
      }
    }
    for(i=0;i<nvsliceinfo;i++){
      if(vsliceinfo[i].loaded==1){
        readvslice(i,LOAD,&errorcode);
      }
    }
    for(ii=0;ii<nslice_loaded;ii++){
      slicedata *slicei;

      i = slice_loaded_list[ii];
      slicei = sliceinfo + i;
      if(slicei->reload==1){
        readslice(slicei->file,i,LOAD,&errorcode);
      }
    }
    islicetype=islicetype_save;
    for(i=0;i<nplot3dinfo;i++){
      if(plot3dinfo[i].loaded==1){
        readplot3d(plot3dinfo[i].file,i,LOAD,&errorcode);
      }
    }
    for(ii=0;ii<npatch_loaded;ii++){
      i = patch_loaded_list[ii];
      readpatch(i,LOAD,&errorcode);
    }
    for(i=0;i<nsmoke3dinfo;i++){
      if(smoke3dinfo[i].loaded==1){
        readsmoke3d(i,LOAD,&errorcode);
      }
    }
    for(i=0;i<npartinfo;i++){
      if(partinfo[i].loaded==1){
        partinfo[i].reload=1;
        readpart(partinfo[i].file,i,UNLOAD,&errorcode);
      }
      else{
        partinfo[i].reload=0;
      }
    }
    npartframes_max=get_min_partframes();
    for(i=0;i<npartinfo;i++){
      if(partinfo[i].reload==1){
        readpart(partinfo[i].file,i,UNLOAD,&errorcode);
      }
    }
    for(i=0;i<npartinfo;i++){
      if(partinfo[i].reload==1){
        readpart(partinfo[i].file,i,LOAD,&errorcode);
      }
    }
    for(i=0;i<nisoinfo;i++){
      isodata *isoi;

      isoi = isoinfo + i;
      if(isoi->loaded==0)continue;
      readiso(isoi->file,i,LOAD,&errorcode);
    }
    UNLOCK_COMPRESS
  //  plotstate=DYNAMIC_PLOTS;
  //  visSmoke=1;
    updatemenu=1;  
    glutPostRedisplay();
  }
  if(value==SHOWFILES){
    glutPostRedisplay();  
    showfiles=1-showfiles;
    updatemenu=1;
    updateslicemenulabels();
    updatevslicemenulabels();
   // updatesmokemenulabels();
    updatesmoke3dmenulabels();
    updatepatchmenulabels();
    updateisomenulabels();
    updatepartmenulabels();
    updatetourmenulabels();
    updateplot3dmenulabels();
  }
  glutSetCursor(GLUT_CURSOR_RIGHT_ARROW);
}


/* ------------------ ShowTourMenu ------------------------ */

void ShowTourMenu(int value){
}


/* ------------------ AvatarTourMenu ------------------------ */

void AvatarTourMenu(int value){
}

void AvatarEvacMenu(int value){
  if(value==-999)return;
  iavatar_evac=value;
  updatemenu=1;
}

  #define MENU_TOUR_EDIT -14
  #define MENU_TOUR_NEW -12
  #define MENU_TOUR_MANUAL -2
/* ------------------ TourMenu ------------------------ */

void TourMenu(int value){
  tourdata *touri;
  int i;

  if(value==-999)return;
  touring=0;
  updatemenu=1;
  glutPostRedisplay();
  switch (value){
  case MENU_TOUR_EDIT:
    DialogMenu(21);
    break;
  case MENU_TOUR_NEW:
    add_new_tour();
    showtour_dialog=0;
    DialogMenu(21);
    break;
  case -13:               
    for(i=0;i<ntours;i++){  // clear all tours
      touri = tourinfo + i;
      touri->display=touri->display2;
    }
    if(viewtourfrompath==1){
      ResetView(RESTORE_EXTERIOR_VIEW);
    }
    from_glui_trainer=0;
    for(i=0;i<ntours;i++){
      touri = tourinfo + i;
      if(touri->display==1){
        selected_tour=touri;
        break;
      }
    }
    selected_tour=NULL;
    break;
  case MENU_TOUR_MANUAL:               
    for(i=0;i<ntours;i++){  // clear all tours
      touri = tourinfo + i;
      touri->display=0;
    }
    if(viewtourfrompath==1){
      ResetView(RESTORE_EXTERIOR_VIEW);
    }
    from_glui_trainer=0;
    selected_tour=NULL;
    if(scriptoutstream!=NULL){
      fprintf(scriptoutstream,"UNLOADTOUR\n");
    }
    showtour_dialog=1;
    DialogMenu(21);
    break;
  case -4:
    edittour=1-edittour;
    if(edittour==1&&showtour_dialog==0){
      showtour_dialog=1;
      show_glui_tour();
    }
    break;
  case -3:               // show all tours
    for(i=0;i<ntours;i++){
      touri = tourinfo + i;
      touri->display=1;
    }
    plotstate=getplotstate(DYNAMIC_PLOTS);
    break;
  case -5:               // view from route
    viewtourfrompath = 1 - viewtourfrompath;
    if(viewtourfrompath==0)ResetView(RESTORE_EXTERIOR_VIEW);
    break;
  case -6:
    tour_constant_vel=1-tour_constant_vel;
    createtourpaths();
    updatetimes();
    break;
  case -1:
    for(i=0;i<ntours;i++){
      touri = tourinfo + i;
      touri->display=0;
    }
    ResetView(RESTORE_EXTERIOR_VIEW);
    defaulttour();
    break;
  case -11: // bird's eye
    break;
  case -21:
    tourlocus_type=0;
    break;
  case -22:
    tourlocus_type=1;
    break;
  default: 
    if(value<-22){
      tourlocus_type=2;
      iavatar_types=(-value-23);
      if(selectedtour_index>=0&&selectedtour_index<ntours){
        tourinfo[selectedtour_index].glui_avatar_index=iavatar_types;
      }
    }
    
    //  show one tour

    if(value>=0&&value<ntours){
      int j;

      touri = tourinfo + value;
      touri->display = 1 - touri->display;
      if(touri->display==1){
        selectedtour_index=value;
        selected_frame=touri->first_frame.next;
        selected_tour=touri;
      }
      else{
        for(j=0;j<ntours;j++){
          tourdata *tourj;

          tourj = tourinfo + j;
          if(touri==tourj||tourj->display==0)continue;
          selectedtour_index=j;
          selected_frame=tourj->first_frame.next;
          selected_tour=tourj;
          break;
        }
      }
    }
    break;
  }
  updateviewtour();
  delete_tourlist();
  create_tourlist();
  update_tourcontrols();
  plotstate=getplotstate(DYNAMIC_PLOTS);
  if(value!=-5&&value!=-4)updatetimes();
  callfrom_tourglui=0;

}

/* ------------------ SetTour ------------------------ */

void SetTour(tourdata *thetour){
  int tournumber;

  if(thetour==NULL)return;
  tournumber = thetour - tourinfo;
  TourMenu(tournumber);
}

/* ------------------ targetMenu ------------------------ */

void TargetMenu(int value){
  int errorcode,i;
  if(value>=0){
    readtarget(targinfo[value].file,value,LOAD,&errorcode);
  }
  else{
    if(value==-1){
      for(i=0;i<ntarginfo;i++){
        readtarget("",i,UNLOAD,&errorcode);
      }
    }
  }
  updatemenu=1;  
  glutPostRedisplay();
}

/* ------------------ EvacMenu ------------------------ */

void EvacMenu(int value){
  int errorcode;
  glutSetCursor(GLUT_CURSOR_WAIT);
  if(value==-11){
    int i;

    for(i=0;i<npartinfo;i++){
      partdata *parti;

      parti=partinfo + i;
      if(parti->evac==0)continue;
      readpart(parti->file,i,UNLOAD,&errorcode);
    }
    npartframes_max=get_min_partframes();
    for(i=0;i<npartinfo;i++){
      partdata *parti;

      parti=partinfo + i;
      if(parti->evac==0)continue;
      ReadEvacFile=1;
      readpart(parti->file,i,LOAD,&errorcode);
      if(scriptoutstream!=NULL){
        fprintf(scriptoutstream,"LOADFILE\n");
        fprintf(scriptoutstream," %s\n",parti->file);
      }
    }
    force_redisplay=1;
    update_framenumber(0);
  }
  if(value>=0){
    ReadEvacFile=1;
    npartframes_max=get_min_partframes();
    readpart(partinfo[value].file,value,LOAD,&errorcode);
    if(scriptoutstream!=NULL){
      fprintf(scriptoutstream,"LOADFILE\n");
      fprintf(scriptoutstream," %s\n",partinfo[value].file);
    }
  }
  else{
    if(value==-1){
      int i;

      for(i=0;i<npartinfo;i++){
        if(partinfo[i].evac==0)continue;
        readpart("",i,UNLOAD,&errorcode);
      }
    }
  }
  updatemenu=1;  
  glutPostRedisplay();
  glutSetCursor(GLUT_CURSOR_LEFT_ARROW);
}

/* ------------------ update_streakvalue ------------------------ */

void update_streakvalue(float value){
  partdata *parti=NULL;
  int i;

  streak_index=-1;
  for(i=0;i<nstreak_value;i++){
    float absdiff;

    absdiff = value-streak_rvalue[i];
    if(absdiff<0.0)absdiff=-absdiff;

    if(absdiff<0.01){
      streak_index=i;
      float_streak5value=streak_rvalue[i];
      break;
    }
  }
  for(i=0;i<npartinfo;i++){
    parti = partinfo + i;
    if(parti->loaded==1)break;
  }
  if(parti!=NULL&&parti->ntimes>1){
    for(i=0;i<parti->ntimes-1;i++){
      if(parti->times[i]<=value&&value<parti->times[i+1]){
        streak5step=i;
        break;
      }
    }
  }
}
/* ------------------ ParticleStreakShowMenu ------------------------ */

void ParticleStreakShowMenu(int value){
  float rvalue;

  if(value==-1)return;
  if(value==-2){
    streak5show=0;
    streak5step=0;
  }
  else if(value==-3){
    showstreakhead=1-showstreakhead;
  }
  else{
    streak5show=1;
    streak5step=0;
    rvalue=streak_rvalue[value];
    update_streakvalue(rvalue);
    update_glui_streakvalue(rvalue);

  }
  updatemenu=1;
  glutPostRedisplay();
}

/* ------------------ Particle5ShowMenu ------------------------ */

void Particle5ShowMenu(int value){
}

/* ------------------ ParticlePropShowMenu ------------------------ */

void ParticlePropShowMenu(int value){
  part5prop *propi;

  int propvalue;

  propvalue = (-value)/10000-1;
  value = -((-value)%10000);

  if(value==-1)return;

  if(value>=0){
    int iprop;
    int i;

    part5show=1;
    parttype=0;
    iprop = value;
    for(i=0;i<npart5prop;i++){
      propi = part5propinfo + i;
      propi->display=0;
    }
    
    propi = part5propinfo + iprop;
    last_prop_display=iprop;
    propi->display=1;
    part5colorindex=iprop;

    if(scriptoutstream!=NULL){
      fprintf(scriptoutstream,"PARTCLASSCOLOR\n");
      fprintf(scriptoutstream," %s\n",propi->label->longlabel);
    }
    current_property = propi;
    if(iprop!=0){
      parttype=1;
    }
    prop_index = iprop;
    partshortlabel=propi->label->shortlabel;
    partunitlabel=propi->label->unit;
    partscale=propi->scale;
  }
  else if(value==-2){
    if(current_property!=NULL){
      unsigned char *vis;
      int i;

      vis = current_property->class_vis;
      for(i=0;i< npartclassinfo;i++){
        vis[i]=1;
      }
    }
  }
  else if(value==-3){
    if(current_property!=NULL){
      unsigned char *vis;
      int i;

      vis = current_property->class_vis;
      for(i=0;i< npartclassinfo;i++){
        vis[i]=0;
      }
    }

  }
  else if(value==-4){
    int i;
    int unhide=1;

    for(i=0;i<npart5prop;i++){
      propi = part5propinfo + i;
      if(propi->particle_property==1){
        if(propi->display==1)unhide=0;
        propi->display=0;
      }
    }
    part5show=0;
    parttype=0;
    if(unhide==1&&last_prop_display>=0){
      ParticlePropShowMenu(last_prop_display);
    }
  }
  else if(value==-5){
    int i;

    for(i=0;i<npart5prop;i++){
      propi = part5propinfo + i;
      if(propi->human_property==1){
        propi->display=0;
      }
    }
    part5show=0;
    parttype=0;
  }
  else if(value==-6){
    show_tracers_always=1-show_tracers_always;
    updatetracers();
  }
  else{
    int iclass;
    int vistype;

    iclass =  (-value - 10)/5;
    vistype = (-value - 10)%5;
    if(vistype==0){
      if(current_property!=NULL){
        unsigned char *vis;

        vis = current_property->class_vis;
        vis[iclass] = 1 - vis[iclass];
        if(scriptoutstream!=NULL){
          fprintf(scriptoutstream,"PARTCLASSTYPE\n");
          fprintf(scriptoutstream," %s\n",current_property->label->longlabel);
        }
      }
    }
    else{
      part5class *partclassj;

      partclassj = partclassinfo + iclass;
      partclassj->vis_type=vistype;
      PropMenu(propvalue);
    }

  }
  updatemenu=1;
  glutPostRedisplay();
}

/* ------------------ ParticleMenu ------------------------ */

void ParticleMenu(int value){
  int errorcode,i;
  int whichpart;
  partdata *parti, *partj;

  glutSetCursor(GLUT_CURSOR_WAIT);
  if(value>=0){
    char  *partfile;

    ReadPartFile=1;
    partfile = partinfo[value].file;
    if(scriptoutstream!=NULL){
      fprintf(scriptoutstream,"LOADFILE\n");
      fprintf(scriptoutstream," %s\n",partfile);
    }
    npartframes_max=get_min_partframes();
    readpart(partfile,value,LOAD,&errorcode);
  }
  else{
    if(value==-1){
      for(i=0;i<npartinfo;i++){
        if(partinfo[i].evac==1)continue;
        readpart("",i,UNLOAD,&errorcode);
      }
    }
    else{
      ReadPartFile=1;
      whichpart=-(10+value);
      partj = partinfo + whichpart;
      if(scriptoutstream!=NULL){
        fprintf(scriptoutstream,"LOADPARTICLES\n");
      }
      npartframes_max=get_min_partframes();
      for(i=0;i<npartinfo;i++){
        parti = partinfo + i;
        if(parti->evac==1)continue;
        readpart(parti->file,i,UNLOAD,&errorcode);
      }
      for(i=0;i<npartinfo;i++){
        parti = partinfo + i;
        if(parti->evac==1)continue;
        if(parti->version==1||strcmp(parti->label.longlabel,partj->label.longlabel)==0){
          readpart(parti->file,i,LOAD,&errorcode);
        }
      }
      force_redisplay=1;
      update_framenumber(0);
    }
  }
  updatemenu=1;  
  glutPostRedisplay();
  glutSetCursor(GLUT_CURSOR_LEFT_ARROW);
}

/* ------------------ ZoneMenu ------------------------ */

void ZoneMenu(int value){
  int i,errorcode;
  if(value>=0){
    if(scriptoutstream!=NULL){
      zonedata *zonei;

      zonei = zoneinfo + value;
      fprintf(scriptoutstream,"LOADFILE\n");
      fprintf(scriptoutstream," %s\n",zonei->file);
    }
    readzone(value,LOAD,&errorcode);
  }
  else{
    for(i=0;i<nzoneinfo;i++){
      readzone(i,UNLOAD,&errorcode);
    }
  }
  updatemenu=1;  
  glutPostRedisplay();
}

/* ------------------ UnloadVSliceMenu ------------------------ */

void UnloadVSliceMenu(int value){
  int errorcode,i;

  updatemenu=1;  
  glutPostRedisplay();
  if(value>=0){
    readvslice(value,UNLOAD,&errorcode);
  }
  else if(value==-1){
    for(i=0;i<nvsliceinfo;i++){
      readvslice(i,UNLOAD,&errorcode);
    }
  }
  else if(value==-2){
    int unload_index;

    unload_index=last_vslice_loadstack();
    if(unload_index>=0&&unload_index<nvsliceinfo){
      readvslice(unload_index,UNLOAD,&errorcode);
    }
  }
}

/* ------------------ UnloadPatchMenu ------------------------ */

void UnloadPatchMenu(int value){
  int errorcode,i;

  updatemenu=1;  
  glutPostRedisplay();
  if(value>=0){
    readpatch(value,UNLOAD,&errorcode);
  }
  else{
    for(i=0;i<npatchinfo;i++){
      readpatch(i,UNLOAD,&errorcode);
    }
  }
}

/* ------------------ UnloadIsoMenu ------------------------ */

void UnloadIsoMenu(int value){
  int errorcode,i;

  updatemenu=1;  
  glutPostRedisplay();
  if(value>=0){
    readiso("",value,UNLOAD,&errorcode);
  }
  else{
    for(i=0;i<nisoinfo;i++){
      readiso("",i,UNLOAD,&errorcode);
    }
  }
}

/* ------------------ UnloadPlot3dMenu ------------------------ */

void UnloadPlot3dMenu(int value){
  int errorcode,i;

  updatemenu=1;  
  glutPostRedisplay();
  if(value>=0){
    readplot3d("",value,UNLOAD,&errorcode);
  }
  else{
    for(i=0;i<nplot3dinfo;i++){
      readplot3d("",i,UNLOAD,&errorcode);
    }
  }
}

/* ------------------ UnloadEvacMenu ------------------------ */

void UnloadEvacMenu(int value){
  int errorcode,i;

  updatemenu=1;  
  glutPostRedisplay();
  if(value>=0){
    readpart("",value,UNLOAD,&errorcode);
  }
  else{
    for(i=0;i<npartinfo;i++){
      if(partinfo[i].evac==0)continue;
      readpart("",i,UNLOAD,&errorcode);
    }
  }
}

/* ------------------ UnloadPartMenu ------------------------ */

void UnloadPartMenu(int value){
  int errorcode,i;

  updatemenu=1;  
  glutPostRedisplay();
  if(value>=0){
    readpart("",value,UNLOAD,&errorcode);
  }
  else{
    for(i=0;i<npartinfo;i++){
      if(partinfo[i].evac==1)continue;
      readpart("",i,UNLOAD,&errorcode);
    }
  }
}

/* ------------------ LoadVSliceMenu ------------------------ */

void LoadVSliceMenu(int value){
  int errorcode;
  int i;

  if(value==-999)return;
  glutSetCursor(GLUT_CURSOR_WAIT);
  if(value==-1){
    for(i=0;i<nvsliceinfo;i++){
      readvslice(i,UNLOAD,&errorcode);
    }
    return;
  }
  else if(value==-20){
    showallslicevectors=1-showallslicevectors;
    updatemenu=1;  
    glutPostRedisplay();
  }
  else if(value>=0){
    vslicedata *vslicei;
    slicedata *slicei;

    readvslice(value, LOAD, &errorcode);
    vslicei = vsliceinfo + value;
    slicei = vslicei->val;
    if(script_multivslice==0&&slicei!=NULL&&scriptoutstream!=NULL){
      char *file;

      file=slicei->file;
      fprintf(scriptoutstream,"LOADVFILE\n");
      fprintf(scriptoutstream," %s\n",file);
    }
  }
  else{
    int submenutype;
    char *submenulabel;
    vslicedata *vslicei;
    slicedata *slicei;
    int dir;

    value = -(1000 + value);
    submenutype=value/4;
    dir=value%4;
    submenutype=subvslice_menuindex[submenutype];
    vslicei = vsliceinfo + submenutype;
    slicei = sliceinfo + vslicei->ival;
    submenulabel = slicei->label.longlabel;
    for(i=0;i<nvsliceinfo;i++){
      char *longlabel;

      vslicei = vsliceinfo + i;
      slicei=sliceinfo + vslicei->ival;
      longlabel = slicei->label.longlabel;
      if(strcmp(longlabel,submenulabel)!=0)continue;
      if(dir!=0&&dir!=slicei->idir)continue;
      readvslice(i,LOAD,&errorcode);
    }
  }
  glutSetCursor(GLUT_CURSOR_LEFT_ARROW);
}

/* ------------------ UnloadSliceMenu ------------------------ */

void UnloadSliceMenu(int value){
  int errorcode,i;

  updatemenu=1;  
  glutPostRedisplay();
  if(value>=0){
    readslice("",value,UNLOAD,&errorcode);
  }
  else{
    if(value==-1){
      for(i=0;i<nsliceinfo;i++){
        readslice("",i,UNLOAD,&errorcode);
      }
    }
    else if(value==-2){
      int unload_index;

      unload_index=last_slice_loadstack();
      if(unload_index>=0&&unload_index<nsliceinfo){
        readslice("",unload_index,UNLOAD,&errorcode);
      }
    }
  }
}

/* ------------------ UnLoadMultiVSliceMenu ------------------------ */

void UnloadMultiVSliceMenu(int value){
  int i;
  multivslicedata *mvslicei;

  if(value>=0){
    mvslicei = multivsliceinfo + value;
    for(i=0;i<mvslicei->nvslices;i++){
      UnloadSliceMenu(mvslicei->ivslices[i]);
    }
  }
  else{
    UnloadSliceMenu(-1);
  }
}


/* ------------------ UnLoadMultiSliceMenu ------------------------ */

void UnloadMultiSliceMenu(int value){
  int i;
  multislicedata *mslicei;

  if(value>=0){
    mslicei = multisliceinfo + value;
    for(i=0;i<mslicei->nslices;i++){
      UnloadSliceMenu(mslicei->islices[i]);
    }
  }
  else{
    UnloadSliceMenu(-1);
  }
}


/* ------------------ ShowVolSmoke3DMenu ------------------------ */

void ShowVolSmoke3DMenu(int value){
  int i;

  if(value==999)return;
  updatemenu=1;
  glutSetCursor(GLUT_CURSOR_WAIT);
  if(value>=0){
    mesh *meshi;
    volrenderdata *vr;

    meshi = meshinfo + value;
    vr = &(meshi->volrenderinfo);
    if(vr->fire!=NULL||vr->smoke!=NULL){
      if(vr->loaded==1){
        vr->display=1-vr->display;
        printf("%s vis state:%i\n",meshi->label,vr->display);
      }
    }
  }
  else if(value==HIDE_ALL){  // hide all
    for(i=0;i<nmeshes;i++){
      mesh *meshi;
      volrenderdata *vr;
      
      meshi = meshinfo + i;
      vr = &(meshi->volrenderinfo);
      if(vr->fire==NULL||vr->smoke==NULL)continue;
      if(vr->loaded==1){
        vr->display=0;
        printf("%s vis state:%i\n",meshi->label,vr->display);
      }
    }
  }
  else if(value==SHOW_ALL){  // show all
    for(i=0;i<nmeshes;i++){
      mesh *meshi;
      volrenderdata *vr;
      
      meshi = meshinfo + i;
      vr = &(meshi->volrenderinfo);
      if(vr->fire==NULL||vr->smoke==NULL)continue;
      if(vr->loaded==1){
        vr->display=1;
        printf("%s vis state:%i\n",meshi->label,vr->display);
      }
    }
  }
  updatemenu=1;  
  glutPostRedisplay();
}

/* ------------------ UnLoadVolSmoke3DMenu ------------------------ */

void UnLoadVolSmoke3DMenu(int value){
  int i;

  if(value==999)return;
  read_vol_mesh=-2;
  updatemenu=1;
  if(value<0){
    if(value==UNLOAD_ALL){
      for(i=0;i<nmeshes;i++){
        mesh *meshi;
        volrenderdata *vr;
      
        meshi = meshinfo + i;
        vr = &(meshi->volrenderinfo);
        if(vr->fire==NULL||vr->smoke==NULL)continue;
        if(vr->loaded==1){
          unload_volsmoke_allframes(vr);
        }
      }
    }
  }
  else{
    mesh *meshi;
    volrenderdata *vr;
    slicedata *fire, *smoke;

    meshi = meshinfo + value;
    vr = &(meshi->volrenderinfo);
    fire = vr->fire;
    smoke = vr->smoke;
    if(fire!=NULL||smoke!=NULL){
      unload_volsmoke_allframes(vr);
    }
  }
  updatemenu=1;  
  read_vol_mesh=-3;
  glutPostRedisplay();
}

/* ------------------ LoadVolSmoke3DMenu ------------------------ */

void LoadVolSmoke3DMenu(int value){
  if(value==999)return;
  updatemenu=1;
  glutSetCursor(GLUT_CURSOR_WAIT);
  if(value>=0){
    mesh *meshi;
    volrenderdata *vr;
    slicedata *fire, *smoke;

    meshi = meshinfo + value;
    vr = &(meshi->volrenderinfo);
    fire = vr->fire;
    smoke = vr->smoke;
    if(smoke!=NULL&&fire!=NULL){
      if(scriptoutstream!=NULL){
        fprintf(scriptoutstream,"LOADVOLSMOKE\n");
        fprintf(scriptoutstream," %i\n",value);
      }
      if(read_vol_mesh==-3){
        read_vol_mesh=value;
        read_volsmoke_allframes_allmeshes();
      }
      else{
        fprintf(stderr,"*** Warning: 3D smoke is currently being loaded\n");
        fprintf(stderr,"   Load data when this is complete.\n");
      }
    }
  }
  else if(value==UNLOAD_ALL){  // unload all
    if(read_vol_mesh==-3){
      UnLoadVolSmoke3DMenu(value);
    }
      else{
        if(read_vol_mesh==-2){
          fprintf(stderr,"*** Warning: data is currently being unloaded\n");
        }
        else{
          fprintf(stderr,"*** Warning: data is currently being loaded\n");
        }
        fprintf(stderr,"    Continue when this is complete.\n");
      }
  }
  else if(value==LOAD_ALL){  // load all
    if(scriptoutstream!=NULL){
      fprintf(scriptoutstream,"LOADVOLSMOKE\n");
      fprintf(scriptoutstream," -1\n");
    }
    if(read_vol_mesh==-3){
      read_vol_mesh=-1;
      read_volsmoke_allframes_allmeshes();
    }
    else{
      if(read_vol_mesh==-2){
        fprintf(stderr,"*** Warning: data is currently being unloaded\n");
      }
      else{
        fprintf(stderr,"*** Warning: data is currently being loaded\n");
      }
      fprintf(stderr,"    Continue when this is complete.\n");
    }
  }
  updatemenu=1;  
  Idle_CB();
  glutPostRedisplay();
  glutSetCursor(GLUT_CURSOR_LEFT_ARROW);
}

/* ------------------ UnLoadSmoke3DMenu ------------------------ */

void UnLoadSmoke3DMenu(int value){
  int errorcode;
  int i;
  smoke3ddata *smoke3di;

  if(value==999)return;
  updatemenu=1;
  if(value<0){
    value= -value;
    for(i=0;i<nsmoke3dinfo;i++){
      smoke3di = smoke3dinfo + i;
      if(smoke3di->loaded==1&&smoke3di->type==value){
        readsmoke3d(i,UNLOAD,&errorcode);
      }
    }
  }
  else{
    readsmoke3d(value,UNLOAD,&errorcode);
  }
}

/* ------------------ LoadSmoke3DMenu ------------------------ */

void LoadSmoke3DMenu(int value){
  int i,errorcode;
  smoke3ddata *smoke3di, *smoke3dj;

  glutSetCursor(GLUT_CURSOR_WAIT);
  if(value>=0){
    if(scriptoutstream!=NULL){
      char *file;

      file = smoke3dinfo[value].file;
      fprintf(scriptoutstream,"LOADFILE\n");
      fprintf(scriptoutstream," %s\n",file);
    }
    if(scriptoutstream==NULL||defer_file_loading==0){
      readsmoke3d(value,LOAD,&errorcode);
    }
  }
  else if(value==-1){
    for(i=0;i<nsmoke3dinfo;i++){
      readsmoke3d(i,UNLOAD,&errorcode);
    }
  }
  else if(value==-9){
    if(scriptoutstream==NULL||defer_file_loading==0){
      for(i=0;i<nsmoke3dinfo;i++){
        smoke3di = smoke3dinfo + i;
        if(smoke3di->loaded==1)continue;
        readsmoke3d(i,LOAD,&errorcode);
      }
    }
  }
  else if(value<=-10){
    value = -(value + 10);
    smoke3dj = smoke3dinfo + value;
    if(scriptoutstream!=NULL){
      fprintf(scriptoutstream,"LOAD3DSMOKE\n");
      fprintf(scriptoutstream," %s\n",smoke3dj->label.longlabel);
    }
    if(scriptoutstream==NULL||defer_file_loading==0){
      for(i=0;i<nsmoke3dinfo;i++){
        smoke3di = smoke3dinfo + i;
        if(strcmp(smoke3di->label.shortlabel,smoke3dj->label.shortlabel)==0){
          readsmoke3d(i,LOAD,&errorcode);
        }
      }
    }
  }
  updatemenu=1;  
  glutPostRedisplay();
  glutSetCursor(GLUT_CURSOR_LEFT_ARROW);
}

/* ------------------ AnySmoke ------------------------ */

int AnySmoke(char *type){

  if(nsmoke3dinfo>0)return 1;
  return 0;
}

/* ------------------ AnySlices ------------------------ */

int AnySlices(char *type){
  int i;

  for(i=0;i<nsliceinfo;i++){
    if(STRCMP(sliceinfo[i].label.longlabel,type)==0)return 1;
  }
  return 0;
}

/* ------------------ LoadAllSlices ------------------------ */

void HideAllSlices(void){
  int i;

  glutSetCursor(GLUT_CURSOR_WAIT);
  for(i=0;i<nsliceinfo;i++){
    sliceinfo[i].display=0;
  }
  updatemenu=1;  
  glutPostRedisplay();
  glutSetCursor(GLUT_CURSOR_LEFT_ARROW);
}

/* ------------------ ShowAllSmoke ------------------------ */

void ShowAllSmoke(void){
  int i;
  for(i=0;i<nsmoke3dinfo;i++){
    smoke3ddata *smoke3di;

    smoke3di = smoke3dinfo + i;
    if(smoke3di->loaded==1)smoke3di->display=1;
  }
  for(i=0;i<nisoinfo;i++){
    isodata *isoi;

    isoi = isoinfo + i;
    if(isoi->loaded==1)isoi->display=1;
  }
}

/* ------------------ HideAllSmoke ------------------------ */

void HideAllSmoke(void){
  int i;
  for(i=0;i<nsmoke3dinfo;i++){
    smoke3ddata *smoke3di;

    smoke3di = smoke3dinfo + i;
    if(smoke3di->loaded==1)smoke3di->display=0;
  }
  for(i=0;i<nisoinfo;i++){
    isodata *isoi;

    isoi = isoinfo + i;
    if(isoi->loaded==1)isoi->display=0;
  }
}

/* ------------------ ShowAllSlices ------------------------ */

void ShowAllSlices(char *type1, char *type2){
  int i;

  glutSetCursor(GLUT_CURSOR_WAIT);
  if(trainer_showall_mslice==1){
    for(i=0;i<nsliceinfo;i++){
      sliceinfo[i].display=0;
      if(sliceinfo[i].loaded==0)continue;
      if(
        type1!=NULL&&STRCMP(sliceinfo[i].label.longlabel,type1)==0||
        type2!=NULL&&STRCMP(sliceinfo[i].label.longlabel,type2)==0
        ){
        sliceinfo[i].display=1;
        islicetype=sliceinfo[i].type;
      }
    }
  }
  else{
    int msliceindex;
  
    if(trainerload==2){
      if(trainerload==trainerload_old){
        trainer_temp_index++;
        if(trainer_temp_index>trainer_temp_n-1){
          trainer_temp_index=0;
        }
      }
      msliceindex=trainer_temp_indexes[trainer_temp_index];
    }
    else{ 
      if(trainerload==trainerload_old){
        trainer_oxy_index++;
        if(trainer_oxy_index>trainer_oxy_n-1){
          trainer_oxy_index=0;
        }
      }
      msliceindex=trainer_oxy_indexes[trainer_oxy_index];
    }
    ShowMultiSliceMenu(HIDE_ALL);
    ShowMultiSliceMenu(msliceindex);
  }
  updatemenu=1;  
  glutPostRedisplay();
  glutSetCursor(GLUT_CURSOR_LEFT_ARROW);
}

void UnloadTerrainMenu(int value);
void LoadTerrainMenu(int value);

/* ------------------ LoadTerrainMenu ------------------------ */

void LoadTerrainMenu(int value){
  int i;
  int errorcode;

  if(value>=0&&value<nterraininfo){
    terraindata *terri;

    terri = terraininfo + value;
    readterrain(terri->file,value,LOAD,&errorcode);
  }
  else if(value==-10){
    UnloadTerrainMenu(value);
  }
  else if(value==-9){
    for(i=0;i<nterraininfo;i++){
      LoadTerrainMenu(i);
    }
  }
  updatemenu=1;  
  glutPostRedisplay();
}

/* ------------------ UnLoadTerrainMenu ------------------------ */

void UnloadTerrainMenu(int value){
  int i;
  int errorcode;

  if(value>=0&&value<nterraininfo){
    readterrain("",value,UNLOAD,&errorcode);
  }
  else if(value==-10){
    for(i=0;i<nterraininfo;i++){
      UnloadTerrainMenu(i);
    }
  }
  updatemenu=1;  
  glutPostRedisplay();

}

/* ------------------ LoadSliceMenu ------------------------ */

void LoadSliceMenu(int value){
  int errorcode,i;

  if(value==-999)return;
  glutSetCursor(GLUT_CURSOR_WAIT);
  if(value>=0){
    char *file;

    file = sliceinfo[value].file;
    if(script_multislice==0&&scriptoutstream!=NULL){
      fprintf(scriptoutstream,"LOADFILE\n");
      fprintf(scriptoutstream," %s\n",file);
    }
    if(scriptoutstream==NULL||defer_file_loading==0){
      if(value<nsliceinfo-nfedinfo){
        readslice(file,value,LOAD,&errorcode);
      }
      else{
        readfed(value,LOAD,FED_SLICE,&errorcode);
      }
    }
  }
  else{
    if(value==-1){
      for(i=0;i<nsliceinfo;i++){
        readslice("",i,UNLOAD,&errorcode);
      }
    }
    else{
      int submenutype;
      char *submenulabel;
      slicedata *slicei;
      int dir;

      value = -(1000 + value);
      submenutype=value/4;
      dir=value%4;
      submenutype=subslice_menuindex[submenutype];
      slicei = sliceinfo + submenutype;
      submenulabel = slicei->label.longlabel;
      for(i=0;i<nsliceinfo;i++){
        char *longlabel;

        slicei = sliceinfo + i;
        longlabel = slicei->label.longlabel;
        if(strcmp(longlabel,submenulabel)!=0)continue;
        if(dir!=0&&dir!=slicei->idir)continue;
        readslice(slicei->file,i,LOAD,&errorcode);
      }
    }
  }
  updatemenu=1;  
  glutPostRedisplay();
  glutSetCursor(GLUT_CURSOR_LEFT_ARROW);
}

/* ------------------ LoadVMultiSliceMenu ------------------------ */

void LoadMultiVSliceMenu(int value){
  int i;
  multivslicedata *mvslicei;

  if(value==-999)return;
  if(value==-20){
    showallslicevectors=1-showallslicevectors;
    updatemenu=1;  
    glutPostRedisplay();
    return;
  }
  if(value>=0){
    mvslicei = multivsliceinfo + value;
    if(scriptoutstream!=NULL){
      if(mvslicei->nvslices>0){
        slicedata *slicei;

        slicei = sliceinfo + mvslicei->ivslices[0];
        fprintf(scriptoutstream,"LOADVSLICE\n");
        fprintf(scriptoutstream," %s\n",slicei->label.longlabel);
        fprintf(scriptoutstream," %i %f\n",slicei->idir,slicei->position_orig);
        script_multivslice=1;
      }
    }
    if(scriptoutstream==NULL||defer_file_loading==0){
      for(i=0;i<mvslicei->nvslices;i++){
        LoadVSliceMenu(mvslicei->ivslices[i]);
      }
    }
    script_multivslice=0;
  }
  else{
    LoadVSliceMenu(-1);
  }
}


/* ------------------ LoadMultiSliceMenu ------------------------ */

void LoadMultiSliceMenu(int value){
  int i;
  multislicedata *mslicei;

  if(value==-999)return;
  if(value>=0){
    mslicei = multisliceinfo + value;
    if(scriptoutstream!=NULL){
      if(mslicei->nslices>0){
        slicedata *slicei;

        slicei = sliceinfo + mslicei->islices[0];
        fprintf(scriptoutstream,"LOADSLICE\n");
        fprintf(scriptoutstream," %s\n",slicei->label.longlabel);
        fprintf(scriptoutstream," %i %f\n",slicei->idir,slicei->position_orig);
        script_multislice=1;
      }
    }
    if(scriptoutstream==NULL||defer_file_loading==0){
      for(i=0;i<mslicei->nslices;i++){
        LoadSliceMenu(mslicei->islices[i]);
      }
      if(mslicei->nslices>0&&mslicei->islices[0]>=nsliceinfo-nfedinfo){
        output_mfed_csv(mslicei);
      }
    }
    script_multislice=0;
  }
  else{
    LoadSliceMenu(-1);
  }
}

/* ------------------ Plot3DListMenu ------------------------ */

void Plot3DListMenu(int value){
  int i;
  plot3ddata *plot3di;

  if(value<0||value>=nplot3dtimelist)return;
  LoadPlot3dMenu(-1);
  if(scriptoutstream!=NULL){
    fprintf(scriptoutstream,"LOADPLOT3D\n");
    fprintf(scriptoutstream," %f\n",plot3dtimelist[value]);
  }
  for(i=0;i<nplot3dinfo;i++){
    plot3di = plot3dinfo + i;
    if(ABS(plot3di->time-plot3dtimelist[value])<0.5){
      LoadPlot3dMenu(i);
    }
  }
}

/* ------------------ update_menu ------------------------ */

void update_menu(void){
  updatemenu=1; 
  glutPostRedisplay();
  glutSetCursor(GLUT_CURSOR_LEFT_ARROW);
}

/* ------------------ LoadPlot3DMenu ------------------------ */

void LoadPlot3dMenu(int value){
  int errorcode;
  int i;

  if(value==997)return;
  glutSetCursor(GLUT_CURSOR_WAIT);
  if(value>=0){
    char *plot3dfile;

    ReadPlot3dFile=1;
    plot3dfile = plot3dinfo[value].file;
    if(scriptoutstream!=NULL&&loadplot3dall==0){
      fprintf(scriptoutstream,"LOADPLOT3D\n");
      fprintf(scriptoutstream," %i %f\n",
        plot3dinfo[value].blocknumber+1,plot3dinfo[value].time);
    }
    if(scriptoutstream==NULL||defer_file_loading==0){
      readplot3d(plot3dfile,value,LOAD,&errorcode);
    }
  }
  else if(value==-1){
    for(i=0;i<nplot3dinfo;i++){
      readplot3d("",i,UNLOAD,&errorcode);
    }
  }
  else{
    value+=100000;
    loadplot3dall=1;
    Plot3DListMenu(value);
    loadplot3dall=0;
  }
  updatemenu=1; 
  glutPostRedisplay();
  glutSetCursor(GLUT_CURSOR_LEFT_ARROW);
}

/* ------------------ LoadIsoMenu ------------------------ */

void LoadIsoMenu(int value){
  int errorcode;
  int i;
  int ii;
  isodata *isoii, *isoi;

  if(value==-2)return;
  glutSetCursor(GLUT_CURSOR_WAIT);
  if(value>=0){
    char *file;

    ReadIsoFile=1;
    file=isoinfo[value].file;
    if(script_iso==0&&scriptoutstream!=NULL){
      fprintf(scriptoutstream,"LOADFILE\n");
      fprintf(scriptoutstream," %s\n",file);
    }
    if(scriptoutstream==NULL||defer_file_loading==0){
      readiso(file,value,LOAD,&errorcode);
    }
  }
  if(value==-1){
    for(i=0;i<nisoinfo;i++){
      isoii = isoinfo + i;
      if(isoii->loaded==1)readiso("",i,UNLOAD,&errorcode);
    }
  }
  if(value<=-10){
    ii = -(value + 10);
    isoii = isoinfo + ii;
    if(scriptoutstream!=NULL){
      script_iso=1;
      fprintf(scriptoutstream,"LOADISO\n");
      fprintf(scriptoutstream," %s\n",isoii->surface_label.longlabel);
    }
    if(scriptoutstream==NULL||defer_file_loading==0){
      for(i=0;i<nisoinfo;i++){
        isoi = isoinfo + i;
        if(isoii->type!=isoi->type)continue;
        LoadIsoMenu(i);
      }
    }
    script_iso=0;
  }
  updatemenu=1;  
  glutPostRedisplay();
  glutSetCursor(GLUT_CURSOR_LEFT_ARROW);
}

/* ------------------ LoadPatchMenu ------------------------ */

void LoadPatchMenu(int value){
  int errorcode;
  int i,ii;
  int patchtypenew;
  patchdata *patchi;

  glutSetCursor(GLUT_CURSOR_WAIT);
  if(value>=0){
    patchtypenew=getpatchtype(patchinfo+value);
    if(patchtypenew!=-1){
      for(ii=0;ii<npatch_loaded;ii++){
        i = patch_loaded_list[ii];
        patchi = patchinfo + i;
        if(patchi->type!=patchtypenew)readpatch(i,UNLOAD,&errorcode);
      }
    }
    if(scriptoutstream!=NULL){
      char *file;

      file = patchinfo[value].file;
      fprintf(scriptoutstream,"LOADFILE\n");
      fprintf(scriptoutstream," %s\n",file);
    }
    if(scriptoutstream==NULL||defer_file_loading==0){
      LOCK_COMPRESS
      readpatch(value,LOAD,&errorcode);
      UNLOCK_COMPRESS
    }
  }
  else if(value<=-10){
    patchdata *patchj;

    value = -(value + 10);
    patchj = patchinfo + value;
    if(scriptoutstream!=NULL){
      fprintf(scriptoutstream,"LOADBOUNDARY\n");
      fprintf(scriptoutstream," %s\n",patchj->label.longlabel);
    }
    if(scriptoutstream==NULL||defer_file_loading==0){
      for(i=0;i<npatchinfo;i++){
        patchi = patchinfo + i;
        if(strcmp(patchi->label.shortlabel,patchj->label.shortlabel)==0&&patchi->filetype==patchj->filetype){
          LOCK_COMPRESS
          readpatch(i,LOAD,&errorcode);
          UNLOCK_COMPRESS
        }
      }
    }
    force_redisplay=1;
    update_framenumber(0);
  }
  else if(value==-3){
    Update_All_Patch_Bounds();
  }
  else{
    for(i=0;i<npatchinfo;i++){
      readpatch(i,UNLOAD,&errorcode);
    }
  }
  updatemenu=1;  
  glutPostRedisplay();
  glutSetCursor(GLUT_CURSOR_LEFT_ARROW);
}

/* ------------------ ShowPatchMenu ------------------------ */

void ShowPatchMenu(int value){
  int val=0,n,i,ii;
//  int sum=0;
  mesh *meshi;
  patchdata *patchi;
  meshi=current_mesh;

  updatemenu=1;  
  updatefacelists=1;
  glutPostRedisplay();
  if(value>=1000){
    patchi=patchinfo+value-1000;
    if(patchi->type==ipatchtype){
      patchi->display=1-patchi->display;
    }
    else{
      patchi->display=1;
      ipatchtype=getpatchtype(patchi);
    }
    update_patchtype();
  }
  if(value==SHOW_CHAR){
    vis_threshold = 1 - vis_threshold;
    updatechar();
  }
  if(value==SHOWALL_BOUNDARY){
    for(ii=0;ii<npatch_loaded;ii++){
      i = patch_loaded_list[ii];
      patchi = patchinfo + i;
      patchi->display=1;
    }
  }
  if(value==HIDEALL_BOUNDARY){
    for(ii=0;ii<npatch_loaded;ii++){
      i = patch_loaded_list[ii];
      patchi = patchinfo + i;
      patchi->display=0;
    }
  }
  if(value<0){
    if(value==EXTERIORwallmenu){
      allexterior = 1-allexterior;
	  showexterior=1-showexterior;
//      for(i=1;i<7;i++){
//        sum+=visPatchType[i];
//      }
      val = allexterior;
      for(n=0;n<meshi->npatches;n++){
        if(meshi->patchtype[n]!=INTERIORwall){
          meshi->visPatches[n]=val;
        }
      }
      for(i=1;i<7;i++){
        visPatchType[i]=val;
      }
    }
    else if(value==INTERIORwallmenu){
      ASSERTFLAG(allinterior);
      allinterior = 1 - allinterior;
      val = allinterior;
      visPatchType[INTERIORwall]=val;
      for(n=0;n<meshi->npatches;n++){
        if(meshi->patchtype[n]==INTERIORwall){
          meshi->visPatches[n]=val;
        }
      }
    }
    else if(value!=DUMMYwallmenu){
      value = -(value+2); /* map xxxwallmenu to xxxwall */
      for(n=0;n<meshi->npatches;n++){
        if(meshi->patchtype[n]==value){
          ASSERTFLAG(meshi->visPatches[n]);
          meshi->visPatches[n] = 1 - meshi->visPatches[n];
          visPatchType[value]=meshi->visPatches[n];
        }
      }
    }
  }
  plotstate=getplotstate(DYNAMIC_PLOTS);
}


/* ------------------ VentMenu ------------------------ */

void VentMenu(int value){

  if(value==-1)return;
  switch (value){
  case 10:
    visVents=1;
    visOpenVents=1;
    visDummyVents=1;
    visOtherVents=1;
    break;
  case 14:
    visOpenVents=1-visOpenVents;
    break;
  case 15:
    visOpenVentsAsOutline = 1 - visOpenVentsAsOutline;
    break;
  case 16:
    visDummyVents = 1 - visDummyVents;
    break;
   case 18:
     show_bothsides_int=1-show_bothsides_int;
     updatefaces=1;
     break;
   case 19:
     show_bothsides_ext = 1 - show_bothsides_ext;
     updatefaces=1;
     break;
   case 20:
     show_transparent_vents=1-show_transparent_vents;
     updatefaces=1;
     break;
   case 21:
     visOtherVents=1-visOtherVents;
     break;
   case 22:
     visVents=0;
     visOpenVents=0;
     visDummyVents=0;
     visOtherVents=0;
     break;
  default:
    ASSERT(FFALSE);
    break;
  }
  updatefacelists=1;
  updatemenu=1;  
  glutPostRedisplay();
}


/* ------------------ ImmersedMenu ------------------------ */
#define IMMERSED_SOLID 0
#define IMMERSED_OUTLINE 1
#define IMMERSED_SOLIDOUTLINE 2
#define IMMERSED_HIDE 7
void ImmersedMenu(int value){
  updatemenu=1;
  switch (value){
    case IMMERSED_SOLIDOUTLINE:
      if(showtrisurface==1&&showtrioutline==1){
        showtrisurface=1;
        showtrioutline=0;
      }
      else{
        showtrisurface=1;
        showtrioutline=1;
      }
      break;
    case IMMERSED_SOLID:
      showtrisurface=1-showtrisurface;
      break;
    case IMMERSED_OUTLINE:
      showtrioutline=1-showtrioutline;
      break;
    case 3:
      showtrinormal=1-showtrinormal;
      break;
    case 4:
      smoothtrinormal=1-smoothtrinormal;
      break;
    case 5:
      hilight_skinny = 1 - hilight_skinny;
      break;
    case 6:
      sort_embedded_geometry=1-sort_embedded_geometry;
      break;
    case IMMERSED_HIDE:
      if(showtrisurface==0&&showtrioutline==0){
        showtrisurface=1;
      }
      else{
        showtrisurface=0;
        showtrioutline=0;
      }
      break;
    default:
      ASSERT(0);
      break;
  }
  visAIso=showtrisurface*1+showtrioutline*2+showtripoints*4;
  glutPostRedisplay();
}

/* ------------------ BlockageMenu ------------------------ */

void BlockageMenu(int value){
  int change_state=0;

  if(solid_state<0)solid_state=visBlocks;
  if(outline_state<0)outline_state=OUTLINE_NONE;
  switch (value){
    case visBLOCKOnlyOutline:
      if(outline_state!=OUTLINE_ONLY){
        outline_state=OUTLINE_ONLY;
      }
      else{
        outline_state=OUTLINE_NONE;
      }
      change_state=1;
      break;
    case visBLOCKAddOutline:
      if(outline_state!=OUTLINE_ADDED){
        outline_state=OUTLINE_ADDED;
        if(solid_state==visBLOCKHide)solid_state=visBLOCKAsInput;
      }
      else{
        outline_state=OUTLINE_NONE;
      }
      change_state=1;
      break;
    case visBLOCKAsInput:
      solid_state=visBLOCKAsInput;
      if(outline_state==OUTLINE_ONLY)outline_state=OUTLINE_ADDED;
      change_state=1;
      break;
    case visBLOCKNormal:
      solid_state=visBLOCKNormal;
      if(outline_state==OUTLINE_ONLY)outline_state=OUTLINE_ADDED;
      change_state=1;
      break;
    case visBLOCKHide:
      outline_state=OUTLINE_NONE;
      solid_state=visBLOCKHide;
      change_state=1;
      break;
  }
  if(change_state==1){
    switch (outline_state){
      case OUTLINE_NONE:
        value=solid_state;
        break;
      case OUTLINE_ONLY:
        value=visBLOCKOutline;
        break;
      case OUTLINE_ADDED:
        switch (solid_state){
          case visBLOCKAsInput:
            value=visBLOCKAsInputOutline;
          break;
          case visBLOCKNormal:
            value=visBLOCKSolidOutline;
            break;
          case BLOCKAGE_HIDDEN:
            value=visBLOCKOutline;
            break;
        }
        break;
    }
  }

  switch (value){
   case visBLOCKAsInputOutline:
   case visBLOCKAsInput:
     visBlocks=value;
     update_trainer_outline();
     break;
   case visSmoothBLOCKSolid:
     smooth_block_solid = 1 - smooth_block_solid;
     break;
   case visBLOCKNormal:
   case visBLOCKOutline:
   case visBLOCKHide:
   case visBLOCKSolidOutline:
     visBlocks=value;
     if(value==visBLOCKSolidOutline||visBLOCKold==visBLOCKSolidOutline)updatefaces=1;
     update_trainer_outline();
     break;
   case BLOCKlocation_grid:
   case BLOCKlocation_exact:
   case BLOCKlocation_cad:
     blocklocation=value;
     break;
   case BLOCKtexture_cad:
     visCadTextures=1-visCadTextures;
     break;
   case visBLOCKSmoothAsNormal:
     visSmoothAsNormal = 1 - visSmoothAsNormal;
     break;
   case visBLOCKTransparent:
     visTransparentBlockage=1-visTransparentBlockage;
     break;
   case SMOOTH_BLOCKAGES:
     menusmooth=1;
     updatesmoothblocks=1;
     break;
   case SMOOTH_ATSTART:
     sb_atstart=1-sb_atstart;
     break;
   default:
     if(value<0){
       value=-value-1;
       if(value>=0&&value<=npropinfo-1){
         propdata *propi;

         propi = propinfo + value;
         propi->blockvis=1-propi->blockvis;
       }
     }
     else{
       ASSERT(FFALSE);
     }
     break;
  }
  visBLOCKold=value;
  updatemenu=1;  
 // updatefaces=1;
  updatefacelists=1;
  updatehiddenfaces=1;
  glutPostRedisplay();
}

/* ------------------ RotateTypeMenu ------------------------ */

void RotateTypeMenu(int value){
  if(value==999)return;
  rotation_type = value;
  update_rotation_type(rotation_type);
  rotation_type_CB(0);
  updatemenu=1;  
  glutPostRedisplay();
}

/* ------------------ TitleMenu ------------------------ */

void TitleMenu(int value){
  updatemenu=1;  
  glutPostRedisplay();
  ASSERTFLAG(visTitle0);
  ASSERTFLAG(visTitle1);
  ASSERTFLAG(visTitle2);
  switch (value){
  case 0:
    visTitle0 = 1 - visTitle0;
    break;
  case 1:
    visTitle1 = 1 - visTitle1;
    break;
  case 2:
    visTitle2 = 1 - visTitle2;
    break;
  case 3:
    visTitle0=1;
    visTitle1=1;
    visTitle2=1;
    break;
  case 4:
    visTitle0=0;
    visTitle1=0;
    visTitle2=0;
    break;
  default:
    ASSERT(FFALSE);
    break;
  }
}


/* ------------------ PropMenu ------------------------ */

void PropMenu(int value){
  int iprop, iobject;

  // value = iobject*npropinfo + iprop

  iprop = value%npropinfo;
  iobject = value/npropinfo;
  if(iprop>=0&&iprop<npropinfo){
    propdata *propi;

    propi = propinfo + iprop;
    if(iobject>=0&&iobject<propi->nsmokeview_ids){
      int i;

      propi->smokeview_id=propi->smokeview_ids[iobject];
      propi->smv_object=propi->smv_objects[iobject];
      updatemenu=1;
      get_indep_var_indices(propi->smv_object,
        propi->vars_indep,propi->nvars_indep,
        propi->vars_indep_index);

      for(i=0;i<npartclassinfo;i++){
        part5class *partclassi;

        partclassi = partclassinfo + i;
        update_partclass_depend(partclassi);

      }

      glutPostRedisplay();
    }
  }
}

/* ------------------ ShowObjectsMenu ------------------------ */

void ShowObjectsMenu(int value){
  sv_object *objecti;
  int i;

  if(value>=0&&value<nobject_defs){
    objecti = object_defs[value];
    objecti->visible = 1 - objecti->visible;
  }
  else if(value==-1){
    for(i=0;i<nobject_defs;i++){
      objecti = object_defs[i];
      objecti->visible=1;
    }
  }
  else if(value==-2){
    for(i=0;i<nobject_defs;i++){
      objecti = object_defs[i];
      objecti->visible=0;
    }
  }
  else if(value==-3){
    select_device=1-select_device;
  }
  else if(value==-4){
    object_outlines=1-object_outlines;
  }
  else if(value==-999){
  }
  else{
    int oldval;

    oldval=device_sphere_segments;
    value=-value;
    device_sphere_segments=value;
    if(oldval!=device_sphere_segments){
      initspheresegs(device_sphere_segments,2*device_sphere_segments);
      initcircle(device_sphere_segments);
    }
  }
  updatemenu=1;
  glutPostRedisplay();
}

/* ------------------ ZoneShowMenu ------------------------ */

void ZoneShowMenu(int value){
  switch (value){
  case 999:
    return;
  case 1:
    visVZone=0;
    visHZone=1;
    visZone=1;
    break;
  case 2:
    visVZone=1;
    visHZone=0;
    visZone=1;
    break;
  case 3:
    visVZone=1; 
    visHZone=1;
    visZone=1;
    break;
  case 4:
    visVZone=0; 
    visHZone=0;
    visSZone=0;
    visZone=0;
    break;
  case 5:
    sethazardcolor=1;
    visSZone=0;
    if(visVZone==0&&visHZone==0)visVZone=1;
    visZone=1;
    break;
  case 6:
    sethazardcolor=0;
    visSZone=0;
    if(visVZone==0&&visHZone==0)visVZone=1;
    visZone=1;
    break;
  case 7:
    sethazardcolor=2;
    visSZone=1;
    visZone=1;
    break;
  case 11:         //solid
    visVentSolid=1;
    visVentLines=0;
    visVents=1;
    break;
  case 12:         //lines
    visVentLines=1;
    visVentSolid=0;
    visVents=1;
    break;
  case 13:         //hide
    visVentLines=0;
    visVentSolid=0;
    break;
  case 14:
    visVents=1-visVents;
    if(visVents==0){
      visVentLines=0;
      visVentSolid=0;
    }
    if(visVents==1){
      visVentSolid=1;
      visVentLines=0;
    }
    break;
  case 15:
    viszonefire=1-viszonefire;
    break;
  default:
    ASSERT(FFALSE);
  }
  updatemenu=1;  
  glutPostRedisplay();
}

/* ------------------ GeometryMenu ------------------------ */

void GeometryMenu(int value){

  switch (value){
  case 3:
    if(isZoneFireModel==0)visFrame=1-visFrame;
    break;
  case 5:
    visFloor=1-visFloor;
    break;
  case 6:
    visWalls=1-visWalls;
    break;
  case 7:
    visCeiling=1-visCeiling;
    break;
    //shaded 17 0
    //stepped 18 1
    //line    19 2
    //texture 20 3
    //hidden 21 4
  case 17:
  case 18:
  case 19:
  case 20:
  case 21:
    if(value==21){
      BlockageMenu(visBlocksSave);
    }
    else{
    //  visBlocksSave=visBlocks;
    //  BlockageMenu(visBLOCKHide);
    }
    visTerrainType=value-17;
    if(visTerrainType==TERRAIN_3D){
      planar_terrain_slice=0;
    }
    else{
      planar_terrain_slice=1;
    }
    Update_Glui_Wui();
    break;
  case 11:
    if(isZoneFireModel)visFrame=1;
    /*
    visFloor=1;
    visWalls=1;
    visCeiling=1;
    */
    visVents=1;
    BlockageMenu(visBLOCKAsInput);
    break;
  case 13:
    visFrame=0;
    visFloor=0;
    visWalls=0;
    visCeiling=0;
    visVents=0;
    visGrid=0;
    BlockageMenu(visBLOCKHide);
    break;
  default:
    ASSERT(FFALSE);
    break;
  }
  updatefacelists=1;
  updatemenu=1;  
  glutPostRedisplay();
}

/* ------------------ MENU_vslice ------------------------ */

void MENU_vslice(int vec_type){
  int ii,i;
  slicedata *sd;
  vslicedata *vd;
  char check[]="*";
  char *menulabel;
  char a_menulabel[1000];
  menulabel=a_menulabel;

  for(ii=0;ii<nvsliceinfo;ii++){
    i=vsliceorderindex[ii];
    vd = vsliceinfo + i;
    if(vd->vec_type!=vec_type)continue;
    sd = sliceinfo + vd->ival;
    if(vd->loaded==1){
      STRCPY(menulabel,check);
      STRCAT(menulabel,sd->menulabel);
    }
    else{
      STRCPY(menulabel,sd->menulabel);
    }
    if(sd->vec_comp==0||showallslicevectors==1)glutAddMenuEntry(menulabel,i);
  }
}

/* ------------------ InitMenus ------------------------ */

void InitMenus(int unload){
  int nscripts;
  int showflag, hideflag;
  int n,i,ii;
  char caselabel[255],chari[4];
  int showall,hideall;
  char levellabel[1024];
  int nsliceloaded;
  int nevacloaded;
  int nsmoke3dloaded;
  int nvolsmoke3dloaded;
  int nvsliceloaded2,nvsliceloaded;
  int nmultisliceloaded;
  int npartloaded;
  int npart5loaded;
  int npart4loaded;
  int npatchloaded;
  int nplot3dloaded;
  int nisoloaded;
  int nvslice0, nvslice1, nvslice2;
  int nvsliceloaded0;
  int nvsliceloaded1;
  char check[]="*";
  multislicedata *mslicei;
  multivslicedata *mvslicei;
  mesh *cmesh;
  char menulabel[1024];
  int j;
  int ntextures_used;
  int multiprop;

static int titlemenu=0, labelmenu=0, colorbarmenu=0, colorbarsmenu=0, colorbarshademenu, smokecolorbarmenu=0, lightingmenu=0, showhidemenu=0;
static int optionmenu=0, rotatetypemenu=0;
static int resetmenu=0, frameratemenu=0, rendermenu=0, smokeviewinimenu=0, inisubmenu=0;
#ifdef pp_COMPRESS
static int compressmenu=0;
#endif
static int showhideslicemenu=0,showvslicemenu=0;
static int plot3dshowmenu=0, staticvariablemenu=0, helpmenu=0, webhelpmenu=0, keyboardhelpmenu=0, mousehelpmenu=0;
static int vectorskipmenu=0,unitsmenu=0;
static int isosurfacemenu=0, isovariablemenu=0, levelmenu=0;
static int fontmenu=0, aperturemenu=0,dialogmenu=0,zoommenu=0,languagemenu=0,fonttestmenu=0;
static int gridslicemenu=0, blockagemenu=0, immersedmenu=0, loadpatchmenu=0, ventmenu=0;
static int loadisomenu=0, isosurfacetypemenu=0;
static int geometrymenu=0, loadunloadmenu=0, reloadmenu=0, aboutmenu=0, disclaimermenu=0, terrain_showmenu=0;
static int scriptmenu=0;
static int scriptlistmenu=0,scriptsteplistmenu=0,scriptrecordmenu=0;
static int loadplot3dmenu=0, unloadvslicemenu=0, unloadslicemenu=0;
static int loadterrainmenu=0, unloadterrainmenu=0;
static int loadsmoke3dmenu=0,loadsmoke3dsootmenu=0,loadsmoke3dhrrmenu=0,loadsmoke3dwatermenu=0;
static int loadvolsmoke3dmenu=0,showvolsmoke3dmenu=0;
static int unloadsmoke3dmenu=0,unloadvolsmoke3dmenu=0;
static int unloadevacmenu=0, unloadpartmenu=0, loadslicemenu=0, loadmultislicemenu=0;
static int *loadsubvslicemenu=NULL, nloadsubvslicemenu=0;
static int *loadsubslicemenu=NULL, nloadsubslicemenu=0, iloadsubslicemenu=0;
static int *loadsubmslicemenu=NULL, nloadsubmslicemenu=0;
static int *loadsubmvslicemenu=NULL, nloadsubmvslicemenu=0;
static int *loadsubplot3dmenu=NULL, nloadsubplot3dmenu=0;
static int loadmultivslicemenu=0, unloadmultivslicemenu=0;
static int unloadmultislicemenu=0, vslicemenu=0, staticslicemenu=0;
static int evacmenu=0, particlemenu=0, particlesubmenu=0, showpatchmenu=0, zonemenu=0, isoshowmenu=0, isolevelmenu=0, smoke3dshowmenu=0;
static int particlepropshowmenu=0,humanpropshowmenu=0;
static int *particlepropshowsubmenu=NULL;
static int particlestreakshowmenu=0;
static int tourmenu=0;
static int avatartourmenu=0,avatarevacmenu=0;
static int trainerviewmenu=0,mainmenu=0,zoneshowmenu=0,particleshowmenu=0,evacshowmenu=0,targetmenu=0;
static int showobjectsmenu=0,spheresegmentmenu=0,propmenu=0;
static int unloadplot3dmenu=0, unloadpatchmenu=0, unloadisomenu=0;
static int showmultislicemenu=0;
static int textureshowmenu=0;
#ifdef _DEBUG
static int menu_count=0;
static int in_menu=0;
#endif

updatemenu=0;
#ifdef _DEBUG
  printf("Updating Menus %i In menu %i\n",menu_count++,in_menu);
  in_menu=1;
#endif
  update_showhidebuttons();
  glutPostRedisplay();
  cmesh=current_mesh;

  nsliceloaded=0;
  for(i=0;i<nsliceinfo;i++){
    slicedata *sd;

    sd = sliceinfo + i;
    if(sd->loaded==1)nsliceloaded++;
  }

  nsmoke3dloaded=0;
  for(i=0;i<nsmoke3dinfo;i++){
    smoke3ddata *smoke3di;

    smoke3di=smoke3dinfo+i;
    if(smoke3di->loaded==1)nsmoke3dloaded++;
  }

  nvolsmoke3dloaded=0;
  for(i=0;i<nmeshes;i++){
    mesh *meshi;
    volrenderdata *vr;

    meshi = meshinfo + i;
    vr = &(meshi->volrenderinfo);
    if(vr->fire==NULL||vr->smoke==NULL)continue;
    if(vr->loaded==1){
      nvolsmoke3dloaded++;
    }
  }

  nvsliceloaded=0;
  for(i=0;i<nvsliceinfo;i++){
    vslicedata *vd;

    vd = vsliceinfo + i;
    if(vd->loaded==1)nvsliceloaded++;
  }

  for(i=0;i<nmultislices;i++){
    mslicei = multisliceinfo + i;
    mslicei->loaded=0;
    mslicei->display=0;
    for(j=0;j<mslicei->nslices;j++){
      slicedata *sd;

      sd = sliceinfo + mslicei->islices[j];
      if(sd->loaded==1)mslicei->loaded++;
      if(sd->display==1)mslicei->display++;
    }
    if(mslicei->loaded>0&&mslicei->loaded<mslicei->nslices){
      mslicei->loaded=-1;
    }
    else if(mslicei->loaded==mslicei->nslices){
      mslicei->loaded=1;
    }
    if(mslicei->display>0&&mslicei->display<mslicei->nslices){
      mslicei->display=-1;
    }
    else if(mslicei->display==mslicei->nslices){
      mslicei->display=1;
    }
  }
  for(i=0;i<nmultivslices;i++){
    mvslicei = multivsliceinfo + i;
    mvslicei->loaded=0;
    mvslicei->display=0;
    for(j=0;j<mvslicei->nvslices;j++){
      vslicedata *vd;

      vd = vsliceinfo + mvslicei->ivslices[j];
      if(vd->loaded==1)mvslicei->loaded++;
      if(vd->display==1)mvslicei->display++;
    }
    if(mvslicei->loaded>0&&mvslicei->loaded<mvslicei->nvslices){
      mvslicei->loaded=-1;
    }
    else if(mvslicei->loaded==mvslicei->nvslices){
      mvslicei->loaded=1;
    }
    if(mvslicei->display>0&&mvslicei->display<mvslicei->nvslices){
      mvslicei->display=-1;
    }
    else if(mvslicei->display==mvslicei->nvslices){
      mvslicei->display=1;
    }
  }


  npart5loaded=0;
  npart4loaded=0;
  npartloaded=0;
  nevacloaded=0;
  for(i=0;i<npartinfo;i++){
    partdata *parti;

    parti = partinfo+i;
    if(parti->loaded==1&&parti->evac==0)npartloaded++;
    if(parti->loaded==1&&parti->evac==1)nevacloaded++;
    if(parti->loaded==1){
      if(parti->version==1)npart5loaded++;
      if(parti->version==0)npart4loaded++;
    }
  }

  nplot3dloaded=0;
  for(i=0;i<nplot3dinfo;i++){
    plot3ddata *plot3di;

    plot3di = plot3dinfo + i;
    if(plot3di->loaded==1)nplot3dloaded++;
  }

  nisoloaded=0;
  for(i=0;i<nisoinfo;i++){
    isodata *isoi;

    isoi = isoinfo + i;
    if(isoi->loaded==1)nisoloaded++;
  }

  npatchloaded=0;
  for(i=0;i<npatchinfo;i++){
    patchdata *patchi;

    patchi = patchinfo + i;
    if(patchi->loaded==1)npatchloaded++;
  }

#define CREATEMENU(menu,Menu) menu=glutCreateMenu(Menu);\
  if(nmenus<10000){\
    strcpy(menuinfo[nmenus].label,#Menu);\
    menuinfo[nmenus++].menuvar=menu;\
  }

  {
    for(i=0;i<nmenus;i++){
      menudata *menui;

      menui = menuinfo + i;

      if(menui->menuvar>0){
        glutDestroyMenu(menui->menuvar);
      }
    }
    nmenus=0;
  }
  if(nloadsubslicemenu>0){
    FREEMEMORY(loadsubslicemenu);
  }
  if(nloadsubmslicemenu>0){
    FREEMEMORY(loadsubmslicemenu);
  }
  if(nloadsubvslicemenu>0){
    FREEMEMORY(loadsubvslicemenu);
  }
  FREEMEMORY(particlepropshowsubmenu);
  if(nloadsubmvslicemenu>0){
    FREEMEMORY(loadsubmvslicemenu);
  }
  if(nloadsubplot3dmenu>0){
    FREEMEMORY(loadsubplot3dmenu);
  }
  if(unload==UNLOAD)return;



/* --------------------------------patch menu -------------------------- */
  if(npatchinfo>0){
    CREATEMENU(showpatchmenu,ShowPatchMenu);
    npatchloaded=0;
    {
      int local_do_threshold=0;

      for(ii=0;ii<npatchinfo;ii++){
        patchdata *patchi;

        i = patchorderindex[ii];
        patchi = patchinfo+i;
        if(patchi->loaded==0)continue;
        npatchloaded++;
        if(patchi->display==1&&patchi->type==ipatchtype){
          STRCPY(menulabel,"*");
          STRCAT(menulabel,patchi->menulabel);
        }
        else{
          STRCPY(menulabel,patchi->menulabel);
        }
        glutAddMenuEntry(menulabel,1000+i);
        if(activate_threshold==1){
          if(
            strncmp(patchi->label.shortlabel,"TEMP",4) == 0||
            strncmp(patchi->label.shortlabel,"temp",4) == 0
            ){
            local_do_threshold=1;
          }
        }
      }
      if(activate_threshold==1&&local_do_threshold==1){
        glutAddMenuEntry("-",DUMMYwallmenu);
        if(vis_threshold==1)glutAddMenuEntry("*char",SHOW_CHAR);
        if(vis_threshold==0)glutAddMenuEntry("char",SHOW_CHAR);
      }

    }
    if(npatchloaded>0){
      glutAddMenuEntry("-",DUMMYwallmenu);
    }
    if(npatchloaded>1){
      glutAddMenuEntry(_("Show all boundary files"),SHOWALL_BOUNDARY);
      glutAddMenuEntry(_("Hide all boundary files"),HIDEALL_BOUNDARY);
      glutAddMenuEntry("-",DUMMYwallmenu);
    }
    if(showexterior==1){
      glutAddMenuEntry(_("*Exterior"),EXTERIORwallmenu);
    }
    if(showexterior==0){
      glutAddMenuEntry(_("Exterior"),EXTERIORwallmenu);
    }
    if(visPatchType[INTERIORwall]==1){
      glutAddMenuEntry(_("*Interior"),INTERIORwallmenu);
    }
    if(visPatchType[INTERIORwall]==0){
      glutAddMenuEntry(_("Interior"),INTERIORwallmenu);
    }
    if(ispatchtype(FRONTwall)==1&&visPatchType[FRONTwall]==1){
      glutAddMenuEntry(_("*Front"),FRONTwallmenu);
    }
    if(ispatchtype(FRONTwall)==1&&visPatchType[FRONTwall]==0){
      glutAddMenuEntry(_("Front"),FRONTwallmenu);
    }
    if(ispatchtype(BACKwall)==1&& visPatchType[BACKwall]==1){
      glutAddMenuEntry(_("*Back"),BACKwallmenu);
    }
    if(ispatchtype(BACKwall)==1&& visPatchType[BACKwall]==0){
      glutAddMenuEntry(_("Back"),BACKwallmenu);
    }
    if(ispatchtype(LEFTwall)==1&& visPatchType[LEFTwall]==1){ 
      glutAddMenuEntry(_("*Left"),LEFTwallmenu);
    }
    if(ispatchtype(LEFTwall)==1&& visPatchType[LEFTwall]==0){
      glutAddMenuEntry(_("Left"),LEFTwallmenu);
    }
    if(ispatchtype(RIGHTwall)==1&&visPatchType[RIGHTwall]==1){
      glutAddMenuEntry(_("*Right"),RIGHTwallmenu);
    }
    if(ispatchtype(RIGHTwall)==1&&visPatchType[RIGHTwall]==0){
      glutAddMenuEntry(_("Right"),RIGHTwallmenu);
    }
    if(ispatchtype(UPwall)==1&&   visPatchType[UPwall]==1){
      glutAddMenuEntry(_("*Up"),UPwallmenu);
    }
    if(ispatchtype(UPwall)==1&&   visPatchType[UPwall]==0){
      glutAddMenuEntry(_("Up"),UPwallmenu);
    }
    if(ispatchtype(DOWNwall)==1&& visPatchType[DOWNwall]==1){
      glutAddMenuEntry(_("*Down"),DOWNwallmenu);
    }
    if(ispatchtype(DOWNwall)==1&& visPatchType[DOWNwall]==0){
      glutAddMenuEntry(_("Down"),DOWNwallmenu);
    }
  }

/* --------------------------------embedded menu -------------------------- */

  CREATEMENU(immersedmenu,ImmersedMenu);
  glutAddMenuEntry(_("View Method:"),999);
  if(showtrisurface==1&&showtrioutline==1){
    glutAddMenuEntry(_(" *Solid and outline"),IMMERSED_SOLIDOUTLINE);
  }
  else{
    glutAddMenuEntry(_(" Solid and outline"),IMMERSED_SOLIDOUTLINE);
  }
  if(showtrisurface==1){
    glutAddMenuEntry(_(" *Solid"),IMMERSED_SOLID);
  }
  else{
    glutAddMenuEntry(_(" Solid"),IMMERSED_SOLID);
  }
  if(showtrioutline==1){
    glutAddMenuEntry(_(" *Outline"),IMMERSED_OUTLINE);
  }
  else{
    glutAddMenuEntry(_(" Outline"),IMMERSED_OUTLINE);
  }
  if(showtrisurface==0&&showtrioutline==0){
    glutAddMenuEntry(_(" *Hide"),IMMERSED_HIDE);
  }
  else{
    glutAddMenuEntry(_(" Hide"),IMMERSED_HIDE);
  }
  if(sort_embedded_geometry==1){
    glutAddMenuEntry(_(" *Sort faces"),6);
  }
  else{
    glutAddMenuEntry(_(" Sort faces"),6);
  }
#ifdef pp_BETA
  if(showtrinormal==1){
    glutAddMenuEntry(_(" *Show normal"),3);
  }
  else{
    glutAddMenuEntry(_(" Show normal"),3);
  }
  if(smoothtrinormal==1){
    glutAddMenuEntry(_(" *Smooth normal"),4);
  }
  else{
    glutAddMenuEntry(_(" Smooth normal"),4);
  }
  if(hilight_skinny==1){
    glutAddMenuEntry(_(" *Hilight skinny triangles"),5);
  }
  else{
    glutAddMenuEntry(_(" Hilight skinny triangles"),5);
  }
#endif

/* --------------------------------blockage menu -------------------------- */
  CREATEMENU(blockagemenu,BlockageMenu);
  if(use_menusmooth==1){
    if(sb_atstart==1){
      glutAddMenuEntry(_("*Smooth blockages at startup"),SMOOTH_ATSTART);
    }
    else{
      glutAddMenuEntry(_("Smooth blockages at startup"),SMOOTH_ATSTART);
    }
    glutAddMenuEntry(_("Smooth blockages now"),SMOOTH_BLOCKAGES);
  }
  glutAddMenuEntry(_("View Method:"),999);
  if(visBlocks==visBLOCKAsInput||visBlocks==visBLOCKAsInputOutline){
    glutAddMenuEntry(_("   *Defined in input file"),visBLOCKAsInput);
  }
   else{
    glutAddMenuEntry(_("   Defined in input file"),visBLOCKAsInput);
  }
  if(use_menusmooth==1){
    if(smooth_block_solid==1){
      glutAddMenuEntry(_("       *Smooth blockages drawn opaque"),visSmoothBLOCKSolid);
    }
    else{
      glutAddMenuEntry(_("       Smooth blockages drawn opaque"),visSmoothBLOCKSolid);
    }
  }
  if(visBlocks==visBLOCKNormal||visBlocks==visBLOCKSolidOutline){
    glutAddMenuEntry(_("   *Solid"),visBLOCKNormal);
    if(nsmoothblocks>0){
      if(visSmoothAsNormal==1){
         glutAddMenuEntry(_("      Smooth"),visBLOCKSmoothAsNormal);
      }
      else{
         glutAddMenuEntry(_("      *Smooth"),visBLOCKSmoothAsNormal);
      }
    }
    if(ntransparentblocks>0){
      if(visTransparentBlockage==1){
         glutAddMenuEntry(_("      *Transparent"),visBLOCKTransparent);
      }
      else{
         glutAddMenuEntry(_("      Transparent"),visBLOCKTransparent);
      }
    }
  }
  else{
    glutAddMenuEntry(_("   Solid"),visBLOCKNormal);
  }
  if(outline_state==OUTLINE_ONLY){
    glutAddMenuEntry(_("   *Outline Only"),visBLOCKOnlyOutline);
  }
  else{
    glutAddMenuEntry(_("   Outline Only"),visBLOCKOnlyOutline);
  }
  if(outline_state==OUTLINE_ADDED){
    glutAddMenuEntry(_("   *Outline Added"),visBLOCKAddOutline);
  }
  else{
    glutAddMenuEntry(_("   Outline Added"),visBLOCKAddOutline);
  }
  if(visBlocks==visBLOCKHide){
    glutAddMenuEntry(_("   *Hidden"),visBLOCKHide);
  }
  else{
    glutAddMenuEntry(_("   Hidden"),visBLOCKHide);
  }
  {
    int nblockprop=0;

    for(i=0;i<npropinfo;i++){
      propdata *propi;

      propi = propinfo + i;
      if(propi->inblockage==1)nblockprop++;
    }
    if(nblockprop>0){
      char propmenulabel[255];

      glutAddMenuEntry("-",999);
      glutAddMenuEntry(_("Show/Hide blockage types:"),999);
      for(i=0;i<npropinfo;i++){
        propdata *propi;

        propi = propinfo + i;
        if(propi->inblockage==1){
          strcpy(propmenulabel,"    ");
          if(propi->blockvis==1)strcat(propmenulabel,"*");
          strcat(propmenulabel,propi->label);
          glutAddMenuEntry(propmenulabel,-i-1);
        }
      }
    }
  }
  glutAddMenuEntry("-",999);
  glutAddMenuEntry(_("Locations:"),999);
  if(blocklocation==BLOCKlocation_grid){
    glutAddMenuEntry(_("   *Actual"),BLOCKlocation_grid);
  }
  else{
    glutAddMenuEntry(_("   Actual"),BLOCKlocation_grid);
  }
  if(blocklocation==BLOCKlocation_exact){
    glutAddMenuEntry(_("   *Requested"),BLOCKlocation_exact);
  }
  else{
    glutAddMenuEntry(_("   Requested"),BLOCKlocation_exact);
  }
  if(ncadgeom>0){
    if(blocklocation==BLOCKlocation_cad){
      glutAddMenuEntry(_("   *Cad"),BLOCKlocation_cad);
    }
    else{
      glutAddMenuEntry(_("   Cad"),BLOCKlocation_cad);
    }
    {
      cadgeom *cd;
      cadlook *cdi;
      int showtexturemenu;

      showtexturemenu=0;
      for(i=0;i<ncadgeom;i++){
        cd = cadgeominfo + i;
        for(j=0;j<cd->ncadlookinfo;j++){
          cdi = cd->cadlookinfo+j;
          if(cdi->textureinfo.loaded==1){
            showtexturemenu=1;
            break;
          }
        }
        if(showtexturemenu==1)break;
      }
      if(showtexturemenu==1){
        if(visCadTextures==1){
          glutAddMenuEntry(_(" *Show CAD textures"),BLOCKtexture_cad);
        }
        else{
          glutAddMenuEntry(_(" Show CAD textures"),BLOCKtexture_cad);
        }
      }
    }
  }


/* --------------------------------level menu -------------------------- */

  if(nplot3dinfo>0){
    CREATEMENU(levelmenu,LevelMenu);
    for(i=1;i<nrgb-1;i++){
      if(colorlabeliso!=NULL){
        char *colorlabel;
        char levellabel2[256];

        colorlabel=&colorlabeliso[plotn-1][nrgb-2-i][0];
        strcpy(levellabel2,"");
        if(plotiso[plotn-1]==nrgb-2-i&&visiso==1){
          strcat(levellabel2,"*");
        }
        strcat(levellabel2,colorlabel);
        glutAddMenuEntry(levellabel2,nrgb-2-i);
      }
      else{
        if(plotiso[plotn]==i&&visiso==1){
          sprintf(chari,"*%i",i+1);
        }
        else{
          sprintf(chari,"%i",i+1);
        }
        glutAddMenuEntry(chari,i+1);
      }
    }
  }

/* --------------------------------static variable menu -------------------------- */

  if(nplot3dinfo>0){
    CREATEMENU(staticvariablemenu,StaticVariableMenu);
    for(n=0;n<numplot3dvars;n++){
      char *p3label;

      p3label = plot3dinfo[0].label[n].shortlabel;
      if(plotn-1==n){
        STRCPY(menulabel,"*");
        STRCAT(menulabel,p3label);
        glutAddMenuEntry(menulabel,n+1);
      }
      else{
        glutAddMenuEntry(p3label,n+1);
      }
    }
  }

/* --------------------------------iso variable menu -------------------------- */

  if(nplot3dinfo>0){
    CREATEMENU(isovariablemenu,IsoVariableMenu);
    for(n=0;n<numplot3dvars;n++){
      char *p3label;

      p3label = plot3dinfo[0].label[n].shortlabel;
      if(plotn-1==n&&visiso==1){
        STRCPY(menulabel,"*");
        STRCAT(menulabel,p3label);
        glutAddMenuEntry(menulabel,n+1);
      }
      else{
        glutAddMenuEntry(p3label,n+1);
      }
    }
  }

/* --------------------------------iso surface menu -------------------------- */
  if(nplot3dinfo>0){
    CREATEMENU(isosurfacetypemenu,IsoSurfaceTypeMenu);
    if(p3dsurfacesmooth==1&&p3dsurfacetype==1){
      glutAddMenuEntry(_("*Smooth"),0);
    }
     else{
       glutAddMenuEntry(_("Smooth"),0);
     }
     if(p3dsurfacesmooth==0&&p3dsurfacetype==1){
       glutAddMenuEntry(_("*Facets"),1);
     }
    else{
      glutAddMenuEntry(_("Facets"),1);
    }
    if(p3dsurfacetype==2)glutAddMenuEntry(_("*Triangles"),2);
    if(p3dsurfacetype!=2)glutAddMenuEntry(_("Triangles"),2);
    if(p3dsurfacetype==3)glutAddMenuEntry(_("*Points"),3);
    if(p3dsurfacetype!=3)glutAddMenuEntry(_("Points"),3);

    CREATEMENU(isosurfacemenu,IsoSurfaceMenu);
    glutAddSubMenu(_("Solution variable"),isovariablemenu);
    glutAddSubMenu(_("Solution value"),levelmenu);
    glutAddSubMenu(_("Surface type"),isosurfacetypemenu);
    glutAddMenuEntry(_("Hide"),1);
  }

/* --------------------------------vector skip menu -------------------------- */

  if(nplot3dinfo>0){
    CREATEMENU(vectorskipmenu,VectorSkipMenu);
    if(visVector==1)glutAddMenuEntry(_("*Show"),-2);
    if(visVector!=1)glutAddMenuEntry(_("Show"),-2);
    glutAddMenuEntry(_("Frequency:"),-1);
    if(vectorskip==1)glutAddMenuEntry(_("*All"),1);
    if(vectorskip!=1)glutAddMenuEntry(_("All"),1);
    if(vectorskip==2)glutAddMenuEntry(_("*Every 2nd"),2);
    if(vectorskip!=2)glutAddMenuEntry(_("Every 2nd"),2);
    if(vectorskip==3)glutAddMenuEntry(_("*Every 3rd"),3);
    if(vectorskip!=3)glutAddMenuEntry(_("Every 3rd"),3);
    if(vectorskip==4)glutAddMenuEntry(_("*Every 4th"),4);
    if(vectorskip!=4)glutAddMenuEntry(_("Every 4th"),4);
  }

  if(ntextures_loaded_used>0){
    CREATEMENU(textureshowmenu,TextureShowMenu);
    ntextures_used=0;
    for(i=0;i<ntextures;i++){
      texture *texti;

      texti = textureinfo + i;
      if(texti->loaded==0||texti->used==0)continue;
      ntextures_used++;
      if(texti->display==1){
        STRCPY(menulabel,check);
        STRCAT(menulabel,texti->file);  
        glutAddMenuEntry(menulabel,i);
      }
      else{
        STRCPY(menulabel,texti->file);
        glutAddMenuEntry(menulabel,i);
      }
    }
    if(ntextures_used>1){
      glutAddMenuEntry("-",-999);
      glutAddMenuEntry(_("Show all"),-1);
      glutAddMenuEntry(_("Hide all"),-2);
    }
  }

/* --------------------------------static slice menu -------------------------- */
  if(nplot3dinfo>0){
    CREATEMENU(staticslicemenu,Plot3DShowMenu);
    glutAddSubMenu(_("Solution variable"),staticvariablemenu);
    if(visz_all==1)glutAddMenuEntry(_("*xy plane"),1);
    if(visz_all==0)glutAddMenuEntry(_("xy plane"),1);
    if(visy_all==1)glutAddMenuEntry(_("*xz plane"),2);
    if(visy_all==0)glutAddMenuEntry(_("xz plane"),2);
    if(visx_all==1)glutAddMenuEntry(_("*yz plane"),3);
    if(visx_all==0)glutAddMenuEntry(_("yz plane"),3);
    if(vectorspresent==1)glutAddSubMenu(_("Flow vectors"),vectorskipmenu);
    if(contour_type==SHADED_CONTOURS){
      glutAddMenuEntry(_("*Continuous contours"),4);
    }
    if(contour_type!=SHADED_CONTOURS){
      glutAddMenuEntry(_("Continuous contours"),4);
    }
    glutAddMenuEntry(_("Show all planes"),5);
    glutAddMenuEntry(_("Hide all planes"),6);

    CREATEMENU(plot3dshowmenu,Plot3DShowMenu);
    if(nplot3dloaded>0){
      for(ii=0;ii<nplot3dinfo;ii++){
        plot3ddata *plot3di;

        i=plot3dorderindex[ii];
        plot3di = plot3dinfo + i;
        if(ii==0){
          glutAddMenuEntry(plot3di->longlabel,997);
        }
        if(ii!=0&&strcmp(plot3di->longlabel,plot3dinfo[plot3dorderindex[ii-1]].longlabel)!=0){
          glutAddMenuEntry(plot3di->longlabel,997);
        }
        if(plot3di->loaded==0)continue;
        if(plotstate==STATIC_PLOTS&&plot3di->display==1){
          STRCPY(menulabel,check);
          STRCAT(menulabel,plot3di->menulabel);  
        }
        else{
          STRCPY(menulabel,plot3di->menulabel);
        }
        glutAddMenuEntry(menulabel,1000+i);
      }
      if(nplot3dloaded>1){
        glutAddMenuEntry("-",997);
        glutAddMenuEntry(_("Show all PLOT3D files"),SHOWALL_PLOT3D);
        glutAddMenuEntry(_("Hide all PLOT3D files"),HIDEALL_PLOT3D);
      }
      glutAddMenuEntry("-",997);
    }
    glutAddSubMenu(_("2D contours"),staticslicemenu);
    if(unload_qdata==0){
      glutAddSubMenu(_("3D contours"),isosurfacemenu);
    }

  }

/* --------------------------------grid slice menu -------------------------- */

  CREATEMENU(gridslicemenu,GridSliceMenu);
  if(visGrid==GridnoProbe||visGrid==GridProbe){
    glutAddMenuEntry(_("*show grid"),GRID_grid); 
  }
  else{
    glutAddMenuEntry(_("show grid"),GRID_grid); 
  }
  if(visGrid==GridProbe||visGrid==noGridProbe){
    glutAddMenuEntry(_("*show grid location"),GRID_probe); 
  }
  else{
    glutAddMenuEntry(_("show grid location"),GRID_probe); 
  }
  glutAddMenuEntry("-",999);
  if(visz_all==1){
    glutAddMenuEntry(_("*xy plane"),GRID_xy);
  }
  else{
    glutAddMenuEntry(_("xy plane"),GRID_xy);
  }
  if(visy_all==1){
    glutAddMenuEntry(_("*xz plane"),GRID_xz);
  }
  else{
    glutAddMenuEntry(_("xz plane"),GRID_xz);
  }
  if(visx_all==1){
    glutAddMenuEntry(_("*yz plane"),GRID_yz);
  }
  else{
    glutAddMenuEntry(_("yz plane"),GRID_yz);
  }
  if(visx_all==0||visy_all==0||visz_all==0){
    glutAddMenuEntry(_("Show all planes"),GRID_showall);
  }
  if(visx_all==1||visy_all==1||visz_all==1){
    glutAddMenuEntry(_("Hide all planes"),GRID_hideall);
  }

/* --------------------------------vent menu -------------------------- */

  CREATEMENU(ventmenu,VentMenu);
  {
    int ntotal_vents=0;

    for(i=0;i<nmeshes;i++){
      mesh *meshi;

      meshi=meshinfo+i;
      ntotal_vents+=meshi->nvents;
    }
    if(ntotal_vents>0){
      if(nopenvents>0){
        if(visOpenVents==1)glutAddMenuEntry(_("*Open"),14);
        if(visOpenVents==0)glutAddMenuEntry(_("Open"),14);
      }
      if(ndummyvents>0){
        if(visDummyVents==1)glutAddMenuEntry(_("*Exterior"),16);
        if(visDummyVents==0)glutAddMenuEntry(_("Exterior"),16);
      }
      if(ntotal_vents>nopenvents+ndummyvents){
        if(visOtherVents==1)glutAddMenuEntry(_("*Other"),21);
        if(visOtherVents==0)glutAddMenuEntry(_("Other"),21);
      }
      if(visOpenVents==1&&visDummyVents==1&&visOtherVents==1){
        glutAddMenuEntry(_("*Show all"),10);
      }
      else{
        glutAddMenuEntry(_("Show all"),10);
      }
      if(visOpenVents==0&&visDummyVents==0&&visOtherVents==0){
        glutAddMenuEntry(_("*Hide all"),22);
      }
      else{
        glutAddMenuEntry(_("Hide all"),22);
      }
      glutAddMenuEntry("-",-1);
      if(nopenvents_nonoutline>0){
        if(visOpenVentsAsOutline==1)glutAddMenuEntry(_("*Open vents as outlines"),15);
        if(visOpenVentsAsOutline==0)glutAddMenuEntry(_("Open vents as outlines"),15);
      }
      if(have_vents_int==1){
        if(show_bothsides_int==1)glutAddMenuEntry(_("*Two sided (interior)"),18);
        if(show_bothsides_int==0)glutAddMenuEntry(_("Two sided (interior)"),18);
      }
      if(show_bothsides_ext==1)glutAddMenuEntry(_("*Two sided (exterior)"),19);
      if(show_bothsides_ext==0)glutAddMenuEntry(_("Two sided (exterior)"),19);
      if(nvent_transparent>0){
        if(show_transparent_vents==1)glutAddMenuEntry(_("*Transparent"),20);
        if(show_transparent_vents==0)glutAddMenuEntry(_("Transparent"),20);
      }
    }
  }
  
/* --------------------------------terrain_showmenu -------------------------- */

  CREATEMENU(terrain_showmenu,GeometryMenu);
  if(visTerrainType==TERRAIN_3D)glutAddMenuEntry(_("*3D surface"),17+TERRAIN_3D);
  if(visTerrainType!=TERRAIN_3D)glutAddMenuEntry(_("3D surface"),17+TERRAIN_3D);
  if(visTerrainType==TERRAIN_2D_STEPPED)glutAddMenuEntry(_("*2D stepped"),17+TERRAIN_2D_STEPPED);
  if(visTerrainType!=TERRAIN_2D_STEPPED)glutAddMenuEntry(_("2D stepped"),17+TERRAIN_2D_STEPPED);
  if(visTerrainType==TERRAIN_2D_LINE)glutAddMenuEntry(_("*2D lines"),17+TERRAIN_2D_LINE);
  if(visTerrainType!=TERRAIN_2D_LINE)glutAddMenuEntry(_("2D lines"),17+TERRAIN_2D_LINE);
  if(terrain_texture!=NULL&&terrain_texture->loaded==1){
    if(visTerrainType==TERRAIN_3D_MAP)glutAddMenuEntry(_("*Image"),17+TERRAIN_3D_MAP);
    if(visTerrainType!=TERRAIN_3D_MAP)glutAddMenuEntry(_("Image"),17+TERRAIN_3D_MAP);
  }
  if(visTerrainType==TERRAIN_HIDDEN)glutAddMenuEntry(_("*Hidden"),17+TERRAIN_HIDDEN);
  if(visTerrainType!=TERRAIN_HIDDEN)glutAddMenuEntry(_("Hidden"),17+TERRAIN_HIDDEN);
    
  if(nobject_defs>0){
    multiprop=0;
    for(i=0;i<npropinfo;i++){
      propdata *propi;

      propi = propinfo + i;
      if(propi->nsmokeview_ids>1)multiprop=1;
    }
    if(multiprop==1){
      for(i=0;i<npropinfo;i++){
        propdata *propi;

        propi = propinfo + i;
        CREATEMENU(propi->menu_id,PropMenu);
        if(propi->nsmokeview_ids>1){
          int jj;

          for(jj=0;jj<propi->nsmokeview_ids;jj++){
            strcpy(menulabel,"");
            if(propi->smokeview_ids[jj]==propi->smokeview_id){
              strcat(menulabel,"*");
            }
            strcat(menulabel,propi->smokeview_ids[jj]);
            glutAddMenuEntry(menulabel,jj*npropinfo+i);
          }
        }
      }
      CREATEMENU(propmenu,PropMenu);
      for(i=0;i<npropinfo;i++){
        propdata *propi;

        propi = propinfo + i;
        if(propi->nsmokeview_ids>1){
          glutAddSubMenu(propi->label,propi->menu_id);
        }
      }
    }

    CREATEMENU(spheresegmentmenu,ShowObjectsMenu);
    if(device_sphere_segments==6){
      glutAddMenuEntry("   *6",-6);
    }
    else{
      glutAddMenuEntry("   6",-6);
    }
    if(device_sphere_segments==12){
      glutAddMenuEntry("   *12",-12);
    }
    else{
      glutAddMenuEntry("   12",-12);
    }
    if(device_sphere_segments==24){
      glutAddMenuEntry("   *24",-24);
    }
    else{
      glutAddMenuEntry("   24",-24);
    }
    if(device_sphere_segments==48){
      glutAddMenuEntry("   *48",-48);
    }
    else{
      glutAddMenuEntry("   48",-48);
    }
  }

  if(nobject_defs>0){
    CREATEMENU(showobjectsmenu,ShowObjectsMenu);
    for(i=0;i<nobject_defs;i++){
      sv_object *obj_typei;

      obj_typei = object_defs[i];
      obj_typei->used_by_device=0;
    }
    for(i=0;i<ndeviceinfo;i++){
      devicedata *devicei;
      propdata *propi;
      int jj;

      devicei = deviceinfo + i;
      propi = devicei->prop;
      if(propi==NULL)continue;
      for(jj=0;jj<propi->nsmokeview_ids;jj++){
        sv_object *objectj;

        objectj = propi->smv_objects[jj];
        objectj->used_by_device=1;
      }
    }
    for(i=0;i<npart5prop;i++){
      part5prop *partpropi;

      partpropi = part5propinfo + i;
      for(j=0;j<npartclassinfo;j++){
        part5class *partclassj;
        propdata *propi;
        int jj;

        if(partpropi->class_present[j]==0)continue;
        partclassj = partclassinfo + j;
        propi = partclassj->prop;
        if(propi==NULL)continue;
        for(jj=0;jj<propi->nsmokeview_ids;jj++){
          sv_object *objectj;

          objectj = propi->smv_objects[jj];
          objectj->used_by_device=1;
        }
      }
    }

    for(i=0;i<nobject_defs;i++){
      sv_object *obj_typei;

      obj_typei = object_defs[i];
      if(obj_typei->used_by_device==1){
        char obj_menu[256];

        strcpy(obj_menu,"");
        if(obj_typei->visible==1){
          strcat(obj_menu,"*");
        }
        strcat(obj_menu,obj_typei->label);
        glutAddMenuEntry(obj_menu,i);
      }
    }
    glutAddMenuEntry("-",-999);
    if(ndeviceinfo>0){
      if(select_device==1){
        glutAddMenuEntry(_("*Select"),-3);
      }
      else{
        glutAddMenuEntry(_("Select"),-3);
      }
    }
    if(object_outlines==0)glutAddMenuEntry(_("Outline"),-4);
    if(object_outlines==1)glutAddMenuEntry(_("*Outline"),-4);
    glutAddMenuEntry(_("Show all"),-1);
    glutAddMenuEntry(_("Hide all"),-2);
    glutAddMenuEntry("-",-999);
    glutAddSubMenu(_("Segments"),spheresegmentmenu);

  }

/* --------------------------------geometry menu -------------------------- */

  CREATEMENU(geometrymenu,GeometryMenu);
  if(showedit_dialog==0&&ntotal_blockages>0)glutAddSubMenu(_("Obstacles"),blockagemenu);
  if(ngeominfo>0)glutAddSubMenu(_("Immersed Solids"),immersedmenu);
  if(nobject_defs>0){
    int num_activedevices=0;

    for(i=0;i<nobject_defs;i++){
      sv_object *obj_typei;

      obj_typei = object_defs[i];
      if(obj_typei->used_by_device==1)num_activedevices++;
    }
    if(num_activedevices>0){
      glutAddSubMenu(_("Objects"),showobjectsmenu);
    }
  }
    //shaded 17 0
    //stepped 18 1
    //line    19 2
    //texture 20 3
    //hidden 21 4
  if(nterraininfo>0){
    glutAddSubMenu(_("Terrain"),terrain_showmenu);
  }
 glutAddSubMenu(_("Surfaces"),ventmenu);
 if(ntotal_blockages>0||isZoneFireModel==0){
    glutAddSubMenu(_("Grid"),gridslicemenu);
  }
  if(isZoneFireModel==0){
    if(visFrame==1)glutAddMenuEntry(_("*Outline"),3);
    if(visFrame==0)glutAddMenuEntry(_("Outline"),3);
  }
  else{
    visFrame=0;
  }
  glutAddMenuEntry(_("Show all"),11);
  glutAddMenuEntry(_("Hide all"),13);

/* --------------------------------title menu -------------------------- */

  ntitles=1;
  if(strlen(TITLE1)!=0)ntitles++;
  if(strlen(TITLE2)!=0)ntitles++;
  if(ntitles>1){
    CREATEMENU(titlemenu,TitleMenu);
    if(visTitle0==1)glutAddMenuEntry(_("*Default"),0);
    if(visTitle0==0)glutAddMenuEntry(_("Default"),0);
    if(strlen(TITLE1)!=0){
      if(visTitle1==1){glutAddMenuEntry(_("*User title 1"),1);}
      if(visTitle1==0){glutAddMenuEntry(_("User title 1"),1);}
    }
    if(strlen(TITLE2)!=0){
      if(visTitle2==1){glutAddMenuEntry(_("*User title 2"),2);}
      if(visTitle2==0){glutAddMenuEntry(_("User title 2"),2);}
    }
    glutAddMenuEntry(_("Show all"),3);
    glutAddMenuEntry(_("Hide all"),4);
  }

/* --------------------------------label menu -------------------------- */

  CREATEMENU(labelmenu,LabelMenu);
  if(visColorbarLabels==1)glutAddMenuEntry(_("*Colorbars"),0);
  if(visColorbarLabels==0)glutAddMenuEntry(_("Colorbars"),0);
  if(visTimeLabels==1)glutAddMenuEntry(_("*Time bars"),1);
  if(visTimeLabels==0)glutAddMenuEntry(_("Time bars"),1);
  if(nticks>0){
    if(visTicks==0)glutAddMenuEntry(_("FDS generated ticks"),12);
    if(visTicks==1)glutAddMenuEntry(_("*FDS generated ticks"),12);
  }
  if(vis_user_ticks==1)glutAddMenuEntry(_("*User settable ticks"),18);
  if(vis_user_ticks==0)glutAddMenuEntry(_("User settable ticks"),18);

  if(ntitles>1){
    glutAddSubMenu(_("Titles"),titlemenu);
  }
  else{
    if(visTitle==1)glutAddMenuEntry(_("*Title"),2);
    if(visTitle==0)glutAddMenuEntry(_("Title"),2);
  }
  if(visaxislabels==1)glutAddMenuEntry(_("*Axis"),6);
  if(visaxislabels==0)glutAddMenuEntry(_("Axis"),6);
  if(visFramerate==1)glutAddMenuEntry(_("*Frame rate"),3);
  if(visFramerate==0)glutAddMenuEntry(_("Frame rate"),3);
  if(visTimelabel==1)glutAddMenuEntry(_("*Time label"),8);
  if(visTimelabel==0)glutAddMenuEntry(_("Time label"),8);
  if(visFramelabel==1)glutAddMenuEntry(_("*Frame label"),9);
  if(visFramelabel==0)glutAddMenuEntry(_("Frame label"),9);
  if(hrrinfo!=NULL){
    if(visHRRlabel==1)glutAddMenuEntry(_("*HRR label"),16);
    if(visHRRlabel==0)glutAddMenuEntry(_("HRR label"),16);
  }
  if(show_hrrcutoff_active==1){
    if(show_hrrcutoff==1)glutAddMenuEntry(_("*HRRPUV cutoff"),17);
    if(show_hrrcutoff==0)glutAddMenuEntry(_("HRRPUV cutoff"),17);
  }
  if(vis_slice_average==1)glutAddMenuEntry(_("*Slice average"),15);
  if(vis_slice_average==0)glutAddMenuEntry(_("Slice average"),15);
  if(nmeshes>1){
    if(visBlocklabel==1)glutAddMenuEntry(_("*Mesh label"),10);
    if(visBlocklabel==0)glutAddMenuEntry(_("Mesh label"),10);
  }
#ifdef pp_memstatus
  if(visAvailmemory==1)glutAddMenuEntry(_("*Memory load"),11);
  if(visAvailmemory==0)glutAddMenuEntry(_("Memory load"),11);
#endif
#ifdef pp_MEMDEBUG
  if(visUsagememory==1)glutAddMenuEntry(_("*Memory usage"),19);
  if(visUsagememory==0)glutAddMenuEntry(_("Memory usage"),19);
#endif
  if(nlabels>0){
    if(visLabels==1)glutAddMenuEntry(_("*Text labels"),7);
    if(visLabels==0)glutAddMenuEntry(_("Text labels"),7);
  }
  if(ntotal_blockages>0||isZoneFireModel==0){
    if(visgridloc==1)glutAddMenuEntry(_("*Grid locations"),14);
    if(visgridloc==0)glutAddMenuEntry(_("Grid locations"),14);
  }

  glutAddMenuEntry(_("Show all"),4);
  glutAddMenuEntry(_("Hide all"),5);

/* --------------------------------rotate type menu -------------------------- */

  CREATEMENU(rotatetypemenu,RotateTypeMenu);
  glutAddMenuEntry("Scene centered:",999);
  switch (rotation_type){
  case EYE_CENTERED:
    glutAddMenuEntry("  2 axis",ROTATION_2AXIS);
    glutAddMenuEntry("  Level (1 axis)",ROTATION_1AXIS);
    glutAddMenuEntry("  3 axis",ROTATION_3AXIS);
    glutAddMenuEntry("*Eye centered",EYE_CENTERED);
    break;
  case ROTATION_2AXIS:
    glutAddMenuEntry("  *2 axis",ROTATION_2AXIS);
    glutAddMenuEntry("  Level (1 axis)",ROTATION_1AXIS);
    glutAddMenuEntry("  3 axis",ROTATION_3AXIS);
    glutAddMenuEntry("Eye centered",EYE_CENTERED);
    break;
  case ROTATION_1AXIS:
    glutAddMenuEntry("  2 axis",ROTATION_2AXIS);
    glutAddMenuEntry("  *Level (1 axis)",ROTATION_1AXIS);
    glutAddMenuEntry("  3 axis",ROTATION_3AXIS);
    glutAddMenuEntry("Eye centered",EYE_CENTERED);
    break;
  case ROTATION_3AXIS:
    glutAddMenuEntry("  2 axis",ROTATION_2AXIS);
    glutAddMenuEntry("  Level (1 axis)",ROTATION_1AXIS);
    glutAddMenuEntry("  *3 axis",ROTATION_3AXIS);
    glutAddMenuEntry("Eye centered",EYE_CENTERED);
    break;
  default:
    ASSERT(FFALSE);
    break;
  }

/* --------------------------------zone show menu -------------------------- */

  if(nzoneinfo>0){
    CREATEMENU(zoneshowmenu,ZoneShowMenu);
    glutAddMenuEntry(_("Layers"),999);
    if(ReadZoneFile==1){
      glutAddMenuEntry(_("  2D smoke colored by:"),999);
      switch (sethazardcolor){
        case 0:
          glutAddMenuEntry(_("    *Temperature"),6);
          glutAddMenuEntry(_("    hazard"),5);
          break;
        case 1:
          glutAddMenuEntry(_("    Temperature"),6);
          glutAddMenuEntry(_("    *hazard"),5);
          break;
        case 2:
          glutAddMenuEntry(_("    Temperature"),6);
          glutAddMenuEntry(_("    hazard"),5);
          break;
        default:
          ASSERT(0);
          break;
      }
      if(sethazardcolor!=2){
        glutAddMenuEntry(_("  2D smoke drawn:"),999);
        if(visHZone==1)glutAddMenuEntry(_("    *horizontally"),1);
        if(visHZone==0)glutAddMenuEntry(_("    horizontally"),1);
        if(visVZone==1)glutAddMenuEntry(_("    *vertically"),2);
        if(visVZone==0)glutAddMenuEntry(_("    vertically"),2);
        }
      switch (sethazardcolor){
        case 0:
        case 1:
          glutAddMenuEntry(_("  3D smoke"),7);
          break;
        case 2:
          glutAddMenuEntry(_("  *3D smoke"),7);
          break;
        default:
          ASSERT(0);
          break;
      }
      if(visZone==0){
        glutAddMenuEntry(_("  *Hide"),4);
      }
      else{
        glutAddMenuEntry(_("  Hide"),4);
      }
    }
    if(nzvents>0){
      if(visVents==1){
        glutAddMenuEntry(_("*Vents"),14);
      }
      else{
        glutAddMenuEntry(_("Vents"),14);
      }
      if(ReadZoneFile==1){
        if(visVentSolid==1){
          glutAddMenuEntry(_("   *Solid"),11);
        }
        else{
          glutAddMenuEntry(_("   Solid"),11);
        }
        if(visVentLines==1){
          glutAddMenuEntry(_("   *Lines"),12);
        }
        else{
          glutAddMenuEntry(_("   Lines"),12);
        }
        if(visVentSolid==0&&visVentLines==0){
          glutAddMenuEntry(_("   *Hide"),13);
        }
        else{
          glutAddMenuEntry(_("   Hide"),13);
        }
      }

    }
    if(nfires>0){
      if(viszonefire==1){
        glutAddMenuEntry(_("*Fires"),15);
      }
      else{
        glutAddMenuEntry(_("Fires"),15);
      }
    }
  }

  /* --------------------------------particle class show menu -------------------------- */

  if(npartclassinfo>0){
    int ntypes;

    CREATEMENU(particlestreakshowmenu,ParticleStreakShowMenu);
    {
      int iii;
      char streaklabel[1024];

      for(iii=0;iii<nstreak_value;iii++){
        if(iii==streak_index){
          sprintf(streaklabel,"*%f",streak_rvalue[iii]);
        }
        else{
          sprintf(streaklabel,"%f",streak_rvalue[iii]);
        }
        trimzeros(streaklabel);
        strcat(streaklabel," s");
        glutAddMenuEntry(streaklabel,iii);
      }
    }
    glutAddMenuEntry("-",-1);
    if(showstreakhead==1){
      glutAddMenuEntry(_("*Particle head"),-3);
    }
    else{
      glutAddMenuEntry(_("Particle head"),-3);
    }
    glutAddMenuEntry(_("Hide"),-2);

// allocate memory for particle property sub-menus

    if(npart5prop*npartclassinfo>0){
      NewMemory((void **)&particlepropshowsubmenu,npart5prop*npartclassinfo*sizeof(int));
    }

      ntypes=0;
      for(i=0;i<npart5prop;i++){
        part5prop *propi;

        propi = part5propinfo + i;
        if(propi->display==0)continue;
        for(j=0;j<npartclassinfo;j++){
          part5class *partclassj;

          if(propi->class_present[j]==0)continue;
          partclassj = partclassinfo + j;
          CREATEMENU(particlepropshowsubmenu[ntypes],ParticlePropShowMenu);
          ntypes++;
          if(propi->class_vis[j]==1){
            strcpy(menulabel,"  *");
          }
          else{
            strcpy(menulabel,"  ");
          }
          strcat(menulabel,partclassj->name);
          if(partclassj->col_diameter>=0||partclassj->col_length>=0||partclassj->device_name!=NULL||
             (partclassj->prop!=NULL&&partclassj->prop->smokeview_id!=NULL)||
             (partclassj->col_u_vel>=0&&partclassj->col_v_vel>=0&&partclassj->col_w_vel>=0)
            ){
            if(propi->class_vis[j]==1){
              strcpy(menulabel,_("using:"));
            }
            else{
              strcpy(menulabel,_("using:"));
            }
            glutAddMenuEntry(menulabel,-10-5*j);
            if(partclassj->kind!=HUMANS){
              if(partclassj->vis_type==PART_POINTS){
                glutAddMenuEntry(_("    *points"),-10-5*j-PART_POINTS);
              }
              else{
                glutAddMenuEntry(_("    points"),-10-5*j-PART_POINTS);
              }
              if(partclassj->col_diameter>=0||partclassj->device_name!=NULL){
                if(partclassj->vis_type==PART_SPHERES){
                  glutAddMenuEntry(_("    *spheres"),-10-5*j-PART_SPHERES);
                }
                else{
                  glutAddMenuEntry(_("    spheres"),-10-5*j-PART_SPHERES);
                }
              }
              if(partclassj->col_length>=0||partclassj->device_name!=NULL||
                (partclassj->col_u_vel>=0&&partclassj->col_v_vel>=0&&partclassj->col_w_vel>=0)){
                if(partclassj->vis_type==PART_LINES){
                  glutAddMenuEntry(_("    *Lines"),-10-5*j-PART_LINES);
                }
                else{
                  glutAddMenuEntry(_("    Lines"),-10-5*j-PART_LINES);
                }
              }
            }
            if(partclassj->smv_device!=NULL&&partclassj->device_name!=NULL||
              (partclassj->prop!=NULL&&partclassj->prop->smokeview_id!=NULL)
              ){
              if(partclassj->device_name!=NULL){
                strcpy(menulabel,"    "); 
                if(partclassj->vis_type==PART_SMV_DEVICE){
                  strcat(menulabel,"*");
                }
                strcat(menulabel,partclassj->device_name);
                glutAddMenuEntry(menulabel,-10-5*j-PART_SMV_DEVICE);
              }
              else if(partclassj->prop!=NULL&&partclassj->prop->smokeview_id!=NULL){
                int iii;
                propdata *propclass;

              // value = iobject*npropinfo + iprop
                propclass=partclassj->prop;
                for(iii=0;iii<propclass->nsmokeview_ids;iii++){
                  int propvalue, showvalue, menuvalue;

                  propvalue = iii*npropinfo + (propclass-propinfo);
                  showvalue = -10-5*j-PART_SMV_DEVICE;
                  menuvalue = (-1-propvalue)*10000 + showvalue;
                  // propvalue = (-menuvalue)/10000-1;
                  // showvalue = -((-menuvalue)%10000)
                  strcpy(menulabel,"    "); 
                  if(partclassj->vis_type==PART_SMV_DEVICE&&propclass->smokeview_ids[iii]==propclass->smokeview_id){
                    strcat(menulabel,"*");
                  }
                  strcat(menulabel,propclass->smokeview_ids[iii]);
                  glutAddMenuEntry(menulabel,menuvalue);
                }
              }
            }
          }
          else{
            glutAddMenuEntry(menulabel,-10-5*j);
          }
        }
      }

    CREATEMENU(particlepropshowmenu,ParticlePropShowMenu);
    if(npart5prop>=0){
      glutAddMenuEntry(_("Color with:"),-1);
      for(i=0;i<npart5prop;i++){
        part5prop *propi;

        propi = part5propinfo + i;
        if(propi->particle_property==0)continue;
        if(propi->display==1){
          strcpy(menulabel,"  *");
        }
        else{
          strcpy(menulabel,"  ");
        }
        strcat(menulabel,propi->label->longlabel);
        glutAddMenuEntry(menulabel,i);
      }
    
      if(part5show==0)glutAddMenuEntry(_("  *Hide"),-4);
      if(part5show==1)glutAddMenuEntry(_("  Hide"),-4);
      glutAddMenuEntry("-",-1);

      glutAddMenuEntry(_("Draw"),-1);
      ntypes=0;
      for(i=0;i<npart5prop;i++){
        part5prop *propi;

        propi = part5propinfo + i;
        if(propi->display==0)continue;
        for(j=0;j<npartclassinfo;j++){
          part5class *partclassj;

          if(propi->class_present[j]==0)continue;
          partclassj = partclassinfo + j;
          if(partclassj->kind==HUMANS)continue;
          ntypes++;
          if(propi->class_vis[j]==1){
            strcpy(menulabel,"  *");
          }
          else{
            strcpy(menulabel,"  ");
          }
          strcat(menulabel,partclassj->name);
          glutAddSubMenu(menulabel,particlepropshowsubmenu[ntypes-1]);
        }
      }

      if(ntypes>1){
        glutAddMenuEntry(_("  Show all"),-2);
        glutAddMenuEntry(_("  Hide all"),-3);
      }
      glutAddMenuEntry("-",-1);
      if(streak5show==1){
        glutAddSubMenu(_("*Streaks"),particlestreakshowmenu);
      }
      else{
        glutAddSubMenu(_("Streaks"),particlestreakshowmenu);
      }
      glutAddMenuEntry("-",-1);
      if(show_tracers_always==0)glutAddMenuEntry(_("Show tracers always"),-6);
      if(show_tracers_always==1)glutAddMenuEntry(_("*Show tracers always"),-6);
    }

    CREATEMENU(humanpropshowmenu,ParticlePropShowMenu);
    if(npart5prop>=0){
      glutAddMenuEntry(_("Color with:"),-1);
      for(i=0;i<npart5prop;i++){
        part5prop *propi;

        propi = part5propinfo + i;
        if(propi->human_property==0)continue;
        if(propi->display==1){
          strcpy(menulabel,"  *");
        }
        else{
          strcpy(menulabel,"  ");
        }
        strcat(menulabel,propi->label->longlabel);
        glutAddMenuEntry(menulabel,i);
      }
    
      if(part5show==0)glutAddMenuEntry(_("  *Hide"),-5);
      if(part5show==1)glutAddMenuEntry(_("  Hide"),-5);
      glutAddMenuEntry("-",-1);
      glutAddMenuEntry(_("Draw"),-1);
      ntypes=0;
      for(i=0;i<npart5prop;i++){
        part5prop *propi;

        propi = part5propinfo + i;
        if(propi->display==0)continue;
        for(j=0;j<npartclassinfo;j++){
          part5class *partclassj;

          if(propi->class_present[j]==0)continue;
          partclassj = partclassinfo + j;
          if(partclassj->kind!=HUMANS)continue;
          ntypes++;
          if(propi->class_vis[j]==1){
            strcpy(menulabel,"  *");
          }
          else{
            strcpy(menulabel,"  ");
          }
          strcat(menulabel,partclassj->name);
          glutAddSubMenu(menulabel,particlepropshowsubmenu[ntypes-1]);
        }
        //break;
      }
      if(ntypes>1){
        glutAddMenuEntry(_("  Show all"),-2);
        glutAddMenuEntry(_("  Hide all"),-3);
      }
      glutAddMenuEntry("-",-1);
      if(streak5show==1){
        glutAddSubMenu(_("  *Streaks"),particlestreakshowmenu);
      }
      else{
        glutAddSubMenu(_("  Streaks"),particlestreakshowmenu);
      }
    }
  }

/* --------------------------------particle show menu -------------------------- */

  if(npartinfo>0&&nevac!=npartinfo){
    CREATEMENU(particleshowmenu,ParticleShowMenu);
    for(ii=0;ii<npartinfo;ii++){
      partdata *parti;

      i = partorderindex[ii];
      parti = partinfo + i;
      if(parti->loaded==0)continue;
      if(parti->evac==1)continue;
      STRCPY(menulabel,"");
      if(parti->display==1)STRCAT(menulabel,"*");
      STRCAT(menulabel,parti->menulabel);
      glutAddMenuEntry(menulabel,-1-i);
    }
    glutAddMenuEntry("-",999);
    if(plotstate==DYNAMIC_PLOTS&&visSmokePart!=0){
      if(visSmokePart==2)glutAddMenuEntry(_("*Particles"),1);
      if(visSmokePart==1)glutAddMenuEntry(_("#Particles"),1);
    }
    else{
      glutAddMenuEntry(_("Particles"),1);
    }
    if(staticframe0==1){
      if(visStaticSmoke==1){
        glutAddMenuEntry(_("*Particles (static)"),5);
      }
      else{
        glutAddMenuEntry(_("Particles (static)"),5);
      }
    }
    if(havesprinkpart==1){
      if(plotstate==DYNAMIC_PLOTS&&visSprinkPart==1){
        glutAddMenuEntry(_("*Droplets"),2);
      }
      else{
        glutAddMenuEntry(_("Droplets"),2);
      }
    }
    showall=0;
    if(plotstate==DYNAMIC_PLOTS){
      if(visSprinkPart==1&&visSmokePart!=0)showall=1;
      if(staticframe0==1&&visStaticSmoke==0)showall=0;
    }
    glutAddMenuEntry("-",999);
    if(showall==1){
      glutAddMenuEntry(_("*Show all"),3);
    }
    else{
      glutAddMenuEntry(_("Show all"),3);
    }
    if(plotstate==DYNAMIC_PLOTS){
      hideall=1;
      if(visSmokePart!=0)hideall=0;
      if(havesprinkpart==1&&visSprinkPart==1)hideall=0;
      if(staticframe0==1&&visStaticSmoke==1)hideall=0;
      if(hideall==1){
        glutAddMenuEntry(_("*Hide all"),4);
      }
      else{
        glutAddMenuEntry(_("Hide all"),4);
      }
    }
  }


/* --------------------------------Evac show menu -------------------------- */

  if(nevac>0){
    CREATEMENU(evacshowmenu,EvacShowMenu);
    for(ii=0;ii<npartinfo;ii++){
      partdata *parti;

      i = partorderindex[ii];
      parti = partinfo + i;
      if(parti->loaded==0)continue;
      if(parti->evac==0)continue;
      STRCPY(menulabel,"");
      if(parti->display==1)STRCAT(menulabel,"*");
      STRCAT(menulabel,parti->menulabel);
      glutAddMenuEntry(menulabel,-1-i);
    }
    glutAddMenuEntry("-",999);
    glutAddMenuEntry(_("Show all"),3);
    if(plotstate==DYNAMIC_PLOTS){
      glutAddMenuEntry(_("Hide all"),4);
    }
  }

/* --------------------------------lighting menu -------------------------- */

  if(showlightmenu==1){
    CREATEMENU(lightingmenu,LightingMenu);
    glutAddMenuEntry(_("OpenGL lighting"),-1);
    if(visLIGHT0==1)glutAddMenuEntry(_("  *Right"),1);
    if(visLIGHT0==0)glutAddMenuEntry(_("  Right"),1);
    if(visLIGHT1==1)glutAddMenuEntry(_("  *Left"),2);
    if(visLIGHT1==0)glutAddMenuEntry(_("  Left"),2);
    glutAddMenuEntry(_("  Flip"),3);
    glutAddMenuEntry(_("  All"),4);
    glutAddMenuEntry(_("  None"),5);
  }


/* -------------------------------- colorbarmenu -------------------------- */
  
  if(nsmoke3dinfo>0&&Read3DSmoke3DFile==1||nvolrenderinfo>0){
    colorbardata *cbi;
    char ccolorbarmenu[256];

    CREATEMENU(smokecolorbarmenu,SmokeColorBarMenu);

    glutAddMenuEntry(_("Smoke map:"),-999);
    for(i=0;i<ncolorbars;i++){
      cbi = colorbarinfo + i;

      strcpy(ccolorbarmenu,"  ");
      if(fire_colorbar_index==i){
        strcat(ccolorbarmenu,"*");
        strcat(ccolorbarmenu,cbi->label);
      }
      else{
        strcat(ccolorbarmenu,cbi->label);
      }
      glutAddMenuEntry(ccolorbarmenu,i);
    }
  }


  /* --------------------------------smoke3d showmenu -------------------------- */
  if(nsmoke3dinfo>0&&Read3DSmoke3DFile==1){
    {
      if(nsmoke3dloaded>0){
        CREATEMENU(smoke3dshowmenu,Smoke3DShowMenu);
        for(i=0;i<nsmoke3dinfo;i++){
          smoke3ddata *smoke3di;

          smoke3di = smoke3dinfo + i;
          if(smoke3di->loaded==0)continue;
          strcpy(menulabel,"");
          if(smoke3di->display==1)strcpy(menulabel,"*");
          strcat(menulabel,smoke3di->menulabel);
          glutAddMenuEntry(menulabel,i);
        }
        glutAddSubMenu(_("Smoke color map"),smokecolorbarmenu);
#ifdef pp_LIGHT
        if(have_lighting==1){
          if(show_smoke_lighting==1)glutAddMenuEntry(_("*Light smoke"),HAVE_LIGHT);
          if(show_smoke_lighting==0)glutAddMenuEntry(_("Light smoke"),HAVE_LIGHT);
        }
#endif
        if(nsmoke3dinfo>1){
          glutAddMenuEntry("-",-999);
          glutAddMenuEntry(_("Show all"),SHOW_ALL);
          glutAddMenuEntry(_("Hide all"),HIDE_ALL);
        }
      }
    }
  }

/* --------------------------------iso level menu -------------------------- */

  if(loaded_isomesh!=NULL&&nisoinfo>0&&ReadIsoFile==1){
    CREATEMENU(isolevelmenu,IsoShowMenu);
    if(loaded_isomesh->nisolevels>0&&loaded_isomesh->showlevels!=NULL){
      showflag=1;
      hideflag=1;
      for(i=0;i<loaded_isomesh->nisolevels;i++){
        if(loaded_isomesh->showlevels[i]==1){
          sprintf(levellabel,"*%f ",loaded_isomesh->isolevels[i]);
          hideflag=0;
        }
        else{
          showflag=0;
          sprintf(levellabel,"%f ",loaded_isomesh->isolevels[i]);
        }
        if(loaded_isomesh->isofilenum!=-1){
          STRCAT(levellabel,isoinfo[loaded_isomesh->isofilenum].surface_label.unit);
        }
        else{
          STRCAT(levellabel,"");
        }
        glutAddMenuEntry(levellabel,100+i);

      }
      if(showflag==1)glutAddMenuEntry(_("*Show all levels"),99);
      if(showflag==0)glutAddMenuEntry(_("Show all levels"),99);
      if(hideflag==1)glutAddMenuEntry(_("*Hide all levels"),98);
      if(hideflag==0)glutAddMenuEntry(_("Hide all levels"),98);
      glutAddMenuEntry("---",93);
      if(transparent_state==ALL_SOLID)glutAddMenuEntry(_("*All levels solid"),94);
      if(transparent_state!=ALL_SOLID)glutAddMenuEntry(_("All levels solid"),94);
      if(transparent_state==ALL_TRANSPARENT)glutAddMenuEntry(_("*All levels transparent"),95);
      if(transparent_state!=ALL_TRANSPARENT)glutAddMenuEntry(_("All levels transparent"),95);
      if(transparent_state==MIN_SOLID)glutAddMenuEntry(_("*Minimum level solid"),96);
      if(transparent_state!=MIN_SOLID)glutAddMenuEntry(_("Minimum level solid"),96);
      if(transparent_state==MAX_SOLID)glutAddMenuEntry(_("*Maximum level solid"),97);
      if(transparent_state!=MAX_SOLID)glutAddMenuEntry(_("Maximum level solid"),97);
    }
    else{
      glutAddMenuEntry(_("Show"),99);
      if(visAIso==0)glutAddMenuEntry(_("*Hide"),98);
      if(visAIso!=0)glutAddMenuEntry(_("Hide"),98);
    }

/* --------------------------------iso show menu -------------------------- */

    if(nisoinfo>0&&ReadIsoFile==1){
      mesh *hmesh;
      isodata *iso2;

      CREATEMENU(isoshowmenu,IsoShowMenu);
      iso2=NULL;
      for(ii=0;ii<nisoinfo;ii++){
        isodata *isoi;

        i = isoorderindex[ii];
        isoi = isoinfo + i;
        if(isoi->loaded==0)continue;
        if(iso2==NULL&&isoi->type==iisotype){
          iso2=isoi;
        }
        if(plotstate==DYNAMIC_PLOTS&&isoi->display==1&&isoi->type==iisotype){
          iso2=isoi;
          STRCPY(menulabel,check);
          STRCAT(menulabel,isoi->menulabel);  
        }
        else{
          STRCPY(menulabel,isoi->menulabel);
        }
        glutAddMenuEntry(menulabel,1000+i);
      }
      if(iso2!=NULL){
        glutAddMenuEntry("-",999);
        STRCPY(menulabel,_("Show all"));
        STRCPY(menulabel," ");
        STRCAT(menulabel,iso2->surface_label.longlabel);
        STRCAT(menulabel," ");
        STRCAT(menulabel,_("isosurfaces"));
        glutAddMenuEntry(menulabel,10001);
        STRCPY(menulabel,_("Hide all"));
        STRCAT(menulabel," ");
        STRCAT(menulabel,iso2->surface_label.longlabel);
        STRCAT(menulabel," ");
        STRCAT(menulabel,_("isosurfaces"));
        glutAddMenuEntry(menulabel,10002);
      }
      glutAddMenuEntry("-",999);
      if((visAIso&1)==1)glutAddMenuEntry(_("*Solid"),1);
      if((visAIso&1)!=1)glutAddMenuEntry(_("Solid"),1);
      if((visAIso&2)==2)glutAddMenuEntry(_("*Outline"),2);
      if((visAIso&2)!=2)glutAddMenuEntry(_("Outline"),2);
      if((visAIso&4)==4)glutAddMenuEntry(_("*Points"),3);
      if((visAIso&4)!=4)glutAddMenuEntry(_("Points"),3);
      hmesh=meshinfo+highlight_mesh;
      if(hmesh->isofilenum!=-1){
        STRCPY(levellabel,isoinfo[hmesh->isofilenum].surface_label.shortlabel);
        STRCAT(levellabel," ");
        STRCAT(levellabel,_("Levels"));
        glutAddSubMenu(levellabel,isolevelmenu);
      }
      if(niso_compressed==0){
        if(smoothtrinormal==1)glutAddMenuEntry(_("*Smooth"),4);
        if(smoothtrinormal==0)glutAddMenuEntry(_("Smooth"),4);
      }
      if(showtrinormal==1)glutAddMenuEntry(_("*Show normals"),5);
      if(showtrinormal==0)glutAddMenuEntry(_("Show normals"),5);
    }
  }


/* -------------------------------- colorbarmenu -------------------------- */

  CREATEMENU(colorbarshademenu,ColorBarMenu);
  if(contour_type==SHADED_CONTOURS){
    glutAddMenuEntry("*Continuous",-17);
    glutAddMenuEntry("Stepped",-18);
    glutAddMenuEntry("Lines",-19);
  }
  else if(contour_type==STEPPED_CONTOURS){
    glutAddMenuEntry("Continuous",-17);
    glutAddMenuEntry("*Stepped",-18);
    glutAddMenuEntry("Lines",-19);
  }else if(contour_type==LINE_CONTOURS){
    glutAddMenuEntry("Continuous",-17);
    glutAddMenuEntry("Stepped",-18);
    glutAddMenuEntry("*Lines",-19);
  }
  glutAddMenuEntry("-",-999);
  if(setbw==0){
    glutAddMenuEntry(_("*Color/BW"),-12);
  }
  else{
    glutAddMenuEntry(_("Color/*BW"),-12);
  }


  CREATEMENU(colorbarsmenu,ColorBarMenu);
  {
    colorbardata *cbi;
    char ccolorbarmenu[256];

    for(i=0;i<ncolorbars;i++){
      cbi = colorbarinfo + i;

      strcpy(ccolorbarmenu,"  ");
      if(colorbartype==i){
        strcat(ccolorbarmenu,"*");
        strcat(ccolorbarmenu,cbi->label);
      }
      else{
        strcat(ccolorbarmenu,cbi->label);
      }
      glutAddMenuEntry(ccolorbarmenu,i);
    }
  }

/* -------------------------------- colorbarmenu -------------------------- */

  CREATEMENU(colorbarmenu,ColorBarMenu);
  glutAddSubMenu("Colorbars",colorbarsmenu);
  glutAddMenuEntry(_("Variations:"),-999);
  if(show_extremedata==1){
    glutAddMenuEntry(_("  *Highlight extreme data"),-7);
  }
  else{
    glutAddMenuEntry(_("  Highlight extreme data"),-7);
  }
  if(colorbarflip==1){
    glutAddMenuEntry(_("  *Flip"),-2);
  }
  else{
    glutAddMenuEntry(_("  Flip"),-2);
  }
  glutAddSubMenu("  Shade Type:",colorbarshademenu);
  if(use_transparency_data==1){
    glutAddMenuEntry(_("  *Transparent (data)"),-13);
  }
  else{
    glutAddMenuEntry(_("  Transparent (data)"),-13);
  }
  glutAddMenuEntry(_("  Reset"),-4);

/* --------------------------------showVslice menu -------------------------- */
  if(nvsliceloaded==0){
    vd_shown=NULL;
  }
  if(nvsliceinfo>0&&nvsliceloaded>0){
    CREATEMENU(showvslicemenu,ShowVSliceMenu);
    nvsliceloaded0=0;
    for(i=0;i<nvsliceinfo;i++){
      vslicedata *vd;
      slicedata *sd;

      vd = vsliceinfo + i;
      if(vd->loaded==0)continue;
      sd = sliceinfo + vd->ival;
      nvsliceloaded0++;
      STRCPY(menulabel,"");
      if(plotstate==DYNAMIC_PLOTS&&sd->type==islicetype&&vd->display==1){
        vd_shown=vd;
        STRCAT(menulabel,"*");
      }
      STRCAT(menulabel,sd->menulabel2);
      if(sd->slicelabel!=NULL){
        STRCAT(menulabel," - ");
        STRCAT(menulabel,sd->slicelabel);
      }
      glutAddMenuEntry(menulabel,i);
    }
    if(show_slice_in_obst==1)glutAddMenuEntry(_("*Show vector slice in blockage"),-11);
    if(show_slice_in_obst==0)glutAddMenuEntry(_("Show vector slice in blockage"),-11);
    if(offset_slice==1)glutAddMenuEntry(_("*Offset vector slice"),-12);
    if(offset_slice==0)glutAddMenuEntry(_("Offset vector slice"),-12);
    if(vd_shown!=NULL&&nvsliceloaded0!=0){
      glutAddMenuEntry("",-10);
      STRCPY(menulabel,_("Show all"));
      STRCAT(menulabel," ");
      STRCAT(menulabel,sliceinfo[vd_shown->ival].label.longlabel);
      STRCAT(menulabel," ");
      STRCAT(menulabel,_("vector slices"));
      glutAddMenuEntry(menulabel,SHOW_ALL);
      STRCPY(menulabel,_("Hide all"));
      STRCAT(menulabel," ");
      STRCAT(menulabel,sliceinfo[vd_shown->ival].label.longlabel);
      STRCAT(menulabel," ");
      STRCAT(menulabel,_("vector slices"));
      glutAddMenuEntry(menulabel,HIDE_ALL);
    }
  }
  if(nsliceinfo>0&&nmultislices<nsliceinfo){
    CREATEMENU(showmultislicemenu,ShowMultiSliceMenu);
    for(i=0;i<nmultislices;i++){
      slicedata *sd;

      mslicei = multisliceinfo + i;
      if(mslicei->loaded==0)continue;
      sd = sliceinfo + mslicei->islices[0];
      STRCPY(menulabel,"");
      if(plotstate==DYNAMIC_PLOTS&&mslicei->display!=0&&mslicei->type==islicetype){
        if(mslicei->display==1){
          STRCAT(menulabel,"*");
        }
        else if(mslicei->display==-1){
          STRCAT(menulabel,"#");
        }
      }
      STRCAT(menulabel,mslicei->menulabel2);
      if(sd->slicelabel!=NULL){
        STRCAT(menulabel," - ");
        STRCAT(menulabel,sd->slicelabel);
      }
      glutAddMenuEntry(menulabel,i);
    }
    if(show_slice_in_obst==1)glutAddMenuEntry(_("*Show multi slice in blockage"),-11);
    if(show_slice_in_obst==0)glutAddMenuEntry(_("Show multi slice in blockage"),-11);
    if(offset_slice==1)glutAddMenuEntry(_("*Offset slice"),-12);
    if(offset_slice==0)glutAddMenuEntry(_("Offset slice"),-12);
    if(show_fed_area==1)glutAddMenuEntry("*Show FED areas",-14);
    if(show_fed_area==0)glutAddMenuEntry("Show FED areas",-14);
  }

/* --------------------------------showslice menu -------------------------- */
  if(nsliceloaded==0){
    sd_shown=NULL;
  }
  if(nsliceinfo>0&&nsliceloaded>0){
    CREATEMENU(showhideslicemenu,ShowHideSliceMenu);
    for(ii=0;ii<nslice_loaded;ii++){
      slicedata *sd;

      i = slice_loaded_list[ii];
      sd = sliceinfo + i;
      if(sd_shown==NULL&&sd->type==islicetype){
        sd_shown = sd;
      }
      STRCPY(menulabel,"");
      if(plotstate==DYNAMIC_PLOTS&&sd->display==1&&sd->type==islicetype){
        sd_shown=sd;
        STRCAT(menulabel,check);
      }
      STRCAT(menulabel,sd->menulabel2);  
      if(sd->slicelabel!=NULL){
        STRCAT(menulabel," - ");
        STRCAT(menulabel,sd->slicelabel);
      }
      glutAddMenuEntry(menulabel,i);
    }
    glutAddMenuEntry("-",-10);
    if(have_terrain_slice()==1){
      if(planar_terrain_slice==1)glutAddMenuEntry(_("*Planar terrain slice"),-13);
      if(planar_terrain_slice==0)glutAddMenuEntry(_("Planar terrain slice"),-13);
    }
    if(show_slice_in_obst==1)glutAddMenuEntry(_("*Show slice in blockage"),-11);
    if(show_slice_in_obst==0)glutAddMenuEntry(_("Show slice in blockage"),-11);
    if(offset_slice==1)glutAddMenuEntry(_("*Offset slice"),-12);
    if(offset_slice==0)glutAddMenuEntry(_("Offset slice"),-12);
    if(show_fed_area==1)glutAddMenuEntry("*Show FED areas",-14);
    if(show_fed_area==0)glutAddMenuEntry("Show FED areas",-14);
    if(nsliceloaded>0&&sd_shown!=NULL){
      glutAddMenuEntry("-",-10);
      STRCPY(menulabel,_("Show all"));
      STRCAT(menulabel," ");
      STRCAT(menulabel,sd_shown->label.longlabel);
      STRCAT(menulabel," ");
      STRCAT(menulabel,_("slices"));
      glutAddMenuEntry(menulabel,SHOW_ALL);
      STRCPY(menulabel,_("Hide all"));
      STRCAT(menulabel," ");
      STRCAT(menulabel,sd_shown->label.longlabel);
      STRCAT(menulabel," ");
      STRCAT(menulabel,_("slices"));
      glutAddMenuEntry(menulabel,HIDE_ALL);
    }
  }

/* -------------------------------- avatartour menu -------------------------- */

  CREATEMENU(avatarevacmenu,AvatarEvacMenu);
  if(navatar_types>0){
    if(iavatar_evac==-1){
      glutAddMenuEntry(_("*Defined in evac file"),-1);
    }
    else{
      glutAddMenuEntry(_("Defined in evac file"),-1);
    }
    glutAddMenuEntry("-",-999);
    for(i=0;i<navatar_types;i++){
      strcpy(menulabel,"");
      if(iavatar_evac==i){
        strcat(menulabel,"*");
      }
      strcat(menulabel,avatar_types[i]->label);
      glutAddMenuEntry(menulabel,i);
    }
  }
  CREATEMENU(avatartourmenu,TourMenu);
  if(navatar_types>0){
    if(selectedtour_index>=0&&selectedtour_index<ntours){
      tourdata *touri;

      touri = tourinfo + selectedtour_index;
      strcpy(menulabel,"For ");
      strcat(menulabel,touri->label);
      glutAddMenuEntry(menulabel,-999);
      glutAddMenuEntry("-",-999);
    }

    for(i=0;i<navatar_types;i++){
      strcpy(menulabel,"");
      if(tourlocus_type==2&&iavatar_types==i){
        strcat(menulabel,"*");
      }
      strcat(menulabel,avatar_types[i]->label);
      glutAddMenuEntry(menulabel,-23-i);
    }
  }

    /* --------------------------------tour menu -------------------------- */

  CREATEMENU(tourmenu,TourMenu);
      
  glutAddMenuEntry(_("New..."),MENU_TOUR_NEW);
  if(ntours>0){
    if(showtour_dialog==1){
      glutAddMenuEntry(_("*Edit..."),MENU_TOUR_EDIT);
    }
    else{
      glutAddMenuEntry(_("Edit..."),MENU_TOUR_EDIT);
    }
    if(ntours>0){
      int use_manual=1;
      
      glutAddMenuEntry("-",-999);
      for(i=0;i<ntours;i++){
        tourdata *touri;

        touri = tourinfo + i;
        if(touri->display==1){
          use_manual=0;
          break;
        }
      }
    }
    for(i=0;i<ntours;i++){
      tourdata *touri;
      int glui_avatar_index_local;

      touri = tourinfo + i;
      if(touri->display==1){
        STRCPY(menulabel,"");
        if(selectedtour_index==i){
          STRCAT(menulabel,"@");
        }
        STRCAT(menulabel,check);
        STRCAT(menulabel,touri->menulabel);  
      }
      else{
        STRCPY(menulabel,touri->menulabel);
      }
      glui_avatar_index_local = touri->glui_avatar_index;
      if(glui_avatar_index_local>=0&&glui_avatar_index_local<navatar_types){
        sv_object *avatari;

        avatari=avatar_types[glui_avatar_index_local];
        strcat(menulabel,"(");
        strcat(menulabel,avatari->label);
        strcat(menulabel,")");
      }
      glutAddMenuEntry(menulabel,i);
    }
    if(selectedtour_index>=0){
      strcpy(menulabel,"");
      if(viewtourfrompath==1)strcat(menulabel,"*");
      strcat(menulabel,"View from ");
      strcat(menulabel,tourinfo[selectedtour_index].label);
      glutAddMenuEntry(menulabel,-5);
    }
    glutAddMenuEntry("-",-999);
    glutAddMenuEntry(_("Show all"),-3);
    glutAddMenuEntry(_("Hide all"),-2);
  }

 /* --------------------------------Show Volume smoke menu -------------------------- */

  if(nvolsmoke3dloaded>0){
    CREATEMENU(showvolsmoke3dmenu,ShowVolSmoke3DMenu);
    if(nvolsmoke3dloaded>1){
      char vlabel[256];

      strcpy(vlabel,_("3D smoke (volume rendered)"));
      strcat(vlabel,_(" - Show all"));
      glutAddMenuEntry(vlabel,SHOW_ALL);

      strcpy(vlabel,_("3D smoke (volume rendered)"));
      strcat(vlabel,_(" - Hide all"));
      glutAddMenuEntry(vlabel,HIDE_ALL);
      glutAddMenuEntry("-",999);
    }
    for(i=0;i<nmeshes;i++){
      mesh *meshi;
      volrenderdata *vr;

      meshi = meshinfo + i;
      vr = &(meshi->volrenderinfo);
      if(vr->fire==NULL||vr->smoke==NULL)continue;
      if(vr->loaded==0)continue;
      strcpy(menulabel,"");
      if(vr->display==1)strcat(menulabel,"*");
      strcat(menulabel,meshi->label);
      glutAddMenuEntry(menulabel,i);
    }
    glutAddMenuEntry("-",-999);
    glutAddSubMenu(_("Smoke color map"),smokecolorbarmenu);
  }

  CREATEMENU(aperturemenu,ApertureMenu);
  if(apertureindex==0)glutAddMenuEntry("*30",0);
  if(apertureindex!=0)glutAddMenuEntry("30",0);
  if(apertureindex==1)glutAddMenuEntry("*45",1);
  if(apertureindex!=1)glutAddMenuEntry("45",1);
  if(apertureindex==2)glutAddMenuEntry("*60",2);
  if(apertureindex!=2)glutAddMenuEntry("60",2);
  if(apertureindex==3)glutAddMenuEntry("*75",3);
  if(apertureindex!=3)glutAddMenuEntry("75",3);
  if(apertureindex==4)glutAddMenuEntry("*90",4);
  if(apertureindex!=4)glutAddMenuEntry("90",4);

  CREATEMENU(zoommenu,ZoomMenu);
  if(zoomindex==0)glutAddMenuEntry("*0.25",0);
  if(zoomindex!=0)glutAddMenuEntry("0.25",0);
  if(zoomindex==1)glutAddMenuEntry("*0.50",1);
  if(zoomindex!=1)glutAddMenuEntry("0.50",1);
  if(zoomindex==2)glutAddMenuEntry("*1.0",2);
  if(zoomindex!=2)glutAddMenuEntry("1.0",2);
  if(zoomindex==3)glutAddMenuEntry("*2.0",3);
  if(zoomindex!=3)glutAddMenuEntry("2.0",3);
  if(zoomindex==4)glutAddMenuEntry("*4.0",4);
  if(zoomindex!=4)glutAddMenuEntry("4.0",4);

  /* --------------------------------reset menu -------------------------- */

  CREATEMENU(resetmenu,ResetMenu);
  {
    char line[256];
    camera *ca;

    if(trainer_mode==1){
      if(visBlocks==visBLOCKOutline){
        glutAddMenuEntry(_("*Outline"),MENU_OUTLINEVIEW);
      }
      else{
        glutAddMenuEntry(_("Outline"),MENU_OUTLINEVIEW);
      }
      glutAddMenuEntry("-",MENU_DUMMY);
    }
    if(trainer_mode==0){
      glutAddMenuEntry(_("Save"),SAVE_VIEWPOINT);
      glutAddMenuEntry(_("Set as Startup"),MENU_STARTUPVIEW);
      glutAddSubMenu(_("Zoom"),zoommenu); //xx
      if(projection_type==1)glutAddMenuEntry(_("Switch to perspective view       ALT+v"),MENU_SIZEPRESERVING);
      if(projection_type==0)glutAddMenuEntry(_("Switch to size preserving view   ALT+v"),MENU_SIZEPRESERVING);
      glutAddMenuEntry("-",MENU_DUMMY);
    }
    for(ca=camera_list_first.next;ca->next!=NULL;ca=ca->next){
      if(trainer_mode==1&&strcmp(ca->name,"internal")==0)continue;
      strcpy(line,"");
      if(ca->view_id==selected_view){
        strcat(line,"*");
      }
      if(trainer_mode==1&&strcmp(ca->name,"external")==0){
        strcat(line,"Outside");
      }
      else{
        strcat(line,ca->name);
      }
      glutAddMenuEntry(line,ca->view_id);
    }
  }
  if(trainer_mode==0&&showtime==1){
    glutAddMenuEntry("-",MENU_DUMMY);
    glutAddMenuEntry(_("Time"),MENU_TIMEVIEW);
  }

  /* --------------------------------showhide menu -------------------------- */

  CREATEMENU(showhidemenu,ShowHideMenu);
  if(ntotal_blockages>0||isZoneFireModel==0){
    glutAddSubMenu(_("Geometry"),geometrymenu);
  }
  glutAddSubMenu(_("Labels"),labelmenu);
  glutAddSubMenu(_("Data coloring"),colorbarmenu);
  glutAddSubMenu(_("Viewpoints"),resetmenu);
  {
    int human_present=0;
    int particle_present=0;

    if(npart5loaded>0){
      for(ii=0;ii<npartinfo;ii++){
        partdata *parti;

        parti = partinfo + ii;
        if(parti->loaded==0)continue;
        if(parti->evac==1)human_present=1;
        if(parti->evac==0)particle_present=1;
      }
      if(particle_present==1){
        glutAddSubMenu(_("Particles"),particlepropshowmenu);
      }
      if(human_present==1){
        glutAddSubMenu(_("Humans"),humanpropshowmenu);
      }
    }
  }
  if(nevac>0&&navatar_types>0){
   // glutAddSubMenu("Use Avatar:",avatarevacmenu);
  }

  if(npart4loaded>0){
    if(havesprinkpart!=0||staticframe0!=0||npartloaded>1){
      glutAddSubMenu(_("Particles"),particleshowmenu);
    }
    else{
      if(ReadPartFile==1&&showsmoke==1)glutAddMenuEntry(_("*Particles"),1);
      if(ReadPartFile==1&&showsmoke==0)glutAddMenuEntry(_("Particles"),1);
    }
  }
  if(partinfo!=NULL&&partinfo[0].version==1){
  }
  else{
    if(nevacloaded>1){
      glutAddSubMenu(_("Evacuation"),evacshowmenu);
    }
    else{
      if(ReadEvacFile==1&&showevac==1)glutAddMenuEntry(_("*Evacuation"),13);
      if(ReadEvacFile==1&&showevac==0)glutAddMenuEntry(_("Evacuation"),13);
    }
  }
  if(ReadIsoFile==1){
    int niso_loaded=0;
    for(i=0;i<nisoinfo;i++){
      isodata *isoi;

      isoi = isoinfo + i;
      if(isoi->loaded==1)niso_loaded++;
    }

    if(niso_loaded>1){
     glutAddSubMenu(_("Animated surfaces"),isoshowmenu);
    }
    else{
     glutAddSubMenu(_("Animated surface"),isoshowmenu);
    }
  }
  if(Read3DSmoke3DFile==1&&nsmoke3dloaded>0){
      glutAddSubMenu(_("3D smoke"),smoke3dshowmenu);
  }
  if(nvolsmoke3dloaded>0){
    char vlabel[256];

    strcpy(vlabel,_("3D smoke (volume rendered)"));
    glutAddSubMenu(vlabel,showvolsmoke3dmenu);
  }

  nvslice0=0, nvslice1=0, nvslice2=0;
  nvsliceloaded0=0, nvsliceloaded1=0;
  nvsliceloaded2=0;
  for(i=0;i<nvsliceinfo;i++){
    vslicedata *vd;

    vd = vsliceinfo+i;
    switch (vd->vec_type){
    case 0:
      nvslice0++;
      if(vd->loaded==1)nvsliceloaded0++;
      break;
    case 1:
      nvslice1++;
      if(vd->loaded==1)nvsliceloaded1++;
      break;
    case 2:
      nvslice2++;
      if(vd->loaded==1)nvsliceloaded1++;
      break;
     default:
      ASSERT(FFALSE);
      break;
    }
  }
  if(nvsliceloaded0+nvsliceloaded1+nvsliceloaded2>0){
    glutAddSubMenu(_("Animated Vector Slices"),showvslicemenu);
  }

  if(nsliceloaded>0){
    glutAddSubMenu(_("Animated Slices"),showhideslicemenu);
    if(nmultislices<nsliceinfo){
      glutAddSubMenu(_("Animated Multi-Slices"),showmultislicemenu);
    }
  }

  if(ReadPlot3dFile==1){
    glutAddSubMenu("Plot3d",plot3dshowmenu);
  }
  if(npatchloaded>0){
    glutAddSubMenu("Boundaries",showpatchmenu);
  }
  if(nzoneinfo>0&&(ReadZoneFile==1||nzvents>0))glutAddSubMenu("Zone",zoneshowmenu);
  if(ReadTargFile==1){
    if(showtarget==1)glutAddMenuEntry(_("*Targets"),2);
    if(showtarget==0)glutAddMenuEntry(_("Targets"),2);
  }
  if(nobject_defs>0){
    int num_activedevices=0;

    if(multiprop==1){
     // glutAddSubMenu("Properties",propmenu);
    }

    for(i=0;i<nobject_defs;i++){
      sv_object *obj_typei;

      obj_typei = object_defs[i];
      if(obj_typei->used==1)num_activedevices++;
    }

    if(num_activedevices>0){
      /*
      if(isZoneFireModel==0||(isZoneFireModel==1&&num_activedevices>1)){
        glutAddSubMenu("Objects",showobjectsmenu);
      }
      */
    }
    else{
      for(i=0;i<nobject_defs;i++){
        sv_object *obj_typei;

        obj_typei = object_defs[i];
        if(obj_typei->used==1){
          char obj_menu[256];

          strcpy(obj_menu,"");
          if(obj_typei->visible==1){
            strcat(obj_menu,"*");
          }
          strcat(obj_menu,obj_typei->label);
          glutAddMenuEntry(obj_menu,i);
        }
      }
    }
  }
  if(ntc_total>0){
    if(isZoneFireModel==1){
      if(visSensor==1)glutAddMenuEntry(_("*Targets"),9);
      if(visSensor==0)glutAddMenuEntry(_("Targets"),9);
      if(hasSensorNorm==1){
        if(visSensorNorm==1)glutAddMenuEntry(_("*Target orientation"),14);
        if(visSensorNorm==0)glutAddMenuEntry(_("Target orientation"),14);
      }
    }
    else{
      if(visSensor==1)glutAddMenuEntry(_("*Thermocouples"),9);
      if(visSensor==0)glutAddMenuEntry(_("Thermocouples"),9);
      if(hasSensorNorm==1){
        if(visSensorNorm==1)glutAddMenuEntry("*Thermocouple norms",14);
        if(visSensorNorm==0)glutAddMenuEntry("Thermocouple norms",14);
      }
    }
  }
  if(titlesafe_offset==0)glutAddMenuEntry(_("Offset window"),12);
  if(titlesafe_offset!=0)glutAddMenuEntry(_("*Offset window"),12);
  if(ntextures_loaded_used>0){
    glutAddSubMenu(_("Textures"),textureshowmenu);
  }
  if(background_flip==1){
    glutAddMenuEntry(_("*Flip background"),15);
  }
  else{
    glutAddMenuEntry(_("Flip background"),15);
  }
#ifdef _DEBUG
  glutAddMenuEntry("Show Memory block info",16);
#endif


/* --------------------------------frame rate menu -------------------------- */

  CREATEMENU(frameratemenu,FrameRateMenu);
  if(frameratevalue==1)glutAddMenuEntry("*1 FPS",1);
  if(frameratevalue!=1)glutAddMenuEntry("1 FPS",1);
  if(frameratevalue==2)glutAddMenuEntry("*2 FPS",2);
  if(frameratevalue!=2)glutAddMenuEntry("2 FPS",2);
  if(frameratevalue==4)glutAddMenuEntry("*4 FPS",4);
  if(frameratevalue!=4)glutAddMenuEntry("4 FPS",4);
  if(frameratevalue==8)glutAddMenuEntry("*8 FPS",8);
  if(frameratevalue!=8)glutAddMenuEntry("8 FPS",8);
  if(frameratevalue==10)glutAddMenuEntry("*10 FPS",10);
  if(frameratevalue!=10)glutAddMenuEntry("10 FPS",10);
  if(frameratevalue==15)glutAddMenuEntry("*15 FPS",15);
  if(frameratevalue!=15)glutAddMenuEntry("15 FPS",15);
  if(frameratevalue==30)glutAddMenuEntry("*30 FPS",30);
  if(frameratevalue!=30)glutAddMenuEntry("30 FPS",30);
  if(frameratevalue==2001)glutAddMenuEntry(_("*Real time"),2001);
  if(frameratevalue!=2001)glutAddMenuEntry(_("Real time"),2001);
  if(frameratevalue==2002)glutAddMenuEntry(_("*2 x Real time"),2002);
  if(frameratevalue!=2002)glutAddMenuEntry(_("2 x Real time"),2002);
  if(frameratevalue==2004)glutAddMenuEntry(_("*4 x Real time"),2004);
  if(frameratevalue!=2004)glutAddMenuEntry(_("4 x Real time"),2004);
  if(frameratevalue!=1000)glutAddMenuEntry(_("Unlimited"),1000);
  if(frameratevalue==1000)glutAddMenuEntry(_("*Unlimited"),1000);
  if(frameratevalue<0){
    glutAddMenuEntry(_("*Step"),-1);
  }
  else{
    glutAddMenuEntry(_("Step"),-1);
  }

/* --------------------------------render menu -------------------------- */
  {
    char renderwindow[1024];
    char renderwindow2[1024];
    char renderwindow3[1024];
    char renderwindow4[1024];
    char rendertemp[1024];

    strcpy(renderwindow,"  ");
    if(renderW==320)strcat(renderwindow,"*");
    strcat(renderwindow,"320x240");

    strcpy(renderwindow2,"  ");
    if(renderW==640)strcat(renderwindow2,"*");
    strcat(renderwindow2,"640x480");

    sprintf(rendertemp,"%i%s%i (current)",screenWidth,"x",screenHeight);
    strcpy(renderwindow3,"  ");
    if(renderW!=320&&renderW!=640&&renderW!=2*screenWidth)strcat(renderwindow3,"*");
    strcat(renderwindow3,rendertemp);

    sprintf(rendertemp,"%i%s%i (2*current)",2*screenWidth,"x",2*screenHeight);
    strcpy(renderwindow4,"  ");
    if(renderW==2*screenWidth)strcat(renderwindow4,"*");
    strcat(renderwindow4,rendertemp);

    CREATEMENU(rendermenu,RenderMenu);
    glutAddMenuEntry("SIZE:",10000);
    glutAddMenuEntry(renderwindow,Render320);
    glutAddMenuEntry(renderwindow2,Render640);
    glutAddMenuEntry(renderwindow3,RenderWindow);
    glutAddMenuEntry(renderwindow4,Render2Window);

    glutAddMenuEntry(_("Type:"),10000);
    if(renderfiletype==0){
      glutAddMenuEntry("  *PNG",RenderPNG);
#ifdef pp_JPEG
      glutAddMenuEntry("  JPEG",RenderJPEG);
#endif
    }
    if(renderfiletype==1){
      glutAddMenuEntry("  PNG",RenderPNG);
#ifdef pp_JPEG
      glutAddMenuEntry("  *JPEG",RenderJPEG);
#endif
    }
    if(renderfiletype==2){
      glutAddMenuEntry("  PNG",RenderPNG);
#ifdef pp_JPEG
      glutAddMenuEntry("  JPEG",RenderJPEG);
#endif
    }

    glutAddMenuEntry("Suffix:",10000);
    if(renderfilelabel==0){
      glutAddMenuEntry("  *Frame number",RenderLABELframenumber);
      glutAddMenuEntry("  Time",RenderLABELtime);
    }
    if(renderfilelabel==1){
      glutAddMenuEntry("  Frame number",RenderLABELframenumber);
      glutAddMenuEntry("  *Time",RenderLABELtime);
    }
    glutAddMenuEntry(_("Number:"),10000);
    glutAddMenuEntry(_("  One Frame"),RenderOnce);
    update_glui_render();
    if(RenderTime==1||touring==1){
      glutAddMenuEntry(_("  All frames"),1);
      glutAddMenuEntry(_("  Every 2nd frame"),2);
      glutAddMenuEntry(_("  Every 3rd frame"),3);
      glutAddMenuEntry(_("  Every 4th frame"),4);
      glutAddMenuEntry(_("  Every 5th frame"),5);
      glutAddMenuEntry(_("  Every 10th frame"),10);
      glutAddMenuEntry(_("  Every 20th frame"),20);
      glutAddMenuEntry(_("  Cancel"),RenderCancel);
    }
  }

   /* --------------------------------viewpoint menu -------------------------- */

  CREATEMENU(dialogmenu,DialogMenu);
  if(nterraininfo>0){
    if(showwui_dialog==1)glutAddMenuEntry(_("*WUI display... ALT+w"),26);
    if(showwui_dialog==0)glutAddMenuEntry(_("WUI display... ALT+w..."),26);
  }
  if(showclip_dialog==1)glutAddMenuEntry(_("*Clip geometry...  ALT+c"),18);
  if(showclip_dialog==0)glutAddMenuEntry(_("Clip geometry...  ALT+c"),18);
#ifdef pp_COMPRESS
  if(smokezippath!=NULL&&(npatchinfo>0||nsmoke3dinfo>0||nsliceinfo>0)){
    if(showgluizip==1)glutAddMenuEntry(_("*Compression/Smokezip...  ALT+z"),24);
    if(showgluizip==0)glutAddMenuEntry(_("Compression/Smokezip...  ALT+z"),24);
  }
#endif
  if(showcolorbar_dialog==1)glutAddMenuEntry(_("*Customize colorbar...  ALT+C"),23);
  if(showcolorbar_dialog==0)glutAddMenuEntry(_("Customize colorbar...  ALT+C"),23);
  if(ndeviceinfo>0){
    if(showdevice_dialog==1)glutAddMenuEntry(_("*Devices/Objects..."),28);
    if(showdevice_dialog==0)glutAddMenuEntry(_("Devices/Objects..."),28);
  }
  if(showdisplay_dialog==1)glutAddMenuEntry(_("*Display...  ALT+d"),22);
  if(showdisplay_dialog==0)glutAddMenuEntry(_("Display...  ALT+d"),22);
  if(isZoneFireModel==0){
    if(showedit_dialog==1)glutAddMenuEntry(_("*Examine blockages...  ALT+e"),16);
    if(showedit_dialog==0)glutAddMenuEntry(_("Examine blockages...  ALT+e"),16);
  }
  if(showbounds_dialog==1)glutAddMenuEntry(_("*File/Bound/Script settings...  ALT+f"),14);
  if(showbounds_dialog==0)glutAddMenuEntry(_("File/Bound/Script settings...  ALT+f"),14);
  if(showmotion_dialog==1)glutAddMenuEntry(_("*Motion/View/Render...  ALT+m"),15);
  if(showmotion_dialog==0)glutAddMenuEntry(_("Motion/View/Render...  ALT+m"),15);
#ifdef pp_SHOOTER
  if(showshooter_dialog==1)glutAddMenuEntry(_("*Particle tracking..."),27);
  if(showshooter_dialog==0)glutAddMenuEntry(_("Particle tracking..."),27);
#endif
  if(nsmoke3dinfo>0||nvolrenderinfo>0){
    if(show3dsmoke_dialog==1)glutAddMenuEntry(_("*3D smoke...  ALT+s"),20);
    if(show3dsmoke_dialog==0)glutAddMenuEntry(_("3D smoke...  ALT+s"),20);
  }
  if(showstereo_dialog==1)glutAddMenuEntry(_("*Stereo parameters..."),19);
  if(showstereo_dialog==0)glutAddMenuEntry(_("Stereo parameters..."),19);
  if(showtour_dialog==1)glutAddMenuEntry(_("*Tours...  ALT+t"),21);
  if(showtour_dialog==0)glutAddMenuEntry(_("Tours...  ALT+t"),21);
  if(trainer_active==1){
    if(showtrainer_dialog==1)glutAddMenuEntry(_("*Trainer..."),25);
    if(showtrainer_dialog==0)glutAddMenuEntry(_("Trainer..."),25);
  }
  glutAddMenuEntry("-",-1);
  glutAddMenuEntry(_("Close all dialogs  ALT+x"),-2);

  /* -------------------------------- font menu -------------------------- */

  if(showfontmenu==1){
    CREATEMENU(fontmenu,FontMenu);
    switch (fontindex){
    case SMALL_FONT:
      glutAddMenuEntry(_("*Normal"),SMALL_FONT);
      glutAddMenuEntry(_("Large"),LARGE_FONT);
#ifdef pp_BETA
      glutAddMenuEntry(_("Scaled"),SCALED_FONT);
#endif
      break;
    case LARGE_FONT:
      glutAddMenuEntry(_("Normal"),SMALL_FONT);
      glutAddMenuEntry(_("*Large"),LARGE_FONT);
#ifdef pp_BETA
      glutAddMenuEntry(_("Scaled"),SCALED_FONT);
#endif
      break;
#ifdef pp_BETA
    case SCALED_FONT:
      glutAddMenuEntry(_("Normal"),SMALL_FONT);
      glutAddMenuEntry(_("Large"),LARGE_FONT);
      glutAddMenuEntry(_("*Scaled"),SCALED_FONT);
#endif
      break;
    default:
      ASSERT(FFALSE);
      break;
    }
  }
  if(nunitclasses>0){
    for(i=0;i<nunitclasses;i++){
      f_units *uci;

      uci = unitclasses + i;
      CheckMemory;

      CREATEMENU(uci->submenuid,UnitsMenu);

      for(j=0;j<uci->nunits;j++){
        if(uci->active==j){
          strcpy(menulabel,"*");
          strcat(menulabel,uci->units[j].unit);
        }
        else{
          strcpy(menulabel,uci->units[j].unit);
        }
        if(smokediff==1&&uci->diff_index==j&&uci->units[j].rel_defined==1){
          strcat(menulabel," rel to ");
          strcat(menulabel,uci->units[j].rel_val);
          strcat(menulabel," ");
          strcat(menulabel,uci->units[0].unit);
        }
        glutAddMenuEntry(menulabel,1000*i+j);
      }
    }
    CREATEMENU(unitsmenu,UnitsMenu);
    for(i=0;i<nunitclasses;i++){
      f_units *uci;

      uci = unitclasses + i;
      if(uci->visible==0)continue;
      glutAddSubMenu(uci->unitclass,uci->submenuid);
    }
    if(vishmsTimelabel==0)glutAddMenuEntry(_("time (h:m:s)"),-2);
    if(vishmsTimelabel==1)glutAddMenuEntry(_("*time (h:m:s)"),-2);
    glutAddMenuEntry(_("Reset"),-1);
  }

/* --------------------------------languagemenu -------------------------- */
#ifdef pp_LANG
#define FONTFILL(IBEG,IEND) \
    for(i=IBEG;i<=IEND;i++){\
      fonttest[i-IBEG]=i;\
    };\
    fonttest[IEND+1-IBEG]=0;\
    strcat(menulabel,"***");\
    strcat(menulabel,fonttest);\
    strcat(menulabel,"***");\
    glutAddMenuEntry(menulabel,-999)

  if(show_lang_menu==1&&nlanglistinfo>0){
    char fonttest[256];

    CREATEMENU(fonttestmenu,LanguageMenu);
    strcpy(menulabel,"   1-32: ");
    FONTFILL(1,32);
    strcpy(menulabel,"  33-64: ");
    FONTFILL(33,64);
    strcpy(menulabel,"  65-96: ");
    FONTFILL(65,96);
    strcpy(menulabel," 97-128: ");
    FONTFILL(97,128);
    strcpy(menulabel,"129-160: ");
    FONTFILL(129,160);
    strcpy(menulabel,"161-192: ");
    FONTFILL(161,192);
    strcpy(menulabel,"193-224: ");
    FONTFILL(193,224);
    strcpy(menulabel,"225-255: ");
    FONTFILL(225,255);

    CREATEMENU(languagemenu,LanguageMenu);
    strcpy(menulabel,"");
    if(strcmp(startup_lang_code,"en")==0){
      strcat(menulabel,"*");
    }
    strcat(menulabel,_("English"));
    strcat(menulabel," (en)");
    glutAddMenuEntry(menulabel,-1);
    for(i=0;i<nlanglistinfo;i++){
      langlistdata *langi;

      langi = langlistinfo + i;
      strcpy(menulabel,"");
      if(strcmp(langi->lang_code,startup_lang_code)==0){
        strcat(menulabel,"*");
      }
      if(strcmp(langi->lang_name,langi->lang_code)==0){
        strcat(menulabel,_("Language code: "));
        strcat(menulabel,langi->lang_code);
      }
      else{
        strcat(menulabel,langi->lang_name);
        strcat(menulabel," (");
        strcat(menulabel,langi->lang_code);
        strcat(menulabel,")");
      }
      glutAddMenuEntry(menulabel,i);
    }
#ifdef pp_BETA
    glutAddSubMenu("Font Test",fonttestmenu);
#endif
  }
#endif
/* --------------------------------option menu -------------------------- */

  CREATEMENU(optionmenu,OptionMenu);
  if(nunitclasses>0)glutAddSubMenu(_("Units"),unitsmenu);
#ifdef pp_SHOWLIGHT
  if(showlightmenu==1)glutAddSubMenu(_("Lighting"),lightingmenu);
#endif
  glutAddSubMenu(_("Rotation type"),rotatetypemenu);
  glutAddSubMenu(_("Max frame rate"),frameratemenu);
  glutAddSubMenu(_("Render"),rendermenu);
  glutAddSubMenu(_("Tours"),tourmenu);
  if(showfontmenu==1)glutAddSubMenu(_("Font"),fontmenu);
#ifdef pp_BENCHMARK
  glutAddMenuEntry("Benchmark",1);
#endif
  if(trainer_active==1)glutAddMenuEntry(_("Trainer menu"),2);
#ifdef pp_LANG
  if(show_lang_menu==1&&nlanglistinfo>0)glutAddSubMenu(_("Language"),languagemenu);
#endif

/* -------------------------------- about menu -------------------------- */

  CREATEMENU(disclaimermenu,AboutMenu);
  glutAddMenuEntry("The US Department of Commerce makes no warranty, expressed or",1);
  glutAddMenuEntry("implied, to users of Smokeview, and accepts no responsibility",1);
  glutAddMenuEntry("for its use. Users of Smokeview assume sole responsibility under",1);
  glutAddMenuEntry("Federal law for determining the appropriateness of its use in any",1);
  glutAddMenuEntry("particular application; for any conclusions drawn from the results",1); 
  glutAddMenuEntry("of its use; and for any actions taken or not taken as a result of",1); 
  glutAddMenuEntry("analysis performed using this tools.",1);
  glutAddMenuEntry("",1);
  glutAddMenuEntry("Smokeview and the companion program FDS is intended for use only",1);
  glutAddMenuEntry("by those competent in the fields of fluid dynamics, thermodynamics,",1);
  glutAddMenuEntry("combustion, and heat transfer, and is intended only to supplement",1);
  glutAddMenuEntry("the informed judgment of the qualified user. These software packages",1);
  glutAddMenuEntry("may or may not have predictive capability when applied to a specific",1);
  glutAddMenuEntry("set of factual circumstances.  Lack of accurate predictions could lead",1);
  glutAddMenuEntry("to erroneous conclusions with regard to fire safety.  All results",1);
  glutAddMenuEntry("should be evaluated by an informed user.",1);

/* -------------------------------- about menu -------------------------- */

  CREATEMENU(aboutmenu,AboutMenu);
  glutAddMenuEntry(TITLERELEASE,1);
  {
#ifdef pp_GPU
    char version_label[256];
#endif

#ifdef BIT64
    sprintf(menulabel,"  Smokeview (64 bit) revision:%i",revision_smv);
#else
    sprintf(menulabel,"  Smokeview (32 bit) revision:%i",revision_smv);
#endif
    glutAddMenuEntry(menulabel,1);
    if(revision_fds>0){
      sprintf(menulabel,"  FDS revision:%i",revision_fds);
      glutAddMenuEntry(menulabel,1);
    }
#ifdef pp_GPU
    strcpy(version_label,_("  OpenGL version:")); 
    strcat(version_label," ");
    strcat(version_label,(char *)glGetString(GL_VERSION));
    glutAddMenuEntry(version_label,1);
    if(gpuactive==1){
      if(usegpu==1){
        strcpy(menulabel,_("  GPU activated. (Press G to deactivate)"));
      }
      else{
        strcpy(menulabel,_("  GPU available but not in use. (Press G to activate)"));
      }
    }
    else{
      strcpy(menulabel,_("  GPU not available"));
    }
    glutAddMenuEntry(menulabel,1);
#endif
#ifdef pp_CULL
    if(cullactive==1&&gpuactive==1){
      if(cullsmoke==1&&usegpu==1){
        strcpy(menulabel,_("  Smoke culling activated. (Press C to deactivate)"));
      }
      else{
        strcpy(menulabel,_("  Smoke culling available but not in use. ( To activate: "));
        strcat(menulabel,_(" Press"));
        if(usegpu==0 && cullsmoke==1)strcat(menulabel," G.)");
        if(usegpu==1 && cullsmoke==0)strcat(menulabel," C.)");
        if(usegpu==0 && cullsmoke==0)strcat(menulabel," G then C.)");
      }
    }
    else{
      strcpy(menulabel,_("  Smoke culling not available"));
    }
    glutAddMenuEntry(menulabel,1);
#endif
    glutAddSubMenu(_("Disclaimer"),disclaimermenu);
  }

  /* --------------------------------web help menu -------------------------- */

  CREATEMENU(webhelpmenu,HelpMenu);
#ifdef WIN32
  glutAddMenuEntry(_("Obtain Documentation"),-3);
  glutAddMenuEntry(_("Report problems"),-1);
  glutAddMenuEntry(_("Download software updates"),-2);
  glutAddMenuEntry(_("FDS/Smokeview website"),-4);
#endif
#ifdef pp_OSX
  glutAddMenuEntry(_("Obtain Documentation"),-3);
  glutAddMenuEntry(_("Report problems"),-1);
  glutAddMenuEntry(_("Download software updates"),-2);
  glutAddMenuEntry(_("FDS/Smokeview website"),-4);
#endif
#ifndef WIN32
#ifndef PP_OSX
  glutAddMenuEntry(_("Download documentation at  http://fire.nist.gov/fds/documentation.html"),1);
  glutAddMenuEntry(_("Report a problem at http://code.google.com/p/fds-smv/issues/"),1);
  glutAddMenuEntry(_("Check for updates at http://code.google.com/p/fds-smv/downloads/"),1);
  glutAddMenuEntry(_("FDS/Smokeview website: http://fire.nist.gov/fds"),1);
#endif
#endif

  /* --------------------------------keyboard help menu -------------------------- */

  CREATEMENU(keyboardhelpmenu,HelpMenu);
  if(plotstate==DYNAMIC_PLOTS){
    glutAddMenuEntry(_("Animation"),1);
    glutAddMenuEntry(_("  t: set/unset single time step mode"),6);
    glutAddMenuEntry(_("  0: reset animation to the initial time"),6);
    glutAddMenuEntry(_("  T: toggle method for interpolating data color"),6);
    if(cellcenter_slice_active==1){
      glutAddMenuEntry(_("     (also, toggles cell center display on/off)"),6);
    glutAddMenuEntry(_("  @: display FDS values in cell centered slices"),6);
    }
    glutAddMenuEntry(_("  u: reload files"),6);
    glutAddMenuEntry(_("  L: unload last slice file loaded"),6);
    glutAddMenuEntry(_("  1-9: number of frames to skip"),6);
  }
  if(rotation_type==EYE_CENTERED){
    glutAddMenuEntry(_("Motion"),1);
    glutAddMenuEntry(_("   left/right cursor: rotate left/right"),1);
    glutAddMenuEntry(_("      up/down cursor: move forward/backward"),1);
    glutAddMenuEntry(_(" CTRL:up/down cursor: move forward/backward 5 times slower"),1);
    glutAddMenuEntry(_(" SHIFT: left/right cursor: rotate 90 degrees"),1);
    glutAddMenuEntry(_("    ALT:left/right cursor: slide left/right"),1);
    glutAddMenuEntry(_("    ALT:   up/down cursor: slide up/down"),1);
    glutAddMenuEntry(_("     INSERT/HOME/PageUP: tilt down/reset/tilt up"),1);
  }
  if(plotstate==STATIC_PLOTS){
    glutAddMenuEntry(_("Plot3D"),1);
    glutAddMenuEntry(_("  x,y,z: toggle contour plot visibility along x, y and z axis"),3);
    glutAddMenuEntry(_("  p: increment plot3d variable"),2);
    glutAddMenuEntry(_("  P: toggle cursor key mappings"),2);
    glutAddMenuEntry(_("  v: toggle flow vector visiblity"),3);
    glutAddMenuEntry(_("  a: change flow vector lengths"),3);
    glutAddMenuEntry(_("  s: change interval between adjacent vectors"),3);
    glutAddMenuEntry(_("  c: toggle between continuous and 2D stepped contours"),3);
    glutAddMenuEntry(_("  i: toggle iso-surface visibility"),2);
  }
  glutAddMenuEntry(_("Misc"),1);
  glutAddMenuEntry(_("  r: render the current scene to an image file"),7);
  glutAddMenuEntry(_("  R:   (same as r but at twice the resolution)"),7);
  if(ntotal_blockages>0||isZoneFireModel==0){
    glutAddMenuEntry(_("  g: toggle grid visibility"),2);
  }
  glutAddMenuEntry(_("  e: toggle between view rotation types: scene centered 2 axis, 1 axis, 3 axis and eye centered"),7);
  glutAddMenuEntry(_("  q: display blockages as specified by user or as used by FDS"),7);
  glutAddMenuEntry(_("  W: toggle clipping - use Options/Clip menu to specify clipping planes"),7);
  glutAddMenuEntry(_("  -: decrement time step, 2D contour planes, 3D contour levels"),2);
  glutAddMenuEntry(_("  space bar: increment time step, 2D contour planes, 3D contour levels"),2);
  glutAddMenuEntry("",1);
  glutAddMenuEntry(_("  ALT+v: toggle projection  method (between perspective and size preserving)"),2);
  if(n_embedded_meshes>0){
    glutAddMenuEntry(_("  ALT+u: toggle coarse slice display in embedded mesh"),2);
  }
  if(cellcenter_slice_active==1){
    glutAddMenuEntry(_("  ALT+y: if current slice is cell centered, toggle interpolation on/off"),2);
  }
  if(caseinifilename!=NULL&&strlen(caseinifilename)>0){
    char inilabel[512];

    sprintf(inilabel,_("  #: save settings to %s"),caseinifilename);
    glutAddMenuEntry(inilabel,2);
  }
  else{
    glutAddMenuEntry(_("  #: save settings (create casename.ini file)"),2);
  }
  glutAddMenuEntry(_("  !: snap scene to closest 45 degree orientation"),2);
  glutAddMenuEntry(_("  ~: level the scene"),2);
  glutAddMenuEntry(_("  &: toggle line anti-aliasing (draw lines smoothly)"),2);

  /* --------------------------------mouse help menu -------------------------- */

  CREATEMENU(mousehelpmenu,HelpMenu);
  switch (rotation_type){
    case ROTATION_2AXIS:
      glutAddMenuEntry(_("horizontal/vertical: rotate about z, x axis"),1);
      break;
    case ROTATION_1AXIS:
      glutAddMenuEntry(_("horizontal: rotate about z axis"),1);
      break;
    case ROTATION_3AXIS:
      glutAddMenuEntry(_("horizontal/vertical: rotate about z, x axis (click near scene center)"),1);
      glutAddMenuEntry(_("clock/counter clockwise: rotate about y axis (click near scene edge)"),1);
      break;
    case EYE_CENTERED:
      glutAddMenuEntry(_("horizontal/vertical: rotate about user location"),1);
      break;
    default:
      ASSERT(0);
      break;
  }
  switch (rotation_type){
    case EYE_CENTERED:
      break;
    case ROTATION_2AXIS:
    case ROTATION_1AXIS:
    case ROTATION_3AXIS:
      glutAddMenuEntry(_("CTRL horizontal/vertical: translate along x, y axis"),1);
      break;
    default:
      ASSERT(0);
      break;
  }
  glutAddMenuEntry(    _("ALT vertical: translate along z axis"),1);
  if(SHOW_gslice_data==1){
    glutAddMenuEntry(_("double-click: rotate/translate 3D node-centered slice"),1);
  }

  /* --------------------------------help menu -------------------------- */

  CREATEMENU(helpmenu,HelpMenu);
  glutAddSubMenu("Web",webhelpmenu);
  glutAddSubMenu("Shortcuts",keyboardhelpmenu);
  glutAddSubMenu("Mouse",mousehelpmenu);
  glutAddSubMenu(_("About"),aboutmenu);

  /* -------------------------------- target menu -------------------------- */

  if(ntarginfo>0){
    CREATEMENU(targetmenu,TargetMenu);
    for(i=0;i<ntarginfo;i++){
      if(targfilenum==i){
        STRCPY(menulabel,check);
        STRCAT(menulabel,targinfo[i].file);  
      }
      else{STRCPY(menulabel,targinfo[i].file);}
      glutAddMenuEntry(menulabel,i);
    }
    glutAddMenuEntry(_("Unload"),-1);
    CheckMemory;
  }

  /* --------------------------------particle menu -------------------------- */

  if(npartinfo>0&&nevac!=npartinfo){
    CREATEMENU(unloadpartmenu,UnloadPartMenu);
    for(ii=0;ii<npartinfo;ii++){
      partdata *parti;

      i = partorderindex[ii];
      parti = partinfo + i;
      if(parti->loaded==1&&parti->evac==0){
        STRCPY(menulabel,parti->menulabel);  
        glutAddMenuEntry(menulabel,i);
      }
    }
    glutAddMenuEntry(_("Unload all"),-1);

    if(nmeshes==1){
      CREATEMENU(particlemenu,ParticleMenu);
    }
    else{
      CREATEMENU(particlesubmenu,ParticleMenu);
    }
    for(ii=0;ii<npartinfo;ii++){
      i = partorderindex[ii];
      if(partinfo[i].evac==1)continue;
      if(partinfo[i].loaded==1){
        STRCPY(menulabel,check);
        STRCAT(menulabel,partinfo[i].menulabel);  
      }
      else{
        STRCPY(menulabel,partinfo[i].menulabel);
      }
      glutAddMenuEntry(menulabel,i);
    }
    {
      int useitem;
      int atleastone=0;
      partdata *parti, *partj;

      if(nmeshes>1){
        CREATEMENU(particlemenu,ParticleMenu);
        if(npartinfo>0){
          if(partinfo->version==1){
            strcpy(menulabel,_("Particles"));
            strcat(menulabel," - ");
            strcat(menulabel,_("All meshes"));
            glutAddMenuEntry(menulabel,-11);
            strcpy(menulabel,_("Particles"));
            strcat(menulabel," - ");
            strcat(menulabel,_("Single mesh"));
            glutAddSubMenu(menulabel,particlesubmenu);
            glutAddMenuEntry("-",-2);
          }
          else{
            for(i=0;i<npartinfo;i++){
              useitem=i;
              parti = partinfo + i;
              if(parti->evac==1)continue;
              for(j=0;j<i;j++){
                partj = partinfo + j;
                if(partj->evac==1)continue;
                if(strcmp(parti->label.longlabel,partj->label.longlabel)==0){
                  useitem=-1;
                  break;
                }
              }
              if(useitem!=-1){
                atleastone=1;
                strcpy(menulabel,parti->label.longlabel);
                strcat(menulabel," - ");
                strcat(menulabel,_("All meshes"));
                glutAddMenuEntry(menulabel,-useitem-10);
              }
            }
            strcpy(menulabel,_("Particles"));
            strcat(menulabel," - ");
            strcat(menulabel,_("Single mesh"));
            glutAddSubMenu(menulabel,particlesubmenu);
            if(atleastone==1)glutAddMenuEntry("-",-2);
          }
        }
      }
    }

    if(npartloaded<=1){
      glutAddMenuEntry(_("Unload"),-1);
    }
     else{
       glutAddSubMenu(_("Unload"),unloadpartmenu);
     }
  }

  if(nevac>0){
    CREATEMENU(unloadevacmenu,UnloadEvacMenu);
    for(ii=0;ii<npartinfo;ii++){
      partdata *parti;

      i = partorderindex[ii];
      parti = partinfo + i;
      if(parti->loaded==1&&parti->evac==0){
        STRCPY(menulabel,parti->menulabel);  
        glutAddMenuEntry(menulabel,i);
      }
    }
    glutAddMenuEntry(_("Unload all"),-1);

    CREATEMENU(evacmenu,EvacMenu);
    {
      int nevacs=0,nevacloaded2=0;

      for(ii=0;ii<npartinfo;ii++){
        partdata *parti;

        parti = partinfo + ii;
        if(parti->evac==1){
          if(parti->loaded==1)nevacloaded2++;
          nevacs++;
        }
      }
      if(nevacs>1){
        strcpy(menulabel,_("Humans - all meshes"));
        glutAddMenuEntry(menulabel,-11);
        glutAddMenuEntry("-",-2);
      }
      for(ii=0;ii<npartinfo;ii++){
        i = partorderindex[ii];
        if(partinfo[i].evac==0)continue;
        if(partinfo[i].loaded==1){
          STRCPY(menulabel,check);
          STRCAT(menulabel,partinfo[i].menulabel);  
        }
        else{
          STRCPY(menulabel,partinfo[i].menulabel);
        }
        glutAddMenuEntry(menulabel,i);
      }
      if(nevacloaded2<=1){
        glutAddMenuEntry(_("Unload"),-1);
      }
       else{
         glutAddSubMenu(_("Unload"),unloadevacmenu);
       }
  }
    }

/* --------------------------------unload and load vslice menus -------------------------- */

  if(nvsliceinfo>0){

    if(nmultivslices<nvsliceinfo){
      CREATEMENU(unloadmultivslicemenu,UnloadMultiVSliceMenu);
      for(i=0;i<nmultivslices;i++){
        mvslicei = multivsliceinfo + i;
        if(mvslicei->loaded!=0){
          glutAddMenuEntry(mvslicei->menulabel2,i);
        }
      }
      glutAddMenuEntry(_("Unload all"),-1);

      nloadsubmvslicemenu=1;
      for(i=1;i<nmultivslices;i++){
        vslicedata *vi, *vim1;
        slicedata *si, *sim1;

        vi = vsliceinfo + (multivsliceinfo+i)->ivslices[0];
        vim1 = vsliceinfo + (multivsliceinfo+i-1)->ivslices[0];
        si = sliceinfo + vi->ival;
        sim1 = sliceinfo + vim1->ival;
        if(strcmp(si->label.longlabel,sim1->label.longlabel)!=0){
          nloadsubmvslicemenu++;
        }
      }
      NewMemory((void **)&loadsubmvslicemenu,nloadsubmvslicemenu*sizeof(int));
      for(i=0;i<nloadsubmvslicemenu;i++){
        loadsubmvslicemenu[i]=0;
      }

      nmultisliceloaded=0;
      nloadsubmvslicemenu=0;
      for(i=0;i<nmultivslices;i++){
        vslicedata *vi, *vim1;
        slicedata *si, *sim1;

        mvslicei = multivsliceinfo + i;

        vi = vsliceinfo + mvslicei->ivslices[0];
        si = sliceinfo + vi->ival;
        if(i>0){
          vim1 = vsliceinfo + (multivsliceinfo+i-1)->ivslices[0];
          sim1 = sliceinfo + vim1->ival;
        }
        if(i==0||(i!=0&&strcmp(si->label.longlabel,sim1->label.longlabel)!=0)){
          CREATEMENU(loadsubmvslicemenu[nloadsubmvslicemenu],LoadMultiVSliceMenu);
          nloadsubmvslicemenu++;
        }

        STRCPY(menulabel,"");
        if(mvslicei->loaded==1){
          STRCAT(menulabel,"*");
          nmultisliceloaded++;
        }
        else if(mvslicei->loaded==-1){
          STRCAT(menulabel,"#");
        }
        else{
        }
        STRCAT(menulabel,mvslicei->menulabel);
        if(si->slicelabel!=NULL){
          STRCAT(menulabel," - ");
          STRCAT(menulabel,si->slicelabel);
        }
        glutAddMenuEntry(menulabel,i);
      }

      nloadsubmvslicemenu=0;
      CREATEMENU(loadmultivslicemenu,LoadMultiVSliceMenu);
      for(i=0;i<nmultivslices;i++){
        vslicedata *vi, *vim1;
        slicedata *si, *sim1;

        vi = vsliceinfo + (multivsliceinfo+i)->ivslices[0];
        si = sliceinfo + vi->ival;
		if(i>0){
		  vim1 = vsliceinfo + (multivsliceinfo+i-1)->ivslices[0];
          sim1 = sliceinfo + vim1->ival;
		}
        if(i==0||(i>0&&strcmp(si->label.longlabel,sim1->label.longlabel)!=0)){
          if(si->vec_comp==0||showallslicevectors==1){
            char mlabel[1024], mlabel2[1024];

            STRCPY(mlabel,si->label.longlabel);
            if(i==0&&si->mesh_type>0||(i>0&&si->mesh_type!=sim1->mesh_type)){
              sprintf(mlabel2,"*** Evac type %i meshes ***",si->mesh_type);
              glutAddMenuEntry(mlabel2,-999);
            }
            glutAddSubMenu(mlabel,loadsubmvslicemenu[nloadsubmvslicemenu]);
          }
          nloadsubmvslicemenu++;
        }
      }
      if(nmultivslices>0)glutAddMenuEntry("-",-999);
      if(showallslicevectors==0)glutAddMenuEntry(_("Show all vector slice entries"),-20);
      if(showallslicevectors==1)glutAddMenuEntry(_("*Show all vector slice entries"),-20);
      if(nmultisliceloaded>1){
        glutAddSubMenu(_("Unload"),unloadmultivslicemenu);
      }
      else{
        glutAddMenuEntry(_("Unload"),-1);
      }
    }

    CREATEMENU(unloadvslicemenu,UnloadVSliceMenu);
    for(ii=0;ii<nvsliceinfo;ii++){
      vslicedata *vd;

      i = vsliceorderindex[ii];
      vd = vsliceinfo + i;
      if(vd->loaded==0)continue;
      glutAddMenuEntry(vd->menulabel2,i);
    }
    glutAddMenuEntry("-",-999);
    //glutAddMenuEntry("Unload last",-2);
    glutAddMenuEntry(_("Unload all"),-1);

    if(nvslice0>0){
      vslicedata *vd, *vdim1,*vdip1;
      if(nvsliceinfo>0){
        nloadsubvslicemenu=1;
        for(ii=1;ii<nvsliceinfo;ii++){
          slicedata *sd, *sdm1;

          i=vsliceorderindex[ii];
          vd = vsliceinfo + i;
          sd = sliceinfo + vd->ival;
		  if(ii>0){
            vdim1 = vsliceinfo + vsliceorderindex[ii-1];
            sdm1 = sliceinfo + vdim1->ival;
		  }
          if(ii==0||strcmp(sd->label.longlabel,sdm1->label.longlabel)!=0){
            nloadsubvslicemenu++;
          }
        }
        NewMemory((void **)&loadsubvslicemenu,nloadsubvslicemenu*sizeof(int));
        for(i=0;i<nloadsubvslicemenu;i++){
          loadsubvslicemenu[i]=0;
        }
        nloadsubvslicemenu=0;
        for(ii=0;ii<nvsliceinfo;ii++){
          slicedata *sd, *sdm1, *sdp1;

          i=vsliceorderindex[ii];
          vd = vsliceinfo + i;
          sd = sliceinfo + vd->ival;
          
          if(ii!=0){
            vdim1 = vsliceinfo + vsliceorderindex[ii-1];
            sdm1 = sliceinfo + vdim1->ival;
          }
          if(ii!=nvsliceinfo-1){
            vdip1 = vsliceinfo + vsliceorderindex[ii+1];
            sdp1 = sliceinfo + vdip1->ival;
          }
          
          if(ii==0||strcmp(sd->label.longlabel,sdm1->label.longlabel)!=0){
            CREATEMENU(loadsubvslicemenu[nloadsubvslicemenu],LoadVSliceMenu);
          }
          if(vd->loaded==1){
            STRCPY(menulabel,check);
            STRCAT(menulabel,sd->menulabel);
          }
          else{
            STRCPY(menulabel,sd->menulabel);
          }
          if(sd->vec_comp==0||showallslicevectors==1)glutAddMenuEntry(menulabel,i);
          if(ii==nvsliceinfo-1||strcmp(sd->label.longlabel,sdp1->label.longlabel)!=0){
            subvslice_menuindex[nloadsubvslicemenu]=vsliceorderindex[ii];
			if(sd->ndirxyz[1]+sd->ndirxyz[2]+sd->ndirxyz[3]>1){
              glutAddMenuEntry("-",-999);
			}
			if(sd->ndirxyz[1]>1){
              glutAddMenuEntry(_("Load All x"),-1000-4*nloadsubvslicemenu-1);
			}
			if(sd->ndirxyz[2]>1){
              glutAddMenuEntry(_("Load All y"),-1000-4*nloadsubvslicemenu-2);
			}
			if(sd->ndirxyz[3]>1){
              glutAddMenuEntry(_("Load All z"),-1000-4*nloadsubvslicemenu-3);
			}
			if(sd->ndirxyz[1]+sd->ndirxyz[2]+sd->ndirxyz[3]>1){
              glutAddMenuEntry(_("Load All"),-1000-4*nloadsubvslicemenu);
			}
          }
          if(ii==0||strcmp(sd->label.longlabel,sdm1->label.longlabel)!=0){
            nloadsubvslicemenu++;
          }
        }
        CREATEMENU(vslicemenu,LoadVSliceMenu);
        nloadsubvslicemenu=0;
        for(ii=0;ii<nvsliceinfo;ii++){
          slicedata *sd, *sdm1;

          i=vsliceorderindex[ii];
          vd = vsliceinfo + i;
          sd = sliceinfo + vd->ival;
		  if(ii>0){
            vdim1 = vsliceinfo + vsliceorderindex[ii-1];
            sdm1 = sliceinfo + vdim1->ival;
		  }
          if(ii==0||strcmp(sd->label.longlabel,sdm1->label.longlabel)!=0){
            if(sd->vec_comp==0||showallslicevectors==1){
              char mlabel[1024], mlabel2[1024];

              STRCPY(mlabel,sd->label.longlabel);
              if(ii==0&&sd->mesh_type>0||(ii>0&&sd->mesh_type!=sdm1->mesh_type)){
                sprintf(mlabel2,"*** Evac type %i mesh ***",sd->mesh_type);
                glutAddMenuEntry(mlabel2,-999);
              }
              glutAddSubMenu(mlabel,loadsubvslicemenu[nloadsubvslicemenu]);
            }
            nloadsubvslicemenu++;
          }
        }
      }
    } 
    if(nvsliceinfo>0)glutAddMenuEntry("-",-999);
    if(showallslicevectors==0)glutAddMenuEntry(_("Show all vector slice entries"),-20);
    if(showallslicevectors==1)glutAddMenuEntry(_("*Show all vector slice entries"),-20);
    if(nvsliceloaded>1){
      glutAddSubMenu(_("Unload"),unloadvslicemenu);
    }
    else{
     glutAddMenuEntry(_("Unload"),-1);
    }
  }

  /* --------------------------------unload and load slice menus -------------------------- */

  if(nterraininfo>0){
    int nterrainloaded=0;

    CREATEMENU(unloadterrainmenu,UnloadTerrainMenu);
    for(i=0;i<nterraininfo;i++){
      terraindata *terri;

      terri = terraininfo + i;
      if(terri->loaded==1){
        nterrainloaded++;
        glutAddMenuEntry(terri->file,i);
      }
    }
    if(nterrainloaded>1){
        glutAddMenuEntry("-",-1);
        glutAddMenuEntry(_("Unload all"),-10);
    }
    CREATEMENU(loadterrainmenu,LoadTerrainMenu);
    if(nterraininfo>1){
      glutAddMenuEntry(_("All terrains"),-9);
      glutAddMenuEntry("-",-1);
    }
    /*
    leaving code commented in case I later decide to load/unload terrain files
    for(i=0;i<nterraininfo;i++){
      char menulabel[256];

      terraindata *terri;

      terri = terraininfo + i;
      strcpy(menulabel,"");
      if(terri->loaded==1)strcat(menulabel,"*");
      strcat(menulabel,terri->file);
      glutAddMenuEntry(menulabel,i);
    }
    */
    if(nterrainloaded==1){
      glutAddMenuEntry("-",-1);
      glutAddMenuEntry(_("Unload terrain"),-10);
    }
    else if(nterrainloaded>1){
      glutAddMenuEntry("-",-1);
      glutAddSubMenu(_("Unload terrain"),unloadterrainmenu);
    }
  }
    if(nsliceinfo>0){

      if(nmultislices<nsliceinfo){
        CREATEMENU(unloadmultislicemenu,UnloadMultiSliceMenu);
        nmultisliceloaded=0;
        for(i=0;i<nmultislices;i++){
          mslicei = multisliceinfo + i;
          if(mslicei->loaded!=0){
            glutAddMenuEntry(mslicei->menulabel2,i);
          }
        }
        glutAddMenuEntry(_("Unload all"),-1);

        nloadsubmslicemenu=1;
        for(i=1;i<nmultislices;i++){
          slicedata *sd, *sdim1;

          sd = sliceinfo+(multisliceinfo + i)->islices[0];
          sdim1 = sliceinfo+(multisliceinfo + i-1)->islices[0];
          if(strcmp(sd->label.longlabel,sdim1->label.longlabel)!=0)nloadsubmslicemenu++;
        }
        NewMemory((void **)&loadsubmslicemenu,nloadsubmslicemenu*sizeof(int));
        for(i=0;i<nloadsubmslicemenu;i++){
          loadsubmslicemenu[i]=0;
        }
        nloadsubmslicemenu=0;
        for(i=0;i<nmultislices;i++){
          slicedata *sd, *sdim1;

          sd = sliceinfo+(multisliceinfo + i)->islices[0];
          if(i>0)sdim1 = sliceinfo+(multisliceinfo + i-1)->islices[0];
          mslicei = multisliceinfo + i;
          if(i==0||strcmp(sd->label.longlabel,sdim1->label.longlabel)!=0){
            CREATEMENU(loadsubmslicemenu[nloadsubmslicemenu],LoadMultiSliceMenu);
            nloadsubmslicemenu++;
          }
          STRCPY(menulabel,"");
          if(mslicei->loaded==1){
            STRCAT(menulabel,"*");
            nmultisliceloaded++;
          }
          else if(mslicei->loaded==-1){
            STRCAT(menulabel,"#");
            nmultisliceloaded++;
          }
          STRCAT(menulabel,mslicei->menulabel);
          if(sd->slicelabel!=NULL){
            STRCAT(menulabel," - ");
            STRCAT(menulabel,sd->slicelabel);
          }
          glutAddMenuEntry(menulabel,i);
        }
        CREATEMENU(loadmultislicemenu,LoadMultiSliceMenu);
        nloadsubmslicemenu=0;
        for(i=0;i<nmultislices;i++){
          slicedata *sd, *sdim1;

          sd = sliceinfo+(multisliceinfo + i)->islices[0];
          if(i>0)sdim1 = sliceinfo+(multisliceinfo + i-1)->islices[0];

          if(i==0||strcmp(sd->label.longlabel,sdim1->label.longlabel)!=0){
            char mlabel[1024], mlabel2[1024];

            STRCPY(mlabel,sd->label.longlabel);
            if(i==0&&sd->mesh_type>0||(i>0&&sd->mesh_type!=sdim1->mesh_type)){
              sprintf(mlabel2,"*** Evac type %i meshes ***",sd->mesh_type);
              if(sd->slicetype==SLICE_CENTER){
                flowlabels *label;

                label = &sd->label;
                if(strcmp(label->shortlabel,"U-VEL")==0||strcmp(label->shortlabel,"V-VEL")==0||strcmp(label->shortlabel,"W-VEL")==0){
                  continue;
                }
              }
              glutAddMenuEntry(mlabel2,-999);
            }
            if(sd->slicetype==SLICE_CENTER){
              flowlabels *label;

              label = &sd->label;
              if(strcmp(label->shortlabel,"U-VEL")==0||strcmp(label->shortlabel,"V-VEL")==0||strcmp(label->shortlabel,"W-VEL")==0){
                continue;
              }
            }
            glutAddSubMenu(mlabel,             loadsubmslicemenu[nloadsubmslicemenu]);
            nloadsubmslicemenu++;
          }
        }
        if(nmultislices>0)glutAddMenuEntry("-",-999);
        if(nmultisliceloaded>1){
          glutAddSubMenu(_("Unload"),unloadmultislicemenu);
        }
        else{
          glutAddMenuEntry(_("Unload"),-1);
        }

      }
      CREATEMENU(unloadslicemenu,UnloadSliceMenu);
      for(i=0;i<nsliceinfo;i++){
        slicedata *sd;

        sd = sliceinfo + sliceorderindex[i];
        if(sd->loaded==1){
          STRCPY(menulabel,sd->menulabel2);  
          glutAddMenuEntry(menulabel,sliceorderindex[i]);
        }
      }
      glutAddMenuEntry("-",-999);
      glutAddMenuEntry(_("Unload last"),-2);
      glutAddMenuEntry(_("Unload all"),-1);

//*** this is where I would put the "sub-slice" menus ordered by type
      nloadsubslicemenu=1;
      for(i=1;i<nsliceinfo;i++){
        slicedata *sd,*sdim1;

        sd = sliceinfo + sliceorderindex[i];
        sdim1 = sliceinfo + sliceorderindex[i-1];
        if(strcmp(sd->label.longlabel,sdim1->label.longlabel)!=0)nloadsubslicemenu++;
      }
      NewMemory((void **)&loadsubslicemenu,nloadsubslicemenu*sizeof(int));
      for(i=0;i<nloadsubslicemenu;i++){
        loadsubslicemenu[i]=0;
      }
      iloadsubslicemenu=0;
      for(i=0;i<nsliceinfo;i++){
        slicedata *sd,*sdim1,*sdip1;

        if(i!=0){
          sdim1 = sliceinfo + sliceorderindex[i-1];
        }
        sd = sliceinfo + sliceorderindex[i];
        if(i!=nsliceinfo-1){
          sdip1 = sliceinfo + sliceorderindex[i+1];
        }
        if(i==0||strcmp(sd->label.longlabel,sdim1->label.longlabel)!=0){
          CREATEMENU(loadsubslicemenu[iloadsubslicemenu],LoadSliceMenu);
        }
        STRCPY(menulabel,"");
        if(sd->loaded==1){
          STRCAT(menulabel,check);
        }
        STRCAT(menulabel,sd->menulabel);
        if(sd->slicelabel!=NULL){
          STRCAT(menulabel," - ");
          STRCAT(menulabel,sd->slicelabel);
        }
        if(sd->menu_show==1){
          glutAddMenuEntry(menulabel,sliceorderindex[i]);
        }
        if(i==nsliceinfo-1||strcmp(sd->label.longlabel,sdip1->label.longlabel)!=0){
          subslice_menuindex[iloadsubslicemenu]=sliceorderindex[i];
  		    if(sd->ndirxyz[1]+sd->ndirxyz[2]+sd->ndirxyz[3]>1){
            glutAddMenuEntry("-",-999);
		      }
		      if(sd->ndirxyz[1]>1){
            glutAddMenuEntry(_("Load All x"),-1000-4*iloadsubslicemenu-1);
		      }
		      if(sd->ndirxyz[2]>1){
            glutAddMenuEntry(_("Load All y"),-1000-4*iloadsubslicemenu-2);
		      }
		      if(sd->ndirxyz[3]>1){
            glutAddMenuEntry(_("Load All z"),-1000-4*iloadsubslicemenu-3);
		      }
		      if(sd->ndirxyz[1]+sd->ndirxyz[2]+sd->ndirxyz[3]>1){
            glutAddMenuEntry(_("Load All"),-1000-4*iloadsubslicemenu);
		      }
        }
        if(i==0||strcmp(sd->label.longlabel,sdim1->label.longlabel)!=0){
          iloadsubslicemenu++;
        }
      }
      CREATEMENU(loadslicemenu,LoadSliceMenu);
      iloadsubslicemenu=0;
      for(i=0;i<nsliceinfo;i++){
        slicedata *sd,*sdim1;

        sd = sliceinfo + sliceorderindex[i];
        if(i>0)sdim1 = sliceinfo + sliceorderindex[i-1];
        if(i==0||strcmp(sd->label.longlabel,sdim1->label.longlabel)!=0){
          char mlabel[1024],mlabel2[1024];;

          STRCPY(mlabel,sd->label.longlabel);
          if(i==0&&sd->mesh_type>0||(i>0&&sd->mesh_type!=sdim1->mesh_type)){
            if(sd->menu_show==1){
              sprintf(mlabel2,"*** Evac type %i mesh ***",sd->mesh_type);
              glutAddMenuEntry(mlabel2,-999);
            }
          }
          if(sd->menu_show==1)glutAddSubMenu(mlabel,loadsubslicemenu[iloadsubslicemenu]);
          iloadsubslicemenu++;
        }
      }
      glutAddMenuEntry("-",-999);
      if(nsliceloaded>1){
        glutAddSubMenu(_("Unload"),unloadslicemenu);
      }
      else{
        glutAddMenuEntry(_("Unload"),-1);
      }
    }

/* --------------------------------unload and load 3d vol smoke menus -------------------------- */

    if(nvolsmoke3dloaded>0){
      CREATEMENU(unloadvolsmoke3dmenu,UnLoadVolSmoke3DMenu);
      if(nvolsmoke3dloaded>1){
        char vlabel[256];

        strcpy(vlabel,_("3D smoke (volume rendered)"));
        strcat(vlabel,_(" - All meshes"));
        glutAddMenuEntry(vlabel,UNLOAD_ALL);
      }
      for(i=0;i<nmeshes;i++){
        mesh *meshi;
        volrenderdata *vr;

        meshi = meshinfo + i;
        vr = &(meshi->volrenderinfo);
        if(vr->fire==NULL||vr->smoke==NULL)continue;
        if(vr->loaded==0)continue;
        glutAddMenuEntry(meshi->label,i);
      }
    }
    if(nvolrenderinfo>0){
      CREATEMENU(loadvolsmoke3dmenu,LoadVolSmoke3DMenu);
      if(nvolrenderinfo>1){
        char vlabel[256];

        strcpy(vlabel,_("3D smoke (volume rendered)"));
        strcat(vlabel,_(" - All meshes"));
        glutAddMenuEntry(vlabel,LOAD_ALL);
        glutAddMenuEntry("-",999);
      }
      for(i=0;i<nmeshes;i++){
        mesh *meshi;
        volrenderdata *vr;

        meshi = meshinfo + i;
        vr = &(meshi->volrenderinfo);
        if(vr->fire==NULL||vr->smoke==NULL)continue;
        strcpy(menulabel,"");
        if(vr->loaded==1)strcat(menulabel,"*");
        strcat(menulabel,meshi->label);
        glutAddMenuEntry(menulabel,i);
      }
      if(nvolsmoke3dloaded==1)glutAddMenuEntry(_("Unload"),UNLOAD_ALL);
      if(nvolsmoke3dloaded>1)glutAddSubMenu(_("Unload"),unloadvolsmoke3dmenu);
    }

    /* --------------------------------unload and load 3d smoke menus -------------------------- */

    {
      smoke3ddata *smoke3di;
      if(nsmoke3dloaded>0){
        CREATEMENU(unloadsmoke3dmenu,UnLoadSmoke3DMenu);
        {
          int nsootloaded=0,nhrrloaded=0,nwaterloaded=0;

          for(i=0;i<nsmoke3dinfo;i++){
            smoke3di=smoke3dinfo + i;
            if(smoke3di->loaded==0)continue;
            switch (smoke3di->type){
            case 1:
              nsootloaded++;
              break;
            case 2:
              nhrrloaded++;
              break;
            case 3:
              nwaterloaded++;
              break;
            default:
              ASSERT(FFALSE);
              break;
            }
          }
          if(nsootloaded>1) glutAddMenuEntry(_("soot density - all meshes"),-1);
          if(nhrrloaded>1)  glutAddMenuEntry(_("HRRPUV - all meshes"),-2);
          if(nwaterloaded>1)glutAddMenuEntry(_("water - all meshes"),-3);
          if(nsootloaded>1||nhrrloaded>1||nwaterloaded>1)glutAddMenuEntry("-",999);
        }
        for(i=0;i<nsmoke3dinfo;i++){
          smoke3di=smoke3dinfo+i;
          if(smoke3di->loaded==0)continue;
          glutAddMenuEntry(smoke3di->menulabel,i);
        }
      }
    }
    {
      smoke3ddata *smoke3di;
      int n_soot_menu=0, n_hrr_menu=0, n_water_menu=0;

      if(nsmoke3dinfo>0){
        if(nmeshes==1){
          CREATEMENU(loadsmoke3dmenu,LoadSmoke3DMenu);
        }
        if(nmeshes>1){
          CREATEMENU(loadsmoke3dsootmenu,LoadSmoke3DMenu);
        }
        for(i=0;i<nsmoke3dinfo;i++){
          smoke3di = smoke3dinfo + i;
          if(smoke3di->type!=1)continue;
          n_soot_menu++;
          strcpy(menulabel,"");
          if(smoke3di->loaded==1){
            strcat(menulabel,"*");
          }
          strcat(menulabel,smoke3di->menulabel);
          glutAddMenuEntry(menulabel,i);
        }
        if(nmeshes>1){
          CREATEMENU(loadsmoke3dhrrmenu,LoadSmoke3DMenu);
        }
        for(i=0;i<nsmoke3dinfo;i++){
          smoke3di = smoke3dinfo + i;
          if(smoke3di->type!=2)continue;
          n_hrr_menu++;
          strcpy(menulabel,"");
          if(smoke3di->loaded==1){
            strcat(menulabel,"*");
          }
          strcat(menulabel,smoke3di->menulabel);
          glutAddMenuEntry(menulabel,i);
        }
        if(nmeshes>1){
          CREATEMENU(loadsmoke3dwatermenu,LoadSmoke3DMenu);
        }
        for(i=0;i<nsmoke3dinfo;i++){
          smoke3di = smoke3dinfo + i;
          if(smoke3di->type!=3)continue;
          n_water_menu++;
          strcpy(menulabel,"");
          if(smoke3di->loaded==1){
            strcat(menulabel,"*");
          }
          strcat(menulabel,smoke3di->menulabel);
          glutAddMenuEntry(menulabel,i);
        }
        if(nmeshes>1){
          CREATEMENU(loadsmoke3dmenu,LoadSmoke3DMenu);
        }
        {
          int useitem;
          smoke3ddata *smoke3dj;

          if(nmeshes>1){
            for(i=0;i<nsmoke3dinfo;i++){
              useitem=i;
              smoke3di=smoke3dinfo + i;
              for(j=0;j<i;j++){
                smoke3dj = smoke3dinfo + j;
                if(strcmp(smoke3di->label.longlabel,smoke3dj->label.longlabel)==0){
                  useitem=-1;
                  break;
                }
              }
              if(useitem!=-1){
                strcpy(menulabel,smoke3di->label.longlabel);
                strcat(menulabel," - ");
                strcat(menulabel,_("All meshes"));
                glutAddMenuEntry(menulabel,-useitem-10);
              }
            }
            glutAddMenuEntry("-",-2);
          }
          if(nmeshes>1){
            if(n_soot_menu>0)glutAddSubMenu(_("soot Mass Fraction - single mesh"),loadsmoke3dsootmenu);
            if(n_hrr_menu>0)glutAddSubMenu(_("HRRPUV - single mesh"),loadsmoke3dhrrmenu);
            if(n_water_menu>0)glutAddSubMenu(_("Water - single mesh"),loadsmoke3dwatermenu);
          }
        }
        if(nsmoke3dloaded==1)glutAddMenuEntry(_("Unload"),-1);
        if(nsmoke3dloaded>1)glutAddSubMenu(_("Unload"),unloadsmoke3dmenu);
      }
    }

/* --------------------------------plot3d menu -------------------------- */

    if(nplot3dinfo>0){
      plot3ddata *plot3dim1, *plot3di;

      CREATEMENU(unloadplot3dmenu,UnloadPlot3dMenu);
      for(ii=0;ii<nplot3dinfo;ii++){
        i=plot3dorderindex[ii];
        plot3di = plot3dinfo + i;
        if(ii==0){
          strcpy(menulabel,plot3di->longlabel);
          glutAddMenuEntry(menulabel,997);
        }
        if(ii!=0&&strcmp(plot3di->longlabel,plot3dinfo[plot3dorderindex[ii-1]].longlabel)!=0){
          glutAddMenuEntry(plot3di->longlabel,997);
        }
        if(plot3di->loaded==0)continue;
        STRCPY(menulabel,plot3dinfo[i].menulabel);  
        glutAddMenuEntry(menulabel,i);
      }
      glutAddMenuEntry("Unload all",-1);


      
      nloadsubplot3dmenu=1;
      for(ii=1;ii<nplot3dinfo;ii++){
        int im1;

        i = plot3dorderindex[ii];
        im1 = plot3dorderindex[ii-1];
        plot3di = plot3dinfo + i;
        plot3dim1 = plot3dinfo + im1;
        if(ABS(plot3di->time-plot3dim1->time)>0.1)nloadsubplot3dmenu++;
      }
      NewMemory((void **)&loadsubplot3dmenu,nloadsubplot3dmenu*sizeof(int));
      for(i=0;i<nloadsubplot3dmenu;i++){
        loadsubplot3dmenu[i]=0;
      }

      nloadsubplot3dmenu=0;
      i = plot3dorderindex[0];
      plot3di = plot3dinfo + i;
      CREATEMENU(loadsubplot3dmenu[nloadsubplot3dmenu],LoadPlot3dMenu);
      strcpy(menulabel,"");
      if(plot3di->loaded==1){
        strcat(menulabel,check);
      }
      strcat(menulabel,plot3di->menulabel);
      glutAddMenuEntry(menulabel,i);
      nloadsubplot3dmenu++;

      for(ii=1;ii<nplot3dinfo;ii++){
        int im1;

        i = plot3dorderindex[ii];
        im1 = plot3dorderindex[ii-1];
        plot3di = plot3dinfo + i;
        plot3dim1 = plot3dinfo + im1;
        if(ABS(plot3di->time-plot3dim1->time)>0.1){
          if(nmeshes>1)glutAddMenuEntry("  All meshes",-100000+nloadsubplot3dmenu-1);
          CREATEMENU(loadsubplot3dmenu[nloadsubplot3dmenu],LoadPlot3dMenu);
          nloadsubplot3dmenu++;
        }
        strcpy(menulabel,"");
        if(plot3di->loaded==1){
          strcat(menulabel,check);
        }
        strcat(menulabel,plot3di->menulabel);
        glutAddMenuEntry(menulabel,i);
      }
      if(nmeshes>1)glutAddMenuEntry(_("  All meshes"),-100000+nloadsubplot3dmenu-1);

      nloadsubplot3dmenu=0;
      CREATEMENU(loadplot3dmenu,LoadPlot3dMenu);
      for(ii=0;ii<nplot3dinfo;ii++){
        int im1;

        i = plot3dorderindex[ii];
        plot3di = plot3dinfo + i;
        if(ii==0){
          strcpy(menulabel,plot3di->longlabel);
          glutAddMenuEntry(menulabel,997);
          sprintf(menulabel,"  %f",plot3di->time);
          trimzeros(menulabel);
          strcat(menulabel," s");
          if(nmeshes>1){
            glutAddSubMenu(menulabel,loadsubplot3dmenu[nloadsubplot3dmenu]);
          }
          else{
            strcpy(menulabel,"  ");
            if(plot3di->loaded==1){
              strcat(menulabel,check);
            }
            strcat(menulabel,plot3di->menulabel);
            glutAddMenuEntry(menulabel,i);
          }
          nloadsubplot3dmenu++;
        }
        if(ii!=0){
          i = plot3dorderindex[ii];
          im1 = plot3dorderindex[ii-1];
          plot3di = plot3dinfo + i;
          plot3dim1 = plot3dinfo + im1;
          if(strcmp(plot3di->longlabel,plot3dim1->longlabel)!=0){
            glutAddMenuEntry(plot3di->longlabel,997);
          }
          if(ABS(plot3di->time-plot3dim1->time)>0.1){
            sprintf(menulabel,"  %f",plot3di->time);
            trimzeros(menulabel);
            strcat(menulabel," s");
            if(nmeshes>1){
              glutAddSubMenu(menulabel,loadsubplot3dmenu[nloadsubplot3dmenu]);
            }
            else{
              strcpy(menulabel,"  ");
              if(plot3di->loaded==1){
                strcat(menulabel,check);
              }
              strcat(menulabel,plot3di->menulabel);
              glutAddMenuEntry(menulabel,i);
            }
            nloadsubplot3dmenu++;
          }
        }
      }
      if(nplot3dloaded>1){
        glutAddSubMenu(_("Unload"),unloadplot3dmenu);
      }
      else{
       glutAddMenuEntry(_("Unload"),-1);
      }
    }

/* --------------------------------load patch menu -------------------------- */

    if(npatchinfo>0){
      nloadpatchsubmenus=0;
      CREATEMENU(unloadpatchmenu,UnloadPatchMenu);
      for(ii=0;ii<npatchinfo;ii++){
        patchdata *patchi;

        i = patchorderindex[ii];
        patchi = patchinfo + i;
        if(patchi->loaded==0)continue;
        STRCPY(menulabel,patchinfo[i].menulabel);
        glutAddMenuEntry(menulabel,i);
      }
      glutAddMenuEntry(_("Unload all"),-1);

      if(nmeshes>1&&loadpatchsubmenus==NULL){
        NewMemory((void **)&loadpatchsubmenus,npatchinfo*sizeof(int));
      }

      if(nmeshes>1){
        CREATEMENU(loadpatchsubmenus[nloadpatchsubmenus],LoadPatchMenu);
        nloadpatchsubmenus++;
      }
      else{
        CREATEMENU(loadpatchmenu,LoadPatchMenu);
      }

      for(ii=0;ii<npatchinfo;ii++){
        patchdata *patch1, *patch2;

        i = patchorderindex[ii];
        patch2 = patchinfo + i;
        if(ii>0){
          patch1 = patchinfo + patchorderindex[ii-1];
          if(nmeshes>1&&strcmp(patch1->label.longlabel,patch2->label.longlabel)!=0){
            CREATEMENU(loadpatchsubmenus[nloadpatchsubmenus],LoadPatchMenu);
            nloadpatchsubmenus++;
          }
        }

        if(patchinfo[i].loaded==1){
          STRCPY(menulabel,check);
          STRCAT(menulabel,patchinfo[i].menulabel);  
        }
        else{
          STRCPY(menulabel,patchinfo[i].menulabel);
        }
        glutAddMenuEntry(menulabel,i);
      }

      if(nmeshes>1){
        CREATEMENU(loadpatchmenu,LoadPatchMenu);
      }

      {
        int useitem;
        patchdata *patchi, *patchj;

        if(nmeshes>1){
          for(i=0;i<npatchinfo;i++){
            useitem=i;
            patchi = patchinfo + i;
            for(j=0;j<i;j++){
              patchj = patchinfo + j;
              if(strcmp(patchi->label.longlabel,patchj->label.longlabel)==0){
                useitem=-1;
                break;
              }
            }
            if(useitem!=-1){
              strcpy(menulabel,patchi->label.longlabel);
              strcat(menulabel," - ");
              strcat(menulabel,_("All meshes"));
              glutAddMenuEntry(menulabel,-useitem-10);
            }
          }
          glutAddMenuEntry("-",-2);
          for(ii=0;ii<npatchinfo;ii++){
            patchdata *patch1, *patch2;

            i = patchorderindex[ii];
            patch2 = patchinfo + i;
            if(ii==0){
              nloadpatchsubmenus=0;
              strcpy(menulabel,patch2->label.longlabel);
              strcat(menulabel," - ");
              strcat(menulabel,_("Single mesh"));
              glutAddSubMenu(menulabel,loadpatchsubmenus[nloadpatchsubmenus]);
              nloadpatchsubmenus++;
            }
            else{
              patch1 = patchinfo + patchorderindex[ii-1];
              if(strcmp(patch1->label.longlabel,patch2->label.longlabel)!=0){
                strcpy(menulabel,patch2->label.longlabel);
                strcat(menulabel," - Single mesh");
                glutAddSubMenu(menulabel,loadpatchsubmenus[nloadpatchsubmenus]);
                nloadpatchsubmenus++;
              }
            }
          }
        }
      }
      glutAddMenuEntry("-",-2);
      glutAddMenuEntry(_("Update bounds"),-3);
      if(npatchloaded>1){
        glutAddSubMenu(_("Unload"),unloadpatchmenu);
      }
      else{
       glutAddMenuEntry(_("Unload"),-1);
      }
    }

/* --------------------------------load iso menu -------------------------- */

    if(nisoinfo>0){
      CREATEMENU(unloadisomenu,UnloadIsoMenu);
      for(ii=0;ii<nisoinfo;ii++){
        isodata *isoi;

        i = isoorderindex[ii];
        isoi = isoinfo + i;
        if(isoi->loaded==0)continue;
        STRCPY(menulabel,isoi->menulabel);  
        glutAddMenuEntry(menulabel,i);
      }
      glutAddMenuEntry("Unload all",-1);

      if(nisoinfo>0){
        if(isosubmenus==NULL){
          NewMemory((void **)&isosubmenus,nisoinfo*sizeof(int));
        }
        nisosubmenus=0;

        CREATEMENU(isosubmenus[nisosubmenus],LoadIsoMenu);
        nisosubmenus++;
      }

      if(nmeshes==1){
        CREATEMENU(loadisomenu,LoadIsoMenu);
      }
      for(ii=0;ii<nisoinfo;ii++){
        isodata *iso1, *iso2;

        i = isoorderindex[ii];
        if(ii>0){
          iso1 = isoinfo + isoorderindex[ii-1];
          iso2 = isoinfo + isoorderindex[ii];
          if(nmeshes>1&&strcmp(iso1->surface_label.longlabel,iso2->surface_label.longlabel)!=0){
            CREATEMENU(isosubmenus[nisosubmenus],LoadIsoMenu);
            nisosubmenus++;
          }
        }
        if(isoinfo[i].loaded==1){
          STRCPY(menulabel,check);
          STRCAT(menulabel,isoinfo[i].menulabel);  
        }
        else{
          STRCPY(menulabel,isoinfo[i].menulabel);
        }
        glutAddMenuEntry(menulabel,i);
      }

      {
        int useitem;
        isodata *isoi, *isoj;

        if(nmeshes>1){
         CREATEMENU(loadisomenu,LoadIsoMenu);
          for(i=0;i<nisoinfo;i++){
            useitem=i;
            isoi = isoinfo + i;
            for(j=0;j<i;j++){
              isoj = isoinfo + j;
              if(strcmp(isoi->surface_label.longlabel,isoj->surface_label.longlabel)==0){
                useitem=-1;
                break;
              }
            }
            if(useitem!=-1){
              strcpy(menulabel,isoi->surface_label.longlabel);
              strcat(menulabel," - ");
              strcat(menulabel,_("All meshes"));
              glutAddMenuEntry(menulabel,-useitem-10);
            }
          }
          glutAddMenuEntry("-",-2);

          for(ii=0;ii<nisoinfo;ii++){
            isodata *iso1, *iso2;

            i = isoorderindex[ii];
            iso1 = isoinfo + i;
            if(ii==0){
              nisosubmenus=0;
              strcpy(menulabel,iso1->surface_label.longlabel);
              strcat(menulabel," - Single mesh");
              glutAddSubMenu(menulabel,isosubmenus[nisosubmenus]);
              nisosubmenus++;
            }
            else{
              iso2 = isoinfo + isoorderindex[ii-1];
              if(strcmp(iso1->surface_label.longlabel,iso2->surface_label.longlabel)!=0){
                strcpy(menulabel,iso1->surface_label.longlabel);
                strcat(menulabel," - Single mesh");
                glutAddSubMenu(menulabel,isosubmenus[nisosubmenus]);
                nisosubmenus++;
              }
            }
          }

       }
     }

      if(nisoloaded>1){
        glutAddSubMenu(_("Unload"),unloadisomenu);
      }
      else{
       glutAddMenuEntry(_("Unload"),-1);
      }
    }

/* --------------------------------zone menu -------------------------- */

    if(nzoneinfo>0){
      CREATEMENU(zonemenu,ZoneMenu);
      for(i=0;i<nzoneinfo;i++){
        zonedata *zonei;

        zonei = zoneinfo + i;
        if(zonefilenum==i){
          STRCPY(menulabel,check);
          STRCAT(menulabel,zonei->file);  
        }
        else{STRCPY(menulabel,zonei->file);}
        STRCAT(menulabel,", ");
        for(n=0;n<3;n++){
          STRCAT(menulabel,zonei->label[n].shortlabel);
          STRCAT(menulabel,", ");
        }
        STRCAT(menulabel,zonei->label[3].shortlabel);
        glutAddMenuEntry(menulabel,i);
      }
      glutAddMenuEntry("Unload",-1);

    }
/* -------------------------------- compress menu -------------------------- */

#ifdef pp_COMPRESS
  if(smokezippath!=NULL&&(npatchinfo>0||nsmoke3dinfo>0||nsliceinfo>0)){
    CREATEMENU(compressmenu,CompressMenu);
    glutAddMenuEntry(_("Compression options"),999);  // -c
    if(overwrite_all==1){
      glutAddMenuEntry(_("  *Overwrite compressed files"),2);  // -f
    }
    else{
      glutAddMenuEntry(_("  Overwrite compressed files"),2);  // -f
    }
    if(compress_autoloaded==1){
      glutAddMenuEntry(_("  *Compress only autoloaded files"),4);  // -f
    }
    else{
      glutAddMenuEntry(_("  Compress only autoloaded files"),4);  // -f
    }
    glutAddMenuEntry("-",999);  // -c
    glutAddMenuEntry(_("Compress now"),3);
    glutAddMenuEntry(_("Erase compressed files"),1);  // -c
  }
#endif


/* --------------------------------inisub menu -------------------------- */
  {
    int n_inifiles;
    inifiledata *inifile;

    n_inifiles=0;
    for(inifile=first_inifile.next;inifile->next!=NULL;inifile=inifile->next){
      if(inifile->file!=NULL&&file_exists(inifile->file)==1){
        n_inifiles++;
      }
    }
    if(n_inifiles>0){
      CREATEMENU(inisubmenu,IniSubMenu);
      if(caseinifilename!=NULL&&file_exists(caseinifilename)==1){
        glutAddMenuEntry(caseinifilename,-1);
      }
      for(inifile=first_inifile.next;inifile->next!=NULL;inifile=inifile->next){
        if(inifile->file!=NULL&&file_exists(inifile->file)==1){
          glutAddMenuEntry(inifile->file,inifile->id);
        }
      }
    }
  }

/* --------------------------------smokeviewini menu -------------------------- */

    CREATEMENU(smokeviewinimenu,SmokeviewiniMenu);


   {
    inifiledata *inifile;
    int n_inifiles;

    n_inifiles=0;
    for(inifile=first_inifile.next;inifile->next!=NULL;inifile=inifile->next){
      if(inifile->file!=NULL&&file_exists(inifile->file)==1){
        n_inifiles++;
      }
    }
    if( n_inifiles>0||file_exists(INIfile)==1||file_exists(caseinifilename)==1||file_exists(smokeviewini)==1){
      if(n_inifiles==0){
        glutAddMenuEntry(_("Read ini files"),1);
      }
      else{
        glutAddSubMenu(_("Read ini files"),inisubmenu);
      }
    }
  }

    glutAddMenuEntry(WRITEINIfile,2);

    STRCPY(caselabel,_("Write"));
    STRCAT(caselabel," ");
    STRCAT(caselabel,caseinifilename);

    glutAddMenuEntry(caselabel,3);

    if(ndeviceinfo>0){
      glutAddMenuEntry("-",999);
      glutAddMenuEntry(_("Read .svo files"),4);
    }

    CREATEMENU(reloadmenu,ReloadMenu);
    glutAddMenuEntry(_("Reload Now"),0);
    if(periodic_value==1)glutAddMenuEntry(_("*Reload every 1 minute"),1);
    if(periodic_value!=1)glutAddMenuEntry(_("Reload every 1 minute"),1);
    if(periodic_value==5)glutAddMenuEntry(_("*Reload every 5 minutes"),5);
    if(periodic_value!=5)glutAddMenuEntry(_("Reload every 5 minutes"),5);
    if(periodic_value==10)glutAddMenuEntry(_("*Reload every 10 minutes"),10);
    if(periodic_value!=10)glutAddMenuEntry(_("Reload every 10 minutes"),10);
    glutAddMenuEntry(_("Stop Rendering"),-1);


    nscripts=0;
    if(script_recording==NULL){
      scriptfiledata *scriptfile;
      STRUCTSTAT statbuffer;

      for(scriptfile=first_scriptfile.next;scriptfile->next!=NULL;scriptfile=scriptfile->next){
        char *file;
        int len;

        file=scriptfile->file;
        if(file==NULL)continue;
        len = strlen(file);
        if(len<=0)continue;
        if(STAT(file,&statbuffer)!=0)continue;

        nscripts++;
      }

      if(nscripts>0){
        CREATEMENU(scriptlistmenu,ScriptMenu);
        for(scriptfile=first_scriptfile.next;scriptfile->next!=NULL;scriptfile=scriptfile->next){
          char *file;
          int len;

          file=scriptfile->file;
          if(file==NULL)continue;
          len = strlen(file);
          if(len<=0)continue;
          if(STAT(file,&statbuffer)!=0)continue;

          strcpy(menulabel,"  ");
          strcat(menulabel,file);
          glutAddMenuEntry(menulabel,scriptfile->id);
        }
        CREATEMENU(scriptsteplistmenu,ScriptMenu2);
        for(scriptfile=first_scriptfile.next;scriptfile->next!=NULL;scriptfile=scriptfile->next){
          char *file;
          int len;

          file=scriptfile->file;
          if(file==NULL)continue;
          len = strlen(file);
          if(len<=0)continue;
          if(STAT(file,&statbuffer)!=0)continue;

          strcpy(menulabel,"  ");
          strcat(menulabel,file);
          glutAddMenuEntry(menulabel,scriptfile->id);
        }
      }
    }
    CREATEMENU(scriptrecordmenu,ScriptMenu);
    if(script_recording==NULL){
      glutAddMenuEntry(_("Start"),SCRIPT_START_RECORDING);
      glutAddMenuEntry(_("Start (disable file loading)"),SCRIPT_START_RECORDING2);
    }
    glutAddMenuEntry(_("Stop"),SCRIPT_STOP_RECORDING);
    
    CREATEMENU(scriptmenu,ScriptMenu);
    if(nscripts>0){
      glutAddSubMenu("Run",scriptlistmenu);
      glutAddSubMenu("Step (using ^)",scriptsteplistmenu);
      if(script_step==1)glutAddMenuEntry(_("Continue"),SCRIPT_CONTINUE);
      if(script_step==1||current_script_command!=NULL)glutAddMenuEntry(_("Cancel"),SCRIPT_CANCEL);
    }
    glutAddSubMenu("Record",scriptrecordmenu);

  /* --------------------------------loadunload menu -------------------------- */
    {
      char loadmenulabel[100];
      char steplabel[100];

      CREATEMENU(loadunloadmenu,LoadUnloadMenu);
      strcpy(steplabel,_("error: steplabel not defined"));
      if(nsmoke3dinfo>0){
        strcpy(loadmenulabel,_("3D smoke"));
        if(smoke3dframeskip>0){
          sprintf(steplabel,"/Skip %i",smoke3dframeskip);
          strcat(loadmenulabel,steplabel);
        }
        glutAddSubMenu(loadmenulabel,loadsmoke3dmenu);
      }
      if(nvolrenderinfo>0&&smokediff==0){
        char vlabel[256];

        strcpy(vlabel,_("3D smoke (volume rendered)"));
        glutAddSubMenu(vlabel,loadvolsmoke3dmenu);
      }
      if(manual_terrain==1&&nterraininfo>0){
        glutAddSubMenu(_("Terrain"),loadterrainmenu);
      }
      if(nsliceinfo>0&&nmultislices<nsliceinfo){
        strcpy(loadmenulabel,"Multi-Slices");
        if(sliceframeskip>0){
          sprintf(steplabel,"/Skip %i",sliceframeskip);
          strcat(loadmenulabel,steplabel);
        }
        glutAddSubMenu(loadmenulabel,loadmultislicemenu);
      }
      if(nvsliceinfo>0&&nmultivslices<nvsliceinfo){
        strcpy(loadmenulabel,"Multi-Vector Slices");
        if(sliceframeskip>0){
          sprintf(steplabel,"/Skip %i",sliceframeskip);
          strcat(loadmenulabel,steplabel);
        }
        glutAddSubMenu(loadmenulabel,loadmultivslicemenu);
      }
      if(nsliceinfo>0){
        strcpy(loadmenulabel,"Slice file");
        if(sliceframeskip>0){
          sprintf(steplabel,"/Skip %i",sliceframeskip);
          strcat(loadmenulabel,steplabel);
        }
        glutAddSubMenu(loadmenulabel,loadslicemenu);
      }
      if(nvsliceinfo>0){
        strcpy(loadmenulabel,"Vector slices");
        if(sliceframestep>1){
          sprintf(steplabel,"/Skip %i",sliceframeskip);
          strcat(loadmenulabel,steplabel);
        }
        glutAddSubMenu(loadmenulabel,vslicemenu);
      }
      if(nisoinfo>0){
        strcpy(loadmenulabel,"Isosurface file");
        if(isoframeskip_global>0){
          sprintf(steplabel,"/Skip %i",isoframeskip_global);
          strcat(loadmenulabel,steplabel);
        }
        glutAddSubMenu(loadmenulabel,loadisomenu);
      }
      if(npatchinfo>0){
        strcpy(loadmenulabel,"Boundary file");
        if(boundframeskip>0){
          sprintf(steplabel,"/Skip %i",boundframeskip);
          strcat(loadmenulabel,steplabel);
        }
        glutAddSubMenu(loadmenulabel,loadpatchmenu);
      }
      if(npartinfo>0){
        if(nevac!=npartinfo){
          strcpy(loadmenulabel,"Particle file");
          if(partframeskip>0||partpointskip>0){
            if(partframeskip>0&&partpointskip>0){
              sprintf(steplabel,"/Skip Frame %i, Point %i",partframeskip,partpointskip);
            }
            else if(partframeskip<=0&&partpointskip>0){
              sprintf(steplabel,"/Skip Point %i",partpointskip);
            }
            else if(partframeskip>0&&partpointskip<=0){
              sprintf(steplabel,"/Skip Frame %i",partframeskip);
            }
            strcat(loadmenulabel,steplabel);
          }
          glutAddSubMenu(loadmenulabel,particlemenu);
        }
        if(nevac>0){
          strcpy(loadmenulabel,"Evacuation");
          if(partframeskip>0||partpointskip>0){
            if(partframeskip>0&&partpointskip>0){
              sprintf(steplabel,"/Skip Frame %i, Point %i",partframeskip,partpointskip);
            }
            else if(partframeskip<=0&&partpointskip>0){
              sprintf(steplabel,"/Skip Point %i",partpointskip);
            }
            else if(partframeskip>0&&partpointskip<=0){
              sprintf(steplabel,"/Skip Frame %i",partframeskip);
            }
            strcat(loadmenulabel,steplabel);
          }
          glutAddSubMenu(loadmenulabel,evacmenu);
        }
      }
      if(nplot3dinfo>0)glutAddSubMenu("Plot3d file",loadplot3dmenu);
      if(ntarginfo>0){
        strcpy(loadmenulabel,"Target file");
        glutAddSubMenu(loadmenulabel,targetmenu);
      }
      if(nzoneinfo>0){
        strcpy(loadmenulabel,"Zone fire file");
        glutAddSubMenu(loadmenulabel,zonemenu);
      }
      if(glui_active==1){      
        glutAddMenuEntry("-",999);
      }
      glutAddSubMenu(_("Configuration files"),smokeviewinimenu);
      glutAddSubMenu(_("Scripts"),scriptmenu);
#ifdef pp_COMPRESS
      if(smokezippath!=NULL&&(npatchinfo>0||nsmoke3dinfo>0||nsliceinfo>0)){
        glutAddSubMenu(_("Compression"),compressmenu);
      }
#endif
      if(showfiles==1)glutAddMenuEntry(_("*Show file names"),SHOWFILES);
      if(showfiles==0)glutAddMenuEntry(_("Show file names"),SHOWFILES);
      glutAddSubMenu(_("Reload"),reloadmenu);
      glutAddMenuEntry(_("Unload all"),UNLOADALL);
    }

/* --------------------------------main menu -------------------------- */
    if(trainer_mode==1){
      CREATEMENU(trainerviewmenu,TrainerViewMenu);
      if(AnySmoke(NULL)==1){
        if(trainerload==1)glutAddMenuEntry(_("*Realistic"),1);
        if(trainerload!=1)glutAddMenuEntry(_("Realistic"),1);
      }
      if(AnySlices("TEMPERATURE")==1){
        if(trainerload==2)glutAddMenuEntry(_("*Temperature"),2);
        if(trainerload!=2)glutAddMenuEntry(_("Temperature"),2);
      }
      if(AnySlices("oxygen")==1||
         AnySlices("oxygen VOLUME FRACTION")==1){
        if(trainerload==3)glutAddMenuEntry(_("*Oxygen"),3);
        if(trainerload!=3)glutAddMenuEntry(_("Oxygen"),3);
      }
      glutAddMenuEntry(_("Clear"),998);
    }

    CREATEMENU(mainmenu,MainMenu);
    if(trainer_mode==0){
      glutAddSubMenu(_("Load/Unload"),loadunloadmenu);
      glutAddSubMenu(_("Show/Hide"),showhidemenu);
      glutAddSubMenu(_("Options"),optionmenu);
      glutAddSubMenu(_("Dialogs"),dialogmenu);
      glutAddSubMenu(_("Help"),helpmenu);
      glutAddMenuEntry(_("Quit"),3);
    }
    if(trainer_active==1){
      if(trainer_mode==1){
        glutAddMenuEntry(_("Smokeview menus"),997);
      }
      else{
        glutAddMenuEntry(_("Trainer menus"),997);
      }
    }
    updatemenu=0;
#ifdef _DEBUG
  in_menu=0;
#endif

}

/* ------------------ MenuStatus ------------------------ */

void MenuStatus_CB(int status, int x, int y){
  float *eye_xyz;
  menustatus=status;
  /* keep scene from "bouncing" around when leaving a menu */
  start_xyz0[0]=x;
  start_xyz0[1]=y;
  /*touring=0;*/
  mouse_down_xy0[0]=x; mouse_down_xy0[1]=y;
  eye_xyz = camera_current->eye;
  eye_xyz0[0]=eye_xyz[0];
  eye_xyz0[1]=eye_xyz[1];
  eye_xyz0[2]=eye_xyz[2];
}
