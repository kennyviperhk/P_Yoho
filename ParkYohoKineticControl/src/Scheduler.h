#ifndef __ParkYohoScheduler__
#define __ParkYohoScheduler__

#include <stdio.h>
#include "ofMain.h"
#include "ofxGui.h"
#include "ofxXmlSettings.h"



class
Scheduler {
public:
    void setup();
    void update();
    void draw();
    void exit();

    
    ofParameter<int> beginHour, beginMin, endHour, endMin;

    int displayBeginHour, displayBeginMin, displayEndHour, displayEndMin;
    ofxButton save;
    
    ofxPanel panel1;
    
    void okBtnPressed();
    
    ofEvent<bool> lightToggleEvent;
    ofEvent<int> homeResetEvent;
    ofEvent<int> changeFormEvent;
    
    bool eventIsTriggered ;
    
    void saveSettings();
    void loadSettings();
    
    ofxXmlSettings XML;
    
};


#endif

