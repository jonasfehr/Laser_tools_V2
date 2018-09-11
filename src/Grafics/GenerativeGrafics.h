//
//  GenerativeGrafics.h
//  Laser_tools_V2
//
//  Created by Jonas Fehr on 19/06/2017.
//
//

#ifndef GenerativeGrafics_h
#define GenerativeGrafics_h

#include "SimpleGrafics.h"

class Particle {
public:
    glm::vec3   position,velocity, acceleration, center;
    float radius;
    
    Particle(glm::vec3 position, float radius){
        this->radius = radius;
        this->position = position;
        this->center = glm::vec3(0.5,0.5,0.);
        velocity = glm::vec3(ofRandom(-1, 1),ofRandom(-1, 1),0.);
        velocity = glm::normalize(velocity);
        velocity = velocity * 0.001 ;
    };
    
    void update(){
        checkCollision();
        position = position + velocity/60.0;
    };
    
    void checkCollision(){
        // collide
        glm::vec3 collisionVector = position + velocity/60.0 - center;
        
        float distance = glm::length(collisionVector);
        if( distance < radius ){
            return; // return if particle inside
        }
        
        // calculate the speed that will be transfered in a collision
        collisionVector = glm::normalize(collisionVector);
        float transferVelocity = glm::dot(velocity, collisionVector);
        
        if (transferVelocity < 0) return;
        
        velocity = velocity + collisionVector * (-transferVelocity);
        
        if( distance > radius ) velocity = velocity + collisionVector * -0.5;
        
        velocity = velocity + (glm::normalize(glm::vec3(ofRandom(-1, 1),ofRandom(-1, 1),0.))*0.1); // add a little random direction
        
    }
    
    glm::vec3 getPosition(){ return position;};
    
};

class MorphinPoly : public PolyGrafic{
public:
    
    MorphinPoly(){
        oscAddr = "/MorphinPoly";
    }
    
    //void setSpOF()
    void setSpeed(float speed) { this->speed = speed; };
    void setNumEdges(int numEdges){ numOfParticles = numEdges; };
    void setRES( float radius, int numEdges, float speed) {
        this->radius = radius;
        numOfParticles = numEdges;
        this->speed = speed;
        update();
    }
    
    void addNewParticle(){
        if(particles.size()>0){
            Particle particle = *new Particle(particles.back().position, radius);
            particles.push_back(particle);
            
        } else {
            glm::vec3 center = glm::vec3(0.5,0.5,0.);
            glm::vec3 position = center;
//            position += glm::rotate( glm::vec3( ofRandom(radius), 0, 0)  , ofRandom(360), glm::vec3(0,0,1) );
            Particle particle = *new Particle(position, radius);
            particles.push_back(particle);
            
        }
        //        particle.setup(mass, origin, initialVelocity);
        //        particle.worldSize = sphereSize;
        //        particle.attractionStrength = attraction;
        //        cout << "+" << endl;
    };
    
    void deleteLastParticle(){
        particles.pop_back();
        //        cout << "-" << endl;
        
    };
    
    
    
    // Method to update location
    void update() {
        //        cout << numOfParticles << " / " << numOfParticlesLast << endl;
        
        
        
        if(numOfParticles > numOfParticlesLast){
            for (int n = 0; n<numOfParticles-numOfParticlesLast; n++){
                addNewParticle();
            }
        }
        else if(numOfParticles < numOfParticlesLast){
            for (int n = 0; n<numOfParticlesLast-numOfParticles; n++){
                deleteLastParticle();
            }
        }
        numOfParticlesLast = particles.size();
        
        
        for(auto & p : particles){
            p.radius = radius;
            p.velocity = glm::normalize(p.velocity);
            p.velocity = p.velocity * speed;
            p.update();
        }
        
        
        if(particles.size()>0){
            poly.clear();
            poly.addVertex(particles[0].getPosition().x, particles[0].getPosition().y);
            for(int i = 1; i<particles.size(); i++){
                poly.lineTo(particles[i].getPosition().x, particles[i].getPosition().y);
            }
            poly.lineTo(particles[0].getPosition().x, particles[0].getPosition().y);
        }
        
        life = 0.;
        
    };
    
    float life = 0.;
    
private:
    int numOfParticles, numOfParticlesLast = 0;
    vector<Particle> particles;
    float speed;
    float radius;
};


class DancingLine : public PolyGrafic{
public:
    
    DancingLine(){
        oscAddr = "/DancingLine";

        seedOffset = ofRandom(1000);
        sync = 1;
        location1 = ofPoint(0,0.);
        location2 = ofPoint(1,0.);
        counterDLines = 0;
    };
    
    //void setSpOF()
    void setSpeed(float _speed) { speed = _speed; };
    void setSync(float _sync) { sync = _sync; };
    
    
    // Method to update location
    void update() {
        
        counterDLines += speed/100;
        
        offset1 = ((ofNoise(counterDLines+seedOffset)));
        offset2 = ((ofNoise(counterDLines*sync+seedOffset)));
        
        
        if(poly.size()<2){
            poly.clear();
            poly.addVertex(location1.x, location1.y+offset1);
            poly.lineTo(location2.x, location2.y+offset2);
        }else{
            poly.getVertices()[0] = ofPoint(location1.x, location1.y+offset1);
            poly.getVertices()[1] = ofPoint(location2.x, location2.y+offset2);
            
        }
    };
private:
    glm::vec3 location1;
    glm::vec3 location2;
    float offset1, offset2;
    float sync;
    float seedOffset;
    float counterDLines;
    float speed;
};

#endif /* GenerativeGrafics_h */
