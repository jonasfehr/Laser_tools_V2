//
//  ofxIldaMapper.h
//  ofxILDA demo
//
//  Created by Jonas Fehr on 29/04/2016.
//  inspired and based on ofxQuadWarp created by lukasz karluk on 19/06/11.
//  https://github.com/julapy/ofxQuadWarp
//  the reimplementation was needed, as the ILDA range is from 0. to 1. instead of the pixel on the canvas.
//
//  instead of using ofCv homography, used the version provided in ofxHomography by Paulo Barcelos
//  https://github.com/paulobarcelos/ofxHomography


#pragma once

#include "ofMain.h"
#include "ofxIldaPoly.h"
#include "ofxXmlSettings.h"

namespace ofxIlda {
    class Mapper {
    public:
        struct {
            bool bMouseEnabled;
            bool bKeyboardShortcuts;
            bool bShow;
            bool bAutoSave;
        } params;
        
        
        //--------------------------------------------------------------
        Mapper() {
            memset(&params, 0, sizeof(params));
            params.bMouseEnabled = false;
            params.bKeyboardShortcuts = false;
            params.bShow = false;
            params.bAutoSave = false;
            
            
            srcPoints[0] = dstPoints[0] = dstPointsOld[0] = ofPoint(0,0);
            srcPoints[1] = dstPoints[1] = dstPointsOld[1] = ofPoint(1,0);
            srcPoints[2] = dstPoints[2] = dstPointsOld[2] = ofPoint(1,1);
            srcPoints[3] = dstPoints[3] = dstPointsOld[3] = ofPoint(0,1);
            
            if(params.bAutoSave){
                load("settings/mapping.xml");
            }
            
            position = ofVec2f(0,0);
            size = ofVec2f(ofGetWidth(), ofGetHeight());
            
            matrix = getMatrix(&srcPoints[0], &dstPoints[0]);
            
            anchorSize = 10;
            anchorSizeHalf = anchorSize * 0.5;
            selectedCornerIndex = -1;
            highlightCornerIndex = -1;
            
            
            setup();
            
        }
        ~Mapper() {
            disableMouseControls();
            disableKeyboardShortcuts();
            
            if(params.bAutoSave){
                save("settings/mapping.xml");
            }
        }
        
        //----------------------------------------------------- setup.
        void setup() {
            enableMouseControls();
            enableKeyboardShortcuts();
        }
        
        void reset() {
            dstPoints[0].set(srcPoints[0]);
            dstPoints[1].set(srcPoints[1]);
            dstPoints[2].set(srcPoints[2]);
            dstPoints[3].set(srcPoints[3]);
        }
        
        
        //--------------------------------------------------------------
        string getString() {
            stringstream s;
            s << "Mapper.params:" << endl;
            s << "bMouseEnabled : " << params.bMouseEnabled << endl;
            s << "bKeyboardShortcuts : " << params.bKeyboardShortcuts << endl;
            s << "bShow : " << params.bShow << endl;
            s << endl;
            //            s << "dstPoint 1 : " << dstPoints[0] << endl;
            //            s << "dstPoint 2 : " << dstPoints[1] << endl;
            //            s << "dstPoint 3 : " << dstPoints[2] << endl;
            //            s << "dstPoint 4 : " << dstPoints[3] << endl;
            s << "selected Corner: " << selectedCornerIndex << endl;
            
            return s.str();
        }
        
