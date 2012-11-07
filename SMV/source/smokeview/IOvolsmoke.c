// $Date$ 
// $Revision$
// $Author$

// svn revision character string
char IOvolsmoke_revision[]="$Revision$";

#include "options.h"
#include "glew.h"
#include <stdio.h>  
#include <stdlib.h>
#include <math.h>
#include <float.h>
#ifdef pp_OSX
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include "string_util.h"
#include "smv_endian.h"
#include "update.h"
#include "smokeviewvars.h"
#include "IOvolsmoke.h"
#include "compress.h"

/* ----------------------- interp3d ----------------------------- */

void get_pt_smokecolor(float *smoke_tran, float **smoke_color, float dstep, float xyz[3], mesh *meshi, int *inobst, char *blank_local){
  int i, j, k;
  int ijk;
  float val000,val100,val010,val110;
  float val001,val101,val011,val111;
  float val00,val01,val10,val11;
  float val0, val1;
  int nx, ny, nz, nxy;
  float dx, dy, dz;
  float dxbar, dybar, dzbar;
  float *vv;
  int ijkcell;
  float *xplt, *yplt, *zplt;
  int ibar, jbar, kbar;
  float *smokedata_local, *firedata_local;
  float kfactor=8700.0;
  float soot_density, temperature;
  int index;
  float black[]={0.0,0.0,0.0,1.0};

  smokedata_local = meshi->volrenderinfo.smokedataptr;
  firedata_local = meshi->volrenderinfo.firedataptr;

  xplt = meshi->xplt_cen;
  yplt = meshi->yplt_cen;
  zplt = meshi->zplt_cen;
  ibar = meshi->ibar;
  jbar = meshi->jbar;
  kbar = meshi->kbar;

  dxbar = xplt[1]-xplt[0];
  dybar = yplt[1]-yplt[0];
  dzbar = zplt[1]-zplt[0];

  nx = ibar + 1;
  ny = jbar + 1;
  nz = kbar + 1;
  nxy = nx*ny;

  GETINDEX(i,xyz[0],xplt[0],dxbar,ibar);
  GETINDEX(j,xyz[1],yplt[0],dybar,jbar);
  GETINDEX(k,xyz[2],zplt[0],dzbar,kbar);

  if(blank_local!=NULL){
    ijkcell=IJKCELL(i,j,k);
    if(blank_local[ijkcell]==0){
      *inobst=1;
      return;
    }
    else{
      *inobst=0;
    }
  }

  ijk = i + j*nx + k*nxy;

  dx = (xyz[0] - xplt[i])/dxbar;
  dx = CLAMP(dx,0.0,1.0);
  dy = (xyz[1] - yplt[j])/dybar;
  dy = CLAMP(dy,0.0,1.0);
  dz = (xyz[2] - zplt[k])/dzbar;
  dz = CLAMP(dz,0.0,1.0);

  if(firedata_local!=NULL){
    float dtemp;

    vv = firedata_local + ijk;
    val000 = vv[0]; // i,j,k
    val100 = vv[1]; // i+1,j,k

    vv += nx;
    val010 = vv[0]; // i,j+1,k
    val110 = vv[1]; // i+1,j+1,k

    vv += (nxy-nx);
    val001 = vv[0]; // i,j,k+1
    val101 = vv[1]; // i+1,j,k+1

    vv += nx;
    val011 = vv[0]; // i,j+1,k+1
    val111 = vv[1]; // i+1,j+1,k+1

    val00 = MIX(dx,val100,val000);
    val10 = MIX(dx,val110,val010);
    val01 = MIX(dx,val101,val001);
    val11 = MIX(dx,val111,val011);
     val0 = MIX(dy, val10, val00);
     val1 = MIX(dy, val11, val01);
    temperature = MIX(dz,val1,val0);
    if(temperature<temperature_cutoff){
      dtemp=(temperature_cutoff-temperature_min)/128;
      GETINDEX(index,temperature,temperature_min,dtemp,128);
    }
    else{
      dtemp=(temperature_max-temperature_cutoff)/128.0;
      GETINDEX(index,temperature,temperature_cutoff,dtemp,128);
      index+=128;
    }
//    dtemp=(1200.0-20.0)/256;
//    GETINDEX(index,temperature,20.0,dtemp,256);
    *smoke_color=rgb_smokecolormap+4*index;
  }
  else{
    *smoke_color=getcolorptr(black);
  }
  if(smokedata_local!=NULL){
    vv = smokedata_local + ijk;
    val000 = vv[0]; // i,j,k
    val100 = vv[1]; // i+1,j,k

    vv += nx;
    val010 = vv[0]; // i,j+1,k
    val110 = vv[1]; // i+1,j+1,k

    vv += (nxy-nx);
    val001 = vv[0]; // i,j,k+1
    val101 = vv[1]; // i+1,j,k+1

    vv += nx;
    val011 = vv[0]; // i,j+1,k+1
    val111 = vv[1]; // i+1,j+1,k+1

    val00 = MIX(dx,val100,val000);
    val10 = MIX(dx,val110,val010);
    val01 = MIX(dx,val101,val001);
    val11 = MIX(dx,val111,val011);
     val0 = MIX(dy,val10,val00);
     val1 = MIX(dy,val11,val01);
     soot_density = MIX(dz,val1,val0);
     if(firedata_local!=NULL&&index>128)soot_density*=5.0;
    *smoke_tran = exp(-kfactor*soot_density*dstep);
  }
}

/* ------------------ init_volrender ------------------------ */

void init_volrender(void){
  int i;

  nvolrenderinfo=0;
  for(i=0;i<nmeshes;i++){
    mesh *meshi;
    volrenderdata *vr;

    meshi = meshinfo + i;
    vr = &(meshi->volrenderinfo);
    vr->rendermeshlabel=meshi->label;
    vr->fire=NULL;
    vr->smoke=NULL;
    vr->timeslist=NULL;
    vr->smokepos=NULL;
    vr->firepos=NULL;
    vr->loaded=0;
    vr->display=0;
    vr->is_compressed=0;
    vr->times_defined=0;
  }
  for(i=0;i<nsliceinfo;i++){
    slicedata *slicei;
    char *shortlabel;
    int blocknumber;
    mesh *meshi;
    volrenderdata *vr;

    slicei = sliceinfo + i;
    blocknumber = slicei->blocknumber;
    if(blocknumber<0||blocknumber>=nmeshes)continue;
    if(file_exists(slicei->reg_file)!=1)continue;

    meshi = meshinfo + blocknumber;
    if(slicei->nslicei!=meshi->ibar+1||slicei->nslicej!=meshi->jbar+1||slicei->nslicek!=meshi->kbar+1)continue;
    vr = &(meshi->volrenderinfo);
    shortlabel = slicei->label.shortlabel;

    if(STRCMP(shortlabel,"temp")==0){  
      vr->fire=slicei;
     continue;
    }
    if(STRCMP(shortlabel,"rho_Soot")==0){
      vr->smoke=slicei;
      continue;
    }
  }
  ijkbarmax=0;
  for(i=0;i<nmeshes;i++){
    mesh *meshi;

    meshi = meshinfo + i;
    if(meshi->ibar>ijkbarmax)ijkbarmax=meshi->ibar;
    if(meshi->jbar>ijkbarmax)ijkbarmax=meshi->jbar;
    if(meshi->kbar>ijkbarmax)ijkbarmax=meshi->kbar;
  }
  for(i=0;i<nmeshes;i++){
    mesh *meshi;
    volrenderdata *vr;

    meshi = meshinfo + i;
    vr = &(meshi->volrenderinfo);
    vr->ntimes=0;
    vr->firedata_full=NULL;
    vr->smokedata_full=NULL;
    vr->c_firedata_view=NULL;
    vr->c_smokedata_view=NULL;
    if(vr->smoke!=NULL){
      int nx, ny, nz, j;

      nvolrenderinfo++;
      vr->ntimes=get_volsmoke_nframes(vr);
      nx = ijkbarmax+1;
      ny = ijkbarmax+1;
      nz = ijkbarmax+1;
      NewMemory((void **)&vr->smokecolor_yz0,4*ny*nz*sizeof(float));
      NewMemory((void **)&vr->smokecolor_yz1,4*ny*nz*sizeof(float));
      NewMemory((void **)&vr->smokecolor_xz0,4*nx*nz*sizeof(float));
      NewMemory((void **)&vr->smokecolor_xz1,4*nx*nz*sizeof(float));
      NewMemory((void **)&vr->smokecolor_xy0,4*nx*ny*sizeof(float));
      NewMemory((void **)&vr->smokecolor_xy1,4*nx*ny*sizeof(float));
      if(vr->ntimes>0){
        NewMemory((void **)&vr->times,vr->ntimes*sizeof(float));
        NewMemory((void **)&vr->firepos,vr->ntimes*sizeof(LINT));
        NewMemory((void **)&vr->smokepos,vr->ntimes*sizeof(LINT));
        NewMemory((void **)&vr->firedataptrs,vr->ntimes*sizeof(float *));
        NewMemory((void **)&vr->smokedataptrs,vr->ntimes*sizeof(float *));
        NewMemory((void **)&vr->dataready,vr->ntimes*sizeof(int));
        NewMemory((void **)&vr->nfiredata_compressed,vr->ntimes*sizeof(int));
        NewMemory((void **)&vr->nsmokedata_compressed,vr->ntimes*sizeof(int));
        for(j=0;j<vr->ntimes;j++){
          vr->firedataptrs[j]=NULL;
          vr->smokedataptrs[j]=NULL;
          vr->nfiredata_compressed[j]=0;
          vr->nsmokedata_compressed[j]=0;
          vr->dataready[j]=0;
        }
      }
    }
    else{
      vr->smokecolor_yz0=NULL;
      vr->smokecolor_yz1=NULL;
      vr->smokecolor_xz0=NULL;
      vr->smokecolor_xz1=NULL;
      vr->smokecolor_xy0=NULL;
      vr->smokecolor_xy1=NULL;
    }
  }
  if(nvolrenderinfo>0){
    NewMemory((void **)&volfacelistinfo,6*nmeshes*sizeof(volfacelistdata));
    NewMemory((void **)&volfacelistinfoptrs,6*nmeshes*sizeof(volfacelistdata *));
  }
#ifdef pp_MERGESMOKE
  can_merge_smoke=Merge_Smoke();
#endif
#ifdef pp_MERGEMESH
  if(nvolrenderinfo>0){
 //   init_supermesh();
  }
#endif


}

