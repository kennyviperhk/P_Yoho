#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    
    ofSetFrameRate(60);
    
    //================== Serial ==================
    
    isArduinoConnected = serialSetup();
    
    SERIAL_PARAMETERES = {"save", "load","online"};
    serialTrigger = true;
    prevSerialTriggerMillis = ofGetElapsedTimeMillis();
    
    for(int i=0; i < NUM_OF_CABLES; i++){
        isArduinoConnectedBySerial.push_back(false);
    }
     initOnUpdate = true;
    //================== debugMode ==================
    
    debugMode = true;
    isEmergencyStop = false;
    for(int i=0; i < arduino.size(); i++){
        receivedString.push_back("");
        prevReceivedString.push_back("");
        updateColor.push_back(255);
    }
    
    currentdisplayLog = "Park Yoho Control Software V1";
    
    guiSetup();
    
    //================== Music Player ==================
    
    musicPlayer.setup();
    
}
//--------------------------------------------------------------
void ofApp::checkArduinoIsConnected(){

    if(currMillis < 2000){

        if(currMillis %5 == 0){
            serialWrite(-1, "C");
            ofLog() << "hello";
        }
    }
}
//--------------------------------------------------------------
void ofApp::update(){
    
    ofSetWindowTitle(ofToString(ofGetFrameRate()));
    
    
    currMillis = ofGetElapsedTimeMillis();

    
    //================== Serial ==================
    receivedString = serialRead();

    
    if(initOnUpdate){
        checkArduinoIsConnected();
    }
    
    for(int i=0; i< receivedString.size(); i++){
        if(stringDecode(receivedString[i]).size()>=1){
            if(stringDecode(receivedString[i])[0] == 1){ //load
                for(int j=0; j< stringDecode(receivedString[i]).size(); j++){
                    if(j< EEPROM.size()){
                        EEPROM[j] = stringDecode(receivedString[i])[j+1];
                    }
                }
            }
            
            if(stringDecode(receivedString[i])[0] == 2){ //online
                ofLog() << "device : " << i << " online";
                isArduinoConnectedBySerial[i] = true;
            }
            
        }
    }
    
    for(int i=0; i< arduino.size(); i++){
       // ofLog() << "receivedString : "<< i << " : "<<receivedString[i];
        if(receivedString[i].size() > 0){
            prevReceivedString[i] = receivedString[i];
            updateColor[i] = 255;
        }
        if(updateColor[i]>0){
            updateColor[i]--;
        }
       // ofLog() << "prevReceivedString : "<< i << " : "<<prevReceivedString[i];
        //ofLog() << "updateColor : "<< i << " : "<<updateColor[i];
        
    }
    //================== Kinectic Visualisation ==================
    
    for(int i=0; i< cablePos.size(); i++){
        kinecticVisualisation.set(i, currentStyle ,ofMap(cablePos[i]->x,0,1000,0,1) ,ofMap(cablePos[i]->y,0,1000,0,1),ofMap(cablePos[i]->z,0,1000,0,1), ofMap(cablePos[i]->w,0,1000,0,1));
    }

    
    //================== Music Player ==================
    musicPlayer.update();
}