        //--------------------------------------------------------------
        void update(const vector<Poly> &origPolys, vector<Poly> &mappedPolys) {
            for(int i = 0; i<4; i++){ // avoid calculating matrix each timeM
                if(dstPoints[i]!=dstPointsOld[i]){
                    matrix = getMatrix(&srcPoints[0], &dstPoints[0]);
                    dstPointsOld[i] = dstPoints[i];
                    i = 4; // matrix is recalculated;
                }
            }
            
            mappedPolys = origPolys;
            for(int i=0; i<mappedPolys.size(); i++) {
                for(int p = 0; p < mappedPolys[i].getVertices().size(); p++){
                    mappedPolys[i].getVertices()[p] = matrix.preMult(mappedPolys[i].getVertices()[p]);
                }
            }
            
        }
        //--------------------------------------------------------------
        // functions to controll the mapping are also copied from ofxQuadWrap
        void enableMouseControls() {
            if(params.bMouseEnabled == true) {
                return;
            }
            params.bMouseEnabled = true;
            ofAddListener(ofEvents().mouseMoved, this, &Mapper::onMouseMoved);
            ofAddListener(ofEvents().mousePressed, this, &Mapper::onMousePressed);
            ofAddListener(ofEvents().mouseDragged, this, &Mapper::onMouseDragged);
            ofAddListener(ofEvents().mouseReleased, this, &Mapper::onMouseReleased);
            ofLogNotice() << "ofxIldaMapper - Mouse control enabled";
            
        }
        
        void disableMouseControls() {
            if(params.bMouseEnabled == false) {
                return;
            }
            params.bMouseEnabled = false;
            try {
                ofRemoveListener(ofEvents().mouseMoved, this, &Mapper::onMouseMoved);
                ofRemoveListener(ofEvents().mousePressed, this, &Mapper::onMousePressed);
                ofRemoveListener(ofEvents().mouseDragged, this, &Mapper::onMouseDragged);
                ofRemoveListener(ofEvents().mouseReleased, this, &Mapper::onMouseReleased);
                ofLogNotice() << "ofxIldaMapper - Mouse control disabled";
                
            }
            catch(Poco::SystemException) {
                return;
            }
        }
        
        void enableKeyboardShortcuts() {
            if(params.bKeyboardShortcuts == true) {
                return;
            }
            params.bKeyboardShortcuts = true;
            ofAddListener(ofEvents().keyPressed, this, &Mapper::keyPressed);
            ofLogNotice() << "ofxIldaMapper - Key control enabled";
            
        }
        
        void disableKeyboardShortcuts() {
            if(params.bKeyboardShortcuts == false) {
                return;
            }
            params.bKeyboardShortcuts = false;
            try {
                ofRemoveListener(ofEvents().keyPressed, this, &Mapper::keyPressed);
                ofLogNotice() << "ofxIldaMapper - Key control disabled";
                
            }
            catch(Poco::SystemException) {
                return;
            }
        }
        
        // DRAW funktions
        //----------------------------------------------------- show / hide.
        void draw(float x = 0, float y = 0, float w = ofGetWidth(), float h = ofGetHeight()) {
            if(params.bShow == false) {
                return;
            }
            
            mox = x;
            moy = y;
            mow = w;
            moh = h;
            
            position.x = x;
            position.y = y;
            size.x = w;
            size.y = h;
            
            drawQuadOutline();
            drawCorners();
            drawHighlightedCorner();
            drawSelectedCorner();
        }
        
        void drawQuadOutline() {
            if(params.bShow == false) {
                return;
            }
            ofSetColor(ofColor::magenta);
            
            for(int i=0; i<4; i++) {
                int j = (i+1) % 4;
                ofDrawLine(dstPoints[i].x * size.x + position.x,
                           dstPoints[i].y * size.y + position.y,
                           dstPoints[j].x * size.x + position.x,
                           dstPoints[j].y * size.y + position.y);
            }
        }
        
        void drawCorners() {
            if(params.bShow == false) {
                return;
            }
            ofSetColor(ofColor::yellow);
            
            for(int i=0; i<4; i++) {
                ofPoint point;
                point.x = dstPoints[i].x * size.x;
                point.y = dstPoints[i].y * size.y;
                
                drawCornerAt(point);
            }
        }
        
        void drawHighlightedCorner() {
            if(params.bShow == false) {
                return;
            }
            if(highlightCornerIndex < 0 || highlightCornerIndex > 3) {
                return;
            }
            ofSetColor(ofColor::magenta);
            
            ofPoint point;
            point.x = dstPoints[highlightCornerIndex].x * size.x;
            point.y = dstPoints[highlightCornerIndex].y * size.y;
            drawCornerAt(point);
        }
        