/* ------------------ get_cum_smokecolor ------------------------ */

void get_cum_smokecolor(float *cum_smokecolor, float *xyzvert, float dstep, mesh *meshi, int iwall){
  float t_intersect, t_intersect_min=FLT_MAX, *boxmin, *boxmax;
  int i;
  int nsteps;
  float dx, dy, dz;
  float distseg, dxseg, dyseg, dzseg;
  float xyz[3];
  float sootdensum;
  float *vert_beg, *vert_end;
  int iwall_min=0;
  float xyzvals[3];
  int isteps;
  char *blank_local;
  float pt_smoketran, *pt_smokecolor;
  float tauhat,alphahat;

  boxmin = meshi->boxmin_scaled;
  boxmax = meshi->boxmax_scaled;

  // xyz(t) = xyzvert + t*(xyzvert - xyzeyeorig )
  // integrate from t=0 to t=t_intersect_min  (if outside mesh)
  //     ie from vertex to nearest wall along a line from the eye position
  //        intersecting the vertex position
  // integrate from t=-1 to t=0 (if inside mesh)
  //     ie from the eye position to the vertex position

  if(meshi->inside==1){
    vert_beg=xyzeyeorig;
    vert_end=xyzvert;
  }
  else{
    vert_beg=xyzvert;
    vert_end=xyzvals;

    dx = xyzvert[0] - xyzeyeorig[0];
    dy = xyzvert[1] - xyzeyeorig[1];
    dz = xyzvert[2] - xyzeyeorig[2];
    for(i=1;i<4;i++){
      int ii;
      float diffmin,diffmax,denom;

      ii=i-1;
      diffmin = boxmin[ii]-xyzvert[ii];
      diffmax = boxmax[ii]-xyzvert[ii];
      denom = xyzvert[ii]-xyzeyeorig[ii];
      if(iwall!=-i&&denom<0.0){
        t_intersect = diffmin/denom;
        if(t_intersect<t_intersect_min){
          t_intersect_min=t_intersect;
          iwall_min=-i;
        }
      }
      if(iwall!=i&&denom>0.0){
        t_intersect = diffmax/denom;
        if(t_intersect<t_intersect_min){
          t_intersect_min=t_intersect;
          iwall_min=i;
        }
      }
    }
    switch (iwall_min){
      case -1:
        vert_end[0] = boxmin[0];
        vert_end[1] = CLAMP(xyzvert[1] + t_intersect_min*dy,boxmin[1],boxmax[1]);
        vert_end[2] = CLAMP(xyzvert[2] + t_intersect_min*dz,boxmin[2],boxmax[2]);
        break;
      case 1:
        vert_end[0] = boxmax[0];
        vert_end[1] = CLAMP(xyzvert[1] + t_intersect_min*dy,boxmin[1],boxmax[1]);
        vert_end[2] = CLAMP(xyzvert[2] + t_intersect_min*dz,boxmin[2],boxmax[2]);
        break;
      case -2:
        vert_end[0] = CLAMP(xyzvert[0] + t_intersect_min*dx,boxmin[0],boxmax[0]);
        vert_end[1] = boxmin[1];
        vert_end[2] = CLAMP(xyzvert[2] + t_intersect_min*dz,boxmin[2],boxmax[2]);
        break;
      case 2:
        vert_end[0] = CLAMP(xyzvert[0] + t_intersect_min*dx,boxmin[0],boxmax[0]);
        vert_end[1] = boxmax[1];
        vert_end[2] = CLAMP(xyzvert[2] + t_intersect_min*dz,boxmin[2],boxmax[2]);
        break;
      case -3:
        vert_end[0] = CLAMP(xyzvert[0] + t_intersect_min*dx,boxmin[0],boxmax[0]);
        vert_end[1] = CLAMP(xyzvert[1] + t_intersect_min*dy,boxmin[1],boxmax[1]);
        vert_end[2] = boxmin[2];
        break;
      case 3:
        vert_end[0] = CLAMP(xyzvert[0] + t_intersect_min*dx,boxmin[0],boxmax[0]);
        vert_end[1] = CLAMP(xyzvert[1] + t_intersect_min*dy,boxmin[1],boxmax[1]);
        vert_end[2] = boxmax[2];
        break;
      default:
        ASSERT(0);
        break;
    }
  }

  dxseg = vert_end[0] - vert_beg[0];
  dyseg = vert_end[1] - vert_beg[1];
  dzseg = vert_end[2] - vert_beg[2];
  distseg = sqrt(dxseg*dxseg+dyseg*dyseg+dzseg*dzseg);
  if(distseg<0.001){
    cum_smokecolor[0]=0.0;
    cum_smokecolor[1]=0.0;
    cum_smokecolor[2]=0.0;
    cum_smokecolor[3]=0.0;
    return;
  }

  nsteps = distseg/dstep;
  if(nsteps<1){
    nsteps=1;
  }
  dstep=distseg/(float)nsteps;
  dstep*=xyzmaxdiff;
  sootdensum=0.0;
  isteps=0;
  if(block_volsmoke==1){
    blank_local=meshi->c_iblank_cell;
  }
  else{
    blank_local=NULL;
  }
  cum_smokecolor[0]=0.0;
  cum_smokecolor[1]=0.0;
  cum_smokecolor[2]=0.0;
  cum_smokecolor[3]=0.0;
  tauhat=1.0;
  alphahat=0.0;
  for(i=0;i<nsteps;i++){
    float factor;
    int inobst;
    float alphai;

    factor = (0.5 + (float)i)/(float)nsteps;

    xyz[0] = MIX(factor,vert_end[0],vert_beg[0]);
    xyz[1] = MIX(factor,vert_end[1],vert_beg[1]);
    xyz[2] = MIX(factor,vert_end[2],vert_beg[2]);

    get_pt_smokecolor(&pt_smoketran,&pt_smokecolor, dstep,xyz, meshi, &inobst, blank_local);
    if(blank_local!=NULL&&inobst==1)break;

    alphai = 1.0 - pt_smoketran;
    alphahat +=  alphai*tauhat;

    cum_smokecolor[0] += alphai*tauhat*pt_smokecolor[0];
    cum_smokecolor[1] += alphai*tauhat*pt_smokecolor[1];
    cum_smokecolor[2] += alphai*tauhat*pt_smokecolor[2];
    tauhat *= pt_smoketran;
  }
  if(alphahat>0.0){
    cum_smokecolor[0]/=alphahat;
    cum_smokecolor[1]/=alphahat;
    cum_smokecolor[2]/=alphahat;
    cum_smokecolor[3]=alphahat;
    if(volbw==1){
      float gray;

      gray=0.299*cum_smokecolor[0] + 0.587*cum_smokecolor[1] + 0.114*cum_smokecolor[2];
      cum_smokecolor[0] = gray;
      cum_smokecolor[1] = gray;
      cum_smokecolor[2] = gray;
    }
  }
  else{
    cum_smokecolor[0]=0.0;
    cum_smokecolor[1]=0.0;
    cum_smokecolor[2]=0.0;
    cum_smokecolor[3]=0.0;
  }
}

/* ------------------ compute_all_smokecolors ------------------------ */

