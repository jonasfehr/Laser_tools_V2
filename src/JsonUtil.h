//
//  jsonUtils.h
//  Laser_tools_V2
//
//  Created by Jonas Fehr on 29/04/2018.
//

#ifndef jsonUtils_h
#define jsonUtils_h

class JsonUtil{
public:
    
    ofParameter<std::string> identifier;

    virtual void addToJson(ofJson & json){}
    virtual void setupFromJson(ofJson & json){};
    
    void save(string path){
        ofJson json;
        addToJson(json);
        ofSavePrettyJson(path, json);
    }

    void load(string path) {
        ofJson json = ofLoadJson(path);
        
        for(auto & j : json){
            setupFromJson(j);
        }
    }
    
    ofJson vec3ToJson(glm::vec3 vec){
        ofJson json;
        json["x"] = vec.x;
        json["y"] = vec.y;
        json["z"] = vec.z;
        return json;
    }
    
    glm::vec3 jsonToVec3( ofJson json){
        glm::vec3 vec;
        vec.x = json["x"];
        vec.y = json["y"];
        vec.z = json["z"];
        return vec;
    }
    
    ofJson fColorToJson(ofFloatColor color){
        ofJson json;
        json["r"] = color.r;
        json["g"] = color.g;
        json["b"] = color.b;
        json["a"] = color.a;
        return json;
    }
    
    ofFloatColor jsonTofColor( ofJson json){
        ofFloatColor color;
        color.r = json["r"];
        color.g = json["g"];
        color.b = json["b"];
        color.a = json["a"];
        return color;
    }

};

#endif /* jsonUtils_h */