        void drawSelectedCorner() {
            if(params.bShow == false) {
                return;
            }
            if(selectedCornerIndex < 0 || selectedCornerIndex > 3) {
                return;
            }
            ofSetColor(ofColor::red);
            
            ofPoint point;
            point.x = dstPoints[highlightCornerIndex].x * size.x;
            point.y = dstPoints[highlightCornerIndex].y * size.y;
            drawCornerAt(point);
        }
        
        void drawCornerAt(const ofPoint & point) {
            ofDrawRectangle(point.x + position.x - anchorSizeHalf,
                            point.y + position.y - anchorSizeHalf,
                            anchorSize, anchorSize);
        }
        
        
        //---------------------------------
        
    protected:
        ofxXmlSettings settingsXML;
        
        
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
        
        void onMouseMoved(ofMouseEventArgs & mouseArgs) {
            if(params.bShow == false) {
                return;
            }
            
            ofPoint mousePoint(mouseArgs.x, mouseArgs.y);
            mousePoint -= position;
            for(int i=0; i<4; i++) {
                ofPoint dstPoint;
                dstPoint.x = dstPoints[i].x * size.x;
                dstPoint.y = dstPoints[i].y * size.y;
                if(mousePoint.distance(dstPoint) <= anchorSizeHalf) {
                    highlightCornerIndex = i;
                    return;
                }
            }
            highlightCornerIndex = -1;
        }
        void onMousePressed(ofMouseEventArgs & mouseArgs) {
            if(params.bShow == false) {
                return;
            }
            
            ofPoint mousePoint(mouseArgs.x, mouseArgs.y);
            mousePoint -= position;
            for(int i=0; i<4; i++) {
                ofPoint dstPoint;
                dstPoint.x = dstPoints[i].x * size.x;
                dstPoint.y = dstPoints[i].y * size.y;
                if(mousePoint.distance(dstPoint) <= anchorSizeHalf) {
                    dstPoint.set(mousePoint);
                    selectedCornerIndex = i;
                    return;
                }
            }
            selectedCornerIndex = -1;
        }
        
        void onMouseDragged(ofMouseEventArgs & mouseArgs){
            if(params.bShow == false) {
                return;
            }
            if(selectedCornerIndex < 0 || selectedCornerIndex > 3) {
                return;
            }
            
            ofPoint mousePoint(mouseArgs.x, mouseArgs.y);
            mousePoint -= position;
            dstPoints[selectedCornerIndex].set(ofPoint(mousePoint.x/size.x, mousePoint.y/size.y));
        }
        void onMouseReleased(ofMouseEventArgs & mouseArgs) {
            if(params.bShow == false) {
                return;
            }
            if(selectedCornerIndex < 0 || selectedCornerIndex > 3) {
                return;
            }
            
            ofPoint mousePoint(mouseArgs.x, mouseArgs.y);
            mousePoint -= position;
            dstPoints[selectedCornerIndex].set(ofPoint(mousePoint.x/size.x, mousePoint.y/size.y));
        }
        void keyPressed(ofKeyEventArgs & keyArgs){
            if(params.bShow == false) {
                return;
            }
            
            switch (keyArgs.key) {
                case '1':
                    selectedCornerIndex = 0;
                    break;
                case '2':
                    selectedCornerIndex = 1;
                    break;
                case '3':
                    selectedCornerIndex = 2;
                    break;
                case '4':
                    selectedCornerIndex = 3;
                    break;
                case 'r':
                    reset();
                    break;
                default:
                    break;
            }
            
            if(selectedCornerIndex < 0 || selectedCornerIndex > 3) {
                return;
            }
            
            float nudgeAmount = 0.01;
            ofPoint & selectedPoint = dstPoints[selectedCornerIndex];
            
            switch (keyArgs.key) {
                case OF_KEY_LEFT:
                    selectedPoint.x -= nudgeAmount;
                    break;
                case OF_KEY_RIGHT:
                    selectedPoint.x += nudgeAmount;
                    break;
                case OF_KEY_UP:
                    selectedPoint.y -= nudgeAmount;
                    break;
                case OF_KEY_DOWN:
                    selectedPoint.y += nudgeAmount;
                    break;
                default:
                    break;
            }
        }
        //----------------------------------------------------- save / load.
        void save(const string& path) {
            ofXml xml;
            xml.addChild("mapping");
            xml.setTo("mapping");
            xml.addChild("src");
            xml.setTo("src");
            for(int i=0; i<4; i++) {
                xml.addChild("point");
                xml.setToChild(i);
                xml.setAttribute("x", ofToString(srcPoints[i].x));
                xml.setAttribute("y", ofToString(srcPoints[i].y));
                xml.setToParent();
            }
            xml.setToParent();
            xml.addChild("dst");
            xml.setTo("dst");
            for(int i=0; i<4; i++) {
                xml.addChild("point");
                xml.setToChild(i);
                xml.setAttribute("x", ofToString(dstPoints[i].x));
                xml.setAttribute("y", ofToString(dstPoints[i].y));
                xml.setToParent();
            }
            xml.setToParent();
            
            xml.setToParent();
            xml.save(path);
        }
        
