// $Date$ 
// $Revision$
// $Author$

#define CPP
#include "options.h"

// svn revision character string
extern "C" char glui_motion_revision[]="$Revision$";

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
#include "IOvolsmoke.h"

#define TRANSLATE_XY 101
#define ROTATE_ZX 102
#define GLUI_Z 2
#define MESH_LIST 4
#define EYE_ROTATE 5
#define EYE_ROTATE_90 6
#define EYELEVEL 7
#define FLOORLEVEL 8

#define LABEL_VIEW 4

#define LIST_VIEW 5
#define ADD_VIEW 6
#define DELETE_VIEW 7
#define RESTORE_VIEW 8
#define REPLACE_VIEW 9
#define STARTUP 10
#define CYCLEVIEWS 11
#define ZOOM 12
#define APERTURE 15
#define CURSOR 13
#define SAVE_SETTINGS 14
#define WINDOW_RESIZE 16
#define WINDOWSIZE_LIST 17
#define SNAPVIEW 21
#define SET_VIEW_XYZ 22
#define GSLICE_TRANSLATE 24
#define GSLICE_NORMAL 27

#define RENDER_TYPE 0
#define RENDER_SIZE_LIST 1
#define RENDER_SKIP_LIST 2
#define RENDER_START 3
#define RENDER_STOP 4
#define RENDER_LABEL 5

void EYEVIEW_CB(int var);
void BUTTON_hide2_CB(int var);
void BUTTON_Reset_CB(int var);
void GSLICE_CB(int var);

GLUI_Listbox *meshlist1=NULL;
GLUI_Listbox *render_size_list=NULL;
GLUI_Listbox *render_skip_list=NULL;
GLUI_Button *render_start=NULL ;
GLUI_Button *render_stop=NULL ;

GLUI *glui_motion=NULL;
GLUI_Panel *panel_rotatebuttons=NULL, *panel_translate=NULL,*panel_close=NULL;
GLUI_Panel *panel_gslice=NULL;
GLUI_Panel *panel_gslice_center=NULL;
GLUI_Panel *panel_gslice_normal=NULL;
GLUI_Panel *panel_gslice_show=NULL;
GLUI_Spinner *SPINNER_clip_left=NULL;
GLUI_Spinner *SPINNER_clip_right=NULL;
GLUI_Spinner *SPINNER_clip_bottom=NULL;
GLUI_Spinner *SPINNER_clip_top=NULL;
GLUI_Checkbox *CHECKBOX_clip_rendered_scene=NULL;

GLUI_Spinner *SPINNER_gslice_center_x=NULL;
GLUI_Spinner *SPINNER_gslice_center_y=NULL;
GLUI_Spinner *SPINNER_gslice_center_z=NULL;
GLUI_Spinner *SPINNER_gslice_normal_az=NULL;
GLUI_Spinner *SPINNER_gslice_normal_elev=NULL;
GLUI_Translation *gslice_htranslate=NULL, *gslice_vtranslate=NULL;

GLUI_Panel *panel_rotate=NULL;
GLUI_Panel *panel_speed=NULL;
GLUI_Panel *panel_height=NULL;
GLUI_Rollout *panel_motion=NULL;
GLUI_Rollout *panel_scene_clip=NULL;
GLUI_Rollout *panel_specify=NULL;
GLUI_Panel *panel_translate2=NULL,*panel_translate3=NULL;
GLUI_Rollout *panel_projection=NULL;
GLUI_Panel *panel_anglebuttons=NULL;
GLUI_RadioGroup *projection_radio=NULL,*eyeview_radio=NULL;
GLUI_RadioGroup *render_type_radio=NULL;
GLUI_RadioGroup *render_label_radio=NULL;
GLUI_Translation *rotate_zx=NULL,*eyerotate_z=NULL;
GLUI_Translation *translate_z=NULL,*translate_xy=NULL;
GLUI_Checkbox *blockpath_checkbox=NULL,*cursor_checkbox=NULL;
GLUI_Checkbox *CHECKBOX_gslice_data=NULL;
GLUI_Button *eyerotate90_z=NULL,*eyelevel=NULL, *floorlevel=NULL, *reset_saved_view=NULL;
GLUI_Button *replace_view=NULL,*add_view=NULL,*delete_view=NULL;
GLUI_Button *startup_button=NULL,*cycle_views_button=NULL;
GLUI_Rollout *reset_panel=NULL;
GLUI_Rollout *render_panel=NULL;
GLUI_Panel *reset_panel1=NULL;
GLUI_Panel *reset_panel2=NULL;
GLUI_EditText *edit_view_label=NULL;
GLUI_Listbox *view_lists=NULL;
GLUI_Listbox *LIST_windowsize=NULL;
GLUI_Spinner *SPINNER_set_view_x=NULL;
GLUI_Spinner *SPINNER_set_view_y=NULL;
GLUI_Spinner *SPINNER_set_view_z=NULL;
GLUI_Spinner *SPINNER_zoom=NULL,*SPINNER_aperture=NULL;
GLUI_Spinner *SPINNER_speed_crawl=NULL, *SPINNER_speed_walk=NULL;
GLUI_Spinner *SPINNER_xx=NULL, *SPINNER_yy=NULL, *SPINNER_zz=NULL;
GLUI_Spinner *SPINNER_window_width=NULL, *SPINNER_window_height=NULL;
GLUI_Button *window_update=NULL ;
GLUI_Button *button_snap=NULL;
GLUI_Panel *panel_scale=NULL;
GLUI_Spinner *SPINNER_scalex=NULL;
GLUI_Spinner *SPINNER_scaley=NULL;
GLUI_Spinner *SPINNER_scalez=NULL;
GLUI_Spinner *SPINNER_nearclip=NULL;
GLUI_Spinner *SPINNER_farclip=NULL;
GLUI_RadioButton *RADIO_button_1a=NULL;
GLUI_RadioButton *RADIO_button_1b=NULL;
GLUI_RadioButton *RADIO_button_1c=NULL;
GLUI_RadioButton *RADIO_button_1d=NULL;
GLUI_RadioButton *RADIO_button_1e=NULL;
GLUI_RadioButton *RADIO_button_1f=NULL;
GLUI_RadioButton *RADIO_button_1g=NULL;
GLUI_Button *BUTTON_motion_1=NULL;
GLUI_Button *BUTTON_motion_2=NULL;

void RENDER_CB(int var);
void enable_disable_views(void);

/* ------------------ update_gslice_parms ------------------------ */

void update_gslice_parms(void){
  GSLICE_CB(GSLICE_NORMAL);
  GSLICE_CB(GSLICE_TRANSLATE);
  SPINNER_gslice_center_x->set_float_val(gslice_xyz[0]);
  SPINNER_gslice_center_y->set_float_val(gslice_xyz[1]);
  SPINNER_gslice_center_z->set_float_val(gslice_xyz[2]);
  SPINNER_gslice_normal_az->set_float_val(gslice_normal_azelev[0]);
  SPINNER_gslice_normal_elev->set_float_val(gslice_normal_azelev[1]);
  CHECKBOX_gslice_data->set_int_val(vis_gslice_data);
}

