﻿/*
  Simple DirectMedia Layer
  Copyright (C) 1997-2012 Sam Lantinga <slouken@libsdl.org>

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
*/

#include "SDL_config.h"

/* General mouse handling code for SDL */

#include "SDL_events.h"
#include "SDL_events_c.h"
#include "SDL_gesture_c.h"

//#include <memory.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include "ite/ith.h"

//#define ENABLE_NORMALIZE_VECTORS_TO_SIMPLIFY_ANGLE_CALCULATION

//TODO: Replace with malloc

#if defined(CFG_LCD_WIDTH) && (CFG_LCD_WIDTH > 0)
#define	GESTURE_LCD_WIDTH	(CFG_LCD_WIDTH)
#else
#define	GESTURE_LCD_WIDTH	(1024)
#endif

#if defined(CFG_LCD_HEIGHT) && (CFG_LCD_HEIGHT > 0)
#define	GESTURE_LCD_HEIGHT	(CFG_LCD_HEIGHT)
#else
#define	GESTURE_LCD_HEIGHT	(600)
#endif

#define MAXPATHSIZE 1024




#define DOLLARNPOINTS 64
#define DOLLARSIZE 256

#define ENABLE_DOLLAR

#define PHI 0.618033989 
#define HW_RATIO_THRESHOLD	2
#define	SLIDE_ANGLE_MARGIN	30
//#define	ENABLE_SLIDE_DBG_MSG


typedef struct {
  float x,y;
} SDL_FloatPoint;

typedef struct {
  SDL_FloatPoint point_1;
  SDL_FloatPoint point_2;
  float xMax,yMax;
  float xMin,yMin;
} SDL_SlidePoint;

typedef struct {
  float length;
  
  int numPoints;
  SDL_FloatPoint p[MAXPATHSIZE];
} SDL_DollarPath;

typedef struct {
  SDL_FloatPoint path[DOLLARNPOINTS];
  unsigned long hash;
} SDL_DollarTemplate;

typedef struct {
  SDL_TouchID id;
  SDL_FloatPoint res;
  SDL_FloatPoint centroid;
  SDL_DollarPath dollarPath;
  Uint16 numDownFingers;

  int numDollarTemplates;
  SDL_DollarTemplate *dollarTemplate;

  SDL_bool recording;
} SDL_GestureTouch;

SDL_GestureTouch *SDL_gestureTouch;
int SDL_numGestureTouches = 0;
SDL_bool recordAll;   

#ifdef	ENABLE_SLIDE_GESTURE
static float  g_SlideTheta = 0; 	//from 0 ~ 359 degree
static float  g_HWratio = 1; 		//reZOOM by H and W
static float  g_SlideDistance = 0;
static SDL_SlidePoint	gSlideP={0};
static SDL_FloatPoint	g_SlideXY={0};
#endif

#if 0
static void PrintPath(SDL_FloatPoint *path)
{
  int i;
    printf("Path:");
  for(i=0;i<DOLLARNPOINTS;i++) {
        printf(" (%f,%f)",path[i].x,path[i].y);
  }
  printf("\n");
}
#endif

int SDL_RecordGesture(SDL_TouchID touchId)
{
  int i;
  if(touchId < 0) recordAll = SDL_TRUE;
  for(i = 0;i < SDL_numGestureTouches; i++) {
    if((touchId < 0) || (SDL_gestureTouch[i].id == touchId)) {
      SDL_gestureTouch[i].recording = SDL_TRUE;
      if(touchId >= 0)
        return 1;
    }      
  }
  return (touchId < 0);
}

static unsigned long SDL_HashDollar(SDL_FloatPoint* points)
{
  unsigned long hash = 5381;
  int i;
  for(i = 0;i < DOLLARNPOINTS; i++) { 
    hash = ((hash<<5) + hash) + (unsigned long)points[i].x;
    hash = ((hash<<5) + hash) + (unsigned long)points[i].y;
  }
  return hash;
}


