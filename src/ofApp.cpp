#include "ofApp.h"


//--------------------------------------------------------------
void ofApp::setup(){
    ofSetFrameRate(60);
    ofBackground(0);
    
//    ofJson json = ofLoadJson("settings/laserprojectors.json");
//    for( auto & j : json){
//        laserProjectors[j["identifier"]].setupFromJson(j);
//    }
//
//    laserProjectors["frontStage"].setup("backStage", 0, ofGetWidth()-ofGetHeight(), 0, ofGetHeight(), ofGetHeight());
//
    laserProjectors["backStage"].load("settings/lp_1.json");
    laserProjectors["frontStage"].load("settings/lp_2.json");

//    laserProjectors["backStage"].setup("frontStage", 1, ofGetWidth()-ofGetHeight()*2 - 10, 0, ofGetHeight(), ofGetHeight());


    oscReceiver.setup(OSC_RECEIVE_PORT);
    
    
    // GUI
    showGui = true;
    gui.setup();

    for(auto & lp : laserProjectors){
        gui.add(lp.second.parameters);
    }

//    // Syphon IN
    syphonIn.setup();
    syphonIn.find("toLaser_displacement", "MadMapper");

    syphonOut.setup("laser", laserProjectors["frontStage"].previewW, laserProjectors["frontStage"].previewH);

}

//--------------------------------------------------------------
void ofApp::update(){
    parseOSC(oscReceiver);

    for(auto & lp : laserProjectors){
        lp.second.update();
    }
}



//--------------------------------------------------------------
void ofApp::draw() {
    
    
    // draw to the screen
    for(auto & lp : laserProjectors){
        if(lp.second.ildaFrame.params.output.doDisplace){

            ofFbo fboDisplMap;
            fboDisplMap.allocate(512, 512);
            fboDisplMap.begin();
            syphonIn.draw(0,0,fboDisplMap.getWidth(), fboDisplMap.getHeight());
            fboDisplMap.end();
            fboDisplMap.readToPixels(lp.second.ildaFrame.params.output.displacementMap);
        }
        
        
        lp.second.draw();
    }

//    ofSetColor(255);
//    ofDrawBitmapString(laserProjectors["frontStage"].ildaFrame.getString(), 10, 30);
    ofSetWindowTitle("Laser tools - fps " + ofToString(ofGetFrameRate(), 0) + " | receive OSC port " + ofToString(OSC_RECEIVE_PORT) +  " | fs: " + laserProjectors["frontStage"].syphonIn.getName() +  " | bs: " + laserProjectors["backStage"].syphonIn.getName());
    
    
    if(showGui) gui.draw();
    
    syphonOut.begin();
    {
        ofClear(0);
        ofSetColor(255);
        ofPushMatrix();
//        ofTranslate(laserProjectors["frontStage"].previewX, laserProjectors["frontStage"].previewY);
        ofScale(laserProjectors["frontStage"].previewW, laserProjectors["frontStage"].previewH);
        
        laserProjectors["frontStage"].ildaFrame.drawLines();
        laserProjectors["frontStage"].ildaFrame.drawFinalPoints();
        
        ofPopMatrix();
    }
    syphonOut.end();
//    syphonOut.draw();
    syphonOut.publish();
    
}
void ofApp::exit(){
}

//--------------------------------------------------------------

void ofApp::keyPressed(int key){
    switch(key) {
        case 'g': showGui = !showGui; break;
            
//        case 'i': syphonIn.next();
        case 'm':
            syphonIn.find("toLaser_displacement", "MadMapper");
            break;

        case 's':
//            gui.saveToFile("settings/general.json");
            laserProjectors["frontStage"].save("settings/lp_1.json");
            laserProjectors["backStage"].save("settings/lp_2.json");
            break;
        case 'l':
//            gui.loadFromFile("settings/general.json");
            laserProjectors["frontStage"].load("settings/lp_1.json");
            laserProjectors["backStage"].load("settings/lp_2.json");
            break;

    }
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){
    laserProjectors["frontStage"].setPreview(w-h*2 - 10, 0, h, h);
    laserProjectors["backStage"].setPreview(w-h, 0, h, h);
}


