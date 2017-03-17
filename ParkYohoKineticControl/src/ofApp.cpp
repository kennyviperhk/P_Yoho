#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    
    ofSetFrameRate(60);
    numOfCables = 20;
    
    //================== Serial ==================
    
    isArduinoConnected = serialSetup();
    
    //================== debugMode ==================
    
    debugMode = true;
    for(int i=0; i < arduino.size(); i++){
        receivedString.push_back("");
        prevReceivedString.push_back("");
        updateColor.push_back(255);
    }

}
//--------------------------------------------------------------
void ofApp::update(){
    
    ofSetWindowTitle(ofToString(ofGetFrameRate()));
    
    //================== Serial ==================
        receivedString = serialRead();

    for(int i=0; i< arduino.size(); i++){
        ofLog() << "receivedString : "<< i << " : "<<receivedString[i];
        if(receivedString[i].size() > 0){
            prevReceivedString[i] = receivedString[i];
            updateColor[i] = 255;
        }
        if(updateColor[i]>0){
            updateColor[i]--;
        }
        ofLog() << "prevReceivedString : "<< i << " : "<<prevReceivedString[i];
        ofLog() << "updateColor : "<< i << " : "<<updateColor[i];
        
    }
    //================== Kinectic Visualisation ==================
    
    float mx = ofMap(mouseX, 0, ofGetScreenWidth(),0,1);
    float my = ofMap(mouseY, 0, ofGetScreenHeight(),0,1);
    
    kinecticVisualisation.set(0,0,mx,my,mx + 0.5, my+ 0.2);
}

//--------------------------------------------------------------
void ofApp::draw(){
    
    if(debugMode){
        ofBackground(100, 0, 0);
        
        ofSetColor(255);
        
        std::stringstream ss;
        ss << "debugMode : "<< debugMode << endl;
        ss << "FrameRate : "<< ofGetFrameRate() << endl;

        ss << "Num of Connected Devices: " << arduino.size() << " / " << numOfCables << endl;
         ofDrawBitmapString(ss.str(), ofVec2f(20, 20));
        

        for(int i=0; i < numOfCables; i++){
            std::stringstream ss2;
            if(isArduinoConnected[i]){
                ofSetColor(0,updateColor[i],updateColor[i]);
                ss2 << "Connected Devices (" << i << ") : " << arduino[i].getPortName() << "receivedMsg: " << prevReceivedString[i] << endl;
            }else{
                ofSetColor(0,0,0);
                ss2 << "Connected Devices (" << i << ") : " << "Disconnected" << endl;
            }
            ofDrawBitmapString(ss2.str(), ofVec2f(20, 25*i + 100));
        }

    }else{
        ofBackground(100, 100, 100);
    
    }
    //================== Kinectic Visualisation ==================
    
    kinecticVisualisation.draw();
}


//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    switch (key){
        case 's':
            style++;
            if(style >1){
                style=0;
            }
            break;
        
        case 'd':
            debugMode = !debugMode;
            break;
            
        case 'w': //write some char for testing
            serialWrite();
            break;

        default:
            break;
            
    }
}

//--------------------------------------------------------------
//-----------------SERIAL COMMUNICATION ------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------

vector<bool> ofApp::serialSetup(){ //int give the connection status of each cables
    
    vector<bool> connectionStatus;
    
    for(int i=0; i< numOfCables; i++){
        connectionStatus.push_back(0);
    }
    
    std::vector<ofx::IO::SerialDeviceInfo> devicesInfo = ofx::IO::SerialDeviceUtils::listDevices();
    ofLogNotice("ofApp::setup") << "Connected Devices: ";
    for (std::size_t i = 0; i < devicesInfo.size(); ++i)
    {
        ofLogNotice("ofApp::setup") << "\t" << devicesInfo[i];
    }
    int a=0;
    if (!devicesInfo.empty())
    {
        
        for (std::size_t i = 0; i < devicesInfo.size(); ++i)
        {
            string portDesc = devicesInfo[i].getDescription();
            ofLog() << "devicesInfo[i].getDescription() : " << devicesInfo[i].getDescription();
            
            if(portDesc.find("USB") != std::string::npos )
            {
                // Connect to the first matching device.
                ofx::IO::BufferedSerialDevice device;
                arduino.push_back(device);
                
                bool success = arduino[a].setup(devicesInfo[i], 115200);
                
                if(success)
                {
                    connectionStatus[a] = true;
                    arduino[a].unregisterAllEvents(this);
                    arduino[a].registerAllEvents(this);
                    
                    ofLogNotice("ofApp::setup") << "Successfully setup " << devicesInfo[i];
                    a++;
                }
                else
                {
                    ofLogNotice("ofApp::setup") << "Unable to setup " << devicesInfo[i];
                }
            }
        }
    }
    else
    {
        ofLogNotice("ofApp::setup") << "No devices connected.";
    }
    
    ofSleepMillis(1000); //Keep it - Arduino Mega needs time to initiate (Autoreset issue)
    
    return connectionStatus;
    
}

void ofApp::serialWrite(){
    for(int i=0; i< arduino.size(); i++){
        // The serial device can throw exeptions.
        try
        {
            std::string text = "Frame Number: " + ofToString(ofGetFrameRate());
            
            ofx::IO::ByteBuffer textBuffer(text);
            
            arduino[i].writeBytes(textBuffer);
            arduino[i].writeByte('\n');
        }
        catch (const std::exception& exc)
        {
            ofLogError("ofApp::update") << exc.what();
        }
    }
    
}

vector<string> ofApp::serialRead(){
    
    vector<string> output;
    for(int i=0; i< arduino.size(); i++){
        output.push_back("");
        // The serial device can throw exeptions.
        try
        {
            // Read all bytes from the device;
            uint8_t buffer[1024];
            
            while (arduino[i].available() > 0)
            {
                std::size_t sz = arduino[i].readBytes(buffer, 1024);
                
                for (std::size_t j = 0; j < sz; ++j)
                {
                    std::cout << buffer[j];
                }
                output[i] = ofToString(buffer);
            }
            
        }
        catch (const std::exception& exc)
        {
            ofLogError("ofApp::update") << exc.what();
        }
    }
    return output;
}

//TODO: Unused for now

/*
 void ofApp::sendChar(){
 vector<uint8_t> hi;
 hi.push_back('D');
 ofx::IO::ByteBuffer buffer(hi);
 for(int i=0; i< arduino.size(); i++){
 arduino[i].writeByte(hi[0]);
 }
 }
 */

void ofApp::onSerialBuffer(const ofx::IO::SerialBufferEventArgs& args)
{
    // Buffers will show up here when the marker character is found.
    SerialMessage message(args.getBuffer().toString(), "", 500);
    serialMessages.push_back(message);
}

void ofApp::onSerialError(const ofx::IO::SerialBufferErrorEventArgs& args)
{
    // Errors and their corresponding buffer (if any) will show up here.
    SerialMessage message(args.getBuffer().toString(),
                          args.getException().displayText(),
                          500);
    serialMessages.push_back(message);
}


//--------------------------------------------------------------
//--------------------------UNUSED -----------------------------
//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
    
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){
    
}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){
    
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){
    
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){
    
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){
    
}





