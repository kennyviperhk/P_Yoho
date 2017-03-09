//
//  KineticController.h
//  ParkYohoKineticControl
//
//  Created by lucas on 2017-03-08.
//
//

#ifndef __ParkYohoKineticControl__KineticController__
#define __ParkYohoKineticControl__KineticController__

#include <stdio.h>
#include "ofMain.h"

class Cable
{
    /*
     holds data and visualises single cable
     
     ?might also do interpolation to from one position?
     */
public:
    float linearRailLeft = 1;//0 is center
    float linearRailRight = 1;//0 is center
    float cableLeft, cableRight;//0 is top position, 1 bottom
    int index; //use index for visualisation position
    int totalCount;

    void draw()
    {
        int zPos = index * drawDepth;

        float extendCenteredCables = (2-((linearRailRight+1)-(1-linearRailLeft)))*100;//extend lenght when cables get closer to center
        ofPushMatrix();
        ofTranslate(0, 100, zPos - totalCount * drawDepth/2);//translate to position, minus center value
        ofRotateY(10);
        ofFill();
        ofSetColor(150, 150, 255, 60);
        ofDrawBox(0, 0, 0, drawWidth, 10, drawDepth-2);//draw top support
        ofNoFill();
        ofSetColor(255);
        float leftTop = linearRailLeft * (drawWidth/-2.);
        float rightTop = linearRailRight * (drawWidth/2.);
        ofDrawBezier(leftTop, 0, 0, leftTop, -100 - cableLeft*300 - extendCenteredCables, 0, rightTop, -100 - cableRight*300- extendCenteredCables, 0, rightTop, 0, 0);
        ofPopMatrix();
    }
    int drawWidth = 200;
    int drawDepth = 20;


};

class KinecticVisualisation
{
public:
    
    KinecticVisualisation()
    {
        cablesPositions.assign(cables, Cable());
        for(int i = 0; i < cablesPositions.size(); i++)
        {
            cablesPositions[i].index = i;
            cablesPositions[i].totalCount = cables;
        }
    }
    
    void draw()
    {
        //temporary test positions
        for(int i = 0; i < cablesPositions.size(); i++)
        {
            float noiseSpeed = 0.1;
            float noiseSpread = 0.06;
            
            cablesPositions[i].linearRailLeft = ofNoise(ofGetElapsedTimef()*noiseSpeed, 0, i*noiseSpread);
            cablesPositions[i].linearRailRight = ofNoise(ofGetElapsedTimef()*noiseSpeed, 1238.44, i*noiseSpread);
            cablesPositions[i].cableLeft = ofNoise(ofGetElapsedTimef()*noiseSpeed, 34774.3, i*noiseSpread);
            cablesPositions[i].cableRight = ofNoise(ofGetElapsedTimef()*noiseSpeed, 283.2, i*noiseSpread);
        }
        
        
        cam.begin();
        for(int i = 0; i < cablesPositions.size(); i++)
        {
            cablesPositions[i].draw();
        }
        cam.end();
    }
    
    ofEasyCam cam;
    
    vector<Cable> cablesPositions;
    
    int cables = 20;

};



#endif /* defined(__ParkYohoKineticControl__KineticController__) */