        void load(const string& path) {
            ofXml xml;
            bool bOk = xml.load(path);
            if(bOk == false) {
                return;
            }
            
            bOk = xml.setTo("mapping");
            if(bOk == false) {
                return;
            }
            
            bOk = xml.setTo("src");
            if(bOk == false) {
                return;
            }
            
            for(int i=0; i<xml.getNumChildren(); i++) {
                bOk = xml.setToChild(i);
                if(bOk == false) {
                    continue;
                }
                srcPoints[i].x = ofToFloat(xml.getAttribute("x"));
                srcPoints[i].y = ofToFloat(xml.getAttribute("y"));
                xml.setToParent();
            }
            xml.setToParent();
            
            bOk = xml.setTo("dst");
            if(bOk == false) {
                return;
            }
            
            for(int i=0; i<xml.getNumChildren(); i++) {
                bOk = xml.setToChild(i);
                if(bOk == false) {
                    continue;
                }
                dstPoints[i].x = ofToFloat(xml.getAttribute("x"));
                dstPoints[i].y = ofToFloat(xml.getAttribute("y"));
                xml.setToParent();
            }
            xml.setToParent();
            xml.setToParent();
        }
        
        // --- functions borrowed from ofxHomography ---
        // https://github.com/paulobarcelos/ofxHomography
        /*
         * Homography Functions adapted from:
         * http://www.openframeworks.cc/forum/viewtopic.php?p=22611
         * Author: arturo castro
         */
        static void gaussian_elimination(float *input, int n){
            // ported to c from pseudocode in
            // http://en.wikipedia.org/wiki/Gaussian_elimination
            
            float * A = input;
            int i = 0;
            int j = 0;
            int m = n-1;
            while (i < m && j < n){
                // Find pivot in column j, starting in row i:
                int maxi = i;
                for(int k = i+1; k<m; k++){
                    if(fabs(A[k*n+j]) > fabs(A[maxi*n+j])){
                        maxi = k;
                    }
                }
                if (A[maxi*n+j] != 0){
                    //swap rows i and maxi, but do not change the value of i
                    if(i!=maxi)
                        for(int k=0;k<n;k++){
                            float aux = A[i*n+k];
                            A[i*n+k]=A[maxi*n+k];
                            A[maxi*n+k]=aux;
                        }
                    //Now A[i,j] will contain the old value of A[maxi,j].
                    //divide each entry in row i by A[i,j]
                    float A_ij=A[i*n+j];
                    for(int k=0;k<n;k++){
                        A[i*n+k]/=A_ij;
                    }
                    //Now A[i,j] will have the value 1.
                    for(int u = i+1; u< m; u++){
                        //subtract A[u,j] * row i from row u
                        float A_uj = A[u*n+j];
                        for(int k=0;k<n;k++){
                            A[u*n+k]-=A_uj*A[i*n+k];
                        }
                        //Now A[u,j] will be 0, since A[u,j] - A[i,j] * A[u,j] = A[u,j] - 1 * A[u,j] = 0.
                    }
                    
                    i++;
                }
                j++;
            }
            
            //back substitution
            for(int i=m-2;i>=0;i--){
                for(int j=i+1;j<n-1;j++){
                    A[i*n+m]-=A[i*n+j]*A[j*n+m];
                    //A[i*n+j]=0;
                }
            }
        }
        
