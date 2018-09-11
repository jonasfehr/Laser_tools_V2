//
//  HotSpotHandler.cpp
//  Laser_tools_V2
//
//  Created by Jonas Fehr on 27/04/2018.
//

#include "HotSpotHandler.h"
//-----------------------------------------------------
// HotSpot
//-----------------------------------------------------
void HotSpot::setup(){
}
void HotSpot::update(){
    int numPoints = 1;
    if(poly.size()<numPoints){
        poly.clear();
        for(int i = 0; i<numPoints; i++){
            poly.addVertex(position);
        }
    }else{
        for(int i = 0; i<numPoints; i++){
            poly.getVertices()[i] = position;
        }
    }
    PolyGrafic::updated();
}


void HotSpot::addToJson(ofJson & json){
    auto &j = json[identifier];
    j["identifier"] = identifier;
    j["color"] = fColorToJson(poly.color);
    j["position"] = vec3ToJson(position);
}
void HotSpot::setupFromJson(ofJson & json){
    identifier = json["identifier"].get<std::string>();
    poly.color = jsonTofColor(json["color"]);
    position = jsonToVec3(json["position"]);
}

//-----------------------------------------------------
// HotSpotHandler
//-----------------------------------------------------
HotSpotHandler::HotSpotHandler(){
    initListeners();
    identifier = "HotSpotHandler";
}

HotSpotHandler::~HotSpotHandler(){
    removeListeners();
     save(presetPath);
}

void HotSpotHandler::setup(int x, int y, int w, int h, string presetPath){
//    this->presetPath = presetPath;
//    load(presetPath);
    

    setPreview(x,y,w,h);
    
//    add("hs_08", 0.5, 0.5);
//    add("hs_09", 0.6, 0.6);
//    add("hs_10", 0.7, 0.7);
//    add("hs_11", 0.8, 0.8);
}
void HotSpotHandler::setPreview(int x, int y, int w, int h){
    origin = glm::vec3(x,y,0);
    scaleUnit = glm::vec3(w,h,1);
}



void HotSpotHandler::update(){
    for( auto & hS : hotSpots){
        hS.second.update();
    }
}

vector<ofxIlda::Poly> HotSpotHandler::getPolys(){
    vector<ofxIlda::Poly> polys;
    for( auto & hS : hotSpots){
        if(hS.second.isUpdated()) polys.push_back(hS.second.poly);
    }
    return polys;
}

void HotSpotHandler::draw(){
    glm::vec3 mousePoint = glm::vec3(ofGetMouseX(), ofGetMouseY(),0.);
    mousePoint = mousePoint-origin;
    mousePoint = mousePoint/scaleUnit;
    
    ofPushMatrix();
    {
        ofTranslate(origin);
        ofScale(scaleUnit);
        for( auto & hS : hotSpots){
            bool isOver = hS.second.hitTest(mousePoint, scaleUnit.x);
            hS.second.drawHandle(scaleUnit.x,isOver);
           
            float textOffset = 6./scaleUnit.x;
            
            ofSetColor(COLOR_ID_STRING);
            ofDrawBitmapString(hS.second.identifier, hS.second.position.x+textOffset, hS.second.position.y-textOffset );
        }
    }
    ofPopMatrix();
}


void HotSpotHandler::add(string identifier, float x, float y, ofFloatColor color){
    HotSpot newHotSpot;
    newHotSpot.identifier = identifier;
    newHotSpot.position = glm::vec3(x, y, 0.);
    newHotSpot.poly.color = color;
    newHotSpot.setup();
    hotSpots[identifier] = newHotSpot;
    }

// --- interaciton
void HotSpotHandler::initListeners(){
    ofAddListener(ofEvents().mousePressed, this, &HotSpotHandler::mousePressed, OF_EVENT_ORDER_BEFORE_APP);
    ofAddListener(ofEvents().mouseReleased, this, &HotSpotHandler::mouseReleased, OF_EVENT_ORDER_BEFORE_APP);
    ofAddListener(ofEvents().mouseDragged, this, &HotSpotHandler::mouseDragged, OF_EVENT_ORDER_BEFORE_APP);
}

void HotSpotHandler::removeListeners(){
    ofRemoveListener(ofEvents().mousePressed, this, &HotSpotHandler::mousePressed, OF_EVENT_ORDER_BEFORE_APP);
    ofRemoveListener(ofEvents().mouseReleased, this, &HotSpotHandler::mouseReleased, OF_EVENT_ORDER_BEFORE_APP);
    ofRemoveListener(ofEvents().mouseDragged, this, &HotSpotHandler::mouseDragged, OF_EVENT_ORDER_BEFORE_APP);
}

void HotSpotHandler::startDragging(string key, ofPoint clickPos) {
    hotSpots[key].startDrag(clickPos);
}

bool HotSpotHandler::mousePressed(ofMouseEventArgs &e){
    
    if(!visible) return false;
    
    bool handleHit = false;
    ofPoint mousePoint = e;
    mousePoint-=origin;
    mousePoint/=scaleUnit;

    for(auto & hs : hotSpots) {
        if(hs.second.hitTest(mousePoint, scaleUnit.x)) {
            startDragging(hs.first, mousePoint);
            handleHit = true;
        }
        
    }
    
    return handleHit;
    
}

bool HotSpotHandler::mouseDragged(ofMouseEventArgs &e){
    
    ofPoint mousePoint = e;
    mousePoint-=origin;
    mousePoint/=scaleUnit;
    
    //ofRectangle bounds(centreHandle, 0, 0);
    bool dragging = false;
    for(auto & hs : hotSpots) {
        if(hs.second.updateDrag(mousePoint)) dragging = true;
    }
    
    isDirty |= dragging;
    
    return dragging;
}


bool HotSpotHandler::mouseReleased(ofMouseEventArgs &e){
    
    bool wasDragging = false;
    
    for(auto & hs : hotSpots) {
        if(hs.second.stopDrag()) wasDragging = true;
    }

    if(wasDragging) save(presetPath);
    
    return wasDragging;
}




// JSON
void HotSpotHandler::addToJson(ofJson & json){
    auto &j = json[identifier];
    j["identifier"] = identifier;
    for( auto & hS : hotSpots){
        hS.second.addToJson(j);
    }
}
void HotSpotHandler::setupFromJson(ofJson & json){
    identifier = json["identifier"].get<std::string>();
    
    for( auto & j : json){
        if(j.is_object()){
        HotSpot  newHotSpot;
        newHotSpot.setupFromJson(j);
        hotSpots[newHotSpot.identifier] = newHotSpot;
        }
    }
}