/* ------------------ update_glui_set_view_xyz ------------------------ */

extern "C" void update_glui_set_view_xyz(float *xyz){
  if(xyz==NULL)return;
  if(SPINNER_set_view_x==NULL||SPINNER_set_view_y==NULL||SPINNER_set_view_z!=NULL)return;
  
  denormalize_xyz(set_view_xyz,xyz);

  SPINNER_set_view_x->set_float_val(set_view_xyz[0]);
  SPINNER_set_view_y->set_float_val(set_view_xyz[1]);
  SPINNER_set_view_z->set_float_val(set_view_xyz[2]);
}

/* ------------------ gluiIdle ------------------------ */

extern "C" void gluiIdle(void){
  GLUI_Master.set_glutIdleFunc(Idle_CB);
}

/* ------------------ gluiIdelNULL ------------------------ */

extern "C" void gluiIdleNULL(void){
  GLUI_Master.set_glutIdleFunc(NULL);
}

/* ------------------ reset_glui_view ------------------------ */

extern "C" void reset_glui_view(int ival){
  
  if(ival==-2){
    view_lists->set_int_val(-1);
    return;
  }
  if(ival!=old_listview){
    view_lists->set_int_val(ival);
  }
  if(ival==-1){
    replace_view->disable();
    edit_view_label->set_text("new view");
  }
  else{
    selected_view=ival;
    replace_view->enable();
    BUTTON_Reset_CB(RESTORE_VIEW);
    enable_disable_views();
  }
}

/* ------------------ enable_reset_saved_view ------------------------ */

extern "C" void enable_reset_saved_view(void){
  if(reset_saved_view!=NULL)reset_saved_view->enable();
}


/* ------------------ update_glui_filelabel ------------------------ */

extern "C" void update_glui_filelabel(int var){
  if(var==0||var==1){
    if(render_label_radio!=NULL){
      int val1;

      val1 = render_label_radio->get_int_val();
      if(val1!=var){
        render_label_radio->set_int_val(var);
      }
    }
  }
}

/* ------------------ update_glui_zoom ------------------------ */

extern "C" void update_glui_zoom(void){
  if(SPINNER_zoom!=NULL)SPINNER_zoom->set_float_val(zoom);
  aperture_glui=zoom2aperture(zoom);
  if(SPINNER_aperture!=NULL)SPINNER_aperture->set_float_val(aperture_glui);
}

/* ------------------ update_camera_label ------------------------ */

extern "C" void update_camera_label(void){
  edit_view_label->set_text(camera_label);
}

/* ------------------ update_cursor_checkbox ------------------------ */

extern "C" void update_cursor_checkbox(void){
  cursor_checkbox->set_int_val(cursorPlot3D);
}

/* ------------------ update_view_list ------------------------ */

extern "C" void update_view_gluilist(void){
  camera *ca;

  for(ca=camera_list_first.next;ca->next!=NULL;ca=ca->next){
    view_lists->add_item(ca->view_id,ca->name);
  }
  view_lists->set_int_val(startup_view_ini);
  selected_view=startup_view_ini;
  enable_disable_views();
  BUTTON_Reset_CB(RESTORE_VIEW);

}

/* ------------------ glui_motion_setup ------------------------ */