//--------------------------------------------------------------
void ofApp::parseOSC(ofxOscReceiver &receiver){
    // hide old messages
    for(int i = 0; i < NUM_MSG_STRINGS; i++){
        if(timers[i] < ofGetElapsedTimef()){
            msg_strings[i] = "";
        }
    }
    
    // check for waiting messages
    while(receiver.hasWaitingMessages()){
        // get the next message
        ofxOscMessage m;
        receiver.getNextMessage(m);
        
        // cout << m.getAddress() << endl;
        std::vector<std::string> address = ofSplitString(m.getAddress(),"/",true);
        
        // select which laser
//        int laserIndx = -1;
//        for(int i = 0; i<laserProjector.size(); i++){
//            if(laserProjectors[i].identifier == address[1]) laserIndx = i;
//        }
//        if(laserIndx == -1){
//            ofLogNotice("No f");
//        }
        
        // change Syphon remote
        if(address[0]=="syphonChannel") {
            if( laserProjectors[address[0]].syphonIn.getNumOfClients() > m.getArgAsInt(0) && m.getArgAsInt(0) >=0 ){
                laserProjectors[address[0]].syphonIn.setByIndex(m.getArgAsInt(0));
            }
        }else if(address[1]=="syphonActivate" || address[1]=="useColorMap" ){
            if( m.getArgAsInt(0) == 0) laserProjectors[address[0]].ildaFrame.params.output.useColorMap = false;
            if( m.getArgAsInt(0) == 1) laserProjectors[address[0]].ildaFrame.params.output.useColorMap = true;
        
        // do Displacement
        }else if(address[1]=="doDisplace"){
            if( m.getArgAsInt(0) == 0) laserProjectors[address[0]].ildaFrame.params.output.doDisplace = false;
            if( m.getArgAsInt(0) == 1) laserProjectors[address[0]].ildaFrame.params.output.doDisplace = true;
        }else if(address[1]=="displaceAmount"){
            laserProjectors[address[0]].ildaFrame.params.output.displaceAmount = m.getArgAsFloat(0);
            
        // change Laser Properties remote
        }else if(address[1]=="LaserEndCount"){
            laserProjectors[address[0]].ildaFrame.params.output.endCount = m.getArgAsInt(0);
            
        }else if(address[1]=="LaserStartCount"){
            laserProjectors[address[0]].ildaFrame.params.output.startCount = m.getArgAsInt(0);
        
        }else if(address[1]=="LaserEndBlanks"){
            laserProjectors[address[0]].ildaFrame.params.output.endBlanks = m.getArgAsInt(0);
            
        }else if(address[1]=="LaserStartBlanks"){
            laserProjectors[address[0]].ildaFrame.params.output.startBlanks = m.getArgAsInt(0);
            
        }else if(address[1]=="LaserNumPoints"){
            laserProjectors[address[0]].ildaFrame.polyProcessor.params.targetPointCount = m.getArgAsInt(0);
        }else if(address[1]=="LaserSpacing"){
            laserProjectors[address[0]].ildaFrame.polyProcessor.params.spacing = m.getArgAsInt(0);
        }else if(address[1]=="LaserOptimize"){
            laserProjectors[address[0]].ildaFrame.polyProcessor.params.optimizeTolerance = m.getArgAsInt(0);
        }else if(address[1]=="LaserSmooth"){
            laserProjectors[address[0]].ildaFrame.polyProcessor.params.smoothAmount = m.getArgAsInt(0);
        }else if(address[1]=="LaserPPS"){
            laserProjectors[address[0]].LASER_PPSx1000 = m.getArgAsInt(0)/1000;
            laserProjectors[address[0]].setPPS(laserProjectors[address[0]].LASER_PPSx1000*1000);

            
//HotSpots
        }else if (address[1]=="HotSpots"){
            if(address[2]=="show"){
                if(m.getArgAsString(0) == "all"){
                    laserProjectors[address[0]].hotSpotHandler.update();
                }else{
                    laserProjectors[address[0]].hotSpotHandler.hotSpots[m.getArgAsString(0)].update();
                }
            }else if(address[2]=="color"){
                laserProjectors[address[0]].hotSpotHandler.hotSpots[m.getArgAsString(0)].setColor( ofFloatColor(m.getArgAsFloat(1), m.getArgAsFloat(2), m.getArgAsFloat(3), m.getArgAsFloat(4)));
                laserProjectors[address[0]].hotSpotHandler.hotSpots[m.getArgAsString(0)].update();
            }
//Bezier
        }else if (address[1]=="Bezier"){
        if(address[2]=="show"){
            if(m.getArgAsString(0) == "all"){
                laserProjectors[address[0]].bezierHandler.update();
            }else{
                laserProjectors[address[0]].bezierHandler.bezierCourves[m.getArgAsString(0)].update();
            }
        }else if(address[2]=="color"){
            laserProjectors[address[0]].bezierHandler.bezierCourves[m.getArgAsString(0)].setColor( ofFloatColor(m.getArgAsFloat(1), m.getArgAsFloat(2), m.getArgAsFloat(3), m.getArgAsFloat(4)));
            laserProjectors[address[0]].bezierHandler.bezierCourves[m.getArgAsString(0)].update();
        }
    }

        
        // Create graphics
        auto z = laserProjectors[address[0]].zones.find(address[1]); // looking for the right Zone
        if(z != laserProjectors[address[0]].zones.end()) {
            
            
// LINE
            if(address[2]=="line"){
                if(address[3]=="color"){
                    z->second.lines[m.getArgAsInt32(0)].setColor( ofFloatColor(m.getArgAsFloat(1), m.getArgAsFloat(2), m.getArgAsFloat(3), m.getArgAsFloat(4)) );
                    
                }else if(address[3]=="cla"){
                    glm::vec3 center;
                    center.x = m.getArgAsFloat(1);
                    center.y = m.getArgAsFloat(2);
                    float length = m.getArgAsFloat(3);
                    float angle = m.getArgAsFloat(4);
                    z->second.lines[m.getArgAsInt32(0)].setCLA(center, length, angle);
                    
                } else if(address[3]=="clacol"){
                    glm::vec3 center;
                    center.x = m.getArgAsFloat(1);
                    center.y = m.getArgAsFloat(2);
                    float length = m.getArgAsFloat(3);
                    float angle = m.getArgAsFloat(4);
                    z->second.lines[m.getArgAsInt32(0)].setCLA(center, length, angle);
                    z->second.lines[m.getArgAsInt32(0)].setColor( ofFloatColor(m.getArgAsFloat(5), m.getArgAsFloat(6), m.getArgAsFloat(7)));
                } else if(address[3]=="spep"){
                    glm::vec3 startPoint = glm::vec3(m.getArgAsFloat(1), m.getArgAsFloat(2),0.);
                    glm::vec3 endPoint = glm::vec3(m.getArgAsFloat(3), m.getArgAsFloat(4),0.);
                    z->second.lines[m.getArgAsInt32(0)].setSpEp(startPoint, endPoint);
                    
                } else if(address[3]=="spepcol"){
                    glm::vec3 startPoint = glm::vec3(m.getArgAsFloat(1), m.getArgAsFloat(2),0.);
                    glm::vec3 endPoint = glm::vec3(m.getArgAsFloat(3), m.getArgAsFloat(4),0.);
                    z->second.lines[m.getArgAsInt32(0)].setSpEp(startPoint, endPoint);
                    z->second.lines[m.getArgAsInt32(0)].setColor( ofFloatColor(m.getArgAsFloat(5), m.getArgAsFloat(6), m.getArgAsFloat(7)));
                }
                
                
// CIRCLE
            }else if (address[2]=="circle"){
                
                if(address[3]=="erase"){
                    z->second.circles.erase(m.getArgAsInt32(0));
                    
                }else if(address[3]=="clear"){
                    z->second.circles.clear();
                    
                }else if(address[3]=="color"){
                    z->second.circles[m.getArgAsInt32(0)].setColor( ofFloatColor(m.getArgAsFloat(1), m.getArgAsFloat(2), m.getArgAsFloat(3), m.getArgAsFloat(4)) );
                    
                }else if(address[3]=="crcol"){
                    
                    glm::vec3 center;
                    center.x = m.getArgAsFloat(1);
                    center.y = m.getArgAsFloat(2);
                    float radius = m.getArgAsFloat(3);
                    
                    z->second.circles[m.getArgAsInt32(0)].setColor( ofFloatColor(m.getArgAsFloat(4), m.getArgAsFloat(5), m.getArgAsFloat(6)));
                    z->second.circles[m.getArgAsInt32(0)].setColor( ofFloatColor(m.getArgAsFloat(4), m.getArgAsFloat(5), m.getArgAsFloat(6)) );
                    z->second.circles[m.getArgAsInt32(0)].setCR(center, radius);
                }else if(address[3]=="crrcol"){
                    
                    glm::vec3 center;
                    center.x = m.getArgAsFloat(1);
                    center.y = m.getArgAsFloat(2);
                    float radius = m.getArgAsFloat(3);
                    int resolution = m.getArgAsFloat(4);
                    
                    z->second.circles[m.getArgAsInt32(0)].setColor( ofFloatColor(m.getArgAsFloat(5), m.getArgAsFloat(6), m.getArgAsFloat(7)));
                    
                    z->second.circles[m.getArgAsInt32(0)].setCR(center, radius);
                    z->second.circles[m.getArgAsInt32(0)].setResolution(resolution);
                }else if(address[3]=="crscol"){
                    
                    glm::vec3 center;
                    center.x = m.getArgAsFloat(1);
                    center.y = m.getArgAsFloat(2);
                    float radius = m.getArgAsFloat(3);
                    float segment = m.getArgAsFloat(4);
                    
                    z->second.circles[m.getArgAsInt32(0)].setColor( ofFloatColor(m.getArgAsFloat(5), m.getArgAsFloat(6), m.getArgAsFloat(7)));
                    
                    z->second.circles[m.getArgAsInt32(0)].setCRS(center, radius, segment);
                }
                
// DOT CIRCLE
            }else if (address[2]=="dotCircle"){
                
                if(address[3]=="erase"){
                    z->second.dotCircles.erase(m.getArgAsInt32(0));
                    
                }else if(address[3]=="clear"){
                    z->second.dotCircles.clear();
                    
                }else if(address[3]=="color"){
                    z->second.dotCircles[m.getArgAsInt32(0)].setColor( ofFloatColor(m.getArgAsFloat(1), m.getArgAsFloat(2), m.getArgAsFloat(3), m.getArgAsFloat(4)));
                }else if(address[3]=="crncol"){
                    
                    glm::vec3 center;
                    center.x = m.getArgAsFloat(1);
                    center.y = m.getArgAsFloat(2);
                    float radius = m.getArgAsFloat(3);
                    int numPoints = m.getArgAsInt(4);
                    
                    z->second.dotCircles[m.getArgAsInt32(0)].setColor( ofFloatColor(m.getArgAsFloat(5), m.getArgAsFloat(6), m.getArgAsFloat(7)));
                    
                    z->second.dotCircles[m.getArgAsInt32(0)].setCRN(center, radius, numPoints);
                }else if(address[3]=="crnaocol"){
                    
                    glm::vec3 center;
                    center.x = m.getArgAsFloat(1);
                    center.y = m.getArgAsFloat(2);
                    float radius = m.getArgAsFloat(3);
                    int numPoints = m.getArgAsInt(4);
                    float aOffset = m.getArgAsFloat(5);

                    
                    z->second.dotCircles[m.getArgAsInt32(0)].setColor( ofFloatColor(m.getArgAsFloat(6), m.getArgAsFloat(7), m.getArgAsFloat(8)));
                    
                    z->second.dotCircles[m.getArgAsInt32(0)].setCRNAo(center, radius, numPoints, aOffset);
                }
                
// GATE
                
            }else if (address[2]=="gate"){
                
                if(address[3]=="erase"){
                    z->second.gates.erase(m.getArgAsInt32(0));
                    
                }else if(address[3]=="clear"){
                    z->second.gates.clear();
                    
                }else if(address[3]=="color"){
                    z->second.gates[m.getArgAsInt32(0)].setColor( ofFloatColor(m.getArgAsFloat(1), m.getArgAsFloat(2), m.getArgAsFloat(3), m.getArgAsFloat(4)));
                    
                }else if(address[3]=="cwh"){
                    
                    glm::vec3 center;
                    center.x = m.getArgAsFloat(1);
                    center.y = m.getArgAsFloat(2);
                    
                    z->second.gates[m.getArgAsInt32(0)].setCWH(center, m.getArgAsFloat(3), m.getArgAsFloat(4));
                    
                }else if(address[3]=="cwhcol"){
                    
                    glm::vec3 center;
                    center.x = m.getArgAsFloat(1);
                    center.y = m.getArgAsFloat(2);
                    
                    z->second.gates[m.getArgAsInt32(0)].setCWH(center, m.getArgAsFloat(3), m.getArgAsFloat(4));
                    z->second.gates[m.getArgAsInt32(0)].setColor( ofFloatColor(m.getArgAsFloat(5), m.getArgAsFloat(6), m.getArgAsFloat(7)));
                }
                
// MORPHINPOLYS
            }else if (address[2]=="morphinPolys"){
                
                if(address[3]=="erase"){
                    z->second.morphinPolys.erase(m.getArgAsInt32(0));
                    
                }else if(address[3]=="clear"){
                    z->second.morphinPolys.clear();
                    
                }else if(address[3]=="color"){
                    z->second.morphinPolys[m.getArgAsInt32(0)].setColor( ofFloatColor(m.getArgAsFloat(1), m.getArgAsFloat(2), m.getArgAsFloat(3), m.getArgAsFloat(4)));
                    
                }else if(address[3]=="res"){
                    
                    z->second.morphinPolys[m.getArgAsInt32(0)].setRES(m.getArgAsFloat(1), m.getArgAsInt(2), m.getArgAsFloat(3));
                    
                }else if(address[3]=="rescol"){
                    
                    z->second.morphinPolys[m.getArgAsInt32(0)].setRES(m.getArgAsFloat(1), m.getArgAsInt(2), m.getArgAsFloat(3));
                    z->second.morphinPolys[m.getArgAsInt32(0)].setColor(ofFloatColor(m.getArgAsFloat(4), m.getArgAsFloat(5), m.getArgAsFloat(6)));
                }
                

// DANCING LINES
            }else if( address[2] == "dancingLines"){
                if(address[3]== "nsscol"){
                    if(z->second.dancingLines.size() == 0){
                        for(int i = 0; i < m.getArgAsInt32(0); i++){
                            DancingLine newLine;
                            
                            z->second.dancingLines.push_back(newLine);
                            z->second.numOfDLinesLast++;
                        }
                        
                    }else{
                        if( m.getArgAsInt32(0) > z->second.numOfDLinesLast){
                            
                            for (int n = 1; n < m.getArgAsInt32(0) - z->second.numOfDLinesLast; n++){
                                
                                DancingLine newLine;
                                
                                //                            newLine.location1 = glm::vec3(0, RENDER_HEIGHT/2);
                                //                            newLine.location2 = glm::vec3(RENDER_WIDTH, RENDER_HEIGHT/2);
                                //                            newLine.lineColor = colorDLines;
                                //                            newLine.freedom = RENDER_HEIGHT*2;
                                
                                z->second.dancingLines.push_back(newLine);
                                z->second.numOfDLinesLast++;
                                
                                for (auto & dl : z->second.dancingLines)    {
                                    dl.setSpeed(m.getArgAsFloat(1));
                                    dl.setSync(m.getArgAsFloat(2));
                                    dl.setColor(ofFloatColor(m.getArgAsFloat(3), m.getArgAsFloat(4), m.getArgAsFloat(5)));
                                }
                                
                                
                            }
                            z->second.numOfDLinesLast = z->second.dancingLines.size();
                            
                        }else if( m.getArgAsInt32(0) < z->second.numOfDLinesLast){
                            for (int n = 0; n<z->second.numOfDLinesLast-m.getArgAsInt32(0); n++){
                                z->second.dancingLines.pop_back();
                                z->second.numOfDLinesLast--;
                            }
                        }
                    }
                }
                
            }else if (address[2]=="text"){
                
                if(address[3]=="erase"){
                    z->second.doText = false;
                    
                }else if(address[3]=="clear"){
                    z->second.doText = false;
                    
                }else if(address[3]=="color"){
                    z->second.textInput.color = ofFloatColor(m.getArgAsFloat(0), m.getArgAsFloat(1), m.getArgAsFloat(2), m.getArgAsFloat(3));
                    
                }else if(address[3]=="str"){
                    
                    z->second.textInput.setText(m.getArgAsString(0));
                    
                    z->second.doText = true;
                    
                }
                else if(address[3]=="size"){
                    
                    z->second.textInput.setSize(m.getArgAsInt(0));
                    
                }
                
                
            } else if (address[2]=="triangle"){
                
                if(address[3]=="erase"){
                    z->second.triangles.erase(m.getArgAsInt32(0));
                    
                }else if(address[3]=="clear"){
                    z->second.triangles.clear();
                    
                }else if(address[3]=="color"){
                    z->second.triangles[m.getArgAsInt32(0)].setColor( ofFloatColor(m.getArgAsFloat(1), m.getArgAsFloat(2), m.getArgAsFloat(3), m.getArgAsFloat(4))) ;
                    
                }
            } else {
                std::cout << "Not found\n";
            }
            
        }
        
    }
}




//--------------------------------------------------------------