static int SaveTemplate(SDL_DollarTemplate *templ, SDL_RWops * src)
{
  if(src == NULL) return 0;

  
  //No Longer storing the Hash, rehash on load
  //if(SDL_RWops.write(src,&(templ->hash),sizeof(templ->hash),1) != 1) return 0;
  
  if(SDL_RWwrite(src,templ->path,
                 sizeof(templ->path[0]),DOLLARNPOINTS) != DOLLARNPOINTS) 
    return 0;

  return 1;
}


int SDL_SaveAllDollarTemplates(SDL_RWops *src)
{
  int i,j,rtrn = 0;
  for(i = 0; i < SDL_numGestureTouches; i++) {
    SDL_GestureTouch* touch = &SDL_gestureTouch[i];
    for(j = 0;j < touch->numDollarTemplates; j++) {
        rtrn += SaveTemplate(&touch->dollarTemplate[i],src);
    }
  }
  return rtrn;  
}

int SDL_SaveDollarTemplate(SDL_GestureID gestureId, SDL_RWops *src)
{
  int i,j;
  for(i = 0; i < SDL_numGestureTouches; i++) {
    SDL_GestureTouch* touch = &SDL_gestureTouch[i];
    for(j = 0;j < touch->numDollarTemplates; j++) {
      if(touch->dollarTemplate[i].hash == gestureId) {
        return SaveTemplate(&touch->dollarTemplate[i],src);
      }
    }
  }
  SDL_SetError("Unknown gestureId");
  return -1;
}

//path is an already sampled set of points
//Returns the index of the gesture on success, or -1
static int SDL_AddDollarGesture_one(SDL_GestureTouch* inTouch, SDL_FloatPoint* path)
{
  SDL_DollarTemplate* dollarTemplate;
  SDL_DollarTemplate *templ;
    int index;

    index = inTouch->numDollarTemplates;
    dollarTemplate = 
        (SDL_DollarTemplate *)SDL_realloc(inTouch->dollarTemplate,
                                          (index + 1) *
                    sizeof(SDL_DollarTemplate));
      if(!dollarTemplate) {
        SDL_OutOfMemory();
        return -1;
      }
      inTouch->dollarTemplate = dollarTemplate;

    templ = &inTouch->dollarTemplate[index];
      SDL_memcpy(templ->path,path,DOLLARNPOINTS*sizeof(SDL_FloatPoint));
      templ->hash = SDL_HashDollar(templ->path);
      inTouch->numDollarTemplates++;    

    return index;
    }

static int SDL_AddDollarGesture(SDL_GestureTouch* inTouch, SDL_FloatPoint* path)
{
    int index = -1;
    int i = 0;
    if (inTouch == NULL) {
        if (SDL_numGestureTouches == 0) return -1;
        for (i = 0; i < SDL_numGestureTouches; i++) {
            inTouch = &SDL_gestureTouch[i];
            index = SDL_AddDollarGesture_one(inTouch, path);
            if (index < 0)
      return -1;
    }
        // Use the index of the last one added.
        return index;
    } else {
        return SDL_AddDollarGesture_one(inTouch, path);
  }
  return -1;
}

int SDL_LoadDollarTemplates(SDL_TouchID touchId, SDL_RWops *src)
{
  int i,loaded = 0;
  SDL_GestureTouch *touch = NULL;
  if(src == NULL) return 0;
  if(touchId >= 0) {
    for(i = 0;i < SDL_numGestureTouches; i++)
      if(SDL_gestureTouch[i].id == touchId)
        touch = &SDL_gestureTouch[i];
    if(touch == NULL) return -1;
  }

  while(1) {
    SDL_DollarTemplate templ;

    if(SDL_RWread(src,templ.path,sizeof(templ.path[0]),DOLLARNPOINTS) < 
       DOLLARNPOINTS) break;

    if(touchId >= 0) {
      //printf("Adding loaded gesture to 1 touch\n");
            if (SDL_AddDollarGesture(touch, templ.path) >= 0)
                loaded++;
    }
    else {
      //printf("Adding to: %i touches\n",SDL_numGestureTouches);
      for(i = 0;i < SDL_numGestureTouches; i++) {
        touch = &SDL_gestureTouch[i];
        //printf("Adding loaded gesture to + touches\n");
        //TODO: What if this fails?
        SDL_AddDollarGesture(touch,templ.path);        
      }
      loaded++;
    }
  }

  return loaded; 
}