extern "C" void glui_motion_setup(int main_window){
  int i;
#define TRANSLATE_SPEED 0.005
  int *rotation_index;
  float *eye_xyz;

  if(camera_label!=NULL){
    FREEMEMORY(camera_label);
  }
  NewMemory((void **)&camera_label,sizeof(GLUI_String));

  strcpy(camera_label,"current");

  eye_xyz=camera_current->eye;

  update_glui_motion=0;
  if(glui_motion!=NULL){
    glui_motion->close();
    glui_motion=NULL;
  }
  glui_motion = GLUI_Master.create_glui(_("Motion/View/Render"),0,0,0);
  if(showmotion_dialog==0)glui_motion->hide();

  panel_motion = glui_motion->add_rollout(_("Scene motion"));

  panel_translate2 = glui_motion->add_panel_to_panel(panel_motion,"",GLUI_PANEL_NONE);
  d_eye_xyz[0]=0.0;
  d_eye_xyz[1]=0.0;
  d_eye_xyz[2]=0.0;
  dsave_eye_xyz[0]=0.0;
  dsave_eye_xyz[1]=0.0;
  dsave_eye_xyz[2]=0.0;

  translate_xy=glui_motion->add_translation_to_panel(panel_translate2,_("Horizontal"),GLUI_TRANSLATION_XY,d_eye_xyz,TRANSLATE_XY,TRANSLATE_CB);
  translate_xy->set_speed(TRANSLATE_SPEED);

  glui_motion->add_column_to_panel(panel_translate2,false);

  translate_z=glui_motion->add_translation_to_panel(panel_translate2,_("Vertical"),GLUI_TRANSLATION_Y,eye_xyz+2,GLUI_Z,TRANSLATE_CB);
  translate_z->set_speed(TRANSLATE_SPEED);

//  panel_rotate = glui_motion->add_panel_to_panel(panel_motion,"Rotate");
  panel_rotatebuttons = glui_motion->add_panel_to_panel(panel_motion,"",GLUI_PANEL_NONE);

  rotate_zx=glui_motion->add_translation_to_panel(panel_rotatebuttons,_("Rotate"),GLUI_TRANSLATION_XY,motion_ab,ROTATE_ZX,TRANSLATE_CB);
  glui_motion->add_column_to_panel(panel_rotatebuttons,false);

  eyerotate_z=glui_motion->add_translation_to_panel(panel_rotatebuttons,_("first person"),GLUI_TRANSLATION_X,motion_dir,EYE_ROTATE,TRANSLATE_CB);
  eyerotate_z->set_speed(180.0/(float)screenWidth);
  eyerotate_z->disable();
 
  eyeview_radio=glui_motion->add_radiogroup_to_panel(panel_motion,&eyeview,0,EYEVIEW_CB);
  RADIO_button_1c=glui_motion->add_radiobutton_to_group(eyeview_radio,_("general rotations"));
  RADIO_button_1d=glui_motion->add_radiobutton_to_group(eyeview_radio,_("first person movement"));
  RADIO_button_1e=glui_motion->add_radiobutton_to_group(eyeview_radio,_("level rotations"));

  rotation_index=&camera_current->rotation_index;
  *rotation_index=nmeshes;
  rotation_index_OLD=nmeshes;
  if(nmeshes>1){
    meshlist1 = glui_motion->add_listbox_to_panel(panel_motion,_("Rotate about:"),rotation_index,MESH_LIST,TRANSLATE_CB);
    for(i=0;i<nmeshes;i++){
      mesh *meshi;

      meshi = meshinfo + i;
      meshlist1->add_item(i,meshi->label);
    }
    meshlist1->add_item(nmeshes,_("world center"));
    meshlist1->set_int_val(*rotation_index);
  }
  panel_anglebuttons = glui_motion->add_panel_to_panel(panel_motion,"",GLUI_PANEL_NONE);
  eyerotate90_z=glui_motion->add_button_to_panel(panel_anglebuttons,"90 deg",EYE_ROTATE_90,TRANSLATE_CB);
  eyerotate90_z->disable();
  eyerotate90_z->set_alignment(GLUI_ALIGN_LEFT);
//  glui_motion->add_column_to_panel(panel_anglebuttons,false);
  button_snap=glui_motion->add_button_to_panel(panel_anglebuttons,_("Snap"),SNAPVIEW,TRANSLATE_CB);

  //glui_motion->add_column(false);


#ifdef pp_BETA
  panel_specify = glui_motion->add_rollout_to_panel(panel_motion,_("Specify"));

  SPINNER_set_view_x=glui_motion->add_spinner_to_panel(panel_specify,"x:",GLUI_SPINNER_FLOAT,set_view_xyz,SET_VIEW_XYZ,TRANSLATE_CB);
  SPINNER_set_view_y=glui_motion->add_spinner_to_panel(panel_specify,"y:",GLUI_SPINNER_FLOAT,set_view_xyz+1,SET_VIEW_XYZ,TRANSLATE_CB);
  SPINNER_set_view_z=glui_motion->add_spinner_to_panel(panel_specify,"z:",GLUI_SPINNER_FLOAT,set_view_xyz+2,SET_VIEW_XYZ,TRANSLATE_CB);
#endif

  panel_gslice = glui_motion->add_rollout(_("General slice motion"),false);
  if(gslice_xyz[0]<-1000000.0&&gslice_xyz[1]<-1000000.0&&gslice_xyz[2]<-1000000.0){
    gslice_xyz[0]=(xbar0+DENORMALIZE_X(xbar))/2.0;
    gslice_xyz[1]=(ybar0+DENORMALIZE_Y(ybar))/2.0;
    gslice_xyz[2]=(zbar0+DENORMALIZE_Z(zbar))/2.0;
  }

  panel_gslice_center = glui_motion->add_panel_to_panel(panel_gslice,_("rotation center"),true);
  SPINNER_gslice_center_x=glui_motion->add_spinner_to_panel(panel_gslice_center,"x:",GLUI_SPINNER_FLOAT,gslice_xyz,GSLICE_TRANSLATE,GSLICE_CB);
  SPINNER_gslice_center_y=glui_motion->add_spinner_to_panel(panel_gslice_center,"y:",GLUI_SPINNER_FLOAT,gslice_xyz+1,GSLICE_TRANSLATE,GSLICE_CB);
  SPINNER_gslice_center_z=glui_motion->add_spinner_to_panel(panel_gslice_center,"z:",GLUI_SPINNER_FLOAT,gslice_xyz+2,GSLICE_TRANSLATE,GSLICE_CB);
  SPINNER_gslice_center_x->set_float_limits(xbar0,DENORMALIZE_X(xbar),GLUI_LIMIT_CLAMP);
  SPINNER_gslice_center_y->set_float_limits(ybar0,DENORMALIZE_Y(ybar),GLUI_LIMIT_CLAMP);
  SPINNER_gslice_center_z->set_float_limits(zbar0,DENORMALIZE_Z(zbar),GLUI_LIMIT_CLAMP);
  GSLICE_CB(GSLICE_TRANSLATE);
  
  panel_gslice_normal = glui_motion->add_panel_to_panel(panel_gslice,_("normal"),true);
  SPINNER_gslice_normal_az=glui_motion->add_spinner_to_panel(panel_gslice_normal,"az:",GLUI_SPINNER_FLOAT,gslice_normal_azelev,GSLICE_NORMAL,GSLICE_CB);
  SPINNER_gslice_normal_elev=glui_motion->add_spinner_to_panel(panel_gslice_normal,"elev:",GLUI_SPINNER_FLOAT,gslice_normal_azelev+1,GSLICE_NORMAL,GSLICE_CB);
  GSLICE_CB(GSLICE_NORMAL);

  panel_gslice_show = glui_motion->add_panel_to_panel(panel_gslice,_("show"),true);
  CHECKBOX_gslice_data=glui_motion->add_checkbox_to_panel(panel_gslice_show,"data",&vis_gslice_data);
  glui_motion->add_checkbox_to_panel(panel_gslice_show,"triangle outline",&show_gslice_triangles);
  glui_motion->add_checkbox_to_panel(panel_gslice_show,"triangulation",&show_gslice_triangulation);
  glui_motion->add_checkbox_to_panel(panel_gslice_show,"plane normal",&show_gslice_normal);
  
  panel_projection = glui_motion->add_rollout(_("Window properties"),false);
  projection_radio=glui_motion->add_radiogroup_to_panel(panel_projection,&projection_type,PROJECTION,TRANSLATE_CB);
  RADIO_button_1a=glui_motion->add_radiobutton_to_group(projection_radio,_("Perspective"));
  RADIO_button_1b=glui_motion->add_radiobutton_to_group(projection_radio,_("Size preserving"));
  SPINNER_zoom=glui_motion->add_spinner_to_panel(panel_projection,_("Zoom"),GLUI_SPINNER_FLOAT,&zoom,ZOOM,TRANSLATE_CB);
  SPINNER_zoom->set_float_limits(0.10,10.0,GLUI_LIMIT_CLAMP);
  aperture_glui=zoom2aperture(zoom);
  SPINNER_aperture=glui_motion->add_spinner_to_panel(panel_projection,_("aperture"),GLUI_SPINNER_FLOAT,&aperture_glui,
    APERTURE,TRANSLATE_CB);
  glui_motion->add_separator_to_panel(panel_projection);
  
  LIST_windowsize = glui_motion->add_listbox_to_panel(panel_projection,_("Size:"),&windowsize_pointer,WINDOWSIZE_LIST,TRANSLATE_CB);
  LIST_windowsize->add_item(0,_("Custom"));
  LIST_windowsize->add_item(1,"-");
  LIST_windowsize->add_item(2, "320x240");
  LIST_windowsize->add_item(3, "640x480");
  LIST_windowsize->add_item(7, "720x480");
  if(max_screenWidth>=800&&max_screenHeight>=480)LIST_windowsize->add_item(4, "800x640");
  if(max_screenWidth>=1024&&max_screenHeight>=768)  LIST_windowsize->add_item(5,"1024x768");
  if(max_screenWidth>=1280&&max_screenHeight>=720)  LIST_windowsize->add_item(9,"1280x720");
  if(max_screenWidth>=1280&&max_screenHeight>=1024)  LIST_windowsize->add_item(6,"1280x1024");
  if(max_screenWidth>=1440&&max_screenHeight>=1024)  LIST_windowsize->add_item(10,"1440x1080");
  if(max_screenWidth>=1920&&max_screenHeight>=1080)  LIST_windowsize->add_item(8,"1920x1080");
  update_windowsizelist();

  SPINNER_window_width = glui_motion->add_spinner_to_panel(panel_projection,_("width"),GLUI_SPINNER_INT,&glui_screenWidth);
  SPINNER_window_width->set_int_limits(100,max_screenWidth);
  SPINNER_window_height = glui_motion->add_spinner_to_panel(panel_projection,_("height"),GLUI_SPINNER_INT,&glui_screenHeight);
  SPINNER_window_height->set_int_limits(100,max_screenHeight);
  window_update=glui_motion->add_button_to_panel(panel_projection,_("Apply"),WINDOW_RESIZE,TRANSLATE_CB);

  render_panel = glui_motion->add_rollout(_("Render"),false);
  render_type_radio=glui_motion->add_radiogroup_to_panel(render_panel,&renderfiletype,RENDER_TYPE,RENDER_CB);
  glui_motion->add_radiobutton_to_group(render_type_radio,"PNG");
#ifdef pp_JPEG
  glui_motion->add_radiobutton_to_group(render_type_radio,"JPEG");
#endif
  glui_motion->add_separator_to_panel(render_panel);
  render_label_radio=glui_motion->add_radiogroup_to_panel(render_panel,&renderfilelabel,RENDER_LABEL,RENDER_CB);
  RADIO_button_1f=glui_motion->add_radiobutton_to_group(render_label_radio,"frame number");
  RADIO_button_1g=glui_motion->add_radiobutton_to_group(render_label_radio,"time (s)");
  update_glui_filelabel(renderfilelabel);


  render_size_index=RenderWindow; 
  render_size_list = glui_motion->add_listbox_to_panel(render_panel,_("Size:"),&render_size_index,RENDER_SIZE_LIST,RENDER_CB);
  render_size_list->add_item(Render320,"320x240");
  render_size_list->add_item(Render640,"640x480");
  render_size_list->add_item(RenderWindow,_("Current"));
  render_size_list->add_item(Render2Window,_("2*Current"));
  render_size_list->set_int_val(render_size_index);

  render_skip_index=RenderOnce; 
  render_skip_list = glui_motion->add_listbox_to_panel(render_panel,_("Frames:"),&render_skip_index,RENDER_SKIP_LIST,RENDER_CB);
  render_skip_list->add_item(RenderOnce,_("One"));
  render_skip_list->add_item(1,_("All"));
  render_skip_list->add_item(2,_("Every 2nd"));
  render_skip_list->add_item(3,_("Every 3rd"));
  render_skip_list->add_item(4,_("Every 4th"));
  render_skip_list->add_item(5,_("Every 5th"));
  render_skip_list->add_item(10,_("Every 10th"));
  render_skip_list->add_item(20,_("Every 20th"));


  panel_scene_clip = glui_motion->add_rollout_to_panel(render_panel,"Clip rendered scene");
  SPINNER_clip_left=glui_motion->add_spinner_to_panel(panel_scene_clip,"left:",GLUI_SPINNER_INT,&render_clip_left);
  SPINNER_clip_left->set_int_limits(0,screenWidth);

  SPINNER_clip_right=glui_motion->add_spinner_to_panel(panel_scene_clip,"right:",GLUI_SPINNER_INT,&render_clip_right);
  SPINNER_clip_right->set_int_limits(0,screenWidth);

  SPINNER_clip_bottom=glui_motion->add_spinner_to_panel(panel_scene_clip,"bottom:",GLUI_SPINNER_INT,&render_clip_bottom);
  SPINNER_clip_bottom->set_int_limits(0,screenHeight);

  SPINNER_clip_top=glui_motion->add_spinner_to_panel(panel_scene_clip,"top:",GLUI_SPINNER_INT,&render_clip_top);
  SPINNER_clip_top->set_int_limits(0,screenHeight);

  CHECKBOX_clip_rendered_scene = glui_motion->add_checkbox_to_panel(panel_scene_clip,"clip rendered scene",&clip_rendered_scene);

  render_start=glui_motion->add_button_to_panel(render_panel,_("Start"),RENDER_START,RENDER_CB);
  render_stop=glui_motion->add_button_to_panel(render_panel,_("Stop"),RENDER_STOP,RENDER_CB);
  
  reset_panel = glui_motion->add_rollout(_("Viewpoints"),false);

  reset_panel1 = glui_motion->add_panel_to_panel(reset_panel,"",false);

  delete_view=glui_motion->add_button_to_panel(reset_panel1,_("Delete"),DELETE_VIEW,BUTTON_Reset_CB);
  delete_view_is_disabled=0;
  startup_button=glui_motion->add_button_to_panel(reset_panel1,_("view at startup"),STARTUP,BUTTON_Reset_CB);
  cycle_views_button=glui_motion->add_button_to_panel(reset_panel1,_("Cycle"),CYCLEVIEWS,BUTTON_Reset_CB);
  view_lists = glui_motion->add_listbox_to_panel(reset_panel1,_("Select"),&i_view_list,LIST_VIEW,BUTTON_Reset_CB);
  view_lists->add_item(-1,"-");

  glui_motion->add_column_to_panel(reset_panel,true);
  reset_panel2 = glui_motion->add_panel_to_panel(reset_panel,"",false);

  replace_view=glui_motion->add_button_to_panel(reset_panel2,_("Replace"),REPLACE_VIEW,BUTTON_Reset_CB);
  add_view=glui_motion->add_button_to_panel(reset_panel2,_("Add"),ADD_VIEW,BUTTON_Reset_CB);
  edit_view_label=glui_motion->add_edittext_to_panel(reset_panel2,_("View name"),GLUI_EDITTEXT_TEXT,camera_label,LABEL_VIEW,BUTTON_Reset_CB);

  panel_scale = glui_motion->add_rollout(_("Scaling/Depth params"),false);
  SPINNER_scalex=glui_motion->add_spinner_to_panel(panel_scale,_("Scale x"),GLUI_SPINNER_FLOAT,mscale);
  SPINNER_scalex->set_float_limits(0.01,100.0,GLUI_LIMIT_CLAMP);

  SPINNER_scaley=glui_motion->add_spinner_to_panel(panel_scale,_("Scale y"),GLUI_SPINNER_FLOAT,mscale+1);
  SPINNER_scaley->set_float_limits(0.01,100.0,GLUI_LIMIT_CLAMP);

  SPINNER_scalez=glui_motion->add_spinner_to_panel(panel_scale,_("Scale z"),GLUI_SPINNER_FLOAT,mscale+2);
  SPINNER_scalez->set_float_limits(0.01,100.0,GLUI_LIMIT_CLAMP);

  SPINNER_nearclip=glui_motion->add_spinner_to_panel(panel_scale,_("Near depth"),GLUI_SPINNER_FLOAT,&nearclip);
  SPINNER_nearclip->set_float_limits(0.001,10.0,GLUI_LIMIT_CLAMP);

  SPINNER_farclip=glui_motion->add_spinner_to_panel(panel_scale,_("Far depth"),GLUI_SPINNER_FLOAT,&farclip);
  SPINNER_farclip->set_float_limits(0.001,10.0,GLUI_LIMIT_CLAMP);

  cursor_checkbox=glui_motion->add_checkbox(_("Map cursor keys for Plot3D use"),&cursorPlot3D,CURSOR,TRANSLATE_CB);
  panel_close = glui_motion->add_panel("",GLUI_PANEL_NONE);

  BUTTON_motion_1=glui_motion->add_button_to_panel(panel_close,_("Save settings"),SAVE_SETTINGS,BUTTON_hide2_CB);

  glui_motion->add_column_to_panel(panel_close,false);

  BUTTON_motion_2=glui_motion->add_button_to_panel(panel_close,_("Close"),1,BUTTON_hide2_CB);

  showhide_translate(eyeview);
  glui_motion->set_main_gfx_window( main_window );
}

