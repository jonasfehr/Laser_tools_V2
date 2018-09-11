//
//  SimpleButton.h
//  Laser_tools_V2
//
//  Created by Jonas Fehr on 26/04/2018.
//

#ifndef SimpleButton_h
#define SimpleButton_h

#include "ofMain.h"
#include "VisualPresets.h"



class SimpleButton : public ofParameter<bool>, public InteractionColors{
    ofImage img;
    bool hasImg;
    
    ofNode origin;
    int size;
    
    void setup(int x, int y, int size){
        origin.setPosition(x,y);
        this->size = size;
    }
    
    void setup(int x, int y, int size, ofImage img){
        this->setup(x,y,size);
        this->img = img;
        hasImg = true;
    }
    
    void draw(){
        if(this) ofSetColor(active);
        else{
//            if(mouseOver) ofSetColor(hover);
//            else ofSetColor(passive);
        }
        ofSetStroke(2);
        ofSet
        if(hasImage) img.draw(rect);
    }
    
};

#endif /* SimpleButton_h */
