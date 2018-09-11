//
//  jsonHelpers.hpp
//  Laser_tools_V2
//
//  Created by Jonas Fehr on 28/04/2018.
//

#ifndef jsonHelpers_h
#define jsonHelpers_h

#include <stdio.h>

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

#endif /* jsonHelpers_h */
