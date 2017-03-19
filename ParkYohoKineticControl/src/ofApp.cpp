#include "ofApp.h"

#define NUM_OF_CABLES 20 //Todo Transfer definition /variables to xml


//--------------------------------------------------------------
void ofApp::setup(){
    
    ofSetFrameRate(60);
    
    //================== Serial ==================
    
    isArduinoConnected = serialSetup();
    SERIAL_PARAMETERES = {"save", "load"};
    serialTrigger = true;
    prevSerialTriggerMills = ofGetElapsedTimeMillis();
    //================== debugMode ==================
    
    debugMode = true;
    for(int i=0; i < arduino.size(); i++){
        receivedString.push_back("");
        prevReceivedString.push_back("");
        updateColor.push_back(255);
    }
    
    currentdisplayLog = "Park Yoho Control Software V1";
    
    guiSetup();
    
}
//--------------------------------------------------------------
//-------------------------- GUI -------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
void ofApp::guiSetup(){
    
    //EEPROM
    parametersDebug.setName("settings");
    guiDebug.setup("EEPROMReadWrite", "settings.xml", ofGetWidth() - 400, 0);
    
    vector<string> EEPROM_names = {"HOME_MAXSPEED", "HOME_ACCELERATION", "MAX_SPEED", "ACCELERATION", "MAX_POSITION"};
    vector<int> EEPROM_min = {0, 0, 0, 0, 0};
    vector<int> EEPROM_max = {500, 500, 1000, 5000, 1000}; //Todo Transfer definition /variables to xml
    
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


    
    parametersCablePos.setName("cablePosition");
    guiCablePos.setup("EEPROMReadWrite", "settings.xml", ofGetWidth() - 200, 0);
    
    for(int i=0; i< NUM_OF_CABLES; i++){
        ofParameter<ofVec4f> a;
        a.set(ofToString(i),ofVec4f(10,10,10,10),ofVec4f(0,0,0,0),ofVec4f(100,100,100,100));
        
        cablePos.push_back(a);
        guiCablePos.add(cablePos[i]);
    }
    
}
//--------------------------------------------------------------
void ofApp::update(){
    
    ofSetWindowTitle(ofToString(ofGetFrameRate()));
    
    //================== Serial ==================
    receivedString = serialRead();
    
    for(int i=0; i< receivedString.size(); i++){
        if(stringDecode(receivedString[i]).size()){
            if(stringDecode(receivedString[i])[0] == 1){ //load
                for(int j=0; j< stringDecode(receivedString[i]).size(); j++){
                    if(j< EEPROM.size()){
                        EEPROM[j] = stringDecode(receivedString[i])[j+1];
                    }
                }
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
    
    float mx = ofMap(mouseX, 0, ofGetScreenWidth(),0,1);
    float my = ofMap(mouseY, 0, ofGetScreenHeight(),0,1);
    
    kinecticVisualisation.set(0, style ,mx,my,mx + 0.5, my+ 0.2);
}

//--------------------------------------------------------------
vector<int> ofApp::stringDecode(string s){
    vector<int> sToIntArray;

    /*
    std::stringstream input(s);
    string segment;
    vector<string> seglist;
    
    while(std::getline(input, segment, '-'))
    {
        seglist.push_back(segment);
        
    }
     */
    

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
      if(seglist[i] == SERIAL_PARAMETERES[j]){

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
    
    for(int i=0; i < sToIntArray.size(); i++){
        ofLog() << "sToIntArray" << i << " : " << sToIntArray[i];
    }
    ofLog() << "EEPROM" << " : " << EEPROM.size();
    ofLog() << "sToIntArray" << " : " << sToIntArray.size();
    if(sToIntArray.size() == EEPROM.size()+1){
        displayLog(ofToString(currentDebugArduinoID) +" EEPROM LOADED");
        return sToIntArray;
    }
    else{
    vector<int> sToIntArray;
        return sToIntArray;
    }
    
}
void ofApp::displayLog(string s=""){
    if(s.size() >0){
        currentdisplayLog = s ;
    }
    ofSetColor(255, 255, 255);
    ofDrawBitmapString("Status: " + currentdisplayLog, 10, ofGetHeight()-10);
}
bool ofApp::is_number(const std::string& s)
{
    std::string::const_iterator it = s.begin();
    while (it != s.end() && std::isdigit(*it)) ++it;
    return !s.empty() && it == s.end();
}
//--------------------------------------------------------------
void ofApp::draw(){
    //================== Debug Mode ==================
    if(debugMode){
        ofBackground(100, 0, 0);
        
        ofSetColor(255);
        
        std::stringstream ss;
        ss << "debugMode : "<< debugMode << endl;
        ss << "FrameRate : "<< ofGetFrameRate() << endl;
        
        ss << "Num of Connected Devices: " << arduino.size() << " / " << NUM_OF_CABLES << endl;
        ss << "Style: " << style << endl;
        
        ofDrawBitmapString(ss.str(), ofVec2f(20, 20));
        
        
        for(int i=0; i < NUM_OF_CABLES; i++){
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
                
                displayLog(ofToString(currentDebugArduinoID) +" EEPROM SAVED");
                serialTrigger = false;
                prevSerialTriggerMills =ofGetElapsedTimeMillis();
                
                
            }
            if(EEPROM_loadBtn){

                serialWrite(currentDebugArduinoID, "L");
                serialTrigger = false;
                prevSerialTriggerMills =ofGetElapsedTimeMillis();
            }
            
        }
        if(ofGetElapsedTimeMillis() -  prevSerialTriggerMills > 200){
            serialTrigger = true;
        }
        
        guiDebug.draw();
        guiCablePos.draw();
        displayLog();
        
 
       // ofLog() <<"guiDebug.mouseReleased(g) " <<
       // guiDebug.unregisterMouseEvents();
        //guiDebug.mouseReleased(g);
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
            serialWrite(-1, "D");
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





