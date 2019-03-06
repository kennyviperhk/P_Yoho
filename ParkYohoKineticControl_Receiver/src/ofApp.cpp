#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    
    debugMode = true;
    
    ofSetFrameRate(25);
    // listen on the given port
    
    cout << "listening for osc messages on port " << R_PORT << "\n";
    receiver.setup(R_PORT);
    
    // open an outgoing connection to HOST:PORT
    sender.setup(S_HOST, S_PORT);
    
    ofBackground(30, 30, 130);
    
    for(int i=0; i< NUM_OF_CABLES; i++){
        
        receivedOSCString.push_back("");
    }
    
    //Serial
    
    for(int i=0; i< NUM_OF_CABLES; i++){
        //working_cable.push_back(0);
        isArduinoConnected.push_back(0);
    }
    isArduinoConnected = serialSetup();
    for (int i = 0; i < isArduinoConnected.size(); i++) {
        ofLog() << "Arduino connected? : " << isArduinoConnected[i];
    }
    
    //init
    
    for(int i=0; i < NUM_OF_CABLES; i++){
        receivedString.push_back("");
        receivedStringBuffer.push_back("");
        isReceivedString.push_back(false);
        updateColor.push_back(255);
        displayText.push_back("");
    }
}



//--------------------------------------------------------------
void ofApp::update(){
    
    ofSetWindowTitle(ofToString(ofGetFrameRate()));
    /*
     if(initOnUpdate){
     checkArduinoIsConnected();
     }
     */
    readOSC();
    
    //================== Serial ==================
    for(int i=0; i < arduinoArranged.size(); i++){
        receivedStringBuffer[i] += ofTrim(serialRead(i));
        
        bool reachEnd = false;
        for (int j=0; j<receivedStringBuffer[i].size(); j++)
        {
            if (receivedStringBuffer[i][j] == '|'){
                receivedStringBuffer[i][j] = ' ';
                reachEnd = true;
            }
        }
        if(reachEnd){
            vector<string> array;
            stringstream ss(receivedStringBuffer[i]);
            string temp;
            while (ss >> temp)
                array.push_back(temp);
            if(array.size()>0){
                receivedString[i] = array[0];
                if(debugMode){
                    ofLog() << "receivedString[i] :" <<receivedString[i];
                }
                reachEnd = false;
                receivedStringBuffer[i] = "";
            }
        }
    }
    
    
    
    
    for(int i=0; i < NUM_OF_CABLES; i++){
        if(isReceivedString[i] && receivedString[i] != "" && receivedString[i] != " "){
            sendOSC(i, receivedString[i]);
            isReceivedString[i] = false;
            //prevReceivedString[i] = receivedString[i];
            displayText[i] = receivedString[i];
            // receivedString[i] = "";
            if(debugMode){
                ofLog() << "receivedString and Send: " << receivedString[i];
            }
            updateColor[i] = 255;
            
        }
        if(updateColor[i]>0){
            updateColor[i]--;
        }
        
        
        
        //}
    }
    
    
}

//--------------------------------------------------------------
void ofApp::draw(){
    
    ofBackground(0, 0, 100);
    ofSetColor(255);
    
    for(int i=0; i< NUM_OF_CABLES; i++){
        //ofSetColor(255);
        std::stringstream ss;
        
        
        ss << "<<< " + ofToString(i+1) + " : "+ receivedOSCString[i] << endl;
        ss << ">>>    : " << displayText[i] << endl;
        ss << " " << endl;
        ofSetColor(255,updateColor[i],updateColor[i]);
        if(i >= 16){
            ofDrawBitmapString(ss.str(),  ofVec2f(350, 30*(i-16) + 20));
        }else{
            ofDrawBitmapString(ss.str(),  ofVec2f(20, 30*i + 20));
        }
    }
    //ss << "FrameRate : "<< ofGetFrameRate() << endl;
}


//--------------------------------------------------------------
//--------------------------- OSC ------------------------------
//--------------------------------------------------------------

