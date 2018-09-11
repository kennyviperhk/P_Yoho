#pragma once

//DEFINITION

#define USEOSC
//#define RECEIVER_IS_WINDOWS
#define RECEIVER_IS_LINUX
//INCLUDES

#include "ofMain.h"
#include "ofEvents.h"

#include "KineticController.h"
#include "MovementController.h"
#include "MusicPlayer.h"
#include "DmxLight.h"
#include "TimelinePlayer.h"


#include "ofxSerial.h"
#include "ofxGui.h"
#include "ofxInputField.h"

#include <iostream>
#include <string>
#include <regex>

#ifdef USEOSC
#include "ofxOsc.h"
#define R_PORT 12346

//#define S_HOST "192.168.0.101"
#define S_HOST "10.0.1.124"
//#define S_HOST "localhost"
#define S_PORT 12345

#else

#endif


#define NUM_OF_SERIAL_TO_INIT 32
#define NUM_OF_CABLES 20 //Todo Transfer definition /variables to xml
#define BAUD 57600 //Todo Transfer definition /variables to xml


#define MAX_X_POS 5000
#define MAX_Y_POS 30000

#define MAX_X_TIME 60000
#define MAX_Y_TIME 60000

#define MAX_X_SPEED 1000
#define MAX_Y_SPEED 1000

#define MAX_X_ACCEL 1000
#define MAX_Y_ACCEL 1000

#define HOME_MAXSPEED     int_array[0]

//RUNNING //TODO REVISE
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

//========== settings ===========
//Timeline
#define SHOW_DELAY_TIME 0 //time to delay video begins after getting serial signal
#define NUM_TIMELINE 14

