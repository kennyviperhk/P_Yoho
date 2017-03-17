#pragma once

#include "ofMain.h"
#include "KineticController.h"

#include "ofxSerial.h"

/*
 notes: 
 
 //command: send motor index [0-4] - position [0-10000] - interpolation style [0-4?] - 3 style parameters ... (example: time to position [0 - 100000ms])

 //use ofserial for now (to avoid framedrop issue)
 
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
    
    int style;
    //================== Serial ==================
    
    vector<bool> serialSetup();
    vector<string> serialRead();
    vector<bool> isArduinoConnected;
    void serialWrite();

    
    vector<ofx::IO::BufferedSerialDevice> arduino;
    
    //unused
    //void sendChar();
    void onSerialBuffer(const ofx::IO::SerialBufferEventArgs& args);
    void onSerialError(const ofx::IO::SerialBufferErrorEventArgs& args);
    
    std::vector<SerialMessage> serialMessages;
    
    string receivedMsg;
    vector<string> prevReceivedString;
    vector<string> receivedString;
    vector<int> updateColor;
    
    //================== debugMode ==================
    
    bool debugMode;
    
    //================== Config ==================
    
    int numOfCables;
};

