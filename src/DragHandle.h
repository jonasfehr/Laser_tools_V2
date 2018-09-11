//
//  DragHandle.h
//  Laser_tools_V2
//
//  Created by Jonas Fehr on 27/04/2018.
//  Original from https://github.com/sebleedelisle/ofxLaser -> DragHandle
//

#ifndef DragHandle_h
#define DragHandle_h

#import "VisualPresets.h"

class DragHandle{
    public :
    glm::vec3 position;

    glm::vec3 clickOffset, startPos;
    bool isDragging = false;
    
    float radius = 5;
    float radiusSqr = radius * radius;

    
    DragHandle() {
        set(0,0);
    };
    
    void set(float xpos, float ypos, float r = 5) {
        
        position.x = xpos;
        position.y = ypos;
        radius = r;
        radiusSqr = radius * radius;
    };
    
    void set(glm::vec3 pos) {
        position = pos;
    };
    
    void drawHandle(float scale, bool isOver = false) {
        
        ofPushStyle();{
            if(isOver) ofSetColor(COLOR_HOVER);
            if(isDragging) ofSetColor(COLOR_ACTIVE);
            
            if(isOver || isDragging){
                ofFill();
                ofDrawCircle(position,radius/scale);
            }
            ofNoFill();
            ofSetLineWidth(1./scale);
            ofSetColor(COLOR_BORDER);
            ofDrawCircle(position,radius/scale);
        }
        ofPopStyle();
    };
    
    void startDrag(glm::vec3 clickPos) {
        glm::vec3 screenPos = position;
        clickOffset = clickPos - screenPos;
        isDragging = true;
        startPos = screenPos;
    };
    
    bool updateDrag(glm::vec3 pos) {
        if(isDragging) {
            position.x = startPos.x + (((pos.x - clickOffset.x) - startPos.x) * (ofGetKeyPressed(OF_KEY_SHIFT)? 0.2 : 1));
            position.y = startPos.y + (((pos.y - clickOffset.y) - startPos.y) * (ofGetKeyPressed(OF_KEY_SHIFT)? 0.2 : 1));
            return true;
        } else {
            return false;
        }
    }
    
    bool stopDrag(){
        if(isDragging) {
            isDragging = false;
            return true;
        } else {
            return false;
        }
    };
    
    bool hitTest(glm::vec3 hitpoint, float scale) {
        float dist = glm::distance(position, hitpoint);
        return (dist < radius/scale);
    }
};


#endif /* DragHandle_h */