        // slightly rewritten version of the function findHomography from ofxHomography
        ofMatrix4x4 getMatrix(const ofPoint* src, const ofPoint* dst){
            float homography[16];
            // create the equation system to be solved
            // src and dst must implement [] operator for point access
            //
            // from: Multiple View Geometry in Computer Vision 2ed
            //       Hartley R. and Zisserman A.
            //
            // x' = xH
            // where H is the homography: a 3 by 3 matrix
            // that transformed to inhomogeneous coordinates for each point
            // gives the following equations for each point:
            //
            // x' * (h31*x + h32*y + h33) = h11*x + h12*y + h13
            // y' * (h31*x + h32*y + h33) = h21*x + h22*y + h23
            //
            // as the homography is scale independent we can let h33 be 1 (indeed any of the terms)
            // so for 4 points we have 8 equations for 8 terms to solve: h11 - h32
            // after ordering the terms it gives the following matrix
            // that can be solved with gaussian elimination:
            
            float P[8][9]={
                {-src[0].x, -src[0].y, -1,   0,   0,  0, src[0].x*dst[0].x, src[0].y*dst[0].x, -dst[0].x }, // h11
                {  0,   0,  0, -src[0].x, -src[0].y, -1, src[0].x*dst[0].y, src[0].y*dst[0].y, -dst[0].y }, // h12
                
                {-src[1].x, -src[1].y, -1,   0,   0,  0, src[1].x*dst[1].x, src[1].y*dst[1].x, -dst[1].x }, // h13
                {  0,   0,  0, -src[1].x, -src[1].y, -1, src[1].x*dst[1].y, src[1].y*dst[1].y, -dst[1].y }, // h21
                
                {-src[2].x, -src[2].y, -1,   0,   0,  0, src[2].x*dst[2].x, src[2].y*dst[2].x, -dst[2].x }, // h22
                {  0,   0,  0, -src[2].x, -src[2].y, -1, src[2].x*dst[2].y, src[2].y*dst[2].y, -dst[2].y }, // h23
                
                {-src[3].x, -src[3].y, -1,   0,   0,  0, src[3].x*dst[3].x, src[3].y*dst[3].x, -dst[3].x }, // h31
                {  0,   0,  0, -src[3].x, -src[3].y, -1, src[3].x*dst[3].y, src[3].y*dst[3].y, -dst[3].y }, // h32
            };
            
            gaussian_elimination(&P[0][0],9);
            
            // gaussian elimination gives the results of the equation system
            // in the last column of the original matrix.
            // opengl needs the transposed 4x4 matrix:
            float aux_H[]={ P[0][8],P[3][8],0,P[6][8],	// h11  h21 0 h31
                P[1][8],P[4][8],0,P[7][8],	// h12  h22 0 h32
                0      ,      0,0,0,		// 0    0   0 0
                P[2][8],P[5][8],0,1};		// h13  h23 0 h33
            
            for(int i=0;i<16;i++) homography[i] = aux_H[i];
            
            return ofMatrix4x4(homography);
        }
        
        
        
        
        static ofPoint toScreenCoordinates(ofPoint point, ofMatrix4x4 homography){
            ofVec4f original;
            ofVec4f screen;
            
            original.x = point.x;
            original.y = point.y;
            original.z = point.z;
            original.w = 1.0;
            
            ofMatrix4x4 transposed = ofMatrix4x4::getTransposedOf(homography);
            
            screen = transposed * original;
            
            screen.x = screen.x / screen.w;
            screen.y = screen.y / screen.w;
            screen.z = screen.z / screen.w;
            
            return ofPoint(screen.x,screen.y, screen.z);
        }
        
        
    };
    
}