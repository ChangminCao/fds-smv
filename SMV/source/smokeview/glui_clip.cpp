// $Date$ 
// $Revision$
// $Author$

#define CPP
#include "options.h"

// svn revision character string
extern "C" char glui_clip_revision[]="$Revision$";

#include <stdio.h>
#include <string.h>
#ifdef pp_OSX
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <math.h>

#include "string_util.h"
#include "smokeviewvars.h"

GLUI *glui_clip=NULL;

GLUI_RadioGroup *radio_clip=NULL;

GLUI_Spinner *SPINNER_clip_xupper=NULL, *SPINNER_clip_xlower=NULL;
GLUI_Spinner *SPINNER_clip_yupper=NULL, *SPINNER_clip_ylower=NULL;
GLUI_Spinner *SPINNER_clip_zupper=NULL, *SPINNER_clip_zlower=NULL;
GLUI_Spinner *SPINNER_moveclip_x=NULL;
GLUI_Spinner *SPINNER_moveclip_y=NULL;
GLUI_Spinner *SPINNER_moveclip_z=NULL;

GLUI_Checkbox *CHECKBOX_clip_xlower=NULL, *CHECKBOX_clip_xupper=NULL;
GLUI_Checkbox *CHECKBOX_clip_ylower=NULL, *CHECKBOX_clip_yupper=NULL;
GLUI_Checkbox *CHECKBOX_clip_zlower=NULL, *CHECKBOX_clip_zupper=NULL;

GLUI_Panel *PANEL_clip_lower=NULL, *PANEL_clip_upper=NULL, *PANEL_clip=NULL,*panel_wrapup=NULL;
GLUI_Panel *PANEL_clipx=NULL, *PANEL_clipX=NULL;
GLUI_Panel *PANEL_clipy=NULL, *PANEL_clipY=NULL;
GLUI_Panel *PANEL_clipz=NULL, *PANEL_clipZ=NULL;

GLUI_Listbox *LIST_mesh=NULL;

GLUI_RadioButton *RADIOBUTTON_clip_1a=NULL;
GLUI_RadioButton *RADIOBUTTON_clip_1b=NULL;
GLUI_RadioButton *RADIOBUTTON_clip_1c=NULL;

GLUI_Button *BUTTON_clip_1=NULL;
GLUI_Button *BUTTON_clip_2=NULL;

#define CLIP_xlower 0
#define CLIP_ylower 1
#define CLIP_zlower 2
#define CLIP_xupper 3
#define CLIP_yupper 4
#define CLIP_zupper 5

#define CLIP_all 12

#define SPINNER_xlower 13
#define SPINNER_ylower 14
#define SPINNER_zlower 15
#define SPINNER_xupper 16
#define SPINNER_yupper 17
#define SPINNER_zupper 18

#define INI_VALS -1
#define DEFAULT_VALS -2

#define CLIP_CLOSE 99
#define SAVE_SETTINGS 98
#define CLIP_MESH 80



void CLIP_CB(int var);
void set_clip_controls(int val);

/* ------------------ glui_clip_setup ------------------------ */