//--------------------------------------------------------------
void ofApp::draw(){
    if(isEmergencyStop){
        ofBackground(255, 0, 0);
        ofSetColor(255);
        
        std::stringstream ss;
        ss << "FrameRate : "<< ofGetFrameRate() << endl;
        ss << "EMERGENCY STOP - 'r' to relase: " << endl;

        ofDrawBitmapString(ss.str(), ofVec2f(20, 20));
    }
    //================== Debug Mode ==================
    else if(debugMode){
        ofBackground(100, 0, 0);
        
        ofSetColor(255);
        
        std::stringstream ss;
        ss << "debugMode : "<< debugMode << endl;
        ss << "FrameRate : "<< ofGetFrameRate() << endl;
        
        ss << "Num of Connected Devices: " << arduino.size() << " / " << NUM_OF_CABLES << endl;
        //ss << "Style: " << currentStyle << endl;
        
        ofDrawBitmapString(ss.str(), ofVec2f(20, 20));
        
        
        for(int i=0; i < NUM_OF_CABLES; i++){
            std::stringstream ss2;
           // if(isArduinoConnected[i]){
            if(isArduinoConnectedBySerial[i]){
                ofSetColor(0,updateColor[i],updateColor[i]);
                ss2 << "Connected Devices (" << i << ") : " << arduino[i].getPortName() << "receivedMsg: " << prevReceivedString[i] << endl;
            }else{
                ofSetColor(0,0,0);
                ss2 << "Connected Devices (" << i << ") : " << "Disconnected" << endl;
            }
            ofDrawBitmapString(ss2.str(), ofVec2f(20, 25*i + 100));
            
            
        }

        if(serialTrigger){
            if(EEPROM_saveBtn){

                string toWrite = "";
                
                for(int i=0; i< EEPROM.size(); i++)
                {
                    toWrite+= ofToString(EEPROM[i]);
                    if(i!=EEPROM.size()-1){
                    toWrite+= "-";
                    }
                }
                ofLog() << "toWrite : "<< toWrite ;
                serialWrite(currentDebugArduinoID, toWrite);
                serialWrite(currentDebugArduinoID, "S");
                
                currentdisplayLog = ofToString(currentDebugArduinoID) +" EEPROM SAVED";
                serialTrigger = false;
                prevSerialTriggerMillis =currMillis;
                
                
            }
            if(EEPROM_loadBtn){

                serialWrite(currentDebugArduinoID, "L");
                serialTrigger = false;
                prevSerialTriggerMillis =currMillis;
            }
            if(style_Btn){
                
                if(currentStyle == 11){
                    string writeInTotal = "LX : ";
                    
                    string toWrite = "";
                    
                    toWrite+= ofToString(currentStyle);
                    toWrite+= "-";
                    
                    toWrite+= ofToString((int)cableSpeed[currentDebugArduinoID]->x);
                    toWrite+= "-";
                    toWrite+= ofToString((int)cableAccel[currentDebugArduinoID]->x);
                    toWrite+= "-";
                    toWrite+= ofToString((int)cablePos[currentDebugArduinoID]->x);
                    toWrite+= "-";
                    
                    writeInTotal=toWrite + " LY:";
                
                    toWrite+= ofToString((int)cableSpeed[currentDebugArduinoID]->y);
                    toWrite+= "-";
                    toWrite+= ofToString((int)cableAccel[currentDebugArduinoID]->y);
                    toWrite+= "-";
                    toWrite+= ofToString((int)cablePos[currentDebugArduinoID]->y);
                    toWrite+= "-";

                    writeInTotal=toWrite +" RX: ";
                    
                    toWrite+= ofToString((int)cableSpeed[currentDebugArduinoID]->z);
                    toWrite+= "-";
                    toWrite+= ofToString((int)cableAccel[currentDebugArduinoID]->z);
                    toWrite+= "-";
                    toWrite+= ofToString((int)cablePos[currentDebugArduinoID]->z);
                    toWrite+= "-";
                    
                    writeInTotal=toWrite +" RY: ";;

                    toWrite+= ofToString((int)cableSpeed[currentDebugArduinoID]->w);
                    toWrite+= "-";
                    toWrite+= ofToString((int)cableAccel[currentDebugArduinoID]->w);
                    toWrite+= "-";
                    toWrite+= ofToString((int)cablePos[currentDebugArduinoID]->w);
                    
                    writeInTotal=toWrite;
                    
                    serialWrite(currentDebugArduinoID, toWrite);
                    currentdisplayLog = writeInTotal;
                
                }
                
            

            }
            
        }
        if(currMillis -  prevSerialTriggerMillis > 200){
            serialTrigger = true;
        }

        
        string t = textField;
        if (t.find('+') != std::string::npos || t.find('=') != std::string::npos)
        {
            t.erase(std::remove(t.begin(), t.end(), '='), t.end());
            t.erase(std::remove(t.begin(), t.end(), '+'), t.end());
            currentdisplayLog = t;
            serialWrite(currentDebugArduinoID, t);
            textField = "";
        }
        
        guiDebug.draw();
        guiCablePos.draw();
        guiCableAccel.draw();
        guiCableSpeed.draw();
        displayLog(currentdisplayLog);

    }else{
        ofBackground(100, 100, 100);
        
    }
    //================== Kinectic Visualisation ==================
    
    kinecticVisualisation.draw();
    
    //================== Music Player ==================
    musicPlayer.draw();

}


