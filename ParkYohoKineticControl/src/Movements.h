#ifndef __ParkYohoMovements__
#define __ParkYohoMovements__

#include <stdio.h>
#include "ofMain.h"
#include "ofxGui.h"
#include "ofxJSON.h"


class Movements {
public:
    void setup();
    void update();
    void draw();
    void exit();
    
    ofEvent<bool> setPointsEvent;
    
    vector<int> setShapeA();
    vector<int> setShapeB();
    
    void incrementShape();
    
    int getCurrShape();
    int getTotalShapes();
    
    vector<vector<int>> setShapes();
    int currShape;
    int numOfShapes;
    
    long currMillis;
    long prevMillis;
    
    int nextTrigger=0;
    
    void set(int id, int r, int g, int b, int w);
    void setAll(int r, int g, int b, int w);
    
    ofxJSONElement settings;
    void saveSettings();
    void loadSettings();
    
    
    vector<int> cableOpA,cableOpB;
    vector<vector<int>> pointsA1, pointsA2, pointsB1, pointsB2;

};


#endif