static float dollarDifference(SDL_FloatPoint* points,SDL_FloatPoint* templ,float ang)
{
  //  SDL_FloatPoint p[DOLLARNPOINTS];
  float dist = 0;
  SDL_FloatPoint p;
  int i;
  for(i = 0; i < DOLLARNPOINTS; i++) {
    p.x = (float)(points[i].x * SDL_cos(ang) - points[i].y * SDL_sin(ang));
    p.y = (float)(points[i].x * SDL_sin(ang) + points[i].y * SDL_cos(ang));
    dist += (float)(SDL_sqrt((p.x-templ[i].x)*(p.x-templ[i].x)+
                 (p.y-templ[i].y)*(p.y-templ[i].y)));
  }
  return dist/DOLLARNPOINTS;
  
}

static float bestDollarDifference(SDL_FloatPoint* points,SDL_FloatPoint* templ)
{
  //------------BEGIN DOLLAR BLACKBOX----------------//
  //-TRANSLATED DIRECTLY FROM PSUDEO-CODE AVAILABLE AT-//
  //-"http://depts.washington.edu/aimgroup/proj/dollar/"-//
  double ta = -M_PI/4;
  double tb = M_PI/4;
  double dt = M_PI/90;
  float x1 = (float)(PHI*ta + (1-PHI)*tb);
  float f1 = dollarDifference(points,templ,x1);
  float x2 = (float)((1-PHI)*ta + PHI*tb);
  float f2 = dollarDifference(points,templ,x2);
  while(SDL_fabs(ta-tb) > dt) {
    if(f1 < f2) {
      tb = x2;
      x2 = x1;
      f2 = f1;
      x1 = (float)(PHI*ta + (1-PHI)*tb);
      f1 = dollarDifference(points,templ,x1);
    }
    else {
      ta = x1;
      x1 = x2;
      f1 = f2;
      x2 = (float)((1-PHI)*ta + PHI*tb);
      f2 = dollarDifference(points,templ,x2);
    }
  }
  /*
  if(f1 <= f2)
    printf("Min angle (x1): %f\n",x1);
  else if(f1 >  f2)
    printf("Min angle (x2): %f\n",x2);
  */
  return SDL_min(f1,f2);  
}

