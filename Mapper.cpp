//
//  Mapper.cpp
//
//  Created by Jonas Fehr on 24/04/18.
//
//

#include "Mapper.h"
#include "VisualPresets.h"

//--------------------------------------------------------------
Mapper::Mapper() {
    identifier = "mapper";
    params.bMouseEnabled = false;
    params.bKeyboardShortcuts = false;
    params.bShow = true;
    params.bAutoSave = true;
    
    parameters.setName(identifier);
    parameters.add(params.bMouseEnabled.set("MouseEnabled", false));
    parameters.add(params.bKeyboardShortcuts.set("KeyboardShortcuts", false));
    parameters.add(params.bShow.set("Show", true));
    parameters.add(params.bAutoSave.set("AutoSave", true));
    
    
    srcPoints[0] = dstPoints[0] = dstPointsOld[0] = ofPoint(0,0);
    srcPoints[1] = dstPoints[1] = dstPointsOld[1] = ofPoint(1,0);
    srcPoints[2] = dstPoints[2] = dstPointsOld[2] = ofPoint(1,1);
    srcPoints[3] = dstPoints[3] = dstPointsOld[3] = ofPoint(0,1);
    
    
    position = ofVec2f(0,0);
    size = ofVec2f(ofGetWidth(), ofGetHeight());
    
    matrix = getMatrix(&srcPoints[0], &dstPoints[0]);
    
    anchorSize = 10;
    anchorSizeHalf = anchorSize * 0.5;
    selectedCornerIndex = -1;
    highlightCornerIndex = -1;
    
    enableMouseControls();
    enableKeyboardShortcuts();
}
Mapper::~Mapper() {
    disableMouseControls();
    disableKeyboardShortcuts();
}

//----------------------------------------------------- setup.


void Mapper::reset() {
    dstPoints[0].set(srcPoints[0]);
    dstPoints[1].set(srcPoints[1]);
    dstPoints[2].set(srcPoints[2]);
    dstPoints[3].set(srcPoints[3]);
}

