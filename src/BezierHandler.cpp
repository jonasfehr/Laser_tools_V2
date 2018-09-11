//
//  BezierHandler.cpp
//  Laser_tools_V2
//
//  Created by Jonas Fehr on 27/04/2018.
//

#include "BezierHandler.h"
//-----------------------------------------------------
// BezierPoint
//-----------------------------------------------------

void BezierPoint_Center::draw(float scale, bool isOver){
    
    ofSetColor(COLOR_BORDER);
    ofSetLineWidth(1/scale);
    ofDrawLine(position, handle_1.position);
    ofDrawLine(position, handle_2.position);
    
    DragHandle::drawHandle(scale, isOver);
    handle_1.drawHandle(scale, isOver);
    handle_2.drawHandle(scale, isOver);
}

void BezierPoint_Cap::draw(float scale, bool isOver){
    
    ofSetColor(COLOR_BORDER);
    ofSetLineWidth(1/scale);
    ofDrawLine(position, handle.position);
    
    DragHandle::drawHandle(scale, isOver);
    handle.drawHandle(scale, isOver);
}

//-----------------------------------------------------
// BezierCourve
//-----------------------------------------------------
void BezierCourve::setup(){
    startBP.handle.position = startBP.position - glm::vec3(0.1, 0, 0);
    centerBP.handle_1.position = centerBP.position - glm::vec3(0.1, 0.1, 0);
    centerBP.handle_2.position = centerBP.position + glm::vec3(0.1, 0.1, 0);
    endBP.handle.position = endBP.position + glm::vec3(0.1, 0, 0);
}

void BezierCourve::update(){
    int res = 20;
    poly.clear();
    poly.addVertex(startBP.position);
    poly.bezierTo(startBP.handle.position, centerBP.handle_1.position, centerBP.position, res);
    poly.bezierTo(centerBP.handle_2.position, endBP.handle.position, endBP.position, res);
    
    updated();
}

void BezierCourve::drawCourve(){
    int res = 20;
    ofPolyline p;
    p.clear();
    p.addVertex(startBP.position);
    p.bezierTo(startBP.handle.position, centerBP.handle_1.position, centerBP.position, res);
    p.bezierTo(centerBP.handle_2.position, endBP.handle.position, endBP.position, res);
    p.draw();
    
}

void BezierCourve::addToJson(ofJson & json){
    auto &j = json[identifier];
    j["identifier"] = identifier;
    j["startBP"] = vec3ToJson(startBP.position);
    j["startBP"]["handle"] = vec3ToJson(startBP.handle.position);
    j["centerBP"] = vec3ToJson(centerBP.position);
    j["centerBP"]["handle_1"] = vec3ToJson(centerBP.handle_1.position);
    j["centerBP"]["handle_2"] = vec3ToJson(centerBP.handle_2.position);
    j["endBP"] = vec3ToJson(endBP.position);
    j["endBP"]["handle"] = vec3ToJson(endBP.handle.position);
}


void BezierCourve::setupFromJson(ofJson & json){
    identifier = json["identifier"].get<std::string>();
    startBP.position = jsonToVec3(json["startBP"]);
    startBP.handle.position = jsonToVec3(json["startBP"]["handle"]);
    centerBP.position = jsonToVec3(json["centerBP"]);
    centerBP.handle_1.position = jsonToVec3(json["centerBP"]["handle_1"]);
    centerBP.handle_2.position = jsonToVec3(json["centerBP"]["handle_2"]);
    endBP.position = jsonToVec3(json["endBP"]);
    endBP.handle.position = jsonToVec3(json["endBP"]["handle"]);
}



//-----------------------------------------------------
// BezierHandler
//-----------------------------------------------------
BezierHandler::BezierHandler(){
    initListeners();
    identifier = "BezierHandler";
}

BezierHandler::~BezierHandler(){
    removeListeners();
    save(presetPath);
}

void BezierHandler::setup(int x, int y, int w, int h, string presetPath){
    
    setPreview(x,y,w,h);
    
//    add("bc_03", glm::vec3(0.2,0.8,0), glm::vec3(0.5,0.8,0), glm::vec3(0.8,0.8,0));
//    add("bc_02", glm::vec3(0.2,0.7,0), glm::vec3(0.5,0.7,0), glm::vec3(0.8,0.7,0));
//    add("bc_01", glm::vec3(0.2,0.6,0), glm::vec3(0.5,0.6,0), glm::vec3(0.8,0.6,0));
//    add("bc_00", glm::vec3(0.2,0.5,0), glm::vec3(0.5,0.5,0), glm::vec3(0.8,0.5,0));
    
//    load(presetPath);

}
void BezierHandler::setPreview(int x, int y, int w, int h){
    origin = glm::vec3(x,y,0);
    scaleUnit = glm::vec3(w,h,1);
}

void BezierHandler::update(){
    for( auto & bC : bezierCourves){
        bC.second.update();
    }
}

vector<ofxIlda::Poly> BezierHandler::getPolys(){
    vector<ofxIlda::Poly> polys;
    for( auto & bC : bezierCourves){
        if(bC.second.isUpdated()) polys.push_back(bC.second.poly);
    }
    return polys;
}

