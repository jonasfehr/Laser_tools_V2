//
//  TextToGrafics.h
//  Laser_tools_V2
//
//  Created by Jonas Fehr on 19/06/2017.
//
//

#ifndef TextToGrafics_h
#define TextToGrafics_h

#include "ofxIldaFrame.h"
#include "ofxIldaRenderTarget.h"

class TextInput{
public:
    vector<ofxIlda::Poly> polys;
    
    ofFloatColor color;
    
    ofxIlda::RenderTarget ildaFbo;
    
    ofTrueTypeFont font;
    
    ofParameterGroup parameters;
    
    string fontPath = "fonts/Zebulon.ttf";
    
    
    
    TextInput() {
        
        ildaFbo.setup(512, 512);
        
        parameters.setName("RenderTarget");
        parameters.add(ildaFbo.parameters);
        
        ofDirectory dir("fonts");
        dir.listDir();
        if(dir.size()>0) fontPath = dir.getPath(0);
        font.load(fontPath, 120);
        
    };
    
    //void setSpOF()
    void setSize(float _size) { font.load(fontPath, _size); };
    
    void setColor(ofColor col)          { color = col; };
    void setColor(ofFloatColor col)     { color = col; };
    
    void setText(string _text) { text = _text; };
    
    
    
    
    
    // Method to update location
    void update() {
        
        //ofPushStyle();
        
        ildaFbo.begin();
        {
            //ofClear(0);
            ofBackground(255);
            
            ofRectangle rect = font.getStringBoundingBox(text, 0,0);
            //            ofRectangle myRect;
            //            myRect.x = 5-rect.width/2;
            //            myRect.y = 5-rect.height;
            //            myRect.width = rect.width+10;
            //            myRect.height = rect.height+10;
            //
            //            ofDrawRectRounded(myRect, 5);
            
            ofSetColor(0);
            glm::vec3 center = glm::vec3(ildaFbo.getWidth()/2, ildaFbo.getHeight()/2, 0.);
            font.drawString(text,center.x-rect.width/2,center.y+rect.height/2);
        }
        ildaFbo.end();
        
        // ofPopStyle();
        
        ofxIlda::Frame frame;
        ildaFbo.update(frame);
        
        polys = frame.getPolys();
    };
private:
    string text;
    
    
};


#endif /* TextToGrafics_h */
