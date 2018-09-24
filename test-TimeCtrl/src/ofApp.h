#pragma once

#define NUM_OF_CABLES 20 //Todo Transfer definition /variables to xml
#define BAUD 57600 //Todo Transfer definition /variables to xml

#define MAX_X_POS 5000
#define MAX_Y_POS 30000

#define MAX_X_TIME 60000
#define MAX_Y_TIME 60000

#include "ofMain.h"
#include "ofxSerial.h"
#include "ofxGui.h"



class SerialMessage
{
public:
    SerialMessage(): fade(0)
    {
    }
    
    SerialMessage(const std::string& _message,
                  const std::string& _exception,
                  int _fade):
    message(_message),
    exception(_exception),
    fade(_fade)
    {
    }
    
    std::string message;
    std::string exception;
    int fade;
};


class ofApp : public ofBaseApp{
    
public:
    void setup();
    void update();
    void draw();
    
    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void mouseEntered(int x, int y);
    void mouseExited(int x, int y);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
    
    int currCableID;
    
    //================== Serial ==================
    vector<bool> isArduinoConnected;
    vector<bool> serialSetup();
    vector<int> stringDecode(string s);
    void serialWrite(int arduinoID, string sw);
    string serialRead(int a);
    
#ifdef USEOSC
    vector<bool> isArduioPort;
    vector<int> arduino;
#else
    vector<ofx::IO::BufferedSerialDevice> comPort;
    vector<ofx::IO::BufferedSerialDevice> arduino;
#endif
    
    vector<SerialMessage> serialMessages;
    
    void onSerialBuffer(const ofx::IO::SerialBufferEventArgs& args);
    void onSerialError(const ofx::IO::SerialBufferErrorEventArgs& args);
    
        vector<string> receivedString;
    
    
    //================== GUI ==================
    
    void setupGui();
    void drawGui();
    
    //Cable Pos
    ofxPanel guiCablePosLx;
    ofxPanel guiCablePosLy;
    ofxPanel guiCablePosRx;
    ofxPanel guiCablePosRy;
    ofParameterGroup parametersCablePos;
    vector<ofParameter<int>> cablePosLx; //lx,ly,rx,ry
    vector<ofParameter<int>> cablePosLy; //lx,ly,rx,ry
    vector<ofParameter<int>> cablePosRx; //lx,ly,rx,ry
    vector<ofParameter<int>> cablePosRy; //lx,ly,rx,ry
    
    
    
    //Cable Time
    ofxPanel guiCableTimeLx;
    ofxPanel guiCableTimeLy;
    ofxPanel guiCableTimeRx;
    ofxPanel guiCableTimeRy;
    ofParameterGroup parametersCableTime;
    vector<ofParameter<int>> cableTimeLx; //lx,ly,rx,ry
    vector<ofParameter<int>> cableTimeLy; //lx,ly,rx,ry
    vector<ofParameter<int>> cableTimeRx; //lx,ly,rx,ry
    vector<ofParameter<int>> cableTimeRy; //lx,ly,rx,ry
    

    
    
    ofParameter<int> currStyle;
    void writeStyle(int s);
    void moveCommandMethod(int method, int c, int whichCurrentCable);
    
    
    bool is_number(const std::string& s);

    
};


