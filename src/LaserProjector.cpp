//
//  LaserProjector.cpp
//  Laser_tools_V2
//
//  Created by Jonas Fehr on 27/04/2018.
//

#include "LaserProjector.h"

void LaserProjector::setup(string identifier, int interfaceID, int x, int y, int w, int h){
    this->identifier = identifier;
    
    setupParameters();
    
    ofxRayComposer::setup(true, interfaceID);
    ofxRayComposer::setPPS(LASER_PPSx1000*1000);
    
    
    ildaFrame.setup();
    
    ofImage colorMap;
    colorMap.load("rainbow.png");
    ildaFrame.params.output.colorMap = colorMap.getPixels();
    
//    addZone("zone_1");
//    addZone("zone_2");
//    addZone("zone_3");
    
//    hotSpotHandler.setup(x,y,w,h);
//    bezierHandler.setup(x,y,w,h);
    
    setPreview(x, y, w, h);
}

void LaserProjector::setupParameters(){
    parameters.setName(deviceID);
    parameters.add(this->identifier.set("identifier:", identifier));
    parameters.add(isActivated.set("isActivated", false));
    parameters.add(showTestPattern.set("showTestPattern", false));
    parameters.add(LASER_PPSx1000.set("LASER_PPSx1000", 30, 0.1, 30));
    parameters.add(ildaFrame.parameters);
};

void LaserProjector::update(){
    ildaFrame.clear();
    
    if(ildaFrame.params.output.useColorMap){
        ofFbo fboColorMap;
        fboColorMap.allocate(512, 512);
        fboColorMap.begin();
        syphonIn.draw(0,0,fboColorMap.getWidth(), fboColorMap.getHeight());
        fboColorMap.end();
        fboColorMap.readToPixels(ildaFrame.params.output.colorMap);
    }
    
    for (auto & z : zones){
        ildaFrame.addPolys(z.second.update());
    }
    
//    hotSpotHandler.update();
    ildaFrame.addPolys(hotSpotHandler.getPolys());
    
//    bezierHandler.update();
    ildaFrame.addPolys(bezierHandler.getPolys());
    
    
    if(showTestPattern){
        ildaFrame.clear();
        TestPattern testPattern;
        ildaFrame.addPolys(testPattern.polys);
    }
    
    ildaFrame.update();
    
    // send points to the DAC
    if(isActivated) ofxRayComposer::setPoints(ildaFrame);
    
}


void LaserProjector::draw(){
    // Draw Projector outher frame
    ofSetColor(COLOR_BORDER);
    ofSetLineWidth(1);
    ofNoFill();
    ofDrawRectangle(previewX,previewY,previewW,previewH);
    int offset = 10;
    ofDrawBitmapString(identifier,previewX+offset,previewY+offset );
    
    // draw Content
    ildaFrame.draw(previewX,previewY,previewW,previewH);
    
    // Draw UI
    for (auto & z : zones)
    {
        z.second.draw(previewX,previewY,previewW,previewH);
    }
    
    hotSpotHandler.draw();
    bezierHandler.draw();
    
}

void LaserProjector::draw(int x, int y, int h, int w){
    setPreview(x,y,w,h);
    this->draw();
}

void LaserProjector::addZone(string name){
    zones[name].identifier = name;
}

void LaserProjector::setPreview(int x, int y, int w, int h){
    previewX = x;
    previewY = y;
    previewW = w;
    previewH = h;
    hotSpotHandler.setPreview(x,y,w,h);
    bezierHandler.setPreview(x,y,w,h);
}


void LaserProjector::addToJson(ofJson & json){
    
    auto & j = json[identifier];
    j["identifier"] = identifier;
    hotSpotHandler.addToJson(j);
    bezierHandler.addToJson(j);
    
    auto & jm = j["MappingZone"];
    for(auto & z:zones ){
        z.second.addToJson(jm);
    }
    j["DAC"]["DeviceID"] = getDeviceID();
    j["DAC"]["ConnectionID"] = getConnectionID();
    j["previewWindow"]["x"] = previewX;
    j["previewWindow"]["y"] = previewY;
    j["previewWindow"]["w"] = previewW;
    j["previewWindow"]["h"] = previewH;

    
    auto & jp = j["parameters"];
    ofSerialize(jp, parameters);
    return json;
}

void LaserProjector::setupFromJson(ofJson & json){
    
    identifier = json["identifier"].get<std::string>();
    
    hotSpotHandler.setupFromJson(json["HotSpotHandler"]);
    bezierHandler.setupFromJson(json["BezierHandler"]);
    
    for(auto & j : json["MappingZone"] ){
        zones[j["identifier"]].setupFromJson(j);
    }
    
    previewX = json["previewWindow"]["x"];
    previewY = json["previewWindow"]["y"];
    previewW = json["previewWindow"]["w"];
    previewH = json["previewWindow"]["h"];

    deviceID = json["DAC"]["DeviceID"];
    ildaFrame.setup();

    
    ofxRayComposer::setup(true, json["DAC"]["ConnectionID"]);
    
    setPreview(previewX, previewY, previewW, previewH);
    
    setupParameters();
    
    auto & jp = json["parameters"];
    ofDeserialize(jp, parameters);

    
    // Syphon IN
    syphonIn.setup();
    syphonIn.find( "toLaser_"+(string)identifier, "MadMapper");
    
    ofFbo fboColorMap;
    fboColorMap.allocate(512, 512);
    fboColorMap.begin();
    syphonIn.draw(0,0,fboColorMap.getWidth(), fboColorMap.getHeight());
    fboColorMap.end();
    fboColorMap.readToPixels(ildaFrame.params.output.colorMap);

}
