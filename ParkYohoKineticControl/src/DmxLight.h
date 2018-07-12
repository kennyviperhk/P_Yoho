#ifndef __ParkYohoDmxLight__
#define __ParkYohoDmxLight__

#include <stdio.h>
#include "ofMain.h"
#include "ofxDmx.h"
#include "ofxGui.h"

#define NUM_OF_DMX 20
#define NUM_OF_CHANNEL 4

class
DmxLight {
public:
    void setup();
    void update();
    void draw();
    void exit();
    
    void set(int id, int r, int g, int b, int w);
    void setAll(int r, int g, int b, int w);
    
    ofxDmx dmx;
    ofxPanel panel1;
    ofxPanel panel2;
    string port;
    int modules, channelsPerModule;
    ofParameter<bool> moduleNum[80];
    
    ofParameter<float> red[80], green[80], blue[80], white[80];
    
    
    ofParameter<float> pred[80], pgreen[80], pblue[80], pwhite[80];
    ofParameter<bool> load, save;
    
};


#endif