//DollarPath contains raw points, plus (possibly) the calculated length
static int dollarNormalize(const SDL_DollarPath *path,SDL_FloatPoint *points)
{
  int i;
  float interval;
  float dist;
  int numPoints = 0;
  SDL_FloatPoint centroid; 
  float xmin,xmax,ymin,ymax;
  float ang;
  float w,h;
  float length = path->length;

  //Calculate length if it hasn't already been done
  if(length <= 0) {
    for(i=1;i<path->numPoints;i++) {
            float dx = path->p[i  ].x - path->p[i-1].x;
            float dy = path->p[i  ].y - path->p[i-1].y;
      length += (float)(SDL_sqrt(dx*dx+dy*dy));
    }
  }

  //Resample
  interval = length/(DOLLARNPOINTS - 1);
  dist = interval;

  centroid.x = 0;centroid.y = 0;
  
  //printf("(%f,%f)\n",path->p[path->numPoints-1].x,path->p[path->numPoints-1].y);
  for(i = 1;i < path->numPoints;i++) {
    float d = (float)(SDL_sqrt((path->p[i-1].x-path->p[i].x)*(path->p[i-1].x-path->p[i].x)+
                             (path->p[i-1].y-path->p[i].y)*(path->p[i-1].y-path->p[i].y)));
    //printf("d = %f dist = %f/%f\n",d,dist,interval);
    while(dist + d > interval) {
      points[numPoints].x = path->p[i-1].x + 
        ((interval-dist)/d)*(path->p[i].x-path->p[i-1].x);
      points[numPoints].y = path->p[i-1].y + 
        ((interval-dist)/d)*(path->p[i].y-path->p[i-1].y);
      centroid.x += points[numPoints].x;
      centroid.y += points[numPoints].y;
      numPoints++;

      dist -= interval;
    }
    dist += d;
  }
  if(numPoints < DOLLARNPOINTS-1) {
    SDL_SetError("ERROR: NumPoints = %i\n",numPoints); 
    return 0;
  }
  //copy the last point
  points[DOLLARNPOINTS-1] = path->p[path->numPoints-1];
  numPoints = DOLLARNPOINTS;

  centroid.x /= numPoints;
  centroid.y /= numPoints;

  //printf("Centroid (%f,%f)",centroid.x,centroid.y);
  //Rotate Points so point 0 is left of centroid and solve for the bounding box
  xmin = centroid.x;
  xmax = centroid.x;
  ymin = centroid.y;
  ymax = centroid.y;
  
  ang = (float)(SDL_atan2(centroid.y - points[0].y,
                    centroid.x - points[0].x));

  for(i = 0;i<numPoints;i++) {                                               
    float px = points[i].x;
    float py = points[i].y;
        points[i].x = (float)((px - centroid.x)*SDL_cos(ang) -
                              (py - centroid.y)*SDL_sin(ang) + centroid.x);
        points[i].y = (float)((px - centroid.x)*SDL_sin(ang) +
                              (py - centroid.y)*SDL_cos(ang) + centroid.y);


    if(points[i].x < xmin) xmin = points[i].x;
    if(points[i].x > xmax) xmax = points[i].x; 
    if(points[i].y < ymin) ymin = points[i].y;
    if(points[i].y > ymax) ymax = points[i].y;
  }

  //Scale points to DOLLARSIZE, and translate to the origin
  w = xmax-xmin;
  h = ymax-ymin;

  for(i=0;i<numPoints;i++) {
    points[i].x = (points[i].x - centroid.x)*DOLLARSIZE/w;
    points[i].y = (points[i].y - centroid.y)*DOLLARSIZE/h;
  }  
  return numPoints;
}

static float dollarRecognize(const SDL_DollarPath *path,int *bestTempl,SDL_GestureTouch* touch)
{
        
        SDL_FloatPoint points[DOLLARNPOINTS];
        int i;
        float bestDiff = 10000;

    dollarNormalize(path,points);

        //PrintPath(points);
        *bestTempl = -1;
        for(i = 0;i < touch->numDollarTemplates;i++) {
                float diff = bestDollarDifference(points,touch->dollarTemplate[i].path);
                if(diff < bestDiff) {bestDiff = diff; *bestTempl = i;}
        }
        return bestDiff;
}

#ifdef	ENABLE_SLIDE_GESTURE
static void addSlidePoint(SDL_SlidePoint *SlidePnt, SDL_TouchFingerEvent *tpfgr, SDL_EventType event_type)
{
	#ifdef	ENABLE_SLIDE_DBG_MSG
	printf("[slide] addPoint::x,y(%d,%d)(%x)\n", tpfgr->x, tpfgr->y, event_type);
	#endif
	
	if(event_type==SDL_FINGERDOWN)
	{
		SlidePnt->point_1.x = (float)tpfgr->x;
    	SlidePnt->point_1.y = (float)tpfgr->y; 
    	
    	SlidePnt->xMin=32768;    	SlidePnt->xMax=0;
    	SlidePnt->yMin=32768;    	SlidePnt->yMax=0;
	}
	if(event_type == SDL_FINGERUP)
	{
		SlidePnt->point_2.x = (float)tpfgr->x;
    	SlidePnt->point_2.y = (float)tpfgr->y; 
	}
    if( (float)tpfgr->x < SlidePnt->xMin) SlidePnt->xMin = (float)tpfgr->x;
    if( (float)tpfgr->x > SlidePnt->xMax) SlidePnt->xMax = (float)tpfgr->x;
    if( (float)tpfgr->y < SlidePnt->yMin) SlidePnt->yMin = (float)tpfgr->y;
    if( (float)tpfgr->y > SlidePnt->yMax) SlidePnt->yMax = (float)tpfgr->y;
}

