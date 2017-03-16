#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    
    
    //================== Serial ==================
    
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
                deviceAccel.push_back(device);
                
                bool success = deviceAccel[a].setup(devicesInfo[i], 115200);
                
                if(success)
                {
                    deviceAccel[a].unregisterAllEvents(this);
                    deviceAccel[a].registerAllEvents(this);
                    
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
    
}


//--------------------------------------------------------------
void ofApp::update(){
    
    ofSetWindowTitle(ofToString(ofGetFrameRate()));
    //================== Serial ==================
    
    receivedVal = serialRead();
    float currAngle = receivedVal[1];
    
    for(int i=0; i< deviceAccel.size(); i++){
        // The serial device can throw exeptions.
        try
        {
            // Read all bytes from the device;
            uint8_t buffer[1024];
            
            while (deviceAccel[i].available() > 0)
            {
                std::size_t sz = deviceAccel[i].readBytes(buffer, 1024);
                
                for (std::size_t i = 0; i < sz; ++i)
                {
                    std::cout << buffer[i];
                }
            }
            
            // Send some new bytes to the device to have them echo'd back.
            std::string text = "Frame Number: " + ofToString(ofGetFrameRate());
            
            ofx::IO::ByteBuffer textBuffer(text);
            
            deviceAccel[i].writeBytes(textBuffer);
            deviceAccel[i].writeByte('\n');
        }
        catch (const std::exception& exc)
        {
            ofLogError("ofApp::update") << exc.what();
        }
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    kinecticVisualisation.draw();
    
}



//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    sendChar();
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    
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




//--------------------SERIAL COMMUNICATION -------------------------------

void ofApp::sendChar(){
    vector<uint8_t> hi;
    hi.push_back('D');
    ofx::IO::ByteBuffer buffer(hi);
    for(int i=0; i< deviceAccel.size(); i++){
        deviceAccel[i].writeByte(hi[0]);
    }
}


vector<float> ofApp::serialRead(){
    
    vector<float> currVal;
    std::vector<SerialMessage>::iterator iter = serialMessages.begin();
    
    receivedMsg = "";
    
    
    while (iter != serialMessages.end())
    {
        
        
        if (iter->fade < 0)
        {
            iter = serialMessages.erase(iter);  //may need this to maintain performance
        }
        else
        {
            
            /*
             ofSetColor(255, ofClamp(iter->fade, 0, 255));
             ofDrawBitmapString(iter->message, ofVec2f(x, y));
             
             y += height;
             
             */
            
            
            /*   if((iter->message).find('l') != std::string::npos){
             dir = "l";
             isNewDirData = true;
             }else if((iter->message).find('r') != std::string::npos){
             dir = "r";
             isNewDirData = true;
             }else if((iter->message).find('m') != std::string::npos){
             dir = "n";
             isNewDirData = false;
             // newDirCounter = ofGetElapsedTimef();
             }else{*/
            receivedMsg = iter->message;
            //  }
            
            
            
            if (!iter->exception.empty())
            {
                // y += height;
            }
            
            ++iter;
        }
    }
    
    
    for(int i =0 ; i< receivedMsg.size(); i++){
        ofLog() << "receivedMsg : "<< receivedMsg ;
        if(receivedMsg.find("ypr") != std::string::npos){
            receivedMsg = receivedMsg.erase(0,3);
            //  ofLog() << "1. receivedMsg.erase(0,3) " << receivedMsg;
            receivedMsg = receivedMsg.erase(0,1);
            // ofLog() << "2. receivedMsg.erase(0,1) " << receivedMsg;
        }else{
            sendChar();
        }
    }
    
    string s(receivedMsg);
    istringstream iss(s);
    
    do
    {
        string sub;
        iss >> sub;
        currVal.push_back(ofToFloat(sub));
        //cout << "Substring: " << sub << endl;
        
    } while (iss);
    
    
    return currVal;
    
}



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



