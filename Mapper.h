//
//  Mapper.h
//  DarkReflections
//
//  Created by Jonas Fehr on 18/08/16.
//
//

#include "ofMain.h"
#include "ofxIldaFrame.h"
//#include <stdio.h>
#include "JsonUtil.h"

#pragma once

    
class Mapper : public JsonUtil {
        public:
        ofParameterGroup parameters;
            struct {
                ofParameter<bool> bMouseEnabled;
                ofParameter<bool> bKeyboardShortcuts;
                ofParameter<bool> bShow;
                ofParameter<bool> bAutoSave;
                string name;
            } params;
    
    Mapper();
    ~Mapper();
    void reset();
    string getString();
    void updateMatrix();
    
    void update(const vector<ofPolyline> &origPolys, vector<ofPolyline> &mappedPolys);
    ofPolyline map(ofPolyline &originalPoly);
    ofxIlda::Poly map(ofxIlda::Poly &originalPoly);
    vector <ofPolyline> map(vector <ofPolyline> &originalPolylines);
    vector <ofxIlda::Poly> map(vector <ofxIlda::Poly> &originalPolylines);

    void remap(ofPolyline &toBeMappedPoly);
    
    void draw(float x = 0, float y = 0, float w = ofGetWidth(), float h = ofGetHeight());
    
    void drawQuadOutline();
    void drawCorners();
    void drawHighlightedCorner();
    void drawSelectedCorner();
    void drawCornerAt(const ofPoint & point);
    
    void enableMouseControls();
    void disableMouseControls();
    void enableKeyboardShortcuts();
    void disableKeyboardShortcuts();
    
    void addToJson(ofJson & json);
    void setupFromJson(ofJson & json);


    ofPoint getDstPoint(int index){
        return dstPoints[index];
    }
    
    
protected:    
    
    ofPoint srcPoints[4] = {ofPoint(0,0),ofPoint(1,0),ofPoint(1,1),ofPoint(0,1)};
    ofPoint dstPoints[4];
    ofPoint dstPointsOld[4];
    
    ofMatrix4x4 matrix;
    
    float mox, moy, mow, moh; // mouse offset
    
    //interaction
    ofPoint position;
    ofVec2f size;
    
    float anchorSize;
    float anchorSizeHalf;
    int selectedCornerIndex;
    int highlightCornerIndex;
    int lastSelected = 0;
    
    void onMouseMoved(ofMouseEventArgs & mouseArgs);
    void onMousePressed(ofMouseEventArgs & mouseArgs);
    void onMouseDragged(ofMouseEventArgs & mouseArgs);
    void onMouseReleased(ofMouseEventArgs & mouseArgs);
    void keyPressed(ofKeyEventArgs & keyArgs);
    void keyReleased(ofKeyEventArgs & keyArgs);
    float nudgeAmount = 0.01;

    
    void gaussian_elimination(float *input, int n);
    ofMatrix4x4 getMatrix(const ofPoint* src, const ofPoint* dst);
    ofPoint toScreenCoordinates(ofPoint point, ofMatrix4x4 homography);
                    
};