extern "C" void glui_clip_setup(int main_window){

  update_glui_clip=0;
  if(glui_clip!=NULL){
    glui_clip->close();
    glui_clip=NULL;
  }
  glui_clip = GLUI_Master.create_glui("clip",0,0,0);
  if(showclip_dialog==0)glui_clip->hide();

  PANEL_clip = glui_clip->add_panel("",GLUI_PANEL_NONE);
  PANEL_clip_lower = glui_clip->add_panel_to_panel(PANEL_clip,_("Clip Lower"));
  PANEL_clipx = glui_clip->add_panel_to_panel(PANEL_clip_lower,"X",GLUI_PANEL_NONE);
  SPINNER_clip_xlower=glui_clip->add_spinner_to_panel(PANEL_clipx,"X",GLUI_SPINNER_FLOAT,&clip_x_val,SPINNER_xlower,CLIP_CB);
  SPINNER_clip_xlower->set_float_limits(xclip_min,xclip_max,GLUI_LIMIT_CLAMP);
  glui_clip->add_column_to_panel(PANEL_clipx,false);
  CHECKBOX_clip_xlower=glui_clip->add_checkbox_to_panel(PANEL_clipx,"",&clip_x,CLIP_xlower,CLIP_CB);

  PANEL_clipy = glui_clip->add_panel_to_panel(PANEL_clip_lower,"Y",GLUI_PANEL_NONE);
  SPINNER_clip_ylower=glui_clip->add_spinner_to_panel(PANEL_clipy,"Y",GLUI_SPINNER_FLOAT,&clip_y_val,SPINNER_ylower,CLIP_CB);
  SPINNER_clip_ylower->set_float_limits(yclip_min,yclip_max,GLUI_LIMIT_CLAMP);
  glui_clip->add_column_to_panel(PANEL_clipy,false);
  CHECKBOX_clip_ylower=glui_clip->add_checkbox_to_panel(PANEL_clipy,"",&clip_y,CLIP_ylower,CLIP_CB);

  PANEL_clipz = glui_clip->add_panel_to_panel(PANEL_clip_lower,"Z",GLUI_PANEL_NONE);
  SPINNER_clip_zlower=glui_clip->add_spinner_to_panel(PANEL_clipz,"Z",GLUI_SPINNER_FLOAT,&clip_z_val,SPINNER_zlower,CLIP_CB);
  SPINNER_clip_zlower->set_float_limits(zclip_min,zclip_max,GLUI_LIMIT_CLAMP);
  glui_clip->add_column_to_panel(PANEL_clipz,false);
  CHECKBOX_clip_zlower=glui_clip->add_checkbox_to_panel(PANEL_clipz,"",&clip_z,CLIP_zlower,CLIP_CB);

  radio_clip = glui_clip->add_radiogroup_to_panel(PANEL_clip,&xyz_clipplane,CLIP_all,CLIP_CB);
  RADIOBUTTON_clip_1a=glui_clip->add_radiobutton_to_group(radio_clip,_("Clipping disabled"));
  RADIOBUTTON_clip_1b=glui_clip->add_radiobutton_to_group(radio_clip,_("Clip blockages and data"));
  RADIOBUTTON_clip_1c=glui_clip->add_radiobutton_to_group(radio_clip,_("Clip blockages"));

  glui_clip->add_column_to_panel(PANEL_clip,false);

  PANEL_clip_upper = glui_clip->add_panel_to_panel(PANEL_clip,_("Clip upper"));

  PANEL_clipX = glui_clip->add_panel_to_panel(PANEL_clip_upper,"X",GLUI_PANEL_NONE);
  SPINNER_clip_xupper=glui_clip->add_spinner_to_panel(PANEL_clipX,"X",GLUI_SPINNER_FLOAT,&clip_X_val,SPINNER_xupper,CLIP_CB);
  SPINNER_clip_xupper->set_float_limits(xclip_min,xclip_max,GLUI_LIMIT_CLAMP);
  glui_clip->add_column_to_panel(PANEL_clipX,false);
  CHECKBOX_clip_xupper=glui_clip->add_checkbox_to_panel(PANEL_clipX,"",&clip_X,CLIP_xupper,CLIP_CB);

  PANEL_clipY = glui_clip->add_panel_to_panel(PANEL_clip_upper,"Y",GLUI_PANEL_NONE);
  SPINNER_clip_yupper=glui_clip->add_spinner_to_panel(PANEL_clipY,"Y",GLUI_SPINNER_FLOAT,&clip_Y_val,SPINNER_yupper,CLIP_CB);
  SPINNER_clip_yupper->set_float_limits(yclip_min,yclip_max,GLUI_LIMIT_CLAMP);
  glui_clip->add_column_to_panel(PANEL_clipY,false);
  CHECKBOX_clip_yupper=glui_clip->add_checkbox_to_panel(PANEL_clipY,"",&clip_Y,CLIP_yupper,CLIP_CB);

  PANEL_clipZ = glui_clip->add_panel_to_panel(PANEL_clip_upper,"Z",GLUI_PANEL_NONE);
  SPINNER_clip_zupper=glui_clip->add_spinner_to_panel(PANEL_clipZ,"Z",GLUI_SPINNER_FLOAT,&clip_Z_val,SPINNER_zupper,CLIP_CB);
  SPINNER_clip_zupper->set_float_limits(zclip_min,zclip_max,GLUI_LIMIT_CLAMP);
  glui_clip->add_column_to_panel(PANEL_clipZ,false);
  CHECKBOX_clip_zupper=glui_clip->add_checkbox_to_panel(PANEL_clipZ,"",&clip_Z,CLIP_zupper,CLIP_CB);

  panel_wrapup = glui_clip->add_panel_to_panel(PANEL_clip,"",GLUI_PANEL_NONE);

  glui_clip->add_column_to_panel(panel_wrapup,false);

  BUTTON_clip_1=glui_clip->add_button_to_panel(panel_wrapup,_("Save settings"),SAVE_SETTINGS,CLIP_CB);

  glui_clip->add_column_to_panel(panel_wrapup,false);

  BUTTON_clip_2=glui_clip->add_button_to_panel(panel_wrapup,_("Close"),CLIP_CLOSE,CLIP_CB);

  if(updateclipvals==1){
    set_clip_controls(INI_VALS);  // clip vals from ini file
  }
  else{
    if(clip_mesh==0){
      set_clip_controls(DEFAULT_VALS);  // clip vals from global scene
    }
    else{
      set_clip_controls(clip_mesh);  // clip vals from mesh clip_mesh
    }
  }

  glui_clip->set_main_gfx_window( main_window );
}