/* ------------------ enable_disable_views ------------------------ */

void enable_disable_views(void){
  int ival;

  ival=view_lists->get_int_val();
  if(ival==-1)return;
  selected_view=ival;
  camera *cex;

  cex=&camera_list_first;
  cex=cex->next;
  cex=cex->next;
  cex=cex->next;
  if(cex->next==NULL){
    cycle_views_button->disable();
  }
  else{
    cycle_views_button->enable();
  }

  switch (ival){
  case 0:
  case 1:
    replace_view->disable();
    delete_view->disable();
   // edit_view_label->disable();
    break;
  default:
    edit_view_label->enable();
    replace_view->enable();
    add_view->enable();
    delete_view->enable();

    break;
  }
}

/* ------------------ update_windowsizelist ------------------------ */

extern "C" void update_windowsizelist(void){
  windowsize_pointer=0;
  glui_screenWidth=screenWidth;
  glui_screenHeight=screenHeight;
  if(SPINNER_window_width!=NULL)SPINNER_window_width->set_int_val(screenWidth);
  if(SPINNER_window_height!=NULL)SPINNER_window_height->set_int_val(screenHeight);

  if(screenWidth==320&&screenHeight==240){
    windowsize_pointer=2;
  }
  if(screenWidth==720&&screenHeight==480){
    windowsize_pointer=7;
  }
  if(screenWidth==640&&screenHeight==480){
    windowsize_pointer=3;
  }
  if(screenWidth==800&&screenHeight==640){
    windowsize_pointer=4;
  }
  if(screenWidth==1024&&screenHeight==768){
    windowsize_pointer=5;
  }
  if(screenWidth==1280&&screenHeight==1024){
    windowsize_pointer=6;
  }
  if(screenWidth==1920&&screenHeight==1080){
    windowsize_pointer=8;
  }
  if(screenWidth==1440&&screenHeight==1080){
    windowsize_pointer=10;
  }
  if(screenWidth==1280&&screenHeight==720){
    windowsize_pointer=9;
  }
  if(LIST_windowsize!=NULL)LIST_windowsize->set_int_val(windowsize_pointer);
}