static float getSlideTheta(SDL_SlidePoint *TwoPnt)
{
	float ang,theta;	
	ang = (float)(SDL_atan2(TwoPnt->point_1.y - TwoPnt->point_2.y, TwoPnt->point_2.x - TwoPnt->point_1.x));
	theta = (float)((180*ang)/M_PI);
	return theta;
}

static void getSlideXYPoint(SDL_SlidePoint *TwoPnt, SDL_FloatPoint *tmp)
{
	tmp->x = (TwoPnt->point_1.x + TwoPnt->point_2.x)/2;
	tmp->y = (TwoPnt->point_1.y + TwoPnt->point_2.y)/2;
}

static float getSlideRatio(SDL_SlidePoint *SlidePnt)
{
	float dx,dy,ratio;
	
	dx = (float)(SlidePnt->xMax - SlidePnt->xMin);
	dy = (float)(SlidePnt->yMax - SlidePnt->yMin);
	
	g_SlideDistance = dx*dx + dy*dy;
	
	if(dx && dy)
	{
		if(dx>dy)	ratio = dx/dy;
		else		ratio = dy/dx;
	}
	else
	{
		if(dx==dy)	ratio = 1;
		else		ratio = 100;
	}
	//printf("gHW=%f,dx=%f,dy=%f,x(%f,%f),y(%f,%f)\n",ratio,dx,dy,SlidePnt->xMax,SlidePnt->xMin,SlidePnt->yMax,SlidePnt->yMin);
	return ratio;
}

static unsigned long getSlideDirection(float theta)
{
	if( theta>=-SLIDE_ANGLE_MARGIN && theta<SLIDE_ANGLE_MARGIN )				return SDL_TG_RIGHT;
	if( theta>=SLIDE_ANGLE_MARGIN && theta<(180-SLIDE_ANGLE_MARGIN) )			return SDL_TG_UP;
	if( theta>=(180-SLIDE_ANGLE_MARGIN) || theta<-(180-SLIDE_ANGLE_MARGIN) )	return SDL_TG_LEFT;
	if( theta>=-(180-SLIDE_ANGLE_MARGIN) && theta<-SLIDE_ANGLE_MARGIN )			return SDL_TG_DOWN;
	return SDL_TG_UNKNOWN;
}
#endif

int SDL_GestureAddTouch(SDL_Touch* touch)
{
  SDL_GestureTouch *gestureTouch = (SDL_GestureTouch *)SDL_realloc(SDL_gestureTouch,
                                               (SDL_numGestureTouches + 1) *
                                               sizeof(SDL_GestureTouch));

  if(!gestureTouch) {
    SDL_OutOfMemory();
    return -1;
  }

  SDL_gestureTouch = gestureTouch;

  SDL_gestureTouch[SDL_numGestureTouches].res.x = touch->xres;
  SDL_gestureTouch[SDL_numGestureTouches].res.y = touch->yres;
  SDL_gestureTouch[SDL_numGestureTouches].numDownFingers = 0;

  SDL_gestureTouch[SDL_numGestureTouches].res.x = touch->xres;
  SDL_gestureTouch[SDL_numGestureTouches].id = touch->id;

  SDL_gestureTouch[SDL_numGestureTouches].numDollarTemplates = 0;

  SDL_gestureTouch[SDL_numGestureTouches].recording = SDL_FALSE;

  SDL_numGestureTouches++;
  return 0;
}

static SDL_GestureTouch * SDL_GetGestureTouch(SDL_TouchID id)
{
  int i;
  for(i = 0;i < SDL_numGestureTouches; i++) {
    //printf("%i ?= %i\n",SDL_gestureTouch[i].id,id);
        if (SDL_gestureTouch[i].id == id)
            return &SDL_gestureTouch[i];
  }
  return NULL;
}