/* ------------------ hide_glui_clip ------------------------ */

extern "C" void hide_glui_clip(void){
  if(glui_clip!=NULL)glui_clip->hide();
  showclip_dialog_save=showclip_dialog;
  showclip_dialog=0;
  updatemenu=1;
}

/* ------------------ show_glui_clip ------------------------ */

extern "C" void show_glui_clip(void){
  showclip_dialog=1;
  if(glui_clip!=NULL)glui_clip->show();
}


/* ------------------ Update_Glui_Clip ------------------------ */

extern "C" void Update_Glui_Clip(void){
  if(CHECKBOX_clip_xlower!=NULL&&CHECKBOX_clip_ylower!=NULL&&CHECKBOX_clip_zlower!=NULL&&
     CHECKBOX_clip_xupper!=NULL&&CHECKBOX_clip_yupper!=NULL&&CHECKBOX_clip_zupper!=NULL){

    CHECKBOX_clip_xlower->set_int_val(clip_x);
    CHECKBOX_clip_ylower->set_int_val(clip_y);
    CHECKBOX_clip_zlower->set_int_val(clip_z);
    CHECKBOX_clip_xupper->set_int_val(clip_X);
    CHECKBOX_clip_yupper->set_int_val(clip_Y);
    CHECKBOX_clip_zupper->set_int_val(clip_Z);
    if(radio_clip!=NULL)radio_clip->set_int_val(xyz_clipplane);
    CLIP_CB(CLIP_all);
  }
}

/* ------------------ CLIP_CB ------------------------ */