void ofApp::readOSC(){
    /*
     // hide old messages
     for(int i = 0; i < NUM_MSG_STRINGS; i++){
     if(timers[i] < ofGetElapsedTimef()){
     msg_strings[i] = "";
     }
     }
     */
    string OSCString = "";
    
    
    // check for waiting messages
    while(receiver.hasWaitingMessages()){
        // get the next message
        ofxOscMessage m;
        receiver.getNextMessage(m);
        
        // check for mouse moved message
        if(m.getAddress() == "/serial/in"){
            // the single argument is a string
            string mouseButtonState = m.getArgAsString(0);
            ofLog() << mouseButtonState;
            
            receivedOSCString[0] = mouseButtonState;
            
        }
        // check for an image being sent (note: the size of the image depends greatly on your network buffer sizes - if an image is too big the message won't come through )
        else{
            // unrecognized message: display on the bottom of the screen
            string msg_string;
            bool gotMsg = false;
            
            msg_string = m.getAddress();
            
            if(m.getAddress() == "/serial/"){
                
                ofLog() << "msg_string : " << msg_string;
                msg_string += ": ";
                for(int i = 0; i < m.getNumArgs(); i++){
                    // get the argument type
                    msg_string += m.getArgTypeName(i);
                    msg_string += ":";
                    // display the argument - make sure we get the right type
                    
                    ofLog() << "OFXOSC_TYPE_INT32 : " << ofToString(m.getArgType(i) == OFXOSC_TYPE_INT32);
                    
                    
                    
                    if((m.getArgType(i) == OFXOSC_TYPE_INT32) == 1){
                        ofLog() << "m.getArgAsInt32(i) : " << i << " : "<< m.getArgAsInt32(i);
                        if(m.getArgType(i) == OFXOSC_TYPE_INT32){
                            //msg_string += ofToString(m.getArgAsInt32(i));
                            currentArduinoID = m.getArgAsInt32(i);
                            ofLog() << "currentArduinoID_Int : " << currentArduinoID;
                            
                        }
                        else if(m.getArgType(i) == OFXOSC_TYPE_FLOAT){
                            msg_string += ofToString(m.getArgAsFloat(i));
                            ofLog() << "currentArduinoID_Float : " << currentArduinoID;
                        }
                        else if(m.getArgType(i) == OFXOSC_TYPE_STRING){
                            msg_string += m.getArgAsString(i);
                            
                           // OSCString = m.getArgAsString(i);
                            ofLog() << "currentArduinoID_String : " << currentArduinoID;
                            
                        }
                        else{
                            msg_string += "unknown";
                            ofLog() << "currentArduinoIDUN : " << currentArduinoID;
                        }
                       // if(debugMode){
                      //      ofLog() << "currentArduinoID : " << currentArduinoID<< " OSCString :  "<< OSCString;
                      //  }
                        
                        /*
                        // if(gotMsg){
                        
                        
                        receivedOSCString[currentArduinoID] = OSCString;
                        
                        serialWrite(currentArduinoID, OSCString);
                        ofLog() << " >>> FINAL SENDING : "<<"currentArduinoID : " << currentArduinoID<< " OSCString :  "<< OSCString;
                        
                        gotMsg = false;
                        //    }
                        */
                        
                        
                    }
                    
                    
                    
                    
                    
                    
                    
                    
                    
                    
                    
                    if((m.getArgType(i) == OFXOSC_TYPE_INT32) == 0){
                        ofLog() << "m.getArgAsInt32(i) : " << i << " : "<< m.getArgAsInt32(i);
                        if(m.getArgType(i) == OFXOSC_TYPE_INT32){
                            //msg_string += ofToString(m.getArgAsInt32(i));
                         //   currentArduinoID = m.getArgAsInt32(i);
                         //   ofLog() << "currentArduinoID_Int : " << currentArduinoID;
                            
                        }
                        else if(m.getArgType(i) == OFXOSC_TYPE_FLOAT){
                            msg_string += ofToString(m.getArgAsFloat(i));
                          //  ofLog() << "currentArduinoID_Float : " << currentArduinoID;
                        }
                        else if(m.getArgType(i) == OFXOSC_TYPE_STRING){
                            msg_string += m.getArgAsString(i);
                            
                            OSCString = m.getArgAsString(i);
                            ofLog() << "currentArduinoID_String : " << currentArduinoID;
                            
                        }
                        else{
                            msg_string += "unknown";
                          //  ofLog() << "currentArduinoIDUN : " << currentArduinoID;
                        }
                        if(debugMode){
                            ofLog() << "currentArduinoID : " << currentArduinoID<< " OSCString :  "<< OSCString;
                        }
                       // if(gotMsg){
                            receivedOSCString[currentArduinoID] = OSCString;
                            
                            serialWrite(currentArduinoID, OSCString);
                            ofLog() << " >>> FINAL SENDING : "<<"currentArduinoID : " << currentArduinoID<< " OSCString :  "<< OSCString;
                            
                            gotMsg = false;
                    //    }
                        
                        
                        
                    }
                   
                }
                
            }
            
            
            
            // add to the list of strings to display
            //       msg_strings[current_msg_string] = msg_string;
            //     timers[current_msg_string] = ofGetElapsedTimef() + 5.0f;
            //   current_msg_string = (current_msg_string + 1) % NUM_MSG_STRINGS;
            // clear the next line
            // msg_strings[current_msg_string] = "";
        }
        
    }
    
    
}

