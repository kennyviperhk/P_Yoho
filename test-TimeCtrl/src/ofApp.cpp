#include "ofApp.h"
//--------------------------------------------------------------
void ofApp::setup(){
    //------------------- Serial -------------------
        isArduinoConnected = serialSetup();
    
    setupGui();
    
   
    currStyle = 12;
    
    currCableID = 0;
    
    
    
    
    for(int i=0; i < NUM_OF_CABLES; i++){
        receivedString.push_back("");

    }
    
    
}


void ofApp::moveCommandMethod(int method, int c, int whichCurrentCable){
    string writeInTotal = "";
    string toWrite = "";
    if (method == 12){//moveTo with time
        //By Time   //STYLE - POS - TIME - POS - TIME - POS - TIME - POS - TIME
        writeInTotal = "LX : ";
        
        toWrite = "";
        
        toWrite+= ofToString(method);
        toWrite+= "-";
        
        toWrite+= ofToString((int)cablePosLx[c]);
        toWrite+= "-";
        toWrite+= ofToString((int)cableTimeLx[c]);
        toWrite+= "-";
        
        writeInTotal=toWrite + " LY:";
        
        toWrite+= ofToString((int)cablePosLy[c]);
        toWrite+= "-";
        toWrite+= ofToString((int)cableTimeLy[c]);
        toWrite+= "-";
        
        writeInTotal=toWrite +" RX: ";
        
        toWrite+= ofToString((int)cablePosRx[c]);
        toWrite+= "-";
        toWrite+= ofToString((int)cableTimeRx[c]);
        toWrite+= "-";
        
        writeInTotal=toWrite +" RY: ";;
        
        toWrite+= ofToString((int)cablePosRy[c]);
        toWrite+= "-";
        toWrite+= ofToString((int)cableTimeRy[c]);
        
    }
    
    writeInTotal=toWrite;
    
    
    if(whichCurrentCable == -1){
        serialWrite(-1, toWrite);
    }else {
        serialWrite(c, toWrite);
    }
}


void ofApp::writeStyle(int s){ //all same = 0, all diff = 1, specific = 2
    ofLog() << "write Style " << s  << " current arduino ID : " << currCableID;

    if (s==0){
        moveCommandMethod(currStyle, currCableID, -1);
    }
    else if (s ==1){
        for(int i=0; i< NUM_OF_CABLES; i++){
            moveCommandMethod(currStyle, i, i);
        }
        
    }else if (s ==2){
        if(currCableID <= NUM_OF_CABLES-1 && currCableID >= 0){
            moveCommandMethod(currStyle, currCableID, currCableID);
        }
    }
}

//--------------------------------------------------------------
//----------------- GUI ------------------------
//--------------------------------------------------------------

void ofApp::setupGui(){
    //--- Cable Position Control ---
    
    parametersCablePos.setName("cablePosition");
    guiCablePosLx.setup("EEPROMReadWrite", "settings.xml", ofGetWidth() - 200, 0);
    guiCablePosLy.setup("EEPROMReadWrite", "settings.xml", ofGetWidth() - 200, 400);
    guiCablePosRx.setup("EEPROMReadWrite", "settings.xml", ofGetWidth() - 100, 0);
    guiCablePosRy.setup("EEPROMReadWrite", "settings.xml", ofGetWidth() - 100, 400);
    for(int i=0; i< NUM_OF_CABLES; i++){
        ofParameter<int> a;
        a.set("P Lx" + ofToString(i+1),0,0,MAX_X_POS); //lx,ly,rx,ry
        ofParameter<int> b;
        b.set("P Ly" + ofToString(i+1),0,0,MAX_Y_POS);
        ofParameter<int> c;
        c.set("P Rx" + ofToString(i+1),0,0,MAX_X_POS);
        ofParameter<int> d;
        d.set("P Ry" + ofToString(i+1),0,0,MAX_Y_POS);
        cablePosLx.push_back(a);
        cablePosLy.push_back(b);
        cablePosRx.push_back(c);
        cablePosRy.push_back(d);
        guiCablePosLx.add(cablePosLx[i]);
        guiCablePosLy.add(cablePosLy[i]);
        guiCablePosRx.add(cablePosRx[i]);
        guiCablePosRy.add(cablePosRy[i]);
    }
    
    
    //--- Cable Time Control ---
    parametersCableTime.setName("cableTime");
    guiCableTimeLx.setup("EEPROMReadWrite", "settings.xml", ofGetWidth() - 410, 0);
    guiCableTimeLy.setup("EEPROMReadWrite", "settings.xml", ofGetWidth() - 410, 400);
    guiCableTimeRx.setup("EEPROMReadWrite", "settings.xml", ofGetWidth() - 300, 0);
    guiCableTimeRy.setup("EEPROMReadWrite", "settings.xml", ofGetWidth() - 300, 400);
    for(int i=0; i< NUM_OF_CABLES; i++){
        ofParameter<int> a;
        a.set("T" + ofToString(i+1),45000,10000,MAX_X_TIME); //lx,ly,rx,ry
        ofParameter<int> b;
        b.set("T" + ofToString(i+1),45000,10000,MAX_Y_TIME);
        ofParameter<int> c;
        c.set("T" + ofToString(i+1),45000,10000,MAX_X_TIME);
        ofParameter<int> d;
        d.set("T" + ofToString(i+1),45000,10000,MAX_Y_TIME);
        cableTimeLx.push_back(a);
        cableTimeLy.push_back(b);
        cableTimeRx.push_back(c);
        cableTimeRy.push_back(d);
        guiCableTimeLx.add(cableTimeLx[i]);
        guiCableTimeLy.add(cableTimeLy[i]);
        guiCableTimeRx.add(cableTimeRx[i]);
        guiCableTimeRy.add(cableTimeRy[i]);
    }
    
    
}

