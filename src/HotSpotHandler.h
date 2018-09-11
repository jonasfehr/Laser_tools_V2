//
//  HotSpotHandler.hpp
//  Laser_tools_V2
//
//  Created by Jonas Fehr on 27/04/2018.
//

#ifndef HotSpotHandler_hpp
#define HotSpotHandler_hpp
#include <stdio.h>

#include "ofMain.h"
#include "SimpleGrafics.h"
#include "DragHandle.h"
#include "JsonUtil.h"

//#include "ofxIldaFrame.h"

class HotSpot : public PolyGrafic, public DragHandle, public JsonUtil{
public:
    ofParameter<bool> on;
    
    void setup();
    void update();
    void drawID();
    
    void addToJson(ofJson & json);
    void setupFromJson(ofJson & json);
};


class HotSpotHandler : public JsonUtil{
public:
    string presetPath;
    
    map<string,HotSpot> hotSpots;
    bool visible = true;
    glm::vec3 origin;
    glm::vec3 scaleUnit = glm::vec3(1,1,1);
    string label;
    bool editable;
    bool isDirty = true;
    
    
    HotSpotHandler();
   ~HotSpotHandler();
    
    void setup(int x, int y, int w, int h, string presetPath = "settings/hotspots.json");
    void setPreview(int x, int y, int w, int h);
    void update();
    void draw();

    void add(string identifier, float x, float y, ofFloatColor color = ofFloatColor(1.,1.,1.,1.));
    
    vector<ofxIlda::Poly> getPolys();
    
    // Interaction
    void initListeners();
    void removeListeners();
    
    bool mousePressed(ofMouseEventArgs &e);
    bool mouseDragged(ofMouseEventArgs &e);
    bool mouseReleased(ofMouseEventArgs &e);
    void startDragging(string key, ofPoint clickPos);
    
    
    // save / load.
    void addToJson(ofJson & json);
    void setupFromJson(ofJson & json);

};

#endif /* HotSpotHandler_hpp */
