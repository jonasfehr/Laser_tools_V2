//
//  circles.h
//  DarkReflections
//
//  Created by Jonas Fehr on 18/08/16.
//
//

#include "ofMain.h"
//#include "ofxGuiExtended.h"
#include "ofxIldaFrame.h"

//#define TWO_PI 6.28318530718

#pragma once

class PolyGrafic{
public:
//    ofPolyline poly;
//    ofFloatColor color;
    ofxIlda::Poly poly;
    string oscAddr = "none";
    
    virtual void update()=0;
    
    void setColor(ofColor color)          { poly.color = color; };
    void setColor(ofFloatColor color)     { poly.color = color; };
    
//    ofColor getColor(){ return poly.color; };
    ofFloatColor getColor(){ return poly.color; };
    //Function to deleate grafic if not updates in a while
    
    bool isUpdated(){
        updatesSinceLastReset++;
        if(updatesSinceLastReset > 2) {
            return false;
        }else{
            return true;
        }
    };
    
    void updated(){
        updatesSinceLastReset = 0;
    }
    
private:
    int updatesSinceLastReset = 0;
};


class SimpleLine : public PolyGrafic{
public:
    
    SimpleLine(){
        oscAddr = "/SimpleLine";
    }
    
    void setCLA(glm::vec3 center, float length, float angle){
        startPoint = center;
        endPoint.x = length * sin(ofDegToRad(angle));
        endPoint.y = length * cos(ofDegToRad(angle));
        endPoint += startPoint;
        
        update();
    }

    
    void setSpEp(glm::vec3 sP, glm::vec3 eP){ startPoint = sP; endPoint = eP; update();};
    
    void setCSrErA(glm::vec3 center, float startRadius, float endRadius, float angle){
        
        startPoint.x = startRadius * sin(ofDegToRad(angle));
        startPoint.y = startRadius * cos(ofDegToRad(angle));
        startPoint += center;
        
        endPoint.x = endRadius * sin(ofDegToRad(angle));
        endPoint.y = endRadius * cos(ofDegToRad(angle));
        endPoint += center;
        
        update();
    }
    
    
    void setStartPoint(glm::vec3 strtPnt){ startPoint = strtPnt; };
    void setEndPoint(glm::vec3 endPnt){ endPoint = endPnt; };
    
    void setRadius(float startR, float endR){
        length = endR-startR;
        endPoint.x = length * sin(ofDegToRad(angle));
        endPoint.y = length * cos(ofDegToRad(angle));
        endPoint += startPoint;
    }
    
    void setRotation(float a){
        angle = a;
        endPoint.x = length * sin(ofDegToRad(angle));
        endPoint.y = length * cos(ofDegToRad(angle));
        endPoint += startPoint;
    }
    
    
    void update(){
        if(poly.size()<2){
            poly.clear();
            poly.addVertex(startPoint);
            poly.addVertex(endPoint);
        }else{
            poly.getVertices()[0] = startPoint;
            poly.getVertices()[1] = endPoint;
        }
        
        updated();
    }
    
    
private:
    glm::vec3 startPoint; // also center
    glm::vec3 endPoint;
    float startRadius = 0;
    float endRadius = 1;
    float length = 0;
    float angle = 0;
};


class SimpleCircle : public PolyGrafic{
public:
    
    SimpleCircle(){
        oscAddr = "/SimpleCircle";
    }
    
    void setResolution( int res)        { resolution = res; };
    void setCenter(glm::vec3 cntrPnt)     { centerPoint = cntrPnt; };
    void setRadius(float r)             { radius = r; };
    
    void setCR(glm::vec3 cntrPnt, float r) {
        centerPoint = cntrPnt;
        radius = r;
        update();
    }
    
    void setCRS(glm::vec3 centerPoint, float radius, float segment) {
        this->centerPoint = centerPoint;
        this->radius = radius;
        this->segment = segment;
        update();
    }
    
    void update(){
        poly.clear();
        int rotation = 270;
        
        poly.arc(centerPoint, radius, radius, rotation-segment/2, 360+rotation+segment/2, true, resolution);
        
        updated();
    };
    
    
private:
    glm::vec3 centerPoint = glm::vec3(0.5,0.5,0.); // also center
    float radius = 0.5;
    int resolution = 90;
    float segment = 360;
};




class SimpleTriangle : public PolyGrafic{
public:
    
    SimpleTriangle(){
        oscAddr = "/SimpleTriangle";
    }
    
    void update(){
        poly.clear();
        poly.addVertex(0.5, 0.);
        poly.addVertex(1., 1.);
        poly.addVertex(0., 1.);
        poly.close();
        
        updated();
    };
    
    
private:
};




class SimpleGate : public PolyGrafic{
public:
    
    SimpleGate(){
        oscAddr = "/SimpleGate";
    }
    
    
    void setCWH(glm::vec3 c, float w, float h) {
        center = c;
        width = w;
        height = h;
        update();
    }
    
    void update(){
        poly.clear();
        
        float w2 = width/2;
        float h2 = height/2;
        
        poly.addVertex(center.x-w2, center.y+h2);
        poly.addVertex(center.x-w2, center.y-h2);
        poly.addVertex(center.x+w2, center.y-h2);
        poly.addVertex(center.x+w2, center.y+h2);
        
        updated();
    };
    
    
private:
    glm::vec3 center = glm::vec3(0.5,0.5,0.); // also center
    float width=1;
    float height = 1;
};










