#ifndef _APP_CONTROLLER_H
#define _APP_CONTROLLER_H

#include "ofMain.h"

//we actually project at 1024 by 768
//but we stretch our texture up from 640 by 480
#define PROJECTION_W 1920
#define PROJECTION_H 1080

//utils for fake horz span 
//on win32
#include "winUtils.h"
#include "ofxGui.h"
//our other app objects
#include "guiSettingsManager.h"
#include "laserTracking.h"
#include "laserSending.h"
#include "imageProjection.h"
#include "baseGui.h"
#include "trackPlayer.h"
#include "colorManager.h"

//our brushes
#define NUM_BRUSHES 3

#include "baseBrush.h"  	//our base brush class - you must inhereit this to add your own
#include "basicVectorBrush.h"
#include "pngBrush.h"  		//the main png brush - allows you to design your own letters - by Theodore Watson
#include "graffLetter.h" 	//3D Looking bubble letters - by Zachary Lieberman
#include "vectorBrush.h" 	//openGL brush - different drawing modes - by Theodore Watson
#include "gestureBrush.h" 	//gesture machine brush - by Zachary Lieberman

#define STATUS_SHOW_TIME 3000  //in ms - this sets the fade time for the status text 

class appController : public baseGui{

	public:
	
		//---------------------------------------------------
		appController();
		void setup();
		void mainLoop();
		void selectPoint(float x, float y);
		void dragPoint(float x, float y);
		void releasePoint();
		void keyPress(int key);
		void keyRelease(int key);
		void draw();
		
	protected:
		void setupBrushes(int w, int h);
		void loadSettings();
		void saveSettings();
		void reloadSettings();
		void clearProjectedImage();
			
		void setupNetwork();
		void handleNetworkSending();
		void trackLaser();
		void manageMusic();
		void updateBrushSettings(bool first);
		void managePainting();		
		
		void drawGUI();		
		void drawStatusMessage();
		
		colorManager CM;
		
		ofVideoPlayer VP;
		bool webMovieLoaded;

		//an array of our base brush class
		baseBrush * brushes[NUM_BRUSHES];
				
		//other stuff	
		laserTracking LT;
		laserSending  LS;
		//guiSettingsManager GUI;
		imageProjection IP;
		trackPlayer TP;
		
		bool toggleGui, full, singleScreenMode, callibration, bInverted;
		int camWidth, camHeight, keyTimer;
		int brushMode;
		
		ofImage settingsImg;
		ofImage noticeImg;

		ofxPanel GUI;
		ofParameterGroup BRUSH_SETTINGS;
		ofParameter<int> BRUSH_MODE;
		ofParameter<int> BRUSH_WIDTH;
		ofParameter<int> BRUSH_NO;
		ofParameter<int> BRUSH_COLOR;
		ofParameter<int> LINE_RES;
		ofParameter<int> PROJ_BRIGHTNESS;
		
		ofParameterGroup DRIPS_SETTINGS;
		ofParameter<bool> DRIPS;
		ofParameter<int> DRIPS_FREQ;
		ofParameter<float> DRIPS_SPEED;
		ofParameter<int> DRIP_DIRECTION;

		ofParameterGroup TRACKING_SETTINGS;
		ofParameter<float> HUE_POINT;
		ofParameter<float> HUE_WIDTH;
		ofParameter<float> SAT_POINT;
		ofParameter<float> VAL_POINT;
		ofParameter<int> MIN_BLOB_SIZE;
		ofParameter<int> ACTIVITY;
		ofParameter<float> JUMP_DIST;

		ofParameterGroup CLEAR_ZONE_SETTINGS;
		ofParameter<bool> CLEAR_ZONE;
		ofParameter<int> CLEAR_THRESH;
		ofParameter<int> CLEAR_X;
		ofParameter<int> CLEAR_Y;
		ofParameter<int> CLEAR_W;
		ofParameter<int> CLEAR_H;

		ofParameterGroup NETWORK_SETTINGS;
		ofParameter<bool> NETWORK_SEND;
		ofParameter<bool> SEND_DATA;
		ofParameter<bool> UDP_OR_TCP;
		ofParameter<int> IP_PT1;
		ofParameter<int> IP_PT2;
		ofParameter<int> IP_PT3;
		ofParameter<int> IP_PT4;
		ofParameter<int> PORT;

		ofParameterGroup MUSIC_SETTINGS;
		ofParameter<bool> MUSIC;
		ofParameter<int> TRACK;
		ofParameter<int> VOL;

		ofParameterGroup CAMERA_SETTINGS;
		ofParameter<bool> USE_CAMERA;
		ofParameter<int> CAM_ID;
		ofParameter<int> CAM_WIDTH;
		ofParameter<int> CAM_HEIGHT;

		void onBrushModeChange(int & i);
		void onEnableNetwork(bool & b);
		void onMusicChange(bool& b);
		void onTrackChange(int & i);
};
#endif