/*
 notes:
 
 //command: send motor index [0-4] - position [0-10000] - interpolation style [0-4?] - 3 style parameters ... (example: time to position [0 - 100000ms])
 
 todo:
 //DETECT HOME
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
    void keyReleased(int key);
    
    void keyPressed(int key){};
    void mouseMoved(int x, int y ){};
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void mouseEntered(int x, int y){};
    void mouseExited(int x, int y){};
    void windowResized(int w, int h){};
    void dragEvent(ofDragInfo dragInfo){};
    void gotMessage(ofMessage msg){};
    
    //========== kinecticVisualisation ===========
    KinecticVisualisation kinecticVisualisation;

    //================== MusicPlayer / Timeline player ==================
    TimelinePlayer timelinePlayer;
    void onKeyframe(Keyframe &kf);
    
    bool isExhibitionMode;
    bool drawMusicPlayer;
    
    MusicPlayer musicPlayer;

    long currMusicDuration;
    void playTrack(bool & t);
    void pauseTrack(bool & t);
    void setTrackisLoop(bool t);
    
    void changeTrackPos(float & pos);
    
    void musicAndTimelineSetup();
    //========== Movement Controller ===========
    
    //vector<MovementController> MovementControllers;
    MovementController MovementController;
    bool drawMovementController;
    
    
    //========== Movement ===========
    void cableOption(int cb);
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
    
    
#ifdef USEOSC
    vector<bool> isArduioPort;
    vector<int> arduino;
#else
    vector<ofx::IO::BufferedSerialDevice> comPort;
    vector<ofx::IO::BufferedSerialDevice> arduino;
#endif
    
    
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
    void guiDraw();
    
    bool debugMode;
    ofxPanel guiDebug;
    ofxPanel guiDebug2;
    ofxPanel guiDebugSingleCableCtrl;

    ofParameterGroup parametersDebug;
    ofXml XML;
    void saveSettings();
    void loadSettings();
    vector<ofParameter<int>> EEPROM;
    vector<ofxButton> EEPROM_btn;
    
    ofxButton EEPROM_saveBtn;
    ofxButton EEPROM_loadBtn;
    ofxButton sendStyleBtn;
    ofxButton sendStyleBtn_all_same;
    ofxButton sendStyleBtn_all;
    ofxButton reset_Btn;
    ofxButton home_Btn;
    ofxToggle all_Tog;
    vector<ofParameter<bool>> working_cable;
    vector<ofParameter<bool>> input_pts;
    vector<ofParameter<bool>> output_pts;
    
    vector<ofParameter<int>> singleCablePos;
    vector<ofParameter<bool>> singleCablePosLoop;
    ofxButton singleCableResetBtn;
    ofxButton singleCableResetAllBtn;
    ofxButton singleCableHomeAllBtn;
    ofxButton singleCableSendStyleBtn;
    
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
    
    //Cable Pos Offset
    ofxPanel guiCablePosLxOffset;
    ofxPanel guiCablePosLyOffset;
    ofxPanel guiCablePosRxOffset;
    ofxPanel guiCablePosRyOffset;
    ofParameterGroup parametersCablePosOffset;
    vector<ofParameter<int>> cablePosLxOffset; //lx,ly,rx,ry
    vector<ofParameter<int>> cablePosLyOffset; //lx,ly,rx,ry
    vector<ofParameter<int>> cablePosRxOffset; //lx,ly,rx,ry
    vector<ofParameter<int>> cablePosRyOffset; //lx,ly,rx,ry
    
    bool showOffset;
    
    //Style 2
    
    ofxPanel guiCablePosLx2;
    ofxPanel guiCablePosLy2;
    ofxPanel guiCablePosRx2;
    ofxPanel guiCablePosRy2;
    ofxPanel guiCableSpeedAccelAll;
    
    vector<ofParameter<int>> cablePosLx2; //lx,ly,rx,ry
    vector<ofParameter<int>> cablePosLy2; //lx,ly,rx,ry
    vector<ofParameter<int>> cablePosRx2; //lx,ly,rx,ry
    vector<ofParameter<int>> cablePosRy2; //lx,ly,rx,ry
    
    ofParameter<int> cableSpeedX;
    ofParameter<int> cableAccelX;
    ofParameter<int> cableSpeedY;
    ofParameter<int> cableAccelY;
    
    
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
    
    ofParameter<int> currCableID;
    ofParameter<int> prevCableID;
    ofParameter<int> currentStyle;
    void writeStyle(int s);
    void moveCommandMethod(int method, int c, int whichCurrentCable);
    
    bool serialTrigger; //TO avoid ofxButton cause multiple click and send mutiple serial command;
    long prevSerialTriggerMillis; //TO avoid ofxButton cause multiple click and send mutiple serial command;
    long prevSingleCableLoopMillis;
    long currMillis;
    
    void displayLog(string s);
    string currentdisplayLog;
    void commandPrompt();
    
    bool isEmergencyStop;
    
    void loadButtonPressed();
    
    ofFbo kineticVisualizationFbo;
    bool drawKineticVisualizationFbo;
    vector<bool> drawDebugGui;
    bool drawPosGui;
    bool drawSpeedAccelGui;
    bool drawTimeGui;
    
    bool drawDmx;
    int num_of_online;
    long prevOnlineCheckingMillis;
    
    //================== Config ==================
    
    //int numOfCables;
    
    
    //================== Song 1 ==================
    
    void movement(int s);
    int songStage;
    ofParameter<int> movementMode;
    int prevSong;
    
    long currTime;
    long prevTime;
    bool setPattern;
    int timeDiff;
    
    void setPoints();
    
    
    //========== DMX Light ===========
    DmxLight DmxLight;
    
    
#ifdef USEOSC
    //================== OSC ==================
    //send
    ofxOscSender sender;
    void sendOSC(int arduino, string s);
    //read
    vector<string> readOSC();
    ofxOscReceiver receiver;
#else
    
#endif
    
    std::stringstream ss_info;
    
    
    //================ Show Control ==============
    void isShowBegin(bool sb);
    bool showBeginTrigger;
    long prevShowBeginMillis;

};