/* ------------------ update_translate ------------------------ */

extern "C" void update_translate(void){
  float *eye_xyz,*angle_zx;

  eye_xyz = camera_current->eye;
  angle_zx = camera_current->angle_zx;

  d_eye_xyz[0]=eye_xyz[0]-eye_xyz0[0];
  d_eye_xyz[1]=eye_xyz[1]-eye_xyz0[1];
  d_eye_xyz[2]=eye_xyz[2]-eye_xyz0[2];

  translate_xy->set_x(d_eye_xyz[0]);
  if(eyeview==WORLD_CENTERED_LEVEL){
    d_eye_xyz[1]=0.0;
  }
  translate_xy->set_y(d_eye_xyz[1]);
  translate_z->set_y(eye_xyz[2]);
  rotate_zx->set_x(angle_zx[0]);
  rotate_zx->set_y(angle_zx[1]);
  eyerotate_z->set_x(camera_current->direction_angle);
}

/* ------------------ update_rotation_index ------------------------ */

void update_rotation_index(int val){
  mesh *meshi;
  int i;
  float *modelview_rotate;
  float *angle_zx;
  int *rotation_index;

  rotation_index = &camera_current->rotation_index;

  *rotation_index=val;
  if(*rotation_index==rotation_index_OLD)return;
  if(*rotation_index>=0&&*rotation_index<nmeshes){
    meshi = meshinfo + *rotation_index;
    camera_current->xcen=meshi->xcen;
    camera_current->ycen=meshi->ycen;
    camera_current->zcen=meshi->zcen;
  }
  else{
    camera_current->xcen=xcenGLOBAL;
    camera_current->ycen=ycenGLOBAL;
    camera_current->zcen=zcenGLOBAL;
  }
  rotation_index_OLD=*rotation_index;
  modelview_rotate = camera_current->modelview;
  for(i=0;i<16;i++){
    modelview_rotate[i]=modelview_current[i];
  }

  angle_zx = camera_current->angle_zx;

  angle_zx[0]=0.; 
  angle_zx[1]=0.; 

  camera_current->direction_angle=0.0;
  camera_current->cos_direction_angle = 1.0;
  camera_current->sin_direction_angle = 0.0;

  camera_current->view_angle=0.0;
  camera_current->cos_view_angle = 1.0;
  camera_current->sin_view_angle = 0.0;

  update_meshlist1(val);

  glutPostRedisplay();

}

/* ------------------ update_projection_type ------------------------ */

extern "C" void update_projection_type(void){
  if(projection_radio!=NULL)projection_radio->set_int_val(projection_type);
  if(projection_type==1){
    if(SPINNER_zoom!=NULL)    SPINNER_zoom->disable();
    if(SPINNER_aperture!=NULL)SPINNER_aperture->disable();
  }
  else{
    if(SPINNER_zoom!=NULL)    SPINNER_zoom->enable();
    if(SPINNER_aperture!=NULL)SPINNER_aperture->enable();
  }
}

extern "C" void update_eyerotate(void){
  eyerotate_z->set_x(camera_current->direction_angle);
}

/* ------------------ showhide_translate ------------------------ */

