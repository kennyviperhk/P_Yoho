#ifndef __ParkYohoMovementController__
#define __ParkYohoMovementController__

#include <stdio.h>
#include "ofMain.h"
#include "ofxGui.h"

class
MovementController {
public:

    
    void setup(int cableNum, int x1, int y1,int w, int h, int x_range, int y_range, int numCtrl);
    void update();
    void draw();
    void setPoints(int whichCtrl, int op, int a, int b, int c,int d);
    /*
     op:0 disabled
     op:1 all
     op:2 even
     op:3 odd
    */
    vector<ofPoint> getPoints();
    int getOption(int op);
    int option;
    
    void setOption(int op, int val);
    
    ofxPanel curveControls;
    ofParameterGroup parametersDebug;
    
    vector<ofParameter<int>> curves;
    
    vector<ofPolyline> trail;
    float x,y;
    float increment;
    float angle;
    
    vector<ofPoint> curveCoord;
    int curveW;
    int curveH;
    
    int max_x_pos;
    int max_y_pos;
    
    int numOfCables;
    int numOfControllers;
    
};


#endif