void compute_all_smokecolors(void){
  int ii;

#ifdef pp_FREEZE_VOLSMOKE
  if(freeze_volsmoke==1)return;
#endif
  for(ii=0;ii<nmeshes;ii++){
    mesh *meshi;
    volrenderdata *vr;
    int iwall;
    float dstep;
    float dx, dy, dz;
    float *x, *y, *z; 
    int ibar, jbar, kbar;
    float *smokecolor;

    meshi = meshinfo + ii;
    vr = &(meshi->volrenderinfo);
    if(vr->loaded==0||vr->display==0)continue;

    x = meshi->xplt;
    y = meshi->yplt;
    z = meshi->zplt;
    ibar = meshi->ibar;
    jbar = meshi->jbar;
    kbar = meshi->kbar;
    dx = x[1] - x[0];
    dy = y[1] - y[0];
    dz = z[1] - z[0];
    dstep = sqrt(dx*dx+dy*dy+dz*dz);
    
    if(vr->smoke==NULL)continue;
    for(iwall=-3;iwall<=3;iwall++){
      float *xyz,xyzarray[3];
      int i, j;

      xyz = xyzarray;
      if(iwall==0||meshi->drawsides[iwall+3]==0)continue;
      switch (iwall){
        case 1:
        case -1:
          if(iwall<0){
            smokecolor=vr->smokecolor_yz0;
            xyz[0] = meshi->x0;
          }
          else{
            smokecolor=vr->smokecolor_yz1;
            xyz[0] = meshi->x1;
          }
          if(vr->firedataptr==NULL||vr->smokedataptr==NULL){
            for(i=0;i<=jbar;i++){
              for(j=0;j<=kbar;j++){
                smokecolor[0]=0.0;
                smokecolor[1]=0.0;
                smokecolor[2]=0.0;
                smokecolor[3]=0.0;
                smokecolor+=4;
              }
            }
          }
          else{
            for(i=0;i<=jbar;i++){
              xyz[1] = y[i];
              for(j=0;j<=kbar;j++){
                xyz[2] = z[j];
                get_cum_smokecolor(smokecolor,xyz,dstep,meshi,iwall);
                smokecolor+=4;
              }
            }
          }
          break;
        case 2:
        case -2:
          if(iwall<0){
            smokecolor=vr->smokecolor_xz0;
            xyz[1] = meshi->y0;
          }
          else{
            smokecolor=vr->smokecolor_xz1;
            xyz[1] = meshi->y1;
          }
          if(vr->firedataptr==NULL||vr->smokedataptr==NULL){
            for(i=0;i<=ibar;i++){
              for(j=0;j<=kbar;j++){
                smokecolor[0]=0.0;
                smokecolor[1]=0.0;
                smokecolor[2]=0.0;
                smokecolor[3]=0.0;
                smokecolor+=4;
              }
            }
          }
          else{
            for(i=0;i<=ibar;i++){
              xyz[0] = x[i];
              for(j=0;j<=kbar;j++){
                xyz[2] = z[j];
                get_cum_smokecolor(smokecolor,xyz,dstep,meshi,iwall);
                smokecolor+=4;
              }
            }
          }
          break;
        case 3:
        case -3:
          if(iwall<0){
            smokecolor=vr->smokecolor_xy0;
            xyz[2]=meshi->z0;
          }
          else{
            smokecolor=vr->smokecolor_xy1;
            xyz[2]=meshi->z1;
          }
          if(vr->firedataptr==NULL||vr->smokedataptr==NULL){
            for(i=0;i<=ibar;i++){
              for(j=0;j<=jbar;j++){
                smokecolor[0]=0.0;
                smokecolor[1]=0.0;
                smokecolor[2]=0.0;
                smokecolor[3]=0.0;
                smokecolor+=4;
              }
            }
          }
          else{
            for(i=0;i<=ibar;i++){
              xyz[0] = x[i];
              for(j=0;j<=jbar;j++){
                xyz[1] = y[j];
                get_cum_smokecolor(smokecolor,xyz,dstep,meshi,iwall);
                smokecolor+=4;
              }
            }
          }
          break;
        default:
          ASSERT(0);
          break;
      }
    }
  }
}

/* ------------------ drawsmoke3dVOL ------------------------ */

void drawsmoke3dVOLdebug(void){
  int ii;

  for(ii=0;ii<nvolfacelistinfo;ii++){
    volfacelistdata *vi;
    mesh *meshi;
    volrenderdata *vr;
    float x[2], y[2], z[2];
    float *xplt, *yplt, *zplt;
    int ibar, jbar, kbar;
    char label[256];
    int iwall;


    vi = volfacelistinfoptrs[ii];
    sprintf(label,"*** %i %2.1f ***",ii,vi->dist2);
    iwall=vi->iwall;
    meshi = vi->facemesh;
    xplt = meshi->xplt;
    yplt = meshi->yplt;
    zplt = meshi->zplt;
    ibar = meshi->ibar;
    jbar = meshi->jbar;
    kbar = meshi->kbar;
    vr = &(meshi->volrenderinfo);

    if(iwall==0||meshi->drawsides[iwall+3]==0)continue;
    switch (iwall){
      case 1:
      case -1:
        if(iwall<0){
          x[0] = meshi->x0;
          x[1] = x[0];
        }
        else{
          x[0]=meshi->x1;
          x[1]=x[0];
        }
        y[0] = yplt[0];
        y[1] = yplt[jbar];
        z[0] = zplt[0];
        z[1] = zplt[kbar];
        output3Text(foregroundcolor, (x[0]+x[1])/2.0,(y[0]+y[1])/2.0,(z[0]+z[1])/2.0, label);
        break;
      case 2:
      case -2:
        if(iwall<0){
          y[0] = meshi->y0;
          y[1] = y[0];
        }
        else{
          y[0] = meshi->y1;
          y[1] = y[0];
        }
        x[0] = xplt[0];
        x[1] = xplt[ibar];
        z[0] = zplt[0];
        z[1] = zplt[kbar];
        output3Text(foregroundcolor, (x[0]+x[1])/2.0,(y[0]+y[1])/2.0,(z[0]+z[1])/2.0, label);
        break;
      case 3:
      case -3:
        if(iwall<0){
          z[0] = meshi->z0;
          z[1] = z[0];
        }
        else{
          z[0] = meshi->z1;
          z[1] = z[0];
        }
        x[0] = xplt[0];
        x[1] = xplt[ibar];
        y[0] = yplt[0];
        y[1] = yplt[jbar];
        output3Text(foregroundcolor, (x[0]+x[1])/2.0,(y[0]+y[1])/2.0,(z[0]+z[1])/2.0, label);
        break;
      default:
        ASSERT(0);
        break;
    }
  }
  glBegin(GL_LINES);
  for(ii=0;ii<nvolfacelistinfo;ii++){
    volfacelistdata *vi;
    mesh *meshi;
    volrenderdata *vr;
    float x[2], y[2], z[2];
    float *xplt, *yplt, *zplt;
    int ibar, jbar, kbar;
    char label[256];
    int iwall;

    sprintf(label,"*** %i ***",ii);

    vi = volfacelistinfoptrs[ii];
    iwall=vi->iwall;
    meshi = vi->facemesh;
    xplt = meshi->xplt;
    yplt = meshi->yplt;
    zplt = meshi->zplt;
    ibar = meshi->ibar;
    jbar = meshi->jbar;
    kbar = meshi->kbar;
    vr = &(meshi->volrenderinfo);

    if(iwall==0||meshi->drawsides[iwall+3]==0)continue;
    switch (iwall){
      case 1:
      case -1:
        if(iwall<0){
          x[0] = meshi->x0;
          glColor3f(1.0,0.0,0.0);
        }
        else{
          x[0]=meshi->x1;
          glColor3f(0.0,0.0,1.0);
        }
        y[0] = yplt[0];
        y[1] = yplt[jbar];
        z[0] = zplt[0];
        z[1] = zplt[kbar];
        glVertex3f(x[0],y[0],z[0]);
        glVertex3f(x[0],y[1],z[1]);
        glVertex3f(x[0],y[1],z[0]);
        glVertex3f(x[0],y[0],z[1]);
        break;
      case 2:
      case -2:
        if(iwall<0){
          y[0] = meshi->y0;
          glColor3f(1.0,0.0,0.0);
        }
        else{
          y[0] = meshi->y1;
          glColor3f(0.0,0.0,1.0);
        }
        x[0] = xplt[0];
        x[1] = xplt[ibar];
        z[0] = zplt[0];
        z[1] = zplt[kbar];
        glVertex3f(x[0],y[0],z[0]);
        glVertex3f(x[1],y[0],z[1]);
        glVertex3f(x[0],y[0],z[1]);
        glVertex3f(x[1],y[0],z[0]);
        break;
      case 3:
      case -3:
        if(iwall<0){
          z[0] = meshi->z0;
          glColor3f(1.0,0.0,0.0);
        }
        else{
          z[0] = meshi->z1;
          glColor3f(0.0,0.0,1.0);
        }
        x[0] = xplt[0];
        x[1] = xplt[ibar];
        y[0] = yplt[0];
        y[1] = yplt[jbar];
        glVertex3f(x[0],y[0],z[0]);
        glVertex3f(x[1],y[1],z[0]);
        glVertex3f(x[0],y[1],z[0]);
        glVertex3f(x[1],y[0],z[0]);
        break;
      default:
        ASSERT(0);
        break;
    }
  }
  glEnd();
}

/* ------------------ drawsmoke3dVOL ------------------------ */

