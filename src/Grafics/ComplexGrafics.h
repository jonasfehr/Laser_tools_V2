//
//  ComplexGrafics.h
//  Laser_tools_V2
//
//  Created by Jonas Fehr on 19/06/2017.
//
//

#ifndef ComplexGrafics_h
#define ComplexGrafics_h




class DotCircle{
public:
    vector <ofPolyline> polys;
    ofFloatColor color;
    
    void setColor(ofColor col)          { color = col; };
    void setColor(ofFloatColor col)     { color = col; };
    ofFloatColor getColor()             { return color; };
    
    void setCenter(glm::vec3 cntrPnt)     { centerPoint = cntrPnt; };
    void setRadius(float r)             { radius = r; };
    void setAngleOffset(float aOffset)             { angleOffset = aOffset; };
    void setNumberOfPoints(int numberOfPoints) { nPoints = numberOfPoints; };
    
    void setCRN(glm::vec3 cntrPnt, float r, int numberOfPoints) {
        centerPoint = cntrPnt;
        radius = r;
        nPoints = numberOfPoints;
        
        update();
    }
    
    void setCRNAo(glm::vec3 cntrPnt, float r, int numberOfPoints, float aOffset) {
        centerPoint = cntrPnt;
        radius = r;
        nPoints = numberOfPoints;
        angleOffset = aOffset;
        
        update();
    }
    
    void update(){
        polys.clear();
        for(int i = 0; i<nPoints; i++){
            ofPolyline p;
            float a = (float)i/nPoints*TWO_PI + angleOffset/360;
            float x = centerPoint.x + cos(a)*radius;
            float y = centerPoint.y + sin(a)*radius;
            p.addVertex( ofPoint( x, y ) );
            polys.push_back(p);
        }
        
        life = 0.;
    };
    
    float life = 0.;
    
private:
    glm::vec3 centerPoint = glm::vec3(0.5,0.5,0.); // also center
    float radius = 0.5;
    int nPoints= 10;
    float angleOffset = 0.;
};


class TestPattern{
public:
    vector<ofPolyline> polys;
    ofFloatColor color;
    
    TestPattern(){
        //        ofPolyline squareWithCross;
        //        squareWithCross.addVertex(0,0);
        //        squareWithCross.addVertex(1,0);
        //        squareWithCross.addVertex(1,1);
        //        squareWithCross.addVertex(0,1);
        //        // Diagonal
        //        squareWithCross.addVertex(0,0);
        //        squareWithCross.addVertex(1,1);
        //        polys.push_back(squareWithCross);
        //
        //        ofPolyline missingDiagonal;
        //        missingDiagonal.addVertex(1,0);
        //        missingDiagonal.addVertex(0,1);
        //        polys.push_back(missingDiagonal);
        //
        //        ofPolyline circle;
        //        circle.arc(ofPoint(0.5,0.5), 0.5, 0.5, 0, 360, true, 40);
        //        polys.push_back(circle);
        
        
        
        ofPolyline square;
        square.addVertex(0,0);
        square.addVertex(1,0);
        square.addVertex(1,1);
        square.addVertex(0,1);
        square.close();
        polys.push_back(square);
        
        float r = 0.5;//.25 * sqrt(2.0f);
        float d1 = r-r/sqrt(2);
        float d2 = 1.-d1;
        
        ofPolyline p1;
        p1.lineTo(d1,d1);//0.25, 0.25);
        p1.lineTo(d2,d2);//0.75, 0.75);
        polys.push_back(p1);
        
        ofPolyline p2;
        p2.lineTo(d2,d1);//0.75, 0.25);
        p2.lineTo(d1,d2);//0.25, 0.75);
        polys.push_back(p2);
        
        ofPolyline p3;
        p3.arc(0.5, 0.5, r, r, 0, 360, 60);
        polys.push_back(p3);
        
        
    }
};


#endif /* ComplexGrafics_h */