extern "C" void showhide_translate(int var){
  float *eye_xyz;

  eye_xyz = camera_current->eye;

  eye_xyz0[0]=eye_xyz[0];
  eye_xyz0[1]=eye_xyz[1];
  eye_xyz0[2]=eye_xyz[2];
  d_eye_xyz[0]=0.0;
  d_eye_xyz[1]=0.0;
  switch (var){
  case WORLD_CENTERED:
    if(panel_translate!=NULL)panel_translate->enable();
    if(rotate_zx!=NULL)rotate_zx->enable();
    if(eyerotate_z!=NULL)eyerotate_z->disable();
    if(eyerotate90_z!=NULL)eyerotate90_z->disable();
    if(blockpath_checkbox!=NULL)blockpath_checkbox->disable();
    if(panel_speed!=NULL)panel_speed->disable();
    if(panel_height!=NULL)panel_height->disable();
    if(eyeview_radio!=NULL)eyeview_radio->set_int_val(eyeview);
    if(eyelevel!=NULL)eyelevel->disable();
    if(floorlevel!=NULL)floorlevel->disable();
    if(meshlist1!=NULL)meshlist1->enable();
    if(button_snap!=NULL)button_snap->enable();
    break;
  case EYE_CENTERED:
    if(panel_translate!=NULL)panel_translate->enable();
    if(rotate_zx!=NULL)rotate_zx->disable();
    if(eyerotate_z!=NULL)eyerotate_z->enable();
    if(eyerotate90_z!=NULL)eyerotate90_z->enable();
    if(blockpath_checkbox!=NULL)blockpath_checkbox->enable();
    if(panel_speed!=NULL)panel_speed->enable();
    if(panel_height!=NULL)panel_height->enable();
    if(eyeview_radio!=NULL)eyeview_radio->set_int_val(eyeview);
    if(eyelevel!=NULL)eyelevel->enable();
    if(floorlevel!=NULL)floorlevel->enable();
    if(meshlist1!=NULL)meshlist1->disable();
    if(button_snap!=NULL)button_snap->disable();
    break;
  case WORLD_CENTERED_LEVEL:
    if(panel_translate!=NULL)panel_translate->enable();
    if(rotate_zx!=NULL)rotate_zx->enable();
    if(eyerotate_z!=NULL)eyerotate_z->disable();
    if(eyerotate90_z!=NULL)eyerotate90_z->disable();
    if(blockpath_checkbox!=NULL)blockpath_checkbox->disable();
    if(panel_speed!=NULL)panel_speed->disable();
    if(panel_height!=NULL)panel_height->disable();
    if(eyeview_radio!=NULL)eyeview_radio->set_int_val(eyeview);
    if(eyelevel!=NULL)eyelevel->disable();
    if(floorlevel!=NULL)floorlevel->disable();
    if(meshlist1!=NULL)meshlist1->enable();
    if(button_snap!=NULL)button_snap->enable();
    break;
  default:
    ASSERT(FFALSE);
  }

}

/* ------------------ GSLICE_CB ------------------------ */

void GSLICE_CB(int var){
  float az, elev;

  switch(var){
    case GSLICE_NORMAL:
      az = gslice_normal_azelev[0];
      if(az<-180.0||az>180.0){
        az+=180.0;
        az=fmod((double)az,360.0);
        if(az<0.0)az+=360.0;
        az-=180.0;
        SPINNER_gslice_normal_az->set_float_val(az);
      }
      elev = gslice_normal_azelev[1];
      if(elev<-180.0||elev>180.0){
        elev+=180.0;
        elev=fmod((double)elev,360.0);
        if(elev<0)elev+=360.0;
        elev-=180.0;
        SPINNER_gslice_normal_elev->set_float_val(elev);
      }
      az*=(3.14159/180.0);
      elev*=(3.14159/180.0);
      gslice_norm[0]=cos(az)*cos(elev);
      gslice_norm[1]=sin(az)*cos(elev);;
      gslice_norm[2]=sin(elev);
      break;
    case GSLICE_TRANSLATE:
      gslice_xyz[0]=CLAMP(gslice_xyz[0],xbar0,DENORMALIZE_X(xbar));
      gslice_xyz[1]=CLAMP(gslice_xyz[1],ybar0,DENORMALIZE_Y(ybar));
      gslice_xyz[2]=CLAMP(gslice_xyz[2],zbar0,DENORMALIZE_Z(zbar));
      break;
  }
}

/* ------------------ TRANSLATE_CB ------------------------ */