//--------------------------------------------------------------
void ofApp::sendOSC(int ar, string s){
    
    
    if(ar == -1){
        for(int i=0; i < NUM_OF_CABLES; i++){
            ofxOscMessage m;
            m.setAddress("/serial/");
            m.addIntArg(i);
            m.addStringArg(s);
            sender.sendMessage(m, false);
        }
    }else{
        ofxOscMessage m;
        m.setAddress("/serial/");
        m.addIntArg(ar);
        m.addStringArg(s);
        sender.sendMessage(m, false);
    }
    if(debugMode) {
        ofLog() << "sending OSC : " << ar << " : " << s;
    }
}





//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    
    if( key == 'a' || key == 'A'){
        sendOSC(ofRandom(20), "online-|");
    }
    if( key == 'c' || key == 'C'){
        serialWrite(-1, "C");
    }
    if( key == 'd' || key == 'd'){
        debugMode = !debugMode;
        ofLog() << " debugMode: " << debugMode;
    }
    /*
     if (key == 'd' || key == 'D') {
     serialWrite(-1, "");
     }
     */
    
}

//--------------------------------------------------------------
//-----------------SERIAL COMMUNICATION ------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------

//--------------------------------------------------------------
void ofApp::checkArduinoIsConnected(){
    
    if(ofGetFrameNum() < 200){
        
        if(300 %10 == 0){
            // for(int i=0; i< arduinoArranged.size(); i++){
            // serialWrite(i, "C");
            //      ofSleepMillis(100);
            //  }
            serialWrite(-1, "C");
            ofLog() << "hello";
        }
    }
}

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
            string portID = devicesInfo[i].getHardwareId();
            ofLog() << "devicesInfo[i].getDescription() : " << devicesInfo[i].getDescription();
            ofLog() << "devicesInfo[i].getHardwareId() : " << devicesInfo[i].getHardwareId();
            
            