//--------------------------------------------------------------
string Mapper::getString() {
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
void Mapper::updateMatrix(){
    for(int i = 0; i<4; i++){ // avoid calculating matrix each timeM
        if(dstPoints[i]!=dstPointsOld[i]){
            matrix = getMatrix(&srcPoints[0], &dstPoints[0]);
            dstPointsOld[i] = dstPoints[i];
            i = 4; // matrix is recalculated;
        }
    }
}


//--------------------------------------------------------------
void Mapper::update(const vector<ofPolyline> &origPolys, vector<ofPolyline> &mappedPolys) {
    updateMatrix();
    
    mappedPolys = origPolys;
    for(int i=0; i<mappedPolys.size(); i++) {
        for(int p = 0; p < mappedPolys[i].getVertices().size(); p++){
            mappedPolys[i].getVertices()[p] = matrix.preMult((ofVec3f)mappedPolys[i].getVertices()[p]);
        }
    }
    
}

void Mapper::remap(ofPolyline &toBeMappedPoly){
    updateMatrix();
    
    for(int p = 0; p < toBeMappedPoly.getVertices().size(); p++){
        toBeMappedPoly.getVertices()[p] = matrix.preMult((ofVec3f)toBeMappedPoly.getVertices()[p]);
    }
}

ofPolyline Mapper::map(ofPolyline &originalPoly){
    updateMatrix();
    
    ofPolyline mappedPoly = originalPoly;
    for(int p = 0; p < originalPoly.getVertices().size(); p++){
        mappedPoly.getVertices()[p] = matrix.preMult((ofVec3f)mappedPoly.getVertices()[p]);
    }
    return mappedPoly;
}

ofxIlda::Poly Mapper::map(ofxIlda::Poly &originalPoly){
    updateMatrix();
    
    ofxIlda::Poly mappedPoly = originalPoly;
    for(int p = 0; p < originalPoly.getVertices().size(); p++){
        mappedPoly.getVertices()[p] = matrix.preMult((ofVec3f)mappedPoly.getVertices()[p]);
    }
    return mappedPoly;
}

vector <ofPolyline> Mapper::map(vector<ofPolyline> &originalPolylines) {
    updateMatrix();
    
    vector <ofPolyline> mappedPolys = originalPolylines;
    for(int i=0; i<mappedPolys.size(); i++) {
        for(int p = 0; p < mappedPolys[i].getVertices().size(); p++){
            mappedPolys[i].getVertices()[p] = matrix.preMult((ofVec3f)mappedPolys[i].getVertices()[p]);
        }
    }
    
    return mappedPolys;
    
}
vector<ofxIlda::Poly> Mapper::map(vector<ofxIlda::Poly> &originalPolylines) {
    updateMatrix();
    
    vector <ofxIlda::Poly> mappedPolys = originalPolylines;
    for(int i=0; i<mappedPolys.size(); i++) {
        for(int p = 0; p < mappedPolys[i].getVertices().size(); p++){
            mappedPolys[i].getVertices()[p] = matrix.preMult((ofVec3f)mappedPolys[i].getVertices()[p]);
        }
    }
    
    return mappedPolys;
    
}


//--------------------------------------------------------------
// functions to controll the mapping are also copied from ofxQuadWrap
void Mapper::enableMouseControls() {
    if(params.bMouseEnabled == true) {
        return;
    }
    params.bMouseEnabled = true;
    ofAddListener(ofEvents().mouseMoved, this, &Mapper::onMouseMoved);
    ofAddListener(ofEvents().mousePressed, this, &Mapper::onMousePressed);
    ofAddListener(ofEvents().mouseDragged, this, &Mapper::onMouseDragged);
    ofAddListener(ofEvents().mouseReleased, this, &Mapper::onMouseReleased);
    ofLogNotice() << "Mapper "<< params.name <<" - Mouse control enabled";
    
}

void Mapper::disableMouseControls() {
    if(params.bMouseEnabled == false) {
        return;
    }
    params.bMouseEnabled = false;
    //    try {
    ofRemoveListener(ofEvents().mouseMoved, this, &Mapper::onMouseMoved);
    ofRemoveListener(ofEvents().mousePressed, this, &Mapper::onMousePressed);
    ofRemoveListener(ofEvents().mouseDragged, this, &Mapper::onMouseDragged);
    ofRemoveListener(ofEvents().mouseReleased, this, &Mapper::onMouseReleased);
    ofLogNotice() << "Mapper "<< params.name <<" - Mouse control disabled";
    
    //    }
    //    catch(Poco::SystemException) {
    //        return;
    //    }
}

void Mapper::enableKeyboardShortcuts() {
    if(params.bKeyboardShortcuts == true) {
        return;
    }
    params.bKeyboardShortcuts = true;
    ofAddListener(ofEvents().keyPressed, this, &Mapper::keyPressed);
    ofAddListener(ofEvents().keyReleased, this, &Mapper::keyReleased);
    
    ofLogNotice() << "Mapper "<< params.name <<" - Key control enabled";
    
}

void Mapper::disableKeyboardShortcuts() {
    if(params.bKeyboardShortcuts == false) {
        return;
    }
    params.bKeyboardShortcuts = false;
    //    try {
    ofRemoveListener(ofEvents().keyPressed, this, &Mapper::keyPressed);
    ofRemoveListener(ofEvents().keyReleased, this, &Mapper::keyReleased);
    
    ofLogNotice() << "Mapper "<< params.name <<" - Key control disabled";
    
    //    }
    //    catch(Poco::SystemException) {
    //        return;
    //    }
}

// DRAW funktions
//----------------------------------------------------- show / hide.
void Mapper::draw(float x, float y, float w, float h) {
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
    
    ofFill();
    drawQuadOutline();
    drawCorners();
    drawHighlightedCorner();
    drawSelectedCorner();
}

void Mapper::drawQuadOutline() {
    if(params.bShow == false) {
        return;
    }
    ofSetColor(COLOR_BORDER);
    
    for(int i=0; i<4; i++) {
        int j = (i+1) % 4;
        ofDrawLine(dstPoints[i].x * size.x + position.x,
                   dstPoints[i].y * size.y + position.y,
                   dstPoints[j].x * size.x + position.x,
                   dstPoints[j].y * size.y + position.y);
    }
}

void Mapper::drawCorners() {
    if(params.bShow == false) {
        return;
    }
    ofSetColor(COLOR_BORDER);
    ofNoFill();
    ofSetLineWidth(1);
    
    for(int i=0; i<4; i++) {
        ofPoint point;
        point.x = dstPoints[i].x * size.x;
        point.y = dstPoints[i].y * size.y;
        
        drawCornerAt(point);
    }
}

void Mapper::drawHighlightedCorner() {
    if(params.bShow == false) {
        return;
    }
    if(highlightCornerIndex < 0 || highlightCornerIndex > 3) {
        return;
    }
    ofSetColor(COLOR_HOVER);
    ofFill();
    
    ofPoint point;
    point.x = dstPoints[highlightCornerIndex].x * size.x;
    point.y = dstPoints[highlightCornerIndex].y * size.y;
    drawCornerAt(point);
}

void Mapper::drawSelectedCorner() {
    if(params.bShow == false) {
        return;
    }
    if(selectedCornerIndex < 0 || selectedCornerIndex > 3) {
        return;
    }
    ofSetColor(COLOR_ACTIVE);
    ofFill();

    ofPoint point;
    point.x = dstPoints[highlightCornerIndex].x * size.x;
    point.y = dstPoints[highlightCornerIndex].y * size.y;
    drawCornerAt(point);
}

void Mapper::drawCornerAt(const ofPoint & point) {
    ofDrawCircle(point.x + position.x,
                 point.y + position.y,
                 anchorSizeHalf);
//    ofDrawRectangle(point.x + position.x - anchorSizeHalf,
//                    point.y + position.y - anchorSizeHalf,
//                    anchorSize, anchorSize);
}


//---------------------------------


void Mapper::onMouseMoved(ofMouseEventArgs & mouseArgs) {
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
void Mapper::onMousePressed(ofMouseEventArgs & mouseArgs) {
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

void Mapper::onMouseDragged(ofMouseEventArgs & mouseArgs){
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
void Mapper::onMouseReleased(ofMouseEventArgs & mouseArgs) {
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
void Mapper::keyPressed(ofKeyEventArgs & keyArgs){
    if(params.bShow == false) {
        return;
    }
    
    if(selectedCornerIndex < 0 || selectedCornerIndex > 3) {
        return;
    }
    
    if(keyArgs.key == OF_KEY_SHIFT ){
        nudgeAmount = 0.001;
    }
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

void Mapper::keyReleased(ofKeyEventArgs & keyArgs){
    if(keyArgs.key == OF_KEY_SHIFT ){
        nudgeAmount = 0.01;
    }
}
//----------------------------------------------------- save / load.
//void Mapper::save(const string& path) {
//    ofSavePrettyJson(path, getJson());
//}
void Mapper::setupFromJson(ofJson & json){
    
    identifier = json["identifier"].get<std::string>();
    
    int i = 0;
    for( auto & src : json["src"]){
        srcPoints[i].x = src["x"];
        srcPoints[i].y = src["y"];
        i++;
    }
    i = 0;
    for( auto & dst : json["dst"]){
        dstPoints[i].x = dst["x"];
        dstPoints[i].y = dst["y"];
        i++;
    }
    
    auto & jp = json["parameters"];
    ofDeserialize(jp, parameters);
}

void Mapper::addToJson(ofJson & json){
    auto & j = json[identifier];
    j["identifier"] = identifier;

    for(int i=0; i<4; i++) {
        j["src"]["point_"+ofToString(i)]["x"] = srcPoints[i].x;
        j["src"]["point_"+ofToString(i)]["y"] = srcPoints[i].y;
    }
    for(int i=0; i<4; i++) {
        j["dst"]["point_"+ofToString(i)]["x"] = dstPoints[i].x;
        j["dst"]["point_"+ofToString(i)]["y"] = dstPoints[i].y;
    }
    
    auto & jp = j["parameters"];
    ofSerialize(jp, parameters);
}



// --- functions borrowed from ofxHomography ---
// https://github.com/paulobarcelos/ofxHomography
/*
 * Homography Functions adapted from:
 * http://www.openframeworks.cc/forum/viewtopic.php?p=22611
 * Author: arturo castro
 */
void Mapper::gaussian_elimination(float *input, int n){
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
ofMatrix4x4 Mapper::getMatrix(const ofPoint* src, const ofPoint* dst){
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
    float aux_H[]={ P[0][8],P[3][8],0,P[6][8],    // h11  h21 0 h31
        P[1][8],P[4][8],0,P[7][8],    // h12  h22 0 h32
        0      ,      0,0,0,        // 0    0   0 0
        P[2][8],P[5][8],0,1};        // h13  h23 0 h33
    
    for(int i=0;i<16;i++) homography[i] = aux_H[i];
    
    return ofMatrix4x4(homography);
}




ofPoint Mapper::toScreenCoordinates(ofPoint point, ofMatrix4x4 homography){
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