int SDL_SendGestureMulti(SDL_GestureTouch* touch,float dTheta,float dDist)
{
  SDL_Event event;
  event.mgesture.type = SDL_MULTIGESTURE;
  event.mgesture.touchId = touch->id;
  event.mgesture.x = touch->centroid.x;
  event.mgesture.y = touch->centroid.y;
  event.mgesture.dTheta = dTheta;
  event.mgesture.dDist = dDist;  
  event.mgesture.numFingers = touch->numDownFingers;
  return SDL_PushEvent(&event) > 0;
}

static int SDL_SendGestureDollar(SDL_GestureTouch* touch,
                          SDL_GestureID gestureId,float error)
{
  SDL_Event event;
  event.dgesture.type = SDL_DOLLARGESTURE;
  event.dgesture.touchId = touch->id;
  /*
    //TODO: Add this to give location of gesture?
  event.mgesture.x = touch->centroid.x;
  event.mgesture.y = touch->centroid.y;
  */
  event.dgesture.gestureId = gestureId;
  event.dgesture.error = error;  
  //A finger came up to trigger this event.
  event.dgesture.numFingers = touch->numDownFingers + 1; 
  return SDL_PushEvent(&event) > 0;
}

#ifdef	ENABLE_SLIDE_GESTURE
int SDL_SendGestureSlide(SDL_GestureTouch* touch,
                          SDL_GestureID gestureId,float error) {
  SDL_Event event;
  event.dgesture.type = SDL_SLIDEGESTURE;
  event.dgesture.touchId = touch->id;

  event.dgesture.gestureId = gestureId;
  event.dgesture.error = error;  

  event.dgesture.x = (Uint32)g_SlideXY.x;
  event.dgesture.y = (Uint32)g_SlideXY.y;

  #ifdef	ENABLE_SLIDE_DBG_MSG
  printf("[slide] Send_event(type=%x, gID=%x)\n", event.dgesture.type, event.dgesture.gestureId);
  #endif

  //A finger came up to trigger this event.
  event.dgesture.numFingers = touch->numDownFingers + 1; 
  return SDL_PushEvent(&event) > 0;
}
#endif

static int SDL_SendDollarRecord(SDL_GestureTouch* touch,SDL_GestureID gestureId)
{
  SDL_Event event;
  event.dgesture.type = SDL_DOLLARRECORD;
  event.dgesture.touchId = touch->id;
  event.dgesture.gestureId = gestureId;
  return SDL_PushEvent(&event) > 0;
}


