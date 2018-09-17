#ifndef __ParkYohoMovementController__
#define __ParkYohoMovementController__

#include <stdio.h>
#include "ofMain.h"
#include "ofxGui.h"

class
MovementController {
public:

    
    void setup(int cableNum, int x1, int y1,int w, int h, int x_range, int y_range);
    void update();
    void draw();
    void setPoints(int a, int b, int c,int d);
    vector<ofPoint> getPoints();
    int getOption(int op);
    int option;
    
    void setOption(int op, int val);
    
    ofxPanel curveControls;
    ofParameterGroup parametersDebug;
    
    vector<ofParameter<int>> curves;
    
    ofPolyline trail;
    float x,y;
    float increment;
    float angle;
    
    ofPoint curveCoord;
    int curveW;
    int curveH;
    
    int max_x_pos;
    int max_y_pos;
    
    int numOfCables;
    
};


#endif