//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    switch (key){
        case 's':
            currentStyle++;
            if(currentStyle >NUM_OF_CABLES){ //todo
                currentStyle=0;
            }
            break;
            
        case 'd':
            debugMode = !debugMode;
            break;
            
        case 'c': //check if arduino online
            serialWrite(-1, "C");
            break;
        
        case 'p': //dialog for serial writing
            commandPrompt();
            break;
            
        case ' ': //emergency Stop
            serialWrite(-1, "E");
            isEmergencyStop = true;
            break;
            
        case 'r': //emergency Stop release
            serialWrite(-1, "R");
            isEmergencyStop = false;
            break;

        
        default:
            break;
            
    }
}

bool ofApp::is_number(const std::string& s)
{
    std::string::const_iterator it = s.begin();
    while (it != s.end() && std::isdigit(*it)) ++it;
    return !s.empty() && it == s.end();
}

void ofApp::commandPrompt(){
    string txt = ofSystemTextBoxDialog("Serial Command", txt);
    serialWrite(currentDebugArduinoID, txt);

}
//--------------------------------------------------------------
//-------------------------- GUI -------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
void ofApp::guiSetup(){
    
    //--- EEPROM ---
    parametersDebug.setName("settings");
    guiDebug.setup("EEPROMReadWrite", "settings.xml", ofGetWidth() - 800, 0);
    
    vector<string> EEPROM_names = {"HOME_MAXSPEED", "HOME_ACCELERATION", "MAX_SPEED_X", "MAX_ACCELERATION_X", "MAX_SPEED_Y","MAX_ACCELERATION_Y","MAX_POSITION_LX","MAX_POSITION_LY","MAX_POSITION_RX","MAX_POSITION_RY","INVERT_DIR_LX","INVERT_DIR_LY","INVERT_DIR_RX","INVERT_DIR_RY"};

    
    vector<int> EEPROM_min = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    vector<int> EEPROM_max = {500, 500, 1000, 5000, 1000,5000, 1000, 1000, 1000, 1000,1, 1, 1,1}; //Todo Transfer definition /variables to xml
    
    guiDebug.add(currentDebugArduinoID.set("ArduinoID",0,0,NUM_OF_CABLES));

    for(int i=0; i< EEPROM_names.size(); i++){
        ofParameter<int> a;
        a.set(EEPROM_names[i],0,EEPROM_min[i],EEPROM_max[i]);
        EEPROM.push_back(a);
        guiDebug.add(EEPROM[i]);
    }
    vector<string> EEPROM_saveLoad_names = {"SAVE", "LOAD"};
    /* EEPROM_btn = {new ofxButton , new ofxButton}; //TODO problem with implementing ofxButton array
     for(int i=0; i< 2; i++){
     //  ofxButton a;
     //  a.setup(EEPROM_saveLoad_names[i]);
     //  EEPROM_btn.push_back(a);
     guiDebug.add(EEPROM_btn[i].setup("SAVE"+ofToString(i),50,50));
     }
     
     */
    guiDebug.add(EEPROM_saveBtn.setup(EEPROM_saveLoad_names[0]));
    guiDebug.add(EEPROM_loadBtn.setup(EEPROM_saveLoad_names[1]));
    guiDebug.add(textField.setup("Serial:", "0-0-0-0-0"));
    guiDebug.add(currentStyle.set("Style",11,0,NUM_OF_CABLES)); //TODO
    guiDebug.add(style_Btn.setup("Set Position:"));
    //textField.addListener(this,&ofApp::serialTextInput);
    //--- Cable Position Control ---
    
    parametersCablePos.setName("cablePosition");
    guiCablePos.setup("EEPROMReadWrite", "settings.xml", ofGetWidth() - 200, 0);
    
    for(int i=0; i< NUM_OF_CABLES; i++){
        ofParameter<ofVec4f> a;
        a.set("Pos " + ofToString(i),ofVec4f(0,0,0,0),ofVec4f(0,0,0,0),ofVec4f(1000,1000,1000,1000)); //lx,ly,rx,ry
        cablePos.push_back(a);
        guiCablePos.add(cablePos[i]);
    }
    //--- Cable Accel Control ---
    
    parametersCableAccel.setName("cableAccel");
    guiCableAccel.setup("EEPROMReadWrite", "settings.xml", ofGetWidth() - 400, 0);
    
    for(int i=0; i< NUM_OF_CABLES; i++){
        ofParameter<ofVec4f> a;
        a.set("Accel " + ofToString(i),ofVec4f(0,0,0,0),ofVec4f(0,0,0,0),ofVec4f(1000,1000,1000,1000)); //lx,ly,rx,ry
        cableAccel.push_back(a);
        guiCableAccel.add(cableAccel[i]);
    }
    //--- Cable Speed Control ---
    
    parametersCableSpeed.setName("cableSpeed");
    guiCableSpeed.setup("EEPROMReadWrite", "settings.xml", ofGetWidth() - 600, 0);
    
    for(int i=0; i< NUM_OF_CABLES; i++){
        ofParameter<ofVec4f> a;
        a.set("Speed " + ofToString(i),ofVec4f(0,0,0,0),ofVec4f(0,0,0,0),ofVec4f(1000,1000,1000,1000)); //lx,ly,rx,ry
        cableSpeed.push_back(a);
        guiCableSpeed.add(cableSpeed[i]);
    }
    
}


