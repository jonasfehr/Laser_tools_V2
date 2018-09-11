//
//  BezierHandler.hpp
//  Laser_tools_V2
//
//  Created by Jonas Fehr on 28/04/2018.
//

#ifndef BezierHandler_h
#define BezierHandler_h

#include <stdio.h>

#include "ofMain.h"
#include "SimpleGrafics.h"
#include "DragHandle.h"
#include "JsonUtil.h"
//#include "jsonHelpers.h"
//#include "ofxIldaFrame.h"
//-----------------------------------------------------
// BezierPoint
//-----------------------------------------------------
class BezierPoint_Center : public DragHandle{
public:
    DragHandle handle_1, handle_2;
    
    void draw(float scale = 1 , bool isOver = false);
};

class BezierPoint_Cap : public DragHandle{
public:
    DragHandle handle;
    
    void draw(float scale = 1 , bool isOver = false);
};

//-----------------------------------------------------
// BezierCourve
//-----------------------------------------------------
class BezierCourve : public PolyGrafic, public JsonUtil{
public:
    
    BezierPoint_Cap startBP, endBP;
    BezierPoint_Center centerBP;
    
    void setup();
    void update();
    void drawCourve();
    
    void addToJson(ofJson & json);
    void setupFromJson(ofJson & json);
};

//-----------------------------------------------------
// BezierHandler
//-----------------------------------------------------
class BezierHandler : public JsonUtil{
public:
    string presetPath;
    
    map<string,BezierCourve> bezierCourves;
    bool visible = true;
    glm::vec3 origin;
    glm::vec3 scaleUnit = glm::vec3(1,1,1);
//    bool editable;
    bool isDirty = true;
    
    vector<DragHandle*> handles;
    
    
    BezierHandler();
    ~BezierHandler();
    
    void setup(int x, int y, int w, int h, string presetPath = "settings/bezierCourves.json");
    void setPreview(int x, int y, int w, int h);

    void update();
    void draw();
    
    void add(string identifier, glm::vec3 start, glm::vec3 center, glm::vec3 end, ofFloatColor color = ofFloatColor(1.,1.,1.,1.));
    
    void updateHandles();
    
    vector<ofxIlda::Poly> getPolys();
    
    // Interaction
    void initListeners();
    void removeListeners();
    
    bool mousePressed(ofMouseEventArgs &e);
    bool mouseDragged(ofMouseEventArgs &e);
    bool mouseReleased(ofMouseEventArgs &e);
    void startDragging(int handleIndex, ofPoint clickPos);
    
    
    // save / load.
    void load(const string& path);
    
    void addToJson(ofJson & json);
    void setupFromJson(ofJson & json);
};
#endif /* BezierHandler_h */

