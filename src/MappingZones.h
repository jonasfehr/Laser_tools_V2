//
//  MappingZones.h
//  Laser_tools_V2
//
//  Created by Jonas Fehr on 26/04/2018.
//

#ifndef MappingZones_h
#define MappingZones_h

#include "LaserGrafics.h"
#include "Mapper.h"
#include "ofxIldaFrame.h"
#include "VisualPresets.h"

class MappingZone : public JsonUtil {
public:
    //    map<int, *PolyGrafic> simpleGrafics;
    map<int, SimpleLine> lines;
    map<int, SimpleCircle> circles;
    map<int, SimpleTriangle> triangles;
    map<int, SimpleGate> gates;
    map<int, DotCircle> dotCircles;
    map<int, MorphinPoly> morphinPolys;
    
    vector<DancingLine> dancingLines;
    int numOfDLinesLast = 0;
    
    Mapper mapper;
    
    TextInput textInput;
    bool doText = false;
    
    MappingZone(){
        identifier = "MappingZone";
    };
    
    void draw(int x, int y, int w, int h){
        mapper.draw(x,y,w,h);
        
        // draw name
        ofSetColor(COLOR_ID_STRING);
        int offset = 10;
        ofDrawBitmapString(identifier, x+mapper.getDstPoint(0).x*w+offset, y+mapper.getDstPoint(0).y*h+offset);
    }
    
    void addToJson(ofJson & json){
        auto &j = json[identifier];
        j["identifier"] = identifier;
        mapper.addToJson(j);
    }
    
    void setupFromJson(ofJson & json){
        
        identifier = json["identifier"].get<std::string>();
        mapper.setupFromJson(json["mapper"]);
    }
    
    vector<ofxIlda::Poly> update(){
        vector<ofxIlda::Poly> polys;
        // iterate through circles
        for(auto & c : circles){
            polys.push_back(mapper.map(c.second.poly));
            
            if(!c.second.isUpdated()) {
                circles.erase(c.first);
                return;
            }
        }
        // iterate through dotCircles
        for(auto & c : dotCircles){
            for(auto & poly : mapper.map(c.second.polys)){
                polys.push_back(poly);
            }
            
            c.second.life += 1;
            if(c.second.life > 5) {
                dotCircles.erase(c.first);
                return;
            }
        }
        // iterate through lines
        for(auto & l : lines){
            polys.push_back(mapper.map(l.second.poly));
            
            if(!l.second.isUpdated()) {
                lines.erase(l.first);
                return;
            }
        }
        
        // iterate through gates
        for(auto & g : gates){
            ofxIlda::Poly poly = mapper.map(g.second.poly);
            polys.push_back(poly);
            
            if(!g.second.isUpdated()) {
                gates.erase(g.first);
                return;
            }
        }
        
        // iterate through morphing Polys
        for(auto & mP : morphinPolys){
            polys.push_back(mapper.map(mP.second.poly));
            
            mP.second.life += 1;
            if(mP.second.life > 10) {
                morphinPolys.erase(mP.first);
                return;
            }
        }
        
        for (auto & dl : dancingLines)    {
            dl.update();
            polys.push_back(mapper.map(dl.poly));
        }
        
        if(doText){
            textInput.update();
            for(auto & poly : mapper.map(textInput.polys)){
                polys.push_back(poly);
            }
        }
        
        // iterate through triangles
        for(auto & t : triangles){
            polys.push_back(mapper.map(t.second.poly));
            
            if(!t.second.isUpdated()) {
                triangles.erase(t.first);
                return;
            }
        }
        return polys;
    }
};


#endif /* MappingZones_h */

