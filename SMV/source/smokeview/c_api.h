int set_slice_bound_min(const char *slice_type, int set, float value);
int set_slice_bound_max(const char *slice_type, int set, float value);

void printsliceinfo();

int loadsmvall(const char *input_filepath);
void renderclip(int flag, int left, int right, int bottom, int top);
void render(const char *filename);
void gsliceview(int data, int show_triangles, int show_triangulation,
                int show_normal);
void gslicepos(float x, float y, float z);
void gsliceorien(float az, float elev);
void settourkeyframe(float keyframe_time);
void settourview(int edittourArg, int mode, int show_tourlocusArg,
                 float tour_global_tensionArg);
int getframe();
void setframe(int framenumber);
float gettime();
void settime(float timeval);
int loadfile(const char *filename);
void loadinifile(const char *filepath);
int loadvfile(const char *filepath);
void loadboundary(const char *filepath);
void label(const char *label);
void load3dsmoke(const char *smoke_type);
void settimebarvisibility(int setting);
void toggletimebarvisibility();
void loadvolsmoke(int meshnumber);
void loadvolsmokeframe(int meshnumber, int framenumber, int flag);
void rendertype(const char *type);
void movietype(const char *type);
void makemovie(const char *name, const char *base, float framerate);
void loadtour(const char *tourname);
void loadparticles(const char *name);
void partclasscolor(const char *color);
void partclasstype(const char *part_type);
void plot3dprops(int variable_index, int showvector, int vector_length_index,
                 int display_type, float vector_length);
void loadplot3d(int meshnumber, float time_local);
void loadiso(const char *type);
void loadslice(const char *type, int axis, float distance);
void loadvslice(const char *type, int axis, float distance);
void unloadall();
void unloadtour();
void exit_smokeview();
void setcolorbarflip();
int getcolorbarflip();
void setviewpoint(const char *viewpoint);
void setrenderdir(const char *dir);
void setwindowsize(int width, int height);
void setgridvisibility(int selection);
void setgridparms(int x_vis, int y_vis, int z_vis, int x_plot, int y_plot, int z_plot);
void setcolorbarindex(int chosen_index);
int getcolorbarindex();
void camera_set_eyeview(int eyeview);
void camera_set_rotation_index(int rotation_index);
void camera_set_view_id(int view_id);
//void viewpoint_set_viewdir(float xcen, float ycen, float zcen);

void camera_mod_eyex(float delta);
void camera_set_eyex(float eyex);
void camera_mod_eyey(float delta);
void camera_set_eyey(float eyey);
void camera_mod_eyez(float delta);
void camera_set_eyez(float eyez);

void camera_mod_az(float delta);
void camera_set_az(float az);
void camera_mod_elev(float delta);
void camera_set_elev(float elev);

void camera_toggle_projection_type();
void camera_set_projection_type(int projection_type);

void set_sceneclip_x(int clipMin, float min, int clipMax, float max);
void set_sceneclip_x_min(int flag, float value);
void set_sceneclip_x_max(int flag, float value);
void set_sceneclip_y(int clipMin, float min, int clipMax, float max);
void set_sceneclip_y_min(int flag, float value);
void set_sceneclip_y_max(int flag, float value);
void set_sceneclip_z(int clipMin, float min, int clipMax, float max);
void set_sceneclip_z_min(int flag, float value);
void set_sceneclip_z_max(int flag, float value);
void set_clipping_mode(int mode);