void SDL_GestureProcessEvent(SDL_Event* event)
{
  float x,y; 
  SDL_FloatPoint path[DOLLARNPOINTS];
  int index;
  int i;
  float pathDx, pathDy;
  SDL_FloatPoint lastP;
  SDL_FloatPoint lastCentroid;
  float lDist;
  float Dist;
  float dtheta;
  float dDist;

  if(event->type == SDL_FINGERMOTION || 
     event->type == SDL_FINGERDOWN ||
     event->type == SDL_FINGERUP) {
    SDL_GestureTouch* inTouch = SDL_GetGestureTouch(event->tfinger.touchId);
    
    //Shouldn't be possible
    if(inTouch == NULL) return;
    
    //printf("@ (%i,%i) with res: (%i,%i)\n",(int)event->tfinger.x, (int)event->tfinger.y,  (int)inTouch->res.x,(int)inTouch->res.y);
    inTouch->res.x = (float)GESTURE_LCD_WIDTH;// ithLcdGetWidth();
    inTouch->res.y = (float)GESTURE_LCD_HEIGHT;// ithLcdGetHeight();
    
    x = ((float)event->tfinger.x)/(float)inTouch->res.x;
    y = ((float)event->tfinger.y)/(float)inTouch->res.y;

#ifdef	ENABLE_SLIDE_GESTURE
    addSlidePoint(&gSlideP, &event->tfinger, event->type);
#endif

    //Finger Up
    if(event->type == SDL_FINGERUP) {
      inTouch->numDownFingers--;
      
#ifdef ENABLE_DOLLAR
      if(inTouch->recording) {
        inTouch->recording = SDL_FALSE;        
        dollarNormalize(&inTouch->dollarPath,path);
        //PrintPath(path);
        if(recordAll) {
          index = SDL_AddDollarGesture(NULL,path);
          for(i = 0;i < SDL_numGestureTouches; i++)
            SDL_gestureTouch[i].recording = SDL_FALSE;
        }
        else {
          index = SDL_AddDollarGesture(inTouch,path);
        }
        
        if(index >= 0) {
          SDL_SendDollarRecord(inTouch,inTouch->dollarTemplate[index].hash);
        }
        else {
          SDL_SendDollarRecord(inTouch,-1);
        }
      }
      else {        
        int bestTempl;
        float error;

        error = dollarRecognize(&inTouch->dollarPath,
                                &bestTempl,inTouch);

#ifdef	ENABLE_SLIDE_GESTURE
		g_HWratio = getSlideRatio(&gSlideP);
		g_SlideTheta = getSlideTheta(&gSlideP);
		getSlideXYPoint(&gSlideP, &g_SlideXY);
		
		#ifdef	ENABLE_SLIDE_DBG_MSG
		printf("[slide] ang=%f, gHW=%f, HW_ratio=%d\n",g_SlideTheta,g_HWratio,HW_RATIO_THRESHOLD);
		#endif
		
        if( (g_HWratio>HW_RATIO_THRESHOLD) && (g_SlideDistance>100) )
        {
        	unsigned long gestureId = getSlideDirection(g_SlideTheta);

        	SDL_SendGestureSlide( inTouch, gestureId, error);
        }
#endif

        if (bestTempl >= 0){
          //Send Event
          unsigned long gestureId = inTouch->dollarTemplate[bestTempl].hash;
          SDL_SendGestureDollar(inTouch,gestureId,error);
		  //printf ("%s\n",);("Dollar error: %f\n",error);
	    }

      }
#endif 
      //inTouch->gestureLast[j] = inTouch->gestureLast[inTouch->numDownFingers];
      if(inTouch->numDownFingers > 0) {
        inTouch->centroid.x = (inTouch->centroid.x*(inTouch->numDownFingers+1)-
                               x)/inTouch->numDownFingers;
        inTouch->centroid.y = (inTouch->centroid.y*(inTouch->numDownFingers+1)-
                               y)/inTouch->numDownFingers;
      }
    }
    else if(event->type == SDL_FINGERMOTION) {
#ifdef ENABLE_NORMALIZE_VECTORS_TO_SIMPLIFY_ANGLE_CALCULATION
		float dx = 1.0;
		float dy = 1.0;
		
		if (inTouch->res.x >0.000001 || inTouch->res.x < -0.000001)
		{
			dx = ((float)event->tfinger.dx)/(float)inTouch->res.x;
		}

		if (inTouch->res.y >0.000001 || inTouch->res.y < -0.000001)
		{
	  		dy = ((float)event->tfinger.dy)/(float)inTouch->res.y;
		}
		
#else
      float dx = ((float)event->tfinger.dx)/(float)inTouch->res.x;
      float dy = ((float)event->tfinger.dy)/(float)inTouch->res.y;
#endif

      //printf("dx,dy: (%f,%f)\n",dx,dy); 
#ifdef ENABLE_DOLLAR
      SDL_DollarPath* path = &inTouch->dollarPath;
      if(path->numPoints < MAXPATHSIZE) {
        path->p[path->numPoints].x = inTouch->centroid.x;
        path->p[path->numPoints].y = inTouch->centroid.y;
        pathDx = 
          (path->p[path->numPoints].x-path->p[path->numPoints-1].x);
        pathDy = 
          (path->p[path->numPoints].y-path->p[path->numPoints-1].y);
        path->length += (float)SDL_sqrt(pathDx*pathDx + pathDy*pathDy);
        path->numPoints++;
      }
#endif
      lastP.x = x - dx;
      lastP.y = y - dy;
      lastCentroid = inTouch->centroid;
      
#ifdef ENABLE_NORMALIZE_VECTORS_TO_SIMPLIFY_ANGLE_CALCULATION
	  if (inTouch->numDownFingers == 0)	inTouch->numDownFingers++;
#endif

      inTouch->centroid.x += dx/inTouch->numDownFingers;
      inTouch->centroid.y += dy/inTouch->numDownFingers;
      //printf("Centrid : (%f,%f)\n",inTouch->centroid.x,inTouch->centroid.y);
      if(inTouch->numDownFingers > 1) {
        SDL_FloatPoint lv; //Vector from centroid to last x,y position
        SDL_FloatPoint v; //Vector from centroid to current x,y position
        //lv = inTouch->gestureLast[j].cv;
        lv.x = lastP.x - lastCentroid.x;
        lv.y = lastP.y - lastCentroid.y;
        lDist = (float)SDL_sqrt(lv.x*lv.x + lv.y*lv.y);
        //printf("lDist = %f\n",lDist);
        v.x = x - inTouch->centroid.x;
        v.y = y - inTouch->centroid.y;
        //inTouch->gestureLast[j].cv = v;
        Dist = (float)SDL_sqrt(v.x*v.x+v.y*v.y);
        // SDL_cos(dTheta) = (v . lv)/(|v| * |lv|)
        
        //Normalize Vectors to simplify angle calculation
#ifdef ENABLE_NORMALIZE_VECTORS_TO_SIMPLIFY_ANGLE_CALCULATION
        if (lDist <0.000001 && lDist > -0.000001)
        	{
        	   lDist = 1.0;
        	}

	if (Dist <0.0000001 && Dist > -0.0000001)
        	{
        	   Dist = 1.0;
        	}
#endif
        lv.x/=lDist;
        lv.y/=lDist;
        v.x/=Dist;
        v.y/=Dist;
        dtheta = (float)SDL_atan2(lv.x*v.y - lv.y*v.x,lv.x*v.x + lv.y*v.y);
        
        dDist = (Dist - lDist);
        if(lDist == 0) {dDist = 0;dtheta = 0;} //To avoid impossible values
        
        //inTouch->gestureLast[j].dDist = dDist;
        //inTouch->gestureLast[j].dtheta = dtheta;
        
        //printf("dDist = %f, dTheta = %f\n",dDist,dtheta);
        //gdtheta = gdtheta*.9 + dtheta*.1;
        //gdDist  =  gdDist*.9 +  dDist*.1
        //knob.r += dDist/numDownFingers;
        //knob.ang += dtheta;
        //printf("thetaSum = %f, distSum = %f\n",gdtheta,gdDist);
        //printf("id: %i dTheta = %f, dDist = %f\n",j,dtheta,dDist);
        SDL_SendGestureMulti(inTouch,dtheta,dDist);
      }
      else {
        //inTouch->gestureLast[j].dDist = 0;
        //inTouch->gestureLast[j].dtheta = 0;
        //inTouch->gestureLast[j].cv.x = 0;
        //inTouch->gestureLast[j].cv.y = 0;
      }
      //inTouch->gestureLast[j].f.p.x = x;
      //inTouch->gestureLast[j].f.p.y = y;
      //break;
      //pressure?
    }
    
    if(event->type == SDL_FINGERDOWN) {

      inTouch->numDownFingers++;
      inTouch->centroid.x = (inTouch->centroid.x*(inTouch->numDownFingers - 1)+ 
                             x)/inTouch->numDownFingers;
      inTouch->centroid.y = (inTouch->centroid.y*(inTouch->numDownFingers - 1)+
                             y)/inTouch->numDownFingers;
      //printf("Finger Down: (%f,%f). Centroid: (%f,%f\n",x,y,
      //     inTouch->centroid.x,inTouch->centroid.y);

#ifdef ENABLE_DOLLAR
      inTouch->dollarPath.length = 0;
      inTouch->dollarPath.p[0].x = x;
      inTouch->dollarPath.p[0].y = y;
      inTouch->dollarPath.numPoints = 1;
#endif
    }
  }
}

void
SDL_GestureQuit(void)
{
    if (SDL_gestureTouch) {
        SDL_free(SDL_gestureTouch);
        SDL_gestureTouch = NULL;
    }
}

  /* vi: set ts=4 sw=4 expandtab: */