void drawsmoke3dVOL(void){
  int iwall;
  int ii;

  if(use_transparency_data==1)transparenton();
  for(ii=0;ii<nvolfacelistinfo;ii++){
    volfacelistdata *vi;
    mesh *meshi;
    volrenderdata *vr;
    int i,j;
    float xx, yy, zz;
    float x[2], y[2], z[2];
    int n00, n01, n10, n11;
    float *xplt, *yplt, *zplt;
    int ibar, jbar, kbar;
    float *smokecolor;

    vi = volfacelistinfoptrs[ii];
    iwall=vi->iwall;
    meshi = vi->facemesh;
    if(meshvisptr[meshi-meshinfo]==0)continue;
    xplt = meshi->xplt;
    yplt = meshi->yplt;
    zplt = meshi->zplt;
    ibar = meshi->ibar;
    jbar = meshi->jbar;
    kbar = meshi->kbar;
    vr = &(meshi->volrenderinfo);

    if(iwall==0||meshi->drawsides[iwall+3]==0)continue;
    if(vr->firedataptr==NULL&&vr->smokedataptr==NULL)continue;

    glBegin(GL_TRIANGLES);
    switch (iwall){
      case 1:
      case -1:
        if(iwall<0){
          xx = meshi->x0;
          smokecolor = vr->smokecolor_yz0;
        }
        else{
          xx=meshi->x1;
          smokecolor = vr->smokecolor_yz1;
        }
        n00 = 0;
        n01 = 4;
        n10 = 4*(kbar+1);
        n11 = 4*(1 + kbar+1);
        for(i=0;i<jbar;i++){
          y[0] = yplt[i];
          y[1] = yplt[i+1];
          for(j=0;j<kbar;j++){
            z[0] = zplt[j];
            z[1] = zplt[j+1];

            if(meshi->inside==0&&iwall>0||meshi->inside!=0&&iwall<0){
              glColor4fv(smokecolor+n00);
              glVertex3f(xx,y[0],z[0]);
              glColor4fv(smokecolor+n10);
              glVertex3f(xx,y[1],z[0]);
              glColor4fv(smokecolor+n11);
              glVertex3f(xx,y[1],z[1]);

              glColor4fv(smokecolor+n00);
              glVertex3f(xx,y[0],z[0]);
              glColor4fv(smokecolor+n11);
              glVertex3f(xx,y[1],z[1]);
              glColor4fv(smokecolor+n01);
              glVertex3f(xx,y[0],z[1]);
            }
            else{
              glColor4fv(smokecolor+n00);
              glVertex3f(xx,y[0],z[0]);
              glColor4fv(smokecolor+n11);
              glVertex3f(xx,y[1],z[1]);
              glColor4fv(smokecolor+n10);
              glVertex3f(xx,y[1],z[0]);

              glColor4fv(smokecolor+n00);
              glVertex3f(xx,y[0],z[0]);
              glColor4fv(smokecolor+n01);
              glVertex3f(xx,y[0],z[1]);
              glColor4fv(smokecolor+n11);
              glVertex3f(xx,y[1],z[1]);
            }
            smokecolor+=4;
          }
         smokecolor+=4;
        }
        break;
      case 2:
      case -2:
        n00 = 0;
        n01 = 4;
        n10 = 4*(kbar+1);
        n11 = 4*(1 + kbar+1);
        if(iwall<0){
          smokecolor = vr->smokecolor_xz0;
          yy=meshi->y0;
        }
        else{
          smokecolor = vr->smokecolor_xz1;
          yy=meshi->y1;
        }
        for(i=0;i<ibar;i++){
          x[0] = xplt[i];
          x[1] = xplt[i+1];
          for(j=0;j<kbar;j++){
            z[0] = zplt[j];
            z[1] = zplt[j+1];
            if(meshi->inside==0&&iwall>0||meshi->inside!=0&&iwall<0){
              glColor4fv(smokecolor+n00);
              glVertex3f(x[0],yy,z[0]);
              glColor4fv(smokecolor+n11);
              glVertex3f(x[1],yy,z[1]);
              glColor4fv(smokecolor+n10);
              glVertex3f(x[1],yy,z[0]);

              glColor4fv(smokecolor+n00);
              glVertex3f(x[0],yy,z[0]);
              glColor4fv(smokecolor+n01);
              glVertex3f(x[0],yy,z[1]);
              glColor4fv(smokecolor+n11);
              glVertex3f(x[1],yy,z[1]);
            }
            else{
              glColor4fv(smokecolor+n00);
              glVertex3f(x[0],yy,z[0]);
              glColor4fv(smokecolor+n10);
              glVertex3f(x[1],yy,z[0]);
              glColor4fv(smokecolor+n11);
              glVertex3f(x[1],yy,z[1]);

              glColor4fv(smokecolor+n00);
              glVertex3f(x[0],yy,z[0]);
              glColor4fv(smokecolor+n11);
              glVertex3f(x[1],yy,z[1]);
              glColor4fv(smokecolor+n01);
              glVertex3f(x[0],yy,z[1]);
            }
            smokecolor+=4;
          }
          smokecolor+=4;
        }
        break;
      case 3:
      case -3:
        n00 = 0;
        n01 = 4;
        n10 = 4*(jbar+1);
        n11 = 4*(1 + jbar+1);
       if(iwall<0){
          smokecolor = vr->smokecolor_xy0;
          zz=meshi->z0;
        }
        else{
          smokecolor = vr->smokecolor_xy1;
          zz=meshi->z1;
        }
        for(i=0;i<ibar;i++){
          x[0] = xplt[i];
          x[1] = xplt[i+1];
          for(j=0;j<jbar;j++){
            y[0] = yplt[j];
            y[1] = yplt[j+1];
            if(meshi->inside==0&&iwall>0||meshi->inside!=0&&iwall<0){
              glColor4fv(smokecolor+n00);
              glVertex3f(x[0],y[0],zz);
              glColor4fv(smokecolor+n10);
              glVertex3f(x[1],y[0],zz);
              glColor4fv(smokecolor+n11);
              glVertex3f(x[1],y[1],zz);

              glColor4fv(smokecolor+n00);
              glVertex3f(x[0],y[0],zz);
              glColor4fv(smokecolor+n11);
              glVertex3f(x[1],y[1],zz);
              glColor4fv(smokecolor+n01);
              glVertex3f(x[0],y[1],zz);
            }
            else{
              glColor4fv(smokecolor+n00);
              glVertex3f(x[0],y[0],zz);
              glColor4fv(smokecolor+n11);
              glVertex3f(x[1],y[1],zz);
              glColor4fv(smokecolor+n10);
              glVertex3f(x[1],y[0],zz);

              glColor4fv(smokecolor+n00);
              glVertex3f(x[0],y[0],zz);
              glColor4fv(smokecolor+n01);
              glVertex3f(x[0],y[1],zz);
              glColor4fv(smokecolor+n11);
              glVertex3f(x[1],y[1],zz);
            }
            smokecolor+=4;
          }
          smokecolor+=4;
        }
        break;
      default:
        ASSERT(0);
        break;
    }
    glEnd();
  }
  if(use_transparency_data==1)transparentoff();
}

/* ------------------ drawsmoke3dGPUVOL ------------------------ */

#ifdef pp_GPU
void drawsmoke3dGPUVOL(void){

  int iwall;
  mesh *meshold=NULL;
  int ii;
  int inside;
  int *drawsides;
  int newmesh;
  float dcell;

//  SVEXTERN int GPUload[30],GPUtime[30],SVDECL(nGPUframes,0),SVDECL(iGPUframes,0);
#ifdef pp_GPUTHROTTLE
  thisGPUtime=glutGet(GLUT_ELAPSED_TIME)/1000.0;
  if(thisGPUtime>lastGPUtime+0.25){
    printf("CPU->GPU %4.1f Mbytes/s\n",4.0*GPUnframes/(thisGPUtime-lastGPUtime)/(1024.0*1024.0));
    lastGPUtime=thisGPUtime;
    GPUnframes=0;
  }
#endif
  if(mouse_down==1&&show_volsmoke_moving==0){
    return;
  }
#ifdef pp_GPUDEPTH
  getDepthTexture();
  glUniform1i(GPUvol_depthtexture,4);
  glUniform2f(GPUvol_screensize,(float)screenWidth,(float)screenHeight);
  glUniform2f(GPUvol_nearfar,fnear,ffar);
  SNIFF_ERRORS("after drawsmoke3dGPUVOL A");
#endif
  glUniform3f(GPUvol_eyepos,xyzeyeorig[0],xyzeyeorig[1],xyzeyeorig[2]);
  glUniform1f(GPUvol_xyzmaxdiff,xyzmaxdiff);
  glUniform1f(GPUvol_opacity_factor,opacity_factor);
  glUniform1f(GPUvol_mass_extinct,mass_extinct);
  glUniform1i(GPUvol_volbw,volbw);
  glUniform1f(GPUvol_temperature_min,temperature_min);
  glUniform1f(GPUvol_temperature_cutoff,temperature_cutoff);
  glUniform1f(GPUvol_temperature_max,temperature_max);
  SNIFF_ERRORS("after drawsmoke3dGPUVOL before loop");
  if(use_transparency_data==1)transparenton();
  for(ii=0;ii<nvolfacelistinfo;ii++){
    volrenderdata *vr;
    volfacelistdata *vi;
    mesh *meshi;
    int i,j;
    float x1, x2, yy1, yy2, z1, z2;
    float xx, yy, zz;
    int ijk_offset[3]={0,0,0};

    vi = volfacelistinfoptrs[ii];
    iwall=vi->iwall;
    meshi = vi->facemesh;

    if(meshvisptr[meshi-meshinfo]==0)continue;
    if(iwall==0||meshi->drawsides[iwall+3]==0)continue;

    vr = &meshi->volrenderinfo;
    if(vr->firedataptr==NULL&&vr->smokedataptr==NULL)continue;

    // VVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVV
    // define parameters for smoke drawing
    
    if(iwall<0){
      xx = meshi->x0;
      yy = meshi->y0;
      zz = meshi->z0;
    }
    else{
      xx = meshi->x1;
      yy = meshi->y1;
      zz = meshi->z1;
    }
    x1 = meshi->x0;
    x2 = meshi->x1;
    yy1 = meshi->y0;
    yy2 = meshi->y1;
    z1 = meshi->z0;
    z2 = meshi->z1;
    dcell = meshi->dcell;;
    inside = meshi->inside;
    drawsides=meshi->drawsides;
    if(meshi!=meshold){
      newmesh=1;
    }
    else{
      newmesh=0;

    }
    if(newmesh==1){
      update_volsmoke_texture(ijk_offset,meshi,vr->smokedataptr,vr->firedataptr);
    }

    // ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
    
    
    if(newmesh==1){
      glUniform1i(GPUvol_inside,inside);
      glUniform3f(GPUvol_boxmin,x1,yy1,z1);
      glUniform3f(GPUvol_boxmax,x2,yy2,z2);
      glUniform1iv(GPUvol_drawsides,7,drawsides);
      if(vr->firedataptr!=NULL){
        glUniform1i(GPUvol_havefire,1);
      }
      else{
        glUniform1i(GPUvol_havefire,0);
      }
      glUniform1i(GPUvol_soot_density,0);
      glUniform1i(GPUvol_fire,1);
      glUniform1i(GPUvol_smokecolormap,2);
      glUniform1i(GPUvol_blockage,3);
      if(mouse_down==1){
        glUniform1f(GPUvol_dcell,4.0*dcell);
      }
      else{
        glUniform1f(GPUvol_dcell,dcell);
      }

      meshold=meshi;
    }
    glBegin(GL_TRIANGLES);

    switch (iwall){
      case 1:
      case -1:
        if(inside==0&&iwall>0||inside!=0&&iwall<0){
          glVertex3f(xx,yy1,z1);
          glVertex3f(xx,yy2,z1);
          glVertex3f(xx,yy2,z2);

          glVertex3f(xx,yy1,z1);
          glVertex3f(xx,yy2,z2);
          glVertex3f(xx,yy1,z2);
        }
        else{
          glVertex3f(xx,yy1,z1);
          glVertex3f(xx,yy2,z2);
          glVertex3f(xx,yy2,z1);

          glVertex3f(xx,yy1,z1);
          glVertex3f(xx,yy1,z2);
          glVertex3f(xx,yy2,z2);
        }
        break;
      case 2:
      case -2:
        if(inside==0&&iwall>0||inside!=0&&iwall<0){
          glVertex3f(x1,yy,z1);
          glVertex3f(x2,yy,z2);
          glVertex3f(x2,yy,z1);

          glVertex3f(x1,yy,z1);
          glVertex3f(x1,yy,z2);
          glVertex3f(x2,yy,z2);
        }
        else{
          glVertex3f(x1,yy,z1);
          glVertex3f(x2,yy,z1);
          glVertex3f(x2,yy,z2);

          glVertex3f(x1,yy,z1);
          glVertex3f(x2,yy,z2);
          glVertex3f(x1,yy,z2);
        }
        break;
      case 3:
      case -3:
        if(inside==0&&iwall>0||inside!=0&&iwall<0){
          glVertex3f(x1,yy1,zz);
          glVertex3f(x2,yy1,zz);
          glVertex3f(x2,yy2,zz);

          glVertex3f(x1,yy1,zz);
          glVertex3f(x2,yy2,zz);
          glVertex3f(x1,yy2,zz);
        }
        else{
          glVertex3f(x1,yy1,zz);
          glVertex3f(x2,yy2,zz);
          glVertex3f(x2,yy1,zz);

          glVertex3f(x1,yy1,zz);
          glVertex3f(x1,yy2,zz);
          glVertex3f(x2,yy2,zz);
        }
        break;
      default:
        ASSERT(0);
        break;
    }
    glEnd();
  }
  SNIFF_ERRORS("after drawsmoke3dGPUVOL after loop");
  if(use_transparency_data==1)transparentoff();
}
#endif