void CLIP_CB(int var){
  int i;

  switch (var){
  case CLIP_MESH:
    if(clip_mesh==0){
      set_clip_controls(DEFAULT_VALS);
    }
    else{
      set_clip_controls(clip_mesh);
    }
    break;
  case SAVE_SETTINGS:
    writeini(LOCAL_INI);
    break;
  case CLIP_CLOSE:
    hide_glui_clip();
    break;
  case CLIP_xlower:
    if(clip_x==0)SPINNER_clip_xlower->disable();
    if(clip_x==1)SPINNER_clip_xlower->enable();
    updatefacelists=1;
    break;
  case CLIP_ylower:
    if(clip_y==0)SPINNER_clip_ylower->disable();
    if(clip_y==1)SPINNER_clip_ylower->enable();
    updatefacelists=1;
    break;
  case CLIP_zlower:
    if(clip_z==0)SPINNER_clip_zlower->disable();
    if(clip_z==1)SPINNER_clip_zlower->enable();
    updatefacelists=1;
    break;
  case CLIP_xupper:
    if(clip_X==0)SPINNER_clip_xupper->disable();
    if(clip_X==1)SPINNER_clip_xupper->enable();
    updatefacelists=1;
    break;
  case CLIP_yupper:
    if(clip_Y==0)SPINNER_clip_yupper->disable();
    if(clip_Y==1)SPINNER_clip_yupper->enable();
    updatefacelists=1;
    break;
  case CLIP_zupper:
    if(clip_Z==0)SPINNER_clip_zupper->disable();
    if(clip_Z==1)SPINNER_clip_zupper->enable();
    updatefacelists=1;
    break;
  case CLIP_all:
    updatefacelists=1;
    update_clipplanes();
    if(xyz_clipplane!=0){
      for(i=0;i<6;i++){
        CLIP_CB(i);
      }
      CHECKBOX_clip_xlower->enable();
      CHECKBOX_clip_ylower->enable();
      CHECKBOX_clip_zlower->enable();
      CHECKBOX_clip_xupper->enable();
      CHECKBOX_clip_yupper->enable();
      CHECKBOX_clip_zupper->enable();
    }
    else{
      SPINNER_clip_xlower->disable();
      SPINNER_clip_ylower->disable();
      SPINNER_clip_zlower->disable();
      SPINNER_clip_xupper->disable();
      SPINNER_clip_yupper->disable();
      SPINNER_clip_zupper->disable();

      CHECKBOX_clip_xlower->disable();
      CHECKBOX_clip_ylower->disable();
      CHECKBOX_clip_zlower->disable();
      CHECKBOX_clip_xupper->disable();
      CHECKBOX_clip_yupper->disable();
      CHECKBOX_clip_zupper->disable();
    }
    glutPostRedisplay();
    break;
  case SPINNER_xlower:
    SPINNER_clip_xupper->set_float_limits(clip_x_val,xclip_max,GLUI_LIMIT_CLAMP);
    updatefacelists=1;
    break;
  case SPINNER_xupper:
    SPINNER_clip_xlower->set_float_limits(xclip_min,clip_X_val,GLUI_LIMIT_CLAMP);
    updatefacelists=1;
    break;
  case SPINNER_ylower:
    SPINNER_clip_yupper->set_float_limits(clip_y_val,yclip_max,GLUI_LIMIT_CLAMP);
    updatefacelists=1;
    break;
  case SPINNER_yupper:
    SPINNER_clip_ylower->set_float_limits(yclip_min,clip_Y_val,GLUI_LIMIT_CLAMP);
    updatefacelists=1;
    break;
  case SPINNER_zlower:
    SPINNER_clip_zupper->set_float_limits(clip_z_val,zclip_max,GLUI_LIMIT_CLAMP);
    updatefacelists=1;
    break;
  case SPINNER_zupper:
    SPINNER_clip_zlower->set_float_limits(zclip_min,clip_Z_val,GLUI_LIMIT_CLAMP);
    updatefacelists=1;
    break;
  default:
    ASSERT(0);
    break;
  }
  if(var>=CLIP_xlower&&var<=CLIP_zupper){
    clip2cam(camera_current);
  }
}

/* ------------------ update_clip_all ------------------------ */

extern "C" void update_clip_all(void){
  CLIP_CB(CLIP_all);
  radio_clip->set_int_val(xyz_clipplane);
}

/* ------------------ set_clip_controls ------------------------ */

void set_clip_controls(int val){
  int i;

  for(i=0;i<6;i++){
    CLIP_CB(i);
  }
  if(val==DEFAULT_VALS){
    clip_x_val = xclip_min;
    clip_y_val = yclip_min;
    clip_z_val = zclip_min;
    clip_X_val = xclip_max;
    clip_Y_val = yclip_max;
    clip_Z_val = zclip_max;
  }
  if(val>=1&&val<=nmeshes){
    mesh *meshi;
    float *xplt, *yplt, *zplt;

    float dxclip, dyclip, dzclip;

    dxclip = (xbarORIG-xbar0ORIG)/1000.0;
    dyclip = (ybarORIG-ybar0ORIG)/1000.0;
    dzclip = (zbarORIG-zbar0ORIG)/1000.0;

    meshi = meshinfo + val - 1;

    xplt = meshi->xplt_orig;
    yplt = meshi->yplt_orig;
    zplt = meshi->zplt_orig;

    clip_x_val = xplt[0] - dxclip;
    clip_y_val = yplt[0] - dyclip;
    clip_z_val = zplt[0] - dzclip;
    clip_X_val = xplt[meshi->ibar] + dxclip;
    clip_Y_val = yplt[meshi->jbar] + dyclip;
    clip_Z_val = zplt[meshi->kbar] + dzclip;
  }
  SPINNER_clip_xlower->set_float_val(clip_x_val);
  SPINNER_clip_ylower->set_float_val(clip_y_val);
  SPINNER_clip_zlower->set_float_val(clip_z_val);
  SPINNER_clip_xupper->set_float_val(clip_X_val);
  SPINNER_clip_yupper->set_float_val(clip_Y_val);
  SPINNER_clip_zupper->set_float_val(clip_Z_val);
}

