//
//  Util.h
//  interface
//
//  Created by Philip Rosedale on 8/24/12.
//  Copyright (c) 2012 High Fidelity, Inc. All rights reserved.
//

#ifndef __interface__Util__
#define __interface__Util__

#ifdef _WIN32
#include "Systime.h"
#else
#include <sys/time.h>
#endif

#include <glm/glm.hpp>

#include <Orientation.h>


float azimuth_to(glm::vec3 head_pos, glm::vec3 source_pos);
float angle_to(glm::vec3 head_pos, glm::vec3 source_pos, float render_yaw, float head_yaw);

float randFloat();
void render_world_box();
void render_vector(glm::vec3 * vec);
int widthText(float scale, int mono, char *string);
void drawtext(int x, int y, float scale, float rotate, float thick, int mono, 
              char const* string, float r=1.0, float g=1.0, float b=1.0);
void drawvec3(int x, int y, float scale, float rotate, float thick, int mono, glm::vec3 vec, 
              float r=1.0, float g=1.0, float b=1.0);
double diffclock(timeval *clock1,timeval *clock2);

void drawGroundPlaneGrid( float size, int resolution );

void renderOrientationDirections( glm::vec3 position, Orientation orientation, float size );


class oTestCase {
public:
    float yaw;
    float pitch;
    float roll;
    
    float frontX;    
    float frontY;
    float frontZ;    
    
    float upX;    
    float upY;
    float upZ;    
    
    float rightX;    
    float rightY;
    float rightZ;    
    
    oTestCase(
        float yaw, float pitch, float roll, 
        float frontX, float frontY, float frontZ,
        float upX, float upY, float upZ,
        float rightX, float rightY, float rightZ
    ) : 
        yaw(yaw),pitch(pitch),roll(roll),
        frontX(frontX),frontY(frontY),frontZ(frontZ),
        upX(upX),upY(upY),upZ(upZ),
        rightX(rightX),rightY(rightY),rightZ(rightZ)
    {};
};


void testOrientationClass();

#endif
