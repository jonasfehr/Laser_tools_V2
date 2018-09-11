#pragma once

#include "ofMain.h"

#include "ofxOsc.h"
#include "ofxGui.h"
#include "ofxSyphon.h"
#include "ofxSyphonUtils.h"
#include "LaserProjector.h"
#include "MappingZones.h"

#include "HotSpotHandler.h"

#define OSC_RECEIVE_PORT 9000
#define NUM_MSG_STRINGS 20

class ofApp : public ofBaseApp{
    
public:
    void setup();
    void update();
    void draw();
    void exit();
    
    void keyPressed  (int key);
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void windowResized(int w, int h);
    
    map<string, LaserProjector> laserProjectors;
    

    // OSC
    ofxOscReceiver oscReceiver;
    void parseOSC(ofxOscReceiver &receiver);
    int current_msg_string;
    string msg_strings[NUM_MSG_STRINGS];
    float timers[NUM_MSG_STRINGS];
    
    
    // GRAFICS
    
    ofVec2f center = ofVec2f(0.5,0.5);

    
    // GUI
    ofxPanel gui;
    bool showGui;
    
    ofxSyphonFbo syphonOut;
    
    ofxSyphonClientDir syphonIn;

};