extern "C" void TRANSLATE_CB(int var){
  float dx, dy;
  float dx2, dy2;
  float *eye_xyz;
  float *direction_angle;
  float *cos_direction_angle, *sin_direction_angle;
  int *rotation_index;
  int i;

#ifdef pp_GPUTHROTTLE
  if(usegpu==1&&showvolrender==1&&show_volsmoke_moving==1&&
     (var==EYE_ROTATE||var==EYE_ROTATE_90||var==ROTATE_ZX||var==TRANSLATE_XY||var==GLUI_Z)
    ){
    float fps;

    thisMOTIONtime=glutGet(GLUT_ELAPSED_TIME)/1000.0;
    fps = MOTIONnframes/(thisMOTIONtime-lastMOTIONtime);
    if(fps>GPU_VOLframemax)return;
    MOTIONnframes++;
    if(thisMOTIONtime>lastMOTIONtime+0.25){
      printf("MOTION: %4.1f fps\n",fps);
      lastMOTIONtime=thisMOTIONtime;
      MOTIONnframes=0;
    }
  }
#endif

  if(var==CURSOR){
    updatemenu=1;
    return;
  }
  eye_xyz = camera_current->eye;
  direction_angle=&camera_current->direction_angle;
  cos_direction_angle=&camera_current->cos_direction_angle;
  sin_direction_angle=&camera_current->sin_direction_angle;
  rotation_index = &camera_current->rotation_index;
  if(selected_view!=-999){
    selected_view=-999;
    updatemenu=1;
  }

  switch (var){

    case EYE_ROTATE:
      *direction_angle=motion_dir[0];
      *cos_direction_angle = cos(PI*(*direction_angle)/180.0);
      *sin_direction_angle = sin(PI*(*direction_angle)/180.0);
      if(glui_move_mode!=EYE_ROTATE){
        eye_xyz0[0]=eye_xyz[0];
        eye_xyz0[1]=eye_xyz[1];
        update_translate();
      }
      glui_move_mode=EYE_ROTATE;
      return;
    case EYE_ROTATE_90:
      {
        float diffangle;
        int intangle;

        intangle = (int)((*direction_angle+45)/90)*90.0;
        diffangle = *direction_angle-intangle;
        if(diffangle<0.0)diffangle = -diffangle;
        if(diffangle>1.0){
          *direction_angle=intangle;
        }
        else{
          *direction_angle+=90.0;
        }
      }
      if(*direction_angle>=360.0)*direction_angle-=360.0;
      TRANSLATE_CB(EYE_ROTATE);
      glui_move_mode=EYE_ROTATE_90;
      return;
    case ROTATE_ZX:
      {
        float *angle_zx;
        angle_zx = camera_current->angle_zx;
        angle_zx[0] = rotate_zx->get_x();
        angle_zx[1] = -rotate_zx->get_y();
      }
      break;
    case WINDOWSIZE_LIST:
      switch (windowsize_pointer){
        case 2:
          glui_screenWidth=320;
          glui_screenHeight=240;
          break;
        case 3:
          glui_screenWidth=640;
          glui_screenHeight=480;
          break;
        case 7:
          glui_screenWidth=720;
          glui_screenHeight=480;
          break;
        case 8:
          glui_screenWidth=1920;
          glui_screenHeight=1080;
          break;
        case 9:
          glui_screenWidth=1280;
          glui_screenHeight=720;
          break;
        case 10:
          glui_screenWidth=1440;
          glui_screenHeight=1080;
          break;
        case 4:
          glui_screenWidth=800;
          glui_screenHeight=640;
          break;
        case 5:
          glui_screenWidth=1024;
          glui_screenHeight=768;
          break;
        case 6:
          glui_screenWidth=1280;
          glui_screenHeight=1024;
          break;
        default:
          ASSERT(0);
          break;
      }
      if(windowsize_pointer>=2){
        SPINNER_window_width->set_int_val(glui_screenWidth);
        SPINNER_window_height->set_int_val(glui_screenHeight);
        setScreenSize(&glui_screenWidth,&glui_screenHeight);
        ResizeWindow(screenWidth,screenHeight);
      }
      break;
    case SNAPVIEW:
      snap_view_angles();
      break;
    case WINDOW_RESIZE:
      setScreenSize(&glui_screenWidth,&glui_screenHeight);
      update_windowsizelist();
      ResizeWindow(screenWidth,screenHeight);
      break;
    case PROJECTION:
      ZoomMenu(-2);
      camera_current->projection_type=projection_type;
      return;
    case EYELEVEL:
      desired_view_height=1.5;
      break;
    case FLOORLEVEL:
      desired_view_height=0.6;
      break;
    case MESH_LIST:
      if(*rotation_index>=0&&*rotation_index<nmeshes){
        update_current_mesh(meshinfo + (*rotation_index));
        update_rotation_index(*rotation_index);
      }
      else{
        update_current_mesh(meshinfo);
        update_rotation_index(nmeshes);
      }
      return;
    case ZOOM:
      zoomindex=-1;
      for(i=0;i<5;i++){
        if(ABS(zoom-zooms[i])<0.001){
          zoomindex=i;
          zoom=zooms[i];
          break;
        }
      }
      camera_current->zoom=zoom;
      aperture_glui=zoom2aperture(zoom);
      if(SPINNER_aperture!=NULL)SPINNER_aperture->set_float_val(aperture_glui);
      break;
    case APERTURE:
      zoom=aperture2zoom(aperture_glui);
      if(zoom<0.1||zoom>10.0){
        if(zoom<0.1)zoom=0.1;
        if(zoom>10.0)zoom=10.0;
        aperture_glui=zoom2aperture(zoom);
        if(SPINNER_aperture!=NULL)SPINNER_aperture->set_float_val(aperture_glui);
      }
      zoomindex=-1;
      for(i=0;i<5;i++){
        if(ABS(zoom-zooms[i])<0.001){
          zoomindex=i;
          zoom=zooms[i];
          aperture_glui=zoom2aperture(zoom);
          if(SPINNER_aperture!=NULL)SPINNER_aperture->set_float_val(aperture_glui);
          break;
        }
      }
      camera_current->zoom=zoom;
      if(SPINNER_zoom!=NULL)SPINNER_zoom->set_float_val(zoom);
      break;
    case SET_VIEW_XYZ:
    case TRANSLATE_XY:
    case GLUI_Z:
      break;
    default:
      ASSERT(0);
      break;
  }
  
  dx = d_eye_xyz[0];
  dy = d_eye_xyz[1];
  if(var==EYE_ROTATE){
    dy=motion_dir[1]*TRANSLATE_SPEED*(float)screenWidth/1800.0;
  }
  if(eyeview==EYE_CENTERED){
    *cos_direction_angle = cos(PI*(*direction_angle)/180.0);
    *sin_direction_angle = sin(PI*(*direction_angle)/180.0);
    dx2 = *cos_direction_angle*dx + *sin_direction_angle*dy;
    dy2 = -(*sin_direction_angle)*dx + (*cos_direction_angle)*dy;
    dx = dx2;
    dy = dy2;
  }

  if(glui_move_mode==EYE_ROTATE){
    getnewpos(eye_xyz,dx,dy,0.0,1.0);
    eye_xyz0[0]=eye_xyz[0];
    eye_xyz0[1]=eye_xyz[1];
    eye_xyz0[2]=eye_xyz[2];
  }
  else{
    eye_xyz[0] = eye_xyz0[0] + dx;
    eye_xyz[1] = eye_xyz0[1] + dy;
  }

  switch (var){
    case SET_VIEW_XYZ:
      reset_glui_view(-1);
      normalize_xyz(eye_xyz,set_view_xyz);
      eye_xyz0[0]=eye_xyz[0];
      eye_xyz0[1]=eye_xyz[1];
      eye_xyz0[2]=eye_xyz[2];
      update_translate();
      break;
    case EYE_ROTATE:
    case TRANSLATE_XY:
      reset_glui_view(-1);
      if(glui_move_mode==EYE_ROTATE){
        eye_xyz0[0]=eye_xyz[0];
        eye_xyz0[1]=eye_xyz[1];
        update_translate();
      }
      if(translate_xy!=NULL){
        translate_xy->set_x(d_eye_xyz[0]);
        translate_xy->set_y(d_eye_xyz[1]);
      }
      glui_move_mode=TRANSLATE_XY;
      break;
    case GLUI_Z:
      reset_glui_view(-1);
      if(glui_move_mode==EYE_ROTATE){
        eye_xyz0[0]=eye_xyz[0];
        eye_xyz0[1]=eye_xyz[1];
        update_translate();
      }
      glui_move_mode=GLUI_Z;
      break;
    case APERTURE:
    case ZOOM:
    case FLOORLEVEL:
    case PROJECTION:
    case WINDOW_RESIZE:
    case WINDOWSIZE_LIST:
    case SNAPVIEW:
      break;
    case ROTATE_ZX:
      reset_glui_view(-1);
      break;
    default:
      ASSERT(FFALSE);
      break;
  }
}

/* ------------------ update_meshlist1 ------------------------ */

extern "C" void update_meshlist1(int val){
  if(meshlist1==NULL)return;
  meshlist1->set_int_val(val);
  if(val>=0&&val<nmeshes){
    eyeview_radio->set_int_val(0);
    handle_eyeview(0);
  }
}

/* ------------------ hide_glui_motion ------------------------ */

extern "C" void hide_glui_motion(void){
  if(glui_motion!=NULL)glui_motion->hide();
  showmotion_dialog_save=showmotion_dialog;
  showmotion_dialog=0;
}

/* ------------------ show_glui_motion_setup ------------------------ */

extern "C" void show_glui_motion(void){
  showmotion_dialog=1;
  if(glui_motion!=NULL)glui_motion->show();
}

/* ------------------ BUTTON_hide2_CB ------------------------ */

void BUTTON_hide2_CB(int var){
  switch (var){
  case 1:
    if(glui_motion!=NULL)glui_motion->hide();
    showmotion_dialog=0;
    updatemenu=1;
    break;
  case SAVE_SETTINGS:
    updatemenu=1;
    writeini(LOCAL_INI);
    break;
  default:
    ASSERT(0);
    break;
  }
}

/* ------------------ dup_view ------------------------ */

int dup_view(void){
  camera *ca;
  char *label;

  label=edit_view_label->get_text();
  if(label==NULL)return 0;
  for(ca=camera_list_first.next;ca->next!=NULL;ca=ca->next){
    if(strcmp(label,ca->name)==0)return 1;
  }
  return 0;
}

/* ------------------ BUTTON_Reset_CB ------------------------ */

