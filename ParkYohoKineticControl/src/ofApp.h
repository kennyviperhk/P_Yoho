#pragma once

#include "ofMain.h"
#include "KineticController.h"
#include "MovementController.h"
#include "MusicPlayer.h"

#include "ofxSerial.h"
#include "ofxGui.h"
#include "ofxInputField.h"



#include <iostream>
#include <string>
#include <regex>

#define NUM_OF_CABLES 20 //Todo Transfer definition /variables to xml
#define BAUD 57600 //Todo Transfer definition /variables to xml


#define HOME_MAXSPEED     int_array[0]
#define HOME_ACCELERATION     int_array[1]
//RUNNING
#define MAX_SPEED_X      int_array[2]
#define MAX_ACCELERATION_X       int_array[3]
#define MAX_SPEED_Y      int_array[4]
#define MAX_ACCELERATION_Y       int_array[5]
#define MAX_POSITION_LX      int_array[6]
#define MAX_POSITION_LY      int_array[7]
#define MAX_POSITION_RX      int_array[8]
#define MAX_POSITION_RY      int_array[9]
#define INVERT_DIR_LX      int_array[10]
#define INVERT_DIR_LY      int_array[11]
#define INVERT_DIR_RX      int_array[12]
#define INVERT_DIR_RY      int_array[13]

/*
 notes: 
 
 //command: send motor index [0-4] - position [0-10000] - interpolation style [0-4?] - 3 style parameters ... (example: time to position [0 - 100000ms])

 todo:
//controllable single module:
 accel, speed, pos (send to arduino), flip dir, 
 
 //call for assure connection
 
 */

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
        void exit();

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
    
    //========== kinecticVisualisation ===========
    
    
    KinecticVisualisation kinecticVisualisation;
    
    //========== MusicPlayer ===========
    
    MusicPlayer musicPlayer;
    bool showMusicPlayer;
    
    //========== Movement Controller ===========
    
    //vector<MovementController> MovementControllers;
    MovementController MovementController;
    bool showMovementController;
    
    //================== Serial ==================
    
    vector<bool> serialSetup();
    string serialRead(int a);
    vector<int> stringDecode(string s);
    bool is_number(const std::string& s);
    vector<bool> isArduinoConnected;
    vector<bool> isArduinoConnectedBySerial;
    void serialWrite(int arduinoID, string sw);
    void checkArduinoIsConnected();
    bool initOnUpdate;
    long checkArduinoMillis; //todo
    
    vector<ofx::IO::BufferedSerialDevice> arduino;
    
    //unused
    //void sendChar();
    void onSerialBuffer(const ofx::IO::SerialBufferEventArgs& args);
    void onSerialError(const ofx::IO::SerialBufferErrorEventArgs& args);
    
    vector<SerialMessage> serialMessages;
    
    string receivedMsg;
    vector<string> prevReceivedString;
    vector<string> receivedStringBuffer;
    vector<string> receivedString;
    vector<int> updateColor;
    
    //function
    void removeSubstrs(string& s, string& p);
    
    vector<string> SERIAL_PARAMETERES;
    
    //================== debugMode ==================
    
    int page;
    int numOfPages;
    void guiSetup();
    
    bool debugMode;
    ofxPanel guiDebug;
    ofParameterGroup parametersDebug;
    ofXml settings;
    vector<ofParameter<int>> EEPROM;
    vector<ofxButton> EEPROM_btn;

    
    ofxButton EEPROM_saveBtn;
    ofxButton EEPROM_loadBtn;
    ofxButton style_Btn;
    ofxButton home_Btn;
    ofxToggle all_Tog;
    
    ofxTextField textField;
    
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
    
    
    //Cable Accel
    ofxPanel guiCableAccelLx;
    ofxPanel guiCableAccelLy;
    ofxPanel guiCableAccelRx;
    ofxPanel guiCableAccelRy;
    //ofxPanel guiCableAccel;
    ofParameterGroup parametersCableAccel;
    vector<ofParameter<int>> cableAccelLx; //lx,ly,rx,ry
    vector<ofParameter<int>> cableAccelLy; //lx,ly,rx,ry
    vector<ofParameter<int>> cableAccelRx; //lx,ly,rx,ry
    vector<ofParameter<int>> cableAccelRy; //lx,ly,rx,ry
    
    //Cable Speed
    ofxPanel guiCableSpeedLx;
    ofxPanel guiCableSpeedLy;
    ofxPanel guiCableSpeedRx;
    ofxPanel guiCableSpeedRy;
    //ofxPanel guiCableAccel;
    ofParameterGroup parametersCableSpeed;
    vector<ofParameter<int>> cableSpeedLx; //lx,ly,rx,ry
    vector<ofParameter<int>> cableSpeedLy; //lx,ly,rx,ry
    vector<ofParameter<int>> cableSpeedRx; //lx,ly,rx,ry
    vector<ofParameter<int>> cableSpeedRy; //lx,ly,rx,ry
    
    ofParameter<int> currentDebugArduinoID;
    ofParameter<int> currentStyle;
    
    bool serialTrigger; //TO avoid ofxButton cause multiple click and send mutiple serial command;
    long prevSerialTriggerMillis; //TO avoid ofxButton cause multiple click and send mutiple serial command;
    long currMillis;
    
    void displayLog(string s);
    string currentdisplayLog;
    void commandPrompt();
    
    bool isEmergencyStop;
    
    void loadButtonPressed();
    
    
    ofFbo kineticVisualizationFbo;
    bool drawKineticVisualizationFbo;
    bool drawDebugGui;

    //================== Config ==================
    
    //int numOfCables;
    
    
    
};