//--------------------------------------------------------------
//-----------------SERIAL COMMUNICATION ------------------------
//--------------------------------------------------------------

vector<bool> ofApp::serialSetup(){ //int give the connection status of each cables
    
    vector<bool> connectionStatus;
    
    for(int i=0; i< NUM_OF_CABLES; i++){
        connectionStatus.push_back(0);
    }
#ifdef USEOSC
    
    
#ifdef RECEIVER_IS_LINUX
    //USING 32 port and Windows, To connect arduino in sequence, we have to do it one by one
    
    vector<int> allComPort;
    
    for(int i=0; i<NUM_OF_SERIAL_TO_INIT; i++){
        allComPort.push_back(i);
    }
    arduino.push_back(allComPort[0]); //1
    arduino.push_back(allComPort[1]); //2
    arduino.push_back(allComPort[2]); //3
    arduino.push_back(allComPort[3]); //4
    arduino.push_back(allComPort[4]); //5
    arduino.push_back(allComPort[5]); //6
    arduino.push_back(allComPort[6]); //7
    arduino.push_back(allComPort[7]); //8
    arduino.push_back(allComPort[8]); //9
    arduino.push_back(allComPort[9]); //10
    arduino.push_back(allComPort[10]); //11
    arduino.push_back(allComPort[11]); //12
    arduino.push_back(allComPort[12]); //13
    arduino.push_back(allComPort[13]); //14
    arduino.push_back(allComPort[14]); //15
    arduino.push_back(allComPort[15]); //16
    arduino.push_back(allComPort[16]); //17
    arduino.push_back(allComPort[19]); //18
    arduino.push_back(allComPort[17]); //19
    arduino.push_back(allComPort[18]); //20
    
    for(int i = 20; i < NUM_OF_SERIAL_TO_INIT; i++){
        arduino.push_back(300+i);
    }
    
    
    for(int i=0; i< NUM_OF_CABLES; i++){
        ofLog() <<arduino[i];
        isArduinoConnected.push_back(true);
    }
#else
    
#endif
    
    
#ifdef RECEIVER_IS_WINDOWS
    //USING 32 port and Windows, To connect arduino in sequence, we have to do it one by one
    
    vector<int> allComPort;
    
    for(int i=0; i<NUM_OF_SERIAL_TO_INIT; i++){
        allComPort.push_back(i);
    }
    arduino.push_back(allComPort[17]); //1
    arduino.push_back(allComPort[21]); //2
    arduino.push_back(allComPort[2]); //3
    arduino.push_back(allComPort[3]); //4
    arduino.push_back(allComPort[5]); //5
    arduino.push_back(allComPort[20]); //6
    arduino.push_back(allComPort[27]); //7
    arduino.push_back(allComPort[6]); //8
    arduino.push_back(allComPort[15]); //9
    arduino.push_back(allComPort[16]); //10
    arduino.push_back(allComPort[19]); //11
    arduino.push_back(allComPort[25]); //12
    arduino.push_back(allComPort[11]); //13
    arduino.push_back(allComPort[12]); //14
    arduino.push_back(allComPort[13]); //15
    arduino.push_back(allComPort[14]); //16
    arduino.push_back(allComPort[31]); //17
    arduino.push_back(allComPort[1]); //18
    arduino.push_back(allComPort[4]); //19
    arduino.push_back(allComPort[18]); //20
    for(int i = 20; i < NUM_OF_SERIAL_TO_INIT; i++){
        arduino.push_back(300+i);
    }
    
    for(int i=0; i< NUM_OF_CABLES; i++){
        ofLog() <<arduino[i];
        isArduinoConnected.push_back(true);
    }
#else
    
    for(int i=0; i< NUM_OF_CABLES; i++){
        arduino.push_back(i);
        isArduinoConnected.push_back(true);
    }
    
#endif
    
#else
    
    std::vector<ofx::IO::SerialDeviceInfo> devicesInfo = ofx::IO::SerialDeviceUtils::listDevices();
    ofLogNotice("ofApp::setup") << "Arduino: ";
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
                    ofSleepMillis(100);
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
    
    
    
#endif
    return connectionStatus;
    
}