void BUTTON_Reset_CB(int var){
  int ival;
  int eyeview_save;
  camera *cam1,*cex,*ca;
  char *label;
  camera *prev, *next;
  int view_id;

  switch (var){
  case RESTORE_EXTERIOR_VIEW:
  case RESTORE_INTERIOR_VIEW:
  case RESTORE_SAVED_VIEW:
    ResetView(var);
    break;
  case SAVE_VIEW:
    strcpy(camera_current->name,camera_label);
    reset_saved_view->enable();
    ViewpointMenu(SAVE_VIEW);
    break;
  case LABEL_VIEW:
    updatemenu=1;
    break;
  case REPLACE_VIEW:
    ival=view_lists->get_int_val();
    selected_view=ival;
    label=edit_view_label->get_text();
    cex=&camera_list_first;
    cex=cex->next;
    cex=cex->next;
    for(ca=cex;ca->next!=NULL;ca=ca->next){
      if(ca->view_id==ival)break;
    }
    if(ival==ca->view_id){
      cam1=ca;
    }
    else{
      return;
    }
    prev=ca->prev;
    next=ca->next;
    view_id=ca->view_id;
    copy_camera(ca,camera_current);
    ca->prev=prev;
    ca->next=next;
    ca->view_id=view_id;

    view_lists->delete_item(ival);
    view_lists->add_item(ival,label);
    strcpy(ca->name,label);

    break;
  case ADD_VIEW:
    if(dup_view()==0){
      add_list_view(NULL);
    }
    else{
      BUTTON_Reset_CB(REPLACE_VIEW);
    }
    break;
  case DELETE_VIEW:
    ival=view_lists->get_int_val();
    label=edit_view_label->get_text();
    cex=&camera_list_first;
    cex=cex->next;
    cex=cex->next;
    for(ca=cex;ca->next!=NULL;ca=ca->next){
      if(ca->view_id==ival)break;
    }
    if(ival==ca->view_id){
      cam1=ca;
    }
    else{
      return;
    }
    view_lists->delete_item(ival);
    prev=cam1->prev;
    delete_camera(cam1);
    if(prev->view_id!=-1){
      view_lists->set_int_val(prev->view_id);
      selected_view=prev->view_id;
    }
    else{
      view_lists->set_int_val(0);
      selected_view=0;
    }
    BUTTON_Reset_CB(RESTORE_VIEW);
    enable_disable_views();
    break;
  case RESTORE_VIEW:
    ival=view_lists->get_int_val();
    selected_view=ival;
    for(ca=camera_list_first.next;ca->next!=NULL;ca=ca->next){
      if(ca->view_id==ival)break;
    }

   eyeview_save = ca->eyeview;
   copy_camera(camera_current,ca);
   if(strcmp(ca->name,"external")==0||strcmp(ca->name,"internal")==0){
     updatezoommenu=1;
   }
   camera_current->eyeview=eyeview_save;
   edit_view_label->set_text(ca->name);
   break;
  case LIST_VIEW:
    ival=view_lists->get_int_val();
    old_listview=-2;
    if(ival==-1&&delete_view_is_disabled==0){
      delete_view->disable();
      delete_view_is_disabled=1;
      break;
    }
    else{
      if(delete_view_is_disabled==1){
        delete_view->enable();
        delete_view_is_disabled=0;
      }
    }
    BUTTON_Reset_CB(RESTORE_VIEW);
    updatezoommenu=1;
    enable_disable_views();
    break;
  case STARTUP:
    startup_view_ini=view_lists->get_int_val();
    selected_view=startup_view_ini;
    writeini(LOCAL_INI);
    break;
  case CYCLEVIEWS:
    ival=view_lists->get_int_val();
    selected_view=ival;
    cex=&camera_list_first;
    cex=cex->next;
    cex=cex->next;
    switch (ival){
    case -1:
    case 0:
    case 1:
      cex=cex->next;
      if(cex->next==NULL)return;
      ival=cex->view_id;
      break;
    default:
      for(ca=cex;ca->next!=NULL;ca=ca->next){
        if(ca->view_id==ival)break;
      }
      cex=ca->next;
      if(cex->next==NULL){
        cex=&camera_list_first;
        cex=cex->next;
        cex=cex->next;
        cex=cex->next;
        if(cex->next==NULL)return;
        ival=cex->view_id;
      }
      else{
        ival=cex->view_id;
      }
      break;
    }
    view_lists->set_int_val(ival);
    selected_view=ival;
    BUTTON_Reset_CB(RESTORE_VIEW);
    break;
    default:
      ASSERT(FFALSE);
      break;
  }
}

/* ------------------ set_startup_view ------------------------ */

extern "C" void set_startup_view(void){
  BUTTON_Reset_CB(STARTUP);
}

/* ------------------ add_list_view ------------------------ */

extern "C" void add_list_view(char *label_in){
  int ival;
  char *label;
  camera *cam1,*cam2,*cex,*ca;

  ival=view_lists->get_int_val();
  if(ival==-1){
    view_lists->set_int_val(0);
    ival=view_lists->get_int_val();
  }
  selected_view=ival;
  label=label_in;
  if(label==NULL)label=edit_view_label->get_text();
  cex=&camera_list_first;
  cex=cex->next;
  cex=cex->next;
  for(ca=cex;ca->next!=NULL;ca=ca->next){
    if(ca->view_id==ival)break;
  }
  if(ival==ca->view_id){
    cam1=ca;
  }
  else{
    cam1=cex;
  }
  cam2 = insert_camera(cam1,camera_current,label);
  if(cam2!=NULL){
    view_lists->add_item(cam2->view_id,cam2->name);
    view_lists->set_int_val(cam2->view_id);
    selected_view=cam2->view_id;
  }
  enable_disable_views();
}

/* ------------------ EYEVIEW_CB ------------------------ */

void EYEVIEW_CB(int var){
  handle_eyeview(0);
}

/* ------------------ RENDER_CB ------------------------ */

void RENDER_CB(int var){
  updatemenu=1;
  switch (var){
    case RENDER_LABEL:
      break;
    case RENDER_TYPE:
      break;
    case RENDER_SIZE_LIST:
      RenderMenu(render_size_index);
      break;
    case RENDER_SKIP_LIST:
      break;
    case RENDER_START:
      if(render_size_index==Render2Window){
        RENDER_CB(RENDER_SIZE_LIST);
      }
      if(RenderTime==1||touring==1){
        RenderMenu(render_skip_index);
      }
      else{
        render_skip_index = RenderOnce;
        render_skip_list->set_int_val(render_skip_index);
        RenderMenu(render_skip_index);
      }
      break;
    case RENDER_STOP:
      RenderMenu(RenderCancel);
      break;
    default:
      ASSERT(0);
      break;
  }
}

/* ------------------ update_glui_render ------------------------ */

extern "C" void update_glui_render(void){
  if(RenderTime==1&&RenderTimeOld==0){
    if(render_skip_list!=NULL&&render_skip_index==RenderOnce){
      render_skip_index=1;
      render_skip_list->set_int_val(render_skip_index);
    }
  }
  RenderTimeOld=RenderTime;
}