void ofApp::displayLog(string s=""){
    if(s.size() >0){
        currentdisplayLog = s ;
    }
    ofSetColor(255, 255, 255);
    ofDrawBitmapString("Status: " + currentdisplayLog, 10, ofGetHeight()-10);
}

//--------------------------------------------------------------
//-----------------SERIAL COMMUNICATION ------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------

vector<bool> ofApp::serialSetup(){ //int give the connection status of each cables
    
    vector<bool> connectionStatus;
    
    for(int i=0; i< NUM_OF_CABLES; i++){
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
            
            if(portDesc.find("USB") != std::string::npos || portDesc.find("Arduino") != std::string::npos )
            {
                // Connect to the first matching device.
                ofx::IO::BufferedSerialDevice device;
                arduino.push_back(device);
                
                bool success = arduino[a].setup(devicesInfo[i], BAUD);
                
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

void ofApp::serialWrite(int arduinoID, string sw){
    if(arduinoID == -1){
        for(int i=0; i< arduino.size(); i++){
            // The serial device can throw exeptions.
            try
            {
                std::string text = sw;
                
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
    else if (isArduinoConnected[arduinoID]==TRUE && arduinoID >= 0)
    {
        
        // The serial device can throw exeptions.
        try
        {
            std::string text = sw;
            
            ofx::IO::ByteBuffer textBuffer(text);
            
            arduino[arduinoID].writeBytes(textBuffer);
            arduino[arduinoID].writeByte('\n');
        }
        catch (const std::exception& exc)
        {
            ofLogError("ofApp::update") << exc.what();
        }
        
        
    }
    else{ ofLog() << "Arduino: " <<arduinoID << "not connected";} // todo put in gui
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

//--------------------------------------------------------------
vector<int> ofApp::stringDecode(string s){
    vector<int> sToIntArray;
    

    for (int i=0; i<s.length(); i++)
    {
        if (s[i] == '-')
            s[i] = ' ';
    }
    
    vector<string> seglist;
    stringstream ss(s);
    string temp;
    while (ss >> temp)
        seglist.push_back(temp);
    
    //ofLog() << "seglist.size() " << seglist.size();
    bool isContainParameter = false;
    
    for(int i=0; i < seglist.size(); i++){
        for(int j=0; j < SERIAL_PARAMETERES.size(); j++){
            if(seglist[i] == SERIAL_PARAMETERES[j]){ //check if anything match with SERIAL_PARAMETERS
                
                sToIntArray.push_back(j);
                isContainParameter= true;
            }
        }
        if(isContainParameter){
            if(i!=0 && is_number(seglist[i])){
                sToIntArray.push_back(std::stoi( seglist[i] ));
            }
        }
    }
    //LOAD
    for(int i=0; i < sToIntArray.size(); i++){
        ofLog() << "sToIntArray" << i << " : " << sToIntArray[i];
    }
    if(sToIntArray.size() == EEPROM.size()+1){
        currentdisplayLog = ofToString(currentDebugArduinoID) +" EEPROM LOADED";
        return sToIntArray;
    }
    else{
        vector<int> sToIntArray;
        return sToIntArray;
    }
    
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