void ofApp::serialWrite(int arduinoID, string sw){
    
#ifdef USEOSC
    if(arduinoID == -1){
        
        for(int i=0; i< NUM_OF_CABLES; i++){
            sendOSC(arduino[i], sw);
            currDisplayLog[i] = sw;
        }
    }else if (arduinoID >= 0 && working_cable[arduinoID] && arduinoID<arduino.size()){
        sendOSC(arduino[arduinoID], sw);
        currDisplayLog[arduinoID] = sw;
    }
#else
    
    if(arduinoID == -1){
        
        int arraySize = 0;
        if(arduino.size() <= NUM_OF_CABLES){
            arraySize = arduino.size();
        }else{
            arraySize = NUM_OF_CABLES;
        }
        for(int i=0; i< arraySize; i++){
            //    for(int i=0; i< arduino.size(); i++){
       //     if(working_cable[i]){
                // The serial device can throw exceptions.
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
         //   }
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
    else{ ofLog() << "Arduino: " <<arduinoID << " >> OFFLINE ";} // todo put in gui
    
    ofLog() << "Arduino Write: " <<arduinoID <<  ":  "<<sw;
#endif
}

string ofApp::serialRead(int a){
    
    string combinedStr = "";
    // for(int i=0; i< arduino.size(); i++){
    
    // The serial device can throw exeptions.
#ifdef USEOSC
    
    
    
#else
    try
    {
        // Read all bytes from the device;
        uint8_t buffer[1024];
        vector<uint8_t> finalBuffer;
        finalBuffer.clear();
        while (arduino[a].available() > 0)
        {
            std::size_t sz = arduino[a].readBytes(buffer, 1024);
            ofLog() << "buffer size: " << sz;
            for (std::size_t j = 0; j < sz; ++j)
            {
                // std::cout << buffer[j];
                //ofLog() << "buf: " << buffer[j];
                if(isalnum(buffer[j]) || buffer[j] == '|' || buffer[j] == '-' ){
                    finalBuffer.push_back(buffer[j]);
                }
                
                
            }
            for(int i = 0; i< finalBuffer.size();i++){
                ofLog() << "New Buf : " << finalBuffer[i];
                combinedStr += ofToString(finalBuffer[i]);
            }
        }
        
    }
    catch (const std::exception& exc)
    {
        ofLogError("ofApp::update") << exc.what();
    }
    //  }
    
#endif
    return combinedStr;
}

//--------------------------------------------------------------

void ofApp::onSerialBuffer(const ofx::IO::SerialBufferEventArgs& args)
{
    // Buffers will show up here when the marker character is found.
    SerialMessage message(args.getBuffer().toString(), "", 500);
    serialMessages.push_back(message);
    ofLog() << "SERIALLLLLLLL : " << message.message;
}

//--------------------------------------------------------------

void ofApp::onSerialError(const ofx::IO::SerialBufferErrorEventArgs& args)
{
    // Errors and their corresponding buffer (if any) will show up here.
    SerialMessage message(args.getBuffer().toString(),
                          args.getException().displayText(),
                          500);
    serialMessages.push_back(message);
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
    /* for(int i=0; i<seglist.size(); i++){
     seglist[i].erase(seglist[i].find_last_not_of(" \n\r\t")+1);
     //  ofLog() << "seglist[i] : " << seglist[i];
     }*/
    /*
    bool isContainParameter = false;
    
    for(int i=0; i < seglist.size(); i++){
        for(int j=0; j < SERIAL_PARAMETERES.size(); j++){
            if(seglist[i] == SERIAL_PARAMETERES[j]){ //check if anything match with SERIAL_PARAMETERS
                ofLog() << "SERIAL_PARAMETERES[j]" << SERIAL_PARAMETERES[j];
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
        ofLog() << "sToIntArray " << i << " : " << sToIntArray[i];
    }

*/
        // vector<int> sToIntArray;
        return sToIntArray;
    
    
}

//--------------------------------------------------------------
void ofApp::update(){
    /*
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
    */
}



//--------------------------------------------------------------
//-------------------------- Utils -----------------------------
//--------------------------------------------------------------

bool ofApp::is_number(const std::string& s)
{
    std::string::const_iterator it = s.begin();
    while (it != s.end() && std::isdigit(*it)) ++it;
    return !s.empty() && it == s.end();
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofBackground(255);
    drawGui();
}


void ofApp::drawGui(){
    guiCablePosLx.draw();
    guiCablePosLy.draw();
    guiCablePosRx.draw();
    guiCablePosRy.draw();
    
    guiCableTimeLx.draw();
    guiCableTimeLy.draw();
    guiCableTimeRx.draw();
    guiCableTimeRy.draw();

}
//--------------------------------------------------------------
void ofApp::keyPressed(int key){

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