#define HEADER_SIZE 4
#define TRAILER_SIZE 4
#define FORTSLICEREAD(var,size) FSEEK(SLICEFILE,HEADER_SIZE,SEEK_CUR);\
                           returncode=fread(var,4,size,SLICEFILE);\
                           if(endianswitch==1)endian_switch(var,size);\
                           FSEEK(SLICEFILE,TRAILER_SIZE,SEEK_CUR)

/* ------------------ get_volsmoke_sizes ------------------------ */

int get_volsmoke_nframes(volrenderdata *vr){
	slicedata *fireslice, *smokeslice;
  FILE *volstream=NULL;
  int framesize;
  LINT skip_local;
  int nframes;
  FILE_SIZE filesize;

  smokeslice=vr->smoke;
  if(load_volcompressed==1&&vr->smoke->vol_file!=NULL){
    volstream=fopen(vr->smoke->vol_file,"rb");
  }
  if(volstream==NULL){
    fireslice=vr->fire;
    framesize = smokeslice->nslicei*smokeslice->nslicej*smokeslice->nslicek;
    framesize *= 4; // convert to bytes
    framesize += HEADER_SIZE + TRAILER_SIZE;

    skip_local =           (HEADER_SIZE+30        +TRAILER_SIZE); // long label
    skip_local +=          (HEADER_SIZE+30        +TRAILER_SIZE); // short label
    skip_local +=          (HEADER_SIZE+30        +TRAILER_SIZE); // unit label
    skip_local +=          (HEADER_SIZE+24        +TRAILER_SIZE); // is1, is2, js1, js2, ks1, ks2

  // nframes = (totalsize - skip_local)/(12 + framesize);

    nframes=0;
    filesize=get_filesize(smokeslice->reg_file);
    if(filesize>0){
      nframes = (int)(filesize-skip_local)/(int)(12 + framesize);
    }
  }
  else{
    unsigned char buffer[32];
// 1,completion,version
// 1,version,n_data_compressedm32,nbytes,n_data_in,time,valmin,valmax,data ....
    FSEEK(volstream,12,SEEK_SET);
    for(nframes=0;;nframes++){
      int ncompressed;

      if(fread(buffer,1,32,volstream)!=32)break;
      ncompressed=*(int *)(buffer+8)-32;
      if(FSEEK(volstream,ncompressed,SEEK_CUR)!=0)break;
    }
    fclose(volstream);
  }
  return nframes;
}

/* ------------------ get_volsmoke_frame_time ------------------------ */

float get_volsmoke_frame_time(volrenderdata *vr, int framenum){
	slicedata *smokeslice;
  FILE *SLICEFILE;
  int framesize,returncode;
  float time_local=0.0;
  int endianswitch=0;
  LINT skip_local;

  if(framenum<0||framenum>=vr->ntimes)return time_local;
  smokeslice=vr->smoke;
  framesize = smokeslice->nslicei*smokeslice->nslicej*smokeslice->nslicek;

  skip_local =           (HEADER_SIZE+30        +TRAILER_SIZE); // long label
  skip_local +=          (HEADER_SIZE+30        +TRAILER_SIZE); // short label
  skip_local +=          (HEADER_SIZE+30        +TRAILER_SIZE); // unit label
  skip_local +=          (HEADER_SIZE+24        +TRAILER_SIZE); // is1, is2, js1, js2, ks1, ks2
  skip_local += framenum*(HEADER_SIZE +4        +TRAILER_SIZE); // framenum time's
  skip_local += (LINT)framenum*(LINT)(HEADER_SIZE +4*framesize+TRAILER_SIZE); // framenum slice data's

  SLICEFILE=fopen(smokeslice->reg_file,"rb");
  if(SLICEFILE==NULL)return time_local;

  returncode=FSEEK(SLICEFILE,skip_local,SEEK_SET); // skip from beginning of file

  FORTSLICEREAD(&time_local,1);
  fclose(SLICEFILE);
  return time_local;
}

/* ------------------ get_volsmoke_all_times ------------------------ */

void get_volsmoke_all_times(volrenderdata *vr){
  int i;
  FILE *volstream=NULL;

  if(load_volcompressed==1&&vr->smoke->vol_file!=NULL){
    volstream=fopen(vr->smoke->vol_file,"rb");
  }

  if(volstream==NULL){
    for(i=0;i<vr->ntimes;i++){
      vr->times[i]=get_volsmoke_frame_time(vr,i);
    }
  }
  else{
    unsigned char buffer[32];
    int ii;
// 1,completion,version
// 1,version,n_data_compressedm32,nbytes,n_data_in,time,valmin,valmax,data ....
    FSEEK(volstream,12,SEEK_SET);
    for(ii=0;ii<vr->ntimes;ii++){
      int ncompressed;
      float *time_local;

      vr->smokepos[ii]=ftell(volstream);
      if(fread(buffer,1,32,volstream)!=32)break;
      ncompressed=*(int *)(buffer+8)-32;
      time_local=(float *)(buffer+20);
      if(FSEEK(volstream,ncompressed,SEEK_CUR)!=0)break;
      vr->times[ii]=*time_local;
    }
    fclose(volstream);
    volstream=NULL;
    if(vr->fire->vol_file!=NULL)volstream=fopen(vr->fire->vol_file,"rb");
    if(volstream!=NULL){
      FSEEK(volstream,12,SEEK_SET);
      for(ii=0;ii<vr->ntimes;ii++){
        int ncompressed;

        vr->firepos[ii]=ftell(volstream);
        if(fread(buffer,1,32,volstream)!=32)break;
        ncompressed=*(int *)(buffer+8)-32;
        if(FSEEK(volstream,ncompressed,SEEK_CUR)!=0)break;
      }
      fclose(volstream);
    }
  }
}