void BezierHandler::draw(){
    ofPoint mousePoint = ofPoint(ofGetMouseX(), ofGetMouseY());
    mousePoint-=origin;
    mousePoint/=scaleUnit;
    
    ofPushMatrix();
    {
        ofTranslate(origin);
        ofScale(scaleUnit);
        
        for( auto & h : handles){
            bool isOver = h->hitTest(mousePoint, scaleUnit.x);
            h->drawHandle(scaleUnit.x,isOver);
        }
        
        for( auto & bC : bezierCourves){
            ofSetColor(COLOR_BORDER);
            ofSetLineWidth(2/scaleUnit.x);
            ofDrawLine(bC.second.startBP.handle.position, bC.second.startBP.position);
            ofDrawLine(bC.second.centerBP.handle_1.position, bC.second.centerBP.position);
            ofDrawLine(bC.second.centerBP.handle_2.position, bC.second.centerBP.position);
            ofDrawLine(bC.second.endBP.handle.position, bC.second.endBP.position);
            bC.second.drawCourve();
            
            float textOffset = 6/scaleUnit.x;
            ofSetColor(COLOR_ID_STRING);
            ofDrawBitmapString(bC.second.identifier, bC.second.startBP.position.x+textOffset, bC.second.startBP.position.y-textOffset );
        }
        
    }
    ofPopMatrix();
}


void BezierHandler::add(string identifier, glm::vec3 start, glm::vec3 center, glm::vec3 end, ofFloatColor color){
    BezierCourve newBezierCourve;
    newBezierCourve.identifier = identifier;
    newBezierCourve.startBP.position = start;
    newBezierCourve.centerBP.position = center;
    newBezierCourve.endBP.position = end;
    newBezierCourve.poly.color = color;
    newBezierCourve.setup();
    bezierCourves[identifier] = newBezierCourve;
    
    updateHandles();
}

void BezierHandler::updateHandles(){
    handles.clear();
    for( auto & bC : bezierCourves){
        handles.push_back(&bC.second.startBP);
        handles.push_back(&bC.second.startBP.handle);
        handles.push_back(&bC.second.centerBP.handle_1);
        handles.push_back(&bC.second.centerBP);
        handles.push_back(&bC.second.centerBP.handle_2);
        handles.push_back(&bC.second.endBP);
        handles.push_back(&bC.second.endBP.handle);
    }
}

// --- interaciton
void BezierHandler::initListeners(){
    ofAddListener(ofEvents().mousePressed, this, &BezierHandler::mousePressed, OF_EVENT_ORDER_BEFORE_APP);
    ofAddListener(ofEvents().mouseReleased, this, &BezierHandler::mouseReleased, OF_EVENT_ORDER_BEFORE_APP);
    ofAddListener(ofEvents().mouseDragged, this, &BezierHandler::mouseDragged, OF_EVENT_ORDER_BEFORE_APP);
}

void BezierHandler::removeListeners(){
    ofRemoveListener(ofEvents().mousePressed, this, &BezierHandler::mousePressed, OF_EVENT_ORDER_BEFORE_APP);
    ofRemoveListener(ofEvents().mouseReleased, this, &BezierHandler::mouseReleased, OF_EVENT_ORDER_BEFORE_APP);
    ofRemoveListener(ofEvents().mouseDragged, this, &BezierHandler::mouseDragged, OF_EVENT_ORDER_BEFORE_APP);
}

void BezierHandler::startDragging(int handleIndex, ofPoint clickPos) {
    handles[handleIndex]->startDrag(clickPos);
}

bool BezierHandler::mousePressed(ofMouseEventArgs &e){
    
    if(!visible) return false;
    
    bool handleHit = false;
    ofPoint mousePoint = e;
    mousePoint-=origin;
    mousePoint/=scaleUnit;
    
    for(unsigned int i = 0; i<handles.size(); i++) {
        if(handles[i]->hitTest(mousePoint, scaleUnit.x)) {
            startDragging(i, mousePoint);
            handleHit = true;
        }
        
    }
    
    return handleHit;
    
}

bool BezierHandler::mouseDragged(ofMouseEventArgs &e){
    
    ofPoint mousePoint = e;
    mousePoint-=origin;
    mousePoint/=scaleUnit;
    
    //ofRectangle bounds(centreHandle, 0, 0);
    bool dragging = false;
    for(unsigned int i = 0; i<handles.size(); i++) {
        if(handles[i]->updateDrag(mousePoint)) dragging = true;
    }
    
    isDirty |= dragging;
    
    return dragging;
}


bool BezierHandler::mouseReleased(ofMouseEventArgs &e){
    
    bool wasDragging = false;
    
    for(unsigned int i = 0; i<handles.size(); i++) {
        if(handles[i]->stopDrag()) wasDragging = true;
    }
    
    if(wasDragging)// save(presetPath);
    
    return wasDragging;
}

// save / load.


void BezierHandler::addToJson(ofJson & json){
    auto &j = json[identifier];
    j["identifier"] = identifier;
    
    for( auto & bC : bezierCourves){
        bC.second.addToJson(j);
    }
    
    return json;
}

void BezierHandler::setupFromJson(ofJson & json){
    identifier = json["identifier"].get<std::string>();

    for( auto & j : json){
        if(j.is_object()){
            BezierCourve newBezierCourve;
            newBezierCourve.setupFromJson(j);
            bezierCourves[j["identifier"]] = newBezierCourve;
        }
    }
    updateHandles();

}


