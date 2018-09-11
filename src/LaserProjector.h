//
//  LaserProjector.h
//  Laser_tools_V2
//
//  Created by Jonas Fehr on 26/04/2018.
//

#ifndef LaserProjector_h
#define LaserProjector_h

#include "ofxRayComposer.h"
#include "ofxIldaFrame.h"
#include "ofMain.h"
#include "MappingZones.h"
#include "LaserGrafics.h"
#include "HotSpotHandler.h"
#include "BezierHandler.h"
#include "ofxGui.h"

#include "ofxSyphonUtils.h"

class LaserProjector : public ofxRayComposer, public JsonUtil{
public:
    ofxIlda::Frame ildaFrame;   // stores and manages ILDA frame drawings
    
    // GUI / control
    ofParameterGroup parameters;
    ofParameter<bool> isActivated;
    ofParameter<bool> showTestPattern;
    ofParameter<int> LASER_PPSx1000;
    
    void setupParameters();
    
    string deviceID;
    
    // content
    map<string, MappingZone> zones;
    HotSpotHandler hotSpotHandler;
    BezierHandler bezierHandler;
    
    ofxSyphonClientDir syphonIn;

    // methods
    void setup(string identifier, int interfaceID, int  x = 0, int y = 0, int w = 512, int h = 512);
    void update();
    void updateZones();
    void draw();
    void draw(int x, int y, int h, int w);
    
    void addZone(string name);
    void addHS(string name);
    void addBC(string name);

    void setPreview(int x, int y, int w, int h);
    
    int previewX, previewY, previewW, previewH;
    
    void addToJson(ofJson & json);
    void setupFromJson(ofJson & json);
};

#endif /* LaserProjector_h */