/* ------------------ read_volsmoke_frame ------------------------ */
#define VOL_OFFSET 32
void read_volsmoke_frame(volrenderdata *vr, int framenum, int *first){
	slicedata *fireslice, *smokeslice;
  FILE *SLICEFILE;
  int framesize,framesize2,returncode;
  LINT skip_local;
  float time_local, *smokeframe_data, *fireframe_data;
  int endianswitch=0;
  char *meshlabel;
  unsigned char *c_smokedata_compressed=NULL, *c_firedata_compressed=NULL;
  unsigned char *c_firedata_compressed2=NULL;
  uLongf n_smokedata_compressed, n_firedata_compressed;
  unsigned int size_before=0, size_after=0;
  FILE *volstream=NULL;

  if(framenum<0||framenum>=vr->ntimes)return;
  meshlabel=vr->rendermeshlabel;
  smokeslice=vr->smoke;
  fireslice=vr->fire;
  framesize = smokeslice->nslicei*smokeslice->nslicej*smokeslice->nslicek;
  framesize2 = framesize+VOL_OFFSET;
  if(compress_volsmoke==1){
    vr->is_compressed=1;
  }
  else{
    vr->is_compressed=0;
  }
  if(vr->is_compressed==1||load_volcompressed==1){
    n_smokedata_compressed=1.01*framesize2+600;
    if(vr->smokedata_full==NULL){
      NewMemory((void **)&vr->smokedata_full,framesize*sizeof(float));
      NewMemory((void **)&vr->smokedata_view,framesize*sizeof(float));
      NewMemory((void **)&vr->c_smokedata_view,framesize2);
    }
    smokeframe_data=vr->smokedata_full;
    if(fireslice!=NULL){
      n_firedata_compressed=1.01*framesize2+600;
      if(vr->firedata_full==NULL){
        NewMemory((void **)&vr->firedata_full,framesize*sizeof(float));
        NewMemory((void **)&vr->firedata_view,framesize*sizeof(float));
        NewMemory((void **)&vr->c_firedata_view,framesize2);
      }
      NewMemory((void **)&c_firedata_compressed,n_firedata_compressed);
      NewMemory((void **)&c_firedata_compressed2,n_firedata_compressed);
      fireframe_data=vr->firedata_full;
    }
  }
  else{
    NewMemory((void **)&smokeframe_data,framesize*sizeof(float));
    NewMemory((void **)&fireframe_data,framesize*sizeof(float));
  }

  if(load_volcompressed==1&&vr->smoke->vol_file!=NULL){
    volstream=fopen(vr->smoke->vol_file,"rb");
  }
  if(volstream==NULL){
    skip_local =           (HEADER_SIZE+30        +TRAILER_SIZE); // long label
    skip_local +=          (HEADER_SIZE+30        +TRAILER_SIZE); // short label
    skip_local +=          (HEADER_SIZE+30        +TRAILER_SIZE); // unit label
    skip_local +=          (HEADER_SIZE+24        +TRAILER_SIZE); // is1, is2, js1, js2, ks1, ks2
    skip_local += framenum*(HEADER_SIZE +4        +TRAILER_SIZE); // framenum time's
    skip_local += (LINT)framenum*(LINT)(HEADER_SIZE +4*framesize+TRAILER_SIZE); // framenum slice data's

    SLICEFILE=fopen(smokeslice->reg_file,"rb");
    if(SLICEFILE==NULL)return;

    returncode=FSEEK(SLICEFILE,skip_local,SEEK_SET); // skip from beginning of file

    FORTSLICEREAD(&time_local,1);
    if(global_times!=NULL&&global_times[itimes]>time_local)restart_time=1;
    if(*first==1){
      *first=0;
      printf("time=%.2f %s: ",time_local,meshlabel);
    }
    else{
      if(time_local>=10.0)printf(" ");
      if(time_local>=100.0)printf(" ");
      if(time_local>=1000.0)printf(" ");
      printf("          %s: ",meshlabel);
    }

    vr->times[framenum]=time_local;
    FORTSLICEREAD(smokeframe_data,framesize);
    CheckMemory;
    size_before+=sizeof(float)*framesize;
    if(vr->is_compressed==1){
      float valmin=0.0;

    // one,file version,ndata_compressed,nbytes 1/2/4,ndata_uncompressed,time_local,valmin,valmax,data ....
      compress_volsliceframe(smokeframe_data, framesize, time_local, &valmin, NULL,
                  &c_smokedata_compressed, &n_smokedata_compressed);
      size_after+=n_smokedata_compressed;
      vr->smokedataptrs[framenum]=c_smokedata_compressed;
    }
    else{
      vr->smokedataptrs[framenum]=smokeframe_data;
    }
    CheckMemory;
    printf("smoke");
    fclose(SLICEFILE);
  }
  else{
    unsigned char buffer[32];
    int ncompressed;

// 1,completion,version
// 1,version,n_data_compressedm32,nbytes,n_data_in,time,valmin,valmax,data ....
    FSEEK(volstream,vr->smokepos[framenum],SEEK_SET);
    fread(buffer,8,4,volstream);
    ncompressed=*(int *)(buffer+8);
    time_local = *(float *)(buffer+20);
    FSEEK(volstream,vr->smokepos[framenum],SEEK_SET);
    NewMemory((void **)&c_smokedata_compressed,ncompressed);
    fread(c_smokedata_compressed,1,ncompressed,volstream);
    vr->smokedataptrs[framenum]=c_smokedata_compressed;

    if(*first==1){
      *first=0;
      printf("time=%.2f %s: ",time_local,meshlabel);
    }
    else{
      if(time_local>=10.0)printf(" ");
      if(time_local>=100.0)printf(" ");
      if(time_local>=1000.0)printf(" ");
      printf("          %s: ",meshlabel);
    }

    vr->times[framenum]=time_local;
    fclose(volstream);
    volstream=NULL;
  }

  if(fireslice!=NULL){
    if(load_volcompressed==1&&vr->fire->vol_file!=NULL){
      volstream=fopen(vr->fire->vol_file,"rb");
    }
    if(volstream==NULL){
      SLICEFILE=fopen(fireslice->reg_file,"rb");
      if(SLICEFILE!=NULL){
        returncode=FSEEK(SLICEFILE,skip_local,SEEK_SET); // skip from beginning of file

        FORTSLICEREAD(&time_local,1);
        vr->times[framenum]=time_local;
        FORTSLICEREAD(fireframe_data,framesize);
        CheckMemory;
        size_before+=sizeof(float)*framesize;
        if(vr->is_compressed==1){
          float valmin=20.0, valmax=1400.0;

          compress_volsliceframe(fireframe_data, framesize,  time_local, &valmin, &valmax,
                  &c_firedata_compressed, &n_firedata_compressed);
          size_after+=n_firedata_compressed;
          vr->firedataptrs[framenum]=c_firedata_compressed;
          vr->nfiredata_compressed[framenum]=n_firedata_compressed;
        }
        else{
          vr->firedataptrs[framenum]=fireframe_data;
        }
        printf(", fire");
        fclose(SLICEFILE);
      }
    }
    else{
      unsigned char buffer[32];
      int ncompressed;

// 1,completion,version
// 1,version,n_data_compressedm32,nbytes,n_data_in,time_local,valmin,valmax,data ....
      FSEEK(volstream,vr->firepos[framenum],SEEK_SET);
      fread(buffer,8,4,volstream);
      ncompressed=*(int *)(buffer+8);
      time_local = *(float *)(buffer+20);
      FSEEK(volstream,vr->firepos[framenum],SEEK_SET);
      NewMemory((void **)&c_firedata_compressed,ncompressed);
      fread(c_firedata_compressed,1,ncompressed,volstream);
      vr->firedataptrs[framenum]=c_firedata_compressed;

      vr->times[framenum]=time_local;
      printf(", fire");
      fclose(volstream);
      volstream=NULL;
    }
  }
  CheckMemory;
  vr->dataready[framenum]=1;
  if(vr->is_compressed==1&&load_volcompressed==0){
    printf(" (%4.1f%s reduction)",(float)size_before/(float)size_after,"X");
  }
  printf("\n");
}

/* ------------------ unload_volsmoke_allframes ------------------------ */

void unload_volsmoke_frame_allmeshes(int framenum){
  int i;

  printf("Unloading smoke frame: %i\n",framenum);
  for(i=0;i<nmeshes;i++){
    mesh *meshi;
    volrenderdata *vr;

    meshi = meshinfo + i;
    vr = &meshi->volrenderinfo;
    if(vr->smoke==NULL||vr->fire==NULL)continue;
    if(vr->loaded==0)continue;
    FREEMEMORY(vr->firedataptrs[framenum]);
    FREEMEMORY(vr->smokedataptrs[framenum]);
//    vr->loaded=0;
//    vr->display=0;
  }
}

/* ------------------ unload_volsmoke_allframes ------------------------ */

void unload_volsmoke_allframes(volrenderdata *vr){
  int i;

  printf("Unloading smoke %s - ",vr->rendermeshlabel);
  for(i=0;i<vr->ntimes;i++){
    FREEMEMORY(vr->firedataptrs[i]);
    FREEMEMORY(vr->smokedataptrs[i]);
    vr->dataready[i]=0;
  }
  vr->loaded=0;
  vr->display=0;
  plotstate = getplotstate(DYNAMIC_PLOTS);
  updatetimes();
  printf("completed\n");
}

/* ------------------ read_volsmoke_allframes ------------------------ */

void read_volsmoke_allframes(volrenderdata *vr){
  int nframes;
  int i;
  int first=1;

  nframes = vr->ntimes;
  for(i=0;i<nframes;i++){
    read_volsmoke_frame(vr, i, &first);
  }
  if(vr->is_compressed==1||load_volcompressed==1){//xyz BEGIN
    vr->smokedataptr = vr->smokedata_view;
    vr->firedataptr = vr->firedata_view;
  }
  else{
    vr->smokedataptr = vr->smokedataptrs[0];  //*** hack
    vr->firedataptr = vr->firedataptrs[0];
  }
  vr->loaded=1;
  vr->display=1;
  plotstate=getplotstate(DYNAMIC_PLOTS);
  stept=1;
  updatetimes();
}

/* ------------------ read_volsmoke_frame_allmeshes ------------------------ */