#ifdef _WIN32
            
            if(portID.find("FTDI") != std::string::npos || portDesc.find("Arduino") != std::string::npos )
            {
                // Connect to the first matching device.
                ofx::IO::BufferedSerialDevice device;
                arduino.push_back(device);
                //string port = devicesInfo[i].getPort();
                
                //myPort= new SerialPort("\\\\.\\COM11",9600);
                
                
                bool success = arduino[a].setup("\\\\.\\" + devicesInfo[i].getPort(), BAUD);
                //define something for Windows (32-bit and 64-bit, this part is common)
#ifdef _WIN64
                //define something for Windows (64-bit only)
#else
                //define something for Windows (32-bit only)
#endif
#elif __APPLE__
#include "TargetConditionals.h"
#if TARGET_IPHONE_SIMULATOR
                // iOS Simulator
#elif TARGET_OS_IPHONE
                // iOS device
#elif TARGET_OS_MAC
                
                if(portID.find("FTDI") != std::string::npos || portDesc.find("Arduino") != std::string::npos || portID.find("USB") != std::string::npos )
                {
                    // Connect to the first matching device.
                    ofx::IO::BufferedSerialDevice device;
                    arduino.push_back(device);
                    //string port = devicesInfo[i].getPort();
                    
                    //myPort= new SerialPort("\\\\.\\COM11",9600);
                    
                    
                    bool success = arduino[a].setup(devicesInfo[i], BAUD);
                    // Other kinds of Mac OS
#else
#   error "Unknown Apple platform"
#endif
#elif __linux__
                    // linux
                    if(portID.find("FTDI") != std::string::npos || portDesc.find("Arduino") != std::string::npos || portID.find("USB") != std::string::npos )
                    {
                        // Connect to the first matching device.
                        ofx::IO::BufferedSerialDevice device;
                        arduino.push_back(device);
                        //string port = devicesInfo[i].getPort();
                        
                        //myPort= new SerialPort("\\\\.\\COM11",9600);
                        
                        
                        bool success = arduino[a].setup(devicesInfo[i], BAUD);
                        // Other kinds of Mac OS
#elif __unix__ // all unices not caught above
                        // Unix
#elif defined(_POSIX_VERSION)
                        // POSIX
#else
#   error "Unknown compiler"
#endif
                        
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
            
            
            
            
            // for(int i=0; i<devicesInfo.size(); i++){
            // arduinoArranged.push_back(i);
            // }
            if(arduino.size() >= NUM_OF_CABLES){
                arduinoArranged.push_back(arduino[0]); //1
                arduinoArranged.push_back(arduino[1]); //2
                arduinoArranged.push_back(arduino[12]); //3
                arduinoArranged.push_back(arduino[23]); //4
                arduinoArranged.push_back(arduino[26]); //5
                arduinoArranged.push_back(arduino[27]); //6
                arduinoArranged.push_back(arduino[28]); //7
                arduinoArranged.push_back(arduino[29]); //8
                arduinoArranged.push_back(arduino[30]); //9
                arduinoArranged.push_back(arduino[31]); //10
                arduinoArranged.push_back(arduino[2]); //11
                arduinoArranged.push_back(arduino[3]); //12
                arduinoArranged.push_back(arduino[4]); //13
                arduinoArranged.push_back(arduino[5]); //14
                arduinoArranged.push_back(arduino[6]); //15
                arduinoArranged.push_back(arduino[7]); //16
                arduinoArranged.push_back(arduino[13]); //17
                arduinoArranged.push_back(arduino[14]); //18
                arduinoArranged.push_back(arduino[15]); //19
                arduinoArranged.push_back(arduino[16]); //20
                
                arduinoArranged.push_back(arduino[7]); //21
                arduinoArranged.push_back(arduino[8]); //22
                arduinoArranged.push_back(arduino[9]); //23
                arduinoArranged.push_back(arduino[10]); //24
                arduinoArranged.push_back(arduino[17]); //25
                arduinoArranged.push_back(arduino[18]); //26
                arduinoArranged.push_back(arduino[19]); //27
                arduinoArranged.push_back(arduino[20]); //28
                arduinoArranged.push_back(arduino[21]); //29
                arduinoArranged.push_back(arduino[22]); //30
                arduinoArranged.push_back(arduino[24]); //31
                arduinoArranged.push_back(arduino[25]); //32
            }
            /*for(int i = 20; i < NUM_OF_SERIAL_TO_INIT; i++){
             arduinoArranged.push_back(300+i);
             }*/
            
            return connectionStatus;
            
        }
        
        void ofApp::serialWrite(int arduinoID, string sw){
            if(arduinoID == -1 && sw != ""){
                int arraySize = 0;
                if(arduinoArranged.size() <= NUM_OF_CABLES){
                    arraySize = arduinoArranged.size();
                }else{
                    arraySize = NUM_OF_CABLES;
                }
                for(int i=0; i< arraySize; i++){
                    
                    //    for(int i=0; i< arduinoArranged.size(); i++){
                    if(isArduinoConnected[i]){
                        // The serial device can throw exceptions.
                        try
                        {
                            std::string text = sw;
                            
                            
                            ofx::IO::ByteBuffer textBuffer(text);
                            
                            arduinoArranged[i].writeBytes(textBuffer);
                            arduinoArranged[i].writeByte('\n');
                        }
                        catch (const std::exception& exc)
                        {
                            ofLogError("ofApp::update") << exc.what();
                        }
                        if(debugMode) {
                            ofLog() << ">>> Arduino Write: " <<arduinoID <<  ":  "<<sw;
                        }
                    }
                }
            }
            else if (isArduinoConnected[arduinoID]==TRUE && arduinoID >= 0  && sw!="")
            {
                
                // The serial device can throw exeptions.
                try
                {
                    std::string text = sw;
                    
                    //sendOSC(arduinoID, sw);
                    
                    ofx::IO::ByteBuffer textBuffer(text);
                    
                    arduinoArranged[arduinoID].writeBytes(textBuffer);
                    arduinoArranged[arduinoID].writeByte('\n');
                }
                catch (const std::exception& exc)
                {
                    ofLogError("ofApp::update") << exc.what();
                }
                
                
            }
            else{ ofLog() << "Arduino: " <<arduinoID << "not connected";} // todo put in gui
            if(debugMode) {
                ofLog() << "Arduino Write: " <<arduinoID <<  ":  "<<sw;
            }
        }
        
        //--------------------------------------------------------------
        
        string ofApp::serialRead(int a){
            
            string combinedStr = "";
            // for(int i=0; i< arduinoArranged.size(); i++){
            
            // The serial device can throw exeptions.
            
            try
            {
                // Read all bytes from the device;
                uint8_t buffer[1024];
                vector<uint8_t> finalBuffer;
                finalBuffer.clear();
                while (arduinoArranged[a].available() > 0)
                {
                    std::size_t sz = arduinoArranged[a].readBytes(buffer, 1024);
                    //ofLog() << "buffer size: " << sz;
                    for (std::size_t j = 0; j < sz; ++j)
                    {
                        // std::cout << buffer[j];
                        //ofLog() << "buf: " << buffer[j];
                        if(isalnum(buffer[j]) || buffer[j] == '|' || buffer[j] == '-' ){
                            finalBuffer.push_back(buffer[j]);
                        }
                        
                        
                    }
                    for(int i = 0; i< finalBuffer.size();i++){
                        //  ofLog() << "New Buf : " << finalBuffer[i];
                        combinedStr += ofToString(finalBuffer[i]);
                        isReceivedString[a] = true;
                    }
                }
                
            }
            catch (const std::exception& exc)
            {
                ofLogError("ofApp::update") << exc.what();
            }
            //  }
            
            
            return combinedStr;
        }
        
        //--------------------------------------------------------------
        /*
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
         if(sToIntArray.size() == EEPROM.size()+1){
         currentdisplayLog = ofToString(currentDebugArduinoID) +" EEPROM LOADED";
         return sToIntArray;
         }
         else{
         // vector<int> sToIntArray;
         return sToIntArray;
         }
         
         }
         */
        
        
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
        
        //--------------------------------------------------------------
        //---------------------- OTHER EVENTS --------------------------
        //--------------------------------------------------------------
        //--------------------------------------------------------------
        void ofApp::exit()
        {
            serialWrite(-1, "V");
            for(int i=0; i< arduinoArranged.size(); i++){
                arduinoArranged[i].unregisterAllEvents(this);
            }
        }
        
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