void read_volsmoke_frame_allmeshes(int framenum, supermesh *smesh){
  int i;
  int first=1;
  int nm;

  if(smesh==NULL){
    int nm=nmeshes;
  }
  else{
    nm=smesh->nmeshes;
  }
  for(i=0;i<nm;i++){
    mesh *meshi;
    volrenderdata *vr;

    if(smesh==NULL){
      meshi = meshinfo + i;
    }
    else{
      meshi = smesh->meshes[i];
    }
    vr = &meshi->volrenderinfo;
    if(vr->fire==NULL||vr->smoke==NULL)continue;
    if(read_vol_mesh==i||read_vol_mesh==VOL_READALL){
      read_volsmoke_frame(vr,framenum,&first);
    }
  }
  for(i=0;i<nm;i++){
    mesh *meshi;
    volrenderdata *vr;

    if(smesh==NULL){
      meshi = meshinfo + i;
    }
    else{
      meshi = smesh->meshes[i];
    }
    vr = &meshi->volrenderinfo;
    if(vr->fire==NULL||vr->smoke==NULL)continue;
    if(read_vol_mesh!=i&&read_vol_mesh!=VOL_READALL)continue;
    if(framenum==0){
      if(vr->is_compressed==1||load_volcompressed==1){
        vr->smokedataptr = vr->smokedata_view;  //*** hack
        vr->firedataptr = vr->firedata_view;  //*** hack
      }
      else{
        vr->smokedataptr = vr->smokedataptrs[0];  //*** hack
        vr->firedataptr = vr->firedataptrs[0];
      }
    }
    vr->loaded=1;
    vr->display=1;
  }
}

/* ------------------ read_volsmoke_allframes_allmeshes2 ------------------------ */

void *read_volsmoke_allframes_allmeshes2(void *arg){
  int i;
  int nframes=0;
  
  for(i=0;i<nmeshes;i++){
    mesh *meshi;
    volrenderdata *vr;

    meshi = meshinfo + i;
    vr = &meshi->volrenderinfo;
    if(vr->fire==NULL||vr->smoke==NULL)continue;
    if(read_vol_mesh!=VOL_READALL&&read_vol_mesh!=i)continue;
    if(vr->ntimes>0){
      nframes=vr->ntimes;
      break;
    }
  }
  for(i=0;i<nframes;i++){
    read_volsmoke_frame_allmeshes(i,NULL);
  }
  read_vol_mesh = VOL_READNONE;
  return NULL;
}

/* ------------------ read_volsmoke_allframes_allmeshes ------------------------ */

void read_volsmoke_allframes_allmeshes(void){
  int i;

  compress_volsmoke=glui_compress_volsmoke;
  load_volcompressed=glui_load_volcompressed;
  for(i=0;i<nmeshes;i++){
    mesh *meshi;
    volrenderdata *vr;

    meshi = meshinfo + i;
    vr = &meshi->volrenderinfo;
    if(vr->fire==NULL||vr->smoke==NULL)continue;
    if(read_vol_mesh!=VOL_READALL&&read_vol_mesh!=i)continue;
    get_volsmoke_all_times(vr);
    vr->loaded=1;
    vr->display=1;
#ifdef pp_GPU
    if(gpuactive==1){
      init_volsmoke_texture(meshi);
    }
#endif
  }
  plotstate=getplotstate(DYNAMIC_PLOTS);
  stept=1;
  updatetimes();
#ifdef pp_THREAD
  if(use_multi_threading==1){
    mt_read_volsmoke_allframes_allmeshes2();
  }
  else{
    read_volsmoke_allframes_allmeshes2(NULL);
  }
#else
  read_volsmoke_allframes_allmeshes2(NULL);
#endif
}

#ifdef pp_GPU

/* ------------------ init_volsmoke_texture ------------------------ */

void init_volsmoke_texture(mesh *meshi){
  GLint border_size=0;
  GLsizei nx, ny, nz;


  printf("Defining smoke and fire textures for %s ...",meshi->label);
  fflush(stdout);

  glActiveTexture(GL_TEXTURE0);
  glGenTextures(1,&meshi->smoke_texture_id);
  glBindTexture(GL_TEXTURE_3D,meshi->smoke_texture_id);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
  nx = meshi->ibar+1;
  ny = meshi->jbar+1;
  nz = meshi->kbar+1;
  if(meshi->smoke_texture_buffer==NULL){
    int i;

    NewMemory((void **)&meshi->smoke_texture_buffer,nx*ny*nz*sizeof(float));
    for(i=0;i<nx*ny*nz;i++){
      meshi->smoke_texture_buffer[i]=0.0;
    }
  }
  glTexImage3D(GL_TEXTURE_3D, 0, GL_R32F, 
    nx, ny, nz, border_size, 
    GL_RED, GL_FLOAT, meshi->smoke_texture_buffer);

  glActiveTexture(GL_TEXTURE1);
  glGenTextures(1,&meshi->fire_texture_id);
  glBindTexture(GL_TEXTURE_3D,meshi->fire_texture_id);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
  nx = meshi->ibar+1;
  ny = meshi->jbar+1;
  nz = meshi->kbar+1;
  if(meshi->fire_texture_buffer==NULL){
    int i;

    NewMemory((void **)&meshi->fire_texture_buffer,nx*ny*nz*sizeof(float));
    for(i=0;i<nx*ny*nz;i++){
      meshi->fire_texture_buffer[i]=0.0;
    }
  }
  glTexImage3D(GL_TEXTURE_3D, 0, GL_R32F, 
    nx, ny, nz, border_size, 
    GL_RED, GL_FLOAT, meshi->fire_texture_buffer);

  if(volsmoke_colormap_id_defined==-1){
    volsmoke_colormap_id_defined=1;
    glActiveTexture(GL_TEXTURE2);
    glGenTextures(1,&volsmoke_colormap_id);
    glBindTexture(GL_TEXTURE_1D,volsmoke_colormap_id);
    glTexImage1D(GL_TEXTURE_1D,0,GL_RGBA,256,0,GL_RGBA,GL_FLOAT,rgb_smokecolormap);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexImage1D(GL_TEXTURE_1D,0,4,256,0,GL_RGBA,GL_FLOAT,rgb_smokecolormap);
  }

#ifndef pp_GPUDEPTH
  glActiveTexture(GL_TEXTURE3);
  glGenTextures(1,&meshi->blockage_texture_id);
  glBindTexture(GL_TEXTURE_3D,meshi->blockage_texture_id);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
  nx = meshi->ibar;
  ny = meshi->jbar;
  nz = meshi->kbar;
  glTexImage3D(GL_TEXTURE_3D, 0, GL_R32F, nx, ny, nz, border_size, GL_RED, GL_FLOAT, meshi->f_iblank_cell);
#endif

  glActiveTexture(GL_TEXTURE0);
  printf("completed\n");
  fflush(stdout);
}

/* ------------------ update_3dsmoke_texture ------------------------ */

void update_volsmoke_texture(int *ijk_offset, mesh *meshi, float *smokedata_local, float *firedata_local){
  GLsizei ni, nj, nk;

//  glGetIntegerv(GL_MAX_TEXTURE_COORDS,&ntextures);
  ni = meshi->ibar+1;
  nj = meshi->jbar+1;
  nk = meshi->kbar+1;
#ifdef pp_GPUTHROTTLE
  GPUnframes+=3*ni*nj*nk;
#endif
  glActiveTexture(GL_TEXTURE0);
  glTexSubImage3D(GL_TEXTURE_3D,0,ijk_offset[0],ijk_offset[1],ijk_offset[2],ni,nj,nk,GL_RED, GL_FLOAT, smokedata_local);

  if(firedata_local!=NULL){  
    glActiveTexture(GL_TEXTURE1);
    glTexSubImage3D(GL_TEXTURE_3D,0,ijk_offset[0],ijk_offset[1],ijk_offset[2],ni,nj,nk,GL_RED, GL_FLOAT, firedata_local);
  }

  glActiveTexture(GL_TEXTURE3);
  ni = meshi->ibar;
  nj = meshi->jbar;
  nk = meshi->kbar;
  glTexSubImage3D(GL_TEXTURE_3D,0,ijk_offset[0],ijk_offset[1],ijk_offset[2],ni,nj,nk,GL_RED, GL_FLOAT, meshi->f_iblank_cell);

  glActiveTexture(GL_TEXTURE0);
}
#endif

#define MLEFT 0
#define MRIGHT 1
#define MFRONT 2
#define MBACK 3
#define MDOWN 4
#define MUP 5
#define MEPS 0.1
/* ------------------ mesh_connect ------------------------ */

int mesh_connect(mesh *mesh_from, int val, mesh *mesh_to){
  float *eps;

  eps = mesh_from->boxeps;
  switch (val){
    case MLEFT:
    case MRIGHT:
      if(mesh_from->jbar!=mesh_to->jbar)return 0;
      if(mesh_from->kbar!=mesh_to->kbar)return 0;
      if( ABS(mesh_from->dbox[1]-mesh_to->dbox[1])>eps[1] )return 0;
      if( ABS(mesh_from->dbox[2]-mesh_to->dbox[2])>eps[2] )return 0;
      if( ABS(mesh_from->y0-mesh_to->y0)>eps[1] )return 0;
      if( ABS(mesh_from->z0-mesh_to->z0)>eps[2] )return 0;
      break;
    case MFRONT:
    case MBACK:
      if(mesh_from->ibar!=mesh_to->ibar)return 0;
      if(mesh_from->kbar!=mesh_to->kbar)return 0;
      if( ABS(mesh_from->dbox[0]-mesh_to->dbox[0])>eps[0] )return 0;
      if( ABS(mesh_from->dbox[2]-mesh_to->dbox[2])>eps[2] )return 0;
      if( ABS(mesh_from->x0-mesh_to->x0)>eps[0] )return 0;
      if( ABS(mesh_from->z0-mesh_to->z0)>eps[2] )return 0;
      break;
    case MDOWN:
    case MUP:
      if(mesh_from->ibar!=mesh_to->ibar)return 0;
      if(mesh_from->jbar!=mesh_to->jbar)return 0;
      if( ABS(mesh_from->dbox[0]-mesh_to->dbox[0])>eps[0] )return 0;
      if( ABS(mesh_from->dbox[1]-mesh_to->dbox[1])>eps[1] )return 0;
      if( ABS(mesh_from->x0-mesh_to->x0)>eps[0] )return 0;
      if( ABS(mesh_from->y0-mesh_to->y0)>eps[1] )return 0;
      break;
  }
  switch (val){
    case MLEFT:
      if( ABS(mesh_from->x1-mesh_to->x0)<eps[0] )return 1;
      break;
    case MRIGHT:
      if( ABS(mesh_from->x0-mesh_to->x1) < eps[0])return 1;
      break;
    case MFRONT:
      if( ABS(mesh_from->y1-mesh_to->y0) < eps[1])return 1;
      break;
    case MBACK:
      if( ABS(mesh_from->y0-mesh_to->y1) < eps[1])return 1;
      break;
    case MDOWN:
      if( ABS(mesh_from->z1-mesh_to->z0) < eps[2])return 1;
      break;
    case MUP:
      if( ABS(mesh_from->z0-mesh_to->z1) < eps[2])return 1;
      break;
  }
  return 0;
}

/* ------------------ init_meshnabors ------------------------ */

mesh *get_minmesh(void){
  int i;
  float mindist=-1.0;
  mesh *minmesh=NULL;
  
  // find mesh closes to origin that is not already in a supermesh

  for(i=0;i<nmeshes;i++){
    mesh *meshi;
    float dist2;

    meshi = meshinfo + i;
    if(meshi->super!=NULL)continue;
    dist2 = meshi->x0*meshi->x0+meshi->y0*meshi->y0+meshi->z0*meshi->z0;
    if(mindist<0.0||dist2<mindist){
      mindist=dist2;
      minmesh=meshi;
    }
  }
  return minmesh;
}

/* ------------------ can_extend ------------------------ */

int extend_mesh(supermesh *smesh, int direction){
  int i;
  int count=0,nbefore;

  nbefore=smesh->nmeshes;
  for(i=0;i<nbefore;i++){
    mesh *nabor;

    nabor = smesh->meshes[i]->nabors[direction];
    if(nabor!=NULL&&nabor->super!=NULL)continue;
    if(nabor==NULL)return 0;
  }
  for(i=0;i<nbefore;i++){
    mesh *nabor;

    nabor = smesh->meshes[i]->nabors[direction];
    if(nabor->super!=NULL)continue;
    smesh->meshes[nbefore+count]=nabor;
    nabor->super=smesh;
    count++;
  }
  if(count==0)return 0;
  smesh->nmeshes=nbefore+count;
  return 1;
}

/* ------------------ init_meshnabors ------------------------ */

void make_smesh(supermesh *smesh, mesh *firstmesh){
  mesh **meshptrs;

  NewMemory((void **)&meshptrs,nmeshes*sizeof(mesh *));
  smesh->meshes=meshptrs;

  smesh->meshes[0]=firstmesh;
  firstmesh->super=smesh;
  smesh->nmeshes=1;
  for(;;){
    int return_val,again;

    again=0;
    return_val = extend_mesh(smesh,MLEFT);
    again = MAX(again,return_val);
    return_val = extend_mesh(smesh,MRIGHT);
    again = MAX(again,return_val);
    return_val = extend_mesh(smesh,MFRONT);
    again = MAX(again,return_val);
    return_val = extend_mesh(smesh,MBACK);
    again = MAX(again,return_val);
    return_val = extend_mesh(smesh,MUP);
    again = MAX(again,return_val);
    return_val = extend_mesh(smesh,MDOWN);
    again = MAX(again,return_val);
    if(again==0)break;
  }
}

/* ------------------ compare_meshes ------------------------ */

int compare_smeshes( const void *arg1, const void *arg2 ){
  mesh *meshi, *meshj;
  float dcell;

  meshi = *(mesh **)arg1;
  meshj = *(mesh **)arg2;
  dcell = MIN(meshi->dcell,meshj->dcell)/2.0;
  if(meshi->z0<meshj->z0-dcell)return -1;
  if(meshi->z0>meshj->z0+dcell)return 1;
  if(meshi->y0<meshj->y0-dcell)return -1;
  if(meshi->y0>meshj->y0+dcell)return 1;
  if(meshi->x0<meshj->x0-dcell)return -1;
  if(meshi->x0>meshj->x0+dcell)return 1;
  return 0;
}

/* ------------------ init_meshnabors ------------------------ */

void init_supermesh(void){
  int i;
  mesh *thismesh;
  supermesh *smesh;

  // determine mesh connectivity

  for(i=0;i<nmeshes;i++){
    mesh *meshi;
    int j;

    meshi = meshinfo + i;
    for(j=i+1;j<nmeshes;j++){
      mesh *meshj;

      meshj = meshinfo + j;

      if(mesh_connect(meshi,MLEFT,meshj)==1){
        meshi->nabors[MRIGHT]=meshj;
        meshj->nabors[MLEFT]=meshi;
        continue;
      }
      if(mesh_connect(meshi,MRIGHT,meshj)==1){
        meshi->nabors[MLEFT]=meshj;
        meshj->nabors[MRIGHT]=meshi;
        continue;
      }
      if(mesh_connect(meshi,MFRONT,meshj)==1){
        meshi->nabors[MBACK]=meshj;
        meshj->nabors[MFRONT]=meshi;
        continue;
      }
      if(mesh_connect(meshi,MBACK,meshj)==1){
        meshi->nabors[MFRONT]=meshj;
        meshj->nabors[MBACK]=meshi;
        continue;
      }
      if(mesh_connect(meshi,MDOWN,meshj)==1){
        meshi->nabors[MUP]=meshj;
        meshj->nabors[MDOWN]=meshi;
      }
      if(mesh_connect(meshi,MUP,meshj)==1){
        meshi->nabors[MDOWN]=meshj;
        meshj->nabors[MUP]=meshi;
      }
    }
  }

  // merge connected meshes to form supermeshes

  nsupermeshinfo=0;
  thismesh = get_minmesh();
  for(smesh=supermeshinfo,thismesh=get_minmesh();thismesh!=NULL;thismesh=get_minmesh(),smesh++){
    make_smesh(smesh,thismesh);
    nsupermeshinfo++;
  }

  for(smesh = supermeshinfo;smesh!=supermeshinfo+nsupermeshinfo;smesh++){
    mesh *nab;

    smesh->x0=smesh->meshes[0]->x0;
    smesh->y0=smesh->meshes[0]->y0;
    smesh->z0=smesh->meshes[0]->z0;
    smesh->x1=smesh->meshes[0]->x1;
    smesh->y1=smesh->meshes[0]->y1;
    smesh->z1=smesh->meshes[0]->z1;
    for(i=1;i<smesh->nmeshes;i++){
      smesh->x0=MIN(smesh->x0,smesh->meshes[i]->x0);
      smesh->y0=MIN(smesh->y0,smesh->meshes[i]->y0);
      smesh->z0=MIN(smesh->z0,smesh->meshes[i]->z0);
      smesh->x1=MAX(smesh->x1,smesh->meshes[i]->x1);
      smesh->y1=MAX(smesh->y1,smesh->meshes[i]->y1);
      smesh->z1=MAX(smesh->z1,smesh->meshes[i]->z1);
    }
    smesh->xcen = (smesh->x0+smesh->x1)/2.0;
    smesh->ycen = (smesh->y0+smesh->y1)/2.0;
    smesh->zcen = (smesh->z0+smesh->z1)/2.0;
    smesh->dcell = smesh->meshes[0]->dcell;

    // sort meshes in supermesh from lower front left to upper back right

    qsort((mesh **)smesh->meshes,sizeof(mesh *),smesh->nmeshes,compare_smeshes);

    // count meshes in supermesh in each direction

    smesh->ni=1;
    smesh->nj=1;
    smesh->nk=1;
    for(nab=smesh->meshes[0];nab->nabors[MRIGHT]!=NULL;nab=nab->nabors[MRIGHT]){
      smesh->ni++;
    }
    for(nab=smesh->meshes[0];nab->nabors[MBACK]!=NULL;nab=nab->nabors[MBACK]){
      smesh->nj++;
    }
    for(nab=smesh->meshes[0];nab->nabors[MUP]!=NULL;nab=nab->nabors[MUP]){
      smesh->nk++;
    }
  }
}

/* ------------------ merge_volsmoke ------------------------ */

void merge_volsmoke(void){
  int i;

  for(i=0;i<nsupermeshinfo;i++){
    supermesh *smesh;
    int iframe,nframes;
    FILE *stream_out,*stream_fire,*stream_smoke;
    char mergefile[1024];
    char *smoke_file;
    volrenderdata *vr;
    float **fireptrs,**smokeptrs;
    float *mergedata;

    smesh = supermeshinfo + i;
    vr = &(smesh->meshes[0]->volrenderinfo);
    nframes = vr->ntimes;

    NewMemory((void **)&fireptrs,smesh->nmeshes*sizeof(float *));
    NewMemory((void **)&smokeptrs,smesh->nmeshes*sizeof(float *));

    for(iframe=0;i<nframes;iframe++){
      read_volsmoke_frame_allmeshes(iframe,smesh);
    }
    smoke_file = vr->smoke->reg_file;
    strcpy(mergefile,smoke_file);
    strcat(mergefile,".vs3d");
    stream_out=fopen(mergefile,"wb");

    FREEMEMORY(fireptrs);
    FREEMEMORY(smokeptrs);
  }
}