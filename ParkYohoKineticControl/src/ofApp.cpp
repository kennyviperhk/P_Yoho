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
    
    page = 0;
    numOfPages = 3;
    debugMode = true;
    isEmergencyStop = false;
    for(int i=0; i < arduino.size(); i++){
        receivedString.push_back("");
        receivedStringBuffer.push_back("");
        prevReceivedString.push_back("");
        updateColor.push_back(255);
    }
    
    currentdisplayLog = "Park Yoho Control Software V1";
    
    guiSetup();
    
    //================== Music Player ==================
    
    showMusicPlayer = false;
    musicPlayer.setup();
    
    //================== Movement Controls ==================
    int mcX = 10;
    int mcY = 200;
    int mcW = 500;
    int mcH = 200;
    int mcHg = 20;
    /*
     for(int i=0; i< 1 ; i++){
     MovementController mc;
     MovementControllers.push_back(mc);
     MovementControllers[i].setup(NUM_OF_CABLES,mcX,mcY+ (i*mcH)+ mcHg,mcW,mcH);
     }
     */
    MovementController.setup(NUM_OF_CABLES,mcX,mcY,mcW,mcH);
    showMovementController = false;
    
    
}


//--------------------------------------------------------------
void ofApp::update(){
    
    ofSetWindowTitle(ofToString(ofGetFrameRate()));
    
    
    currMillis = ofGetElapsedTimeMillis();
    
    
    //================== Serial ==================
    for(int i=0; i < arduino.size(); i++){
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
                ofLog() << "receivedString[i] :" <<receivedString[i];
                reachEnd = false;
                receivedStringBuffer[i] = "";
            }
        }
    }
    
    
    
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
            receivedString[i]="";
            updateColor[i] = 255;
        }
        if(updateColor[i]>0){
            updateColor[i]--;
        }
        // ofLog() << "prevReceivedString : "<< i << " : "<<prevReceivedString[i];
        //ofLog() << "updateColor : "<< i << " : "<<updateColor[i];
        
    }
    //================== Kinectic Visualisation ==================
    
    
    vector<ofPoint> mcPoints = MovementController.getPoints();
    
    
    for(int i=0; i< NUM_OF_CABLES; i++){
        
        kinecticVisualisation.set(NUM_OF_CABLES ,i, currentStyle ,ofMap(cablePosLx[i],0,1000,0,2) ,ofMap(cablePosLy[i],0,1000,0,1),ofMap(cablePosRx[i],0,1000,0,2), ofMap(cablePosRy[i],0,1000,0,1));
        
    }
    
    
    kineticVisualizationFbo.begin();
    ofClear(255, 0, 0,0);
    kinecticVisualisation.draw();
    kineticVisualizationFbo.end();
    
    
    //================== Music Player ==================
    musicPlayer.update();
    
    //================== Movement Controls ==================
    /*
     for(int i=0; i< MovementControllers.size(); i++){
     MovementControllers[i].update();
     }
     */
    MovementController.update();
    
}

//--------------------------------------------------------------
void ofApp::draw(){
    if(isEmergencyStop){
        ofBackground(255, 0, 0);
        ofSetColor(255);
        
        std::stringstream ss;
        ss << "FrameRate : "<< ofGetFrameRate() << endl;
        ss << "EMERGENCY STOP - 'r' to relase: " << endl;
        ss << "EMERGENCY STOP - 'q' to reset all arduino: " << endl;
        
        ofDrawBitmapString(ss.str(), ofVec2f(20, 20));
    }
    //================== Debug Mode ==================
    else if(debugMode){
        ofBackground(100, 0, 0);
        
        ofSetColor(255);
        
        
        
        if(drawKineticVisualizationFbo){
            kineticVisualizationFbo.draw(0,200,ofGetWidth()*0.7,ofGetHeight()*0.7);
        }
        
        //================== Debug Gui ==================
        
        std::stringstream ss;
        ss << "debugMode : "<< debugMode << endl;
        ss << "FrameRate : "<< ofGetFrameRate() << endl;
        ss << "Page : "<< page << endl;
        
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
            if(home_Btn){
                serialWrite(currentDebugArduinoID, "H");
                //manual home "G"
                //serialWrite(currentDebugArduinoID, 1-1-1-1);
                //serialWrite(currentDebugArduinoID, "G");
            }
            if(EEPROM_loadBtn){
                
            }
            if(style_Btn){
                
                if(currentStyle == 11){
                    string writeInTotal = "LX : ";
                    
                    string toWrite = "";
                    
                    toWrite+= ofToString(currentStyle);
                    toWrite+= "-";
                    
                    toWrite+= ofToString((int)cableSpeedLx[currentDebugArduinoID]);
                    toWrite+= "-";
                    toWrite+= ofToString((int)cableAccelLx[currentDebugArduinoID]);
                    toWrite+= "-";
                    toWrite+= ofToString((int)cablePosLx[currentDebugArduinoID]);
                    toWrite+= "-";
                    
                    writeInTotal=toWrite + " LY:";
                    
                    toWrite+= ofToString((int)cableSpeedLy[currentDebugArduinoID]);
                    toWrite+= "-";
                    toWrite+= ofToString((int)cableAccelLy[currentDebugArduinoID]);
                    toWrite+= "-";
                    toWrite+= ofToString((int)cablePosLy[currentDebugArduinoID]);
                    toWrite+= "-";
                    
                    writeInTotal=toWrite +" RX: ";
                    
                    toWrite+= ofToString((int)cableSpeedRx[currentDebugArduinoID]);
                    toWrite+= "-";
                    toWrite+= ofToString((int)cableAccelRx[currentDebugArduinoID]);
                    toWrite+= "-";
                    toWrite+= ofToString((int)cablePosRx[currentDebugArduinoID]);
                    toWrite+= "-";
                    
                    writeInTotal=toWrite +" RY: ";;
                    
                    toWrite+= ofToString((int)cableSpeedRy[currentDebugArduinoID]);
                    toWrite+= "-";
                    toWrite+= ofToString((int)cableAccelRy[currentDebugArduinoID]);
                    toWrite+= "-";
                    toWrite+= ofToString((int)cablePosRy[currentDebugArduinoID]);
                    
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
        
        if(drawDebugGui){
            guiDebug.draw();
        }
        
        guiCablePosLx.draw();
        guiCablePosLy.draw();
        guiCablePosRx.draw();
        guiCablePosRy.draw();
        guiCableAccelLx.draw();
        guiCableAccelLy.draw();
        guiCableAccelRx.draw();
        guiCableAccelRy.draw();
        guiCableSpeedLx.draw();
        guiCableSpeedLy.draw();
        guiCableSpeedRx.draw();
        guiCableSpeedRy.draw();
        
        displayLog(currentdisplayLog);
        
    }else{
       // ofBackground(100, 100, 100);
        
    }
    //================== Music Player ==================
    if(showMusicPlayer){
        musicPlayer.draw();
    }
    
    //================== Movement Controller ==================
    /*
     for(int i=0; i< MovementControllers.size(); i++){
     MovementControllers[i].draw();
     }
     */
    if(showMovementController){
        MovementController.draw();
    }
    

    if(page == 0){
        drawDebugGui = true;
        drawKineticVisualizationFbo = true;
        showMusicPlayer = false;
        showMovementController = false;
    }else if(page == 1){
        drawDebugGui = false;
        drawKineticVisualizationFbo = false;
        showMusicPlayer = true;
        showMovementController = false;
    }else{
        drawDebugGui = false;
        drawKineticVisualizationFbo = true;
        showMusicPlayer = false;
        showMovementController = true;
    }
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
            
        case 'q': //reset Arduino
            if(isEmergencyStop){
                serialWrite(-1, "Q");
                isEmergencyStop = false;
            }
            break;
            
        case 'm': //showMusicPlayer
            showMusicPlayer = !showMusicPlayer;
            break;
            
        case 358: //leftArrow;
            page++;
            if(page >= numOfPages ){
                page = 0;
            }
            break;
            
        case 356: //leftArrow;
            page--;
            if(page < 0 ){
                page = numOfPages-1;
            }
            
            break;
            
            
            
        default:
            break;
            
    }
    ofLog() << "page " << page;
}

//--------------------------------------------------------------
//-------------------------- Functions -------------------------
//--------------------------------------------------------------

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
    guiDebug.setup("EEPROMReadWrite", "settings.xml", ofGetWidth() - 630, 0);
    guiDebug.setSize(100, 1000);
    guiDebug.setWidthElements(100);
    
    
    vector<string> EEPROM_names = {"HOME_MAXSPEED", "MAX_SPEED_X", "MAX_ACCELERATION_X", "MAX_SPEED_Y","MAX_ACCELERATION_Y","MAX_POSITION_LX","MAX_POSITION_LY","MAX_POSITION_RX","MAX_POSITION_RY","INVERT_DIR_LX","INVERT_DIR_LY","INVERT_DIR_RX","INVERT_DIR_RY"};
    
    
    vector<int> EEPROM_min = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    vector<int> EEPROM_max = {500, 1000, 5000, 1000,5000, 1000, 1000, 1000, 1000, 1, 1, 1, 1}; //Todo Transfer definition /variables to xml
    
    guiDebug.add(currentDebugArduinoID.set("ArduinoID",0,0,NUM_OF_CABLES));
    
    for(int i=0; i< EEPROM_names.size(); i++){
        ofParameter<int> a;
        a.set(EEPROM_names[i],0,EEPROM_min[i],EEPROM_max[i]);
        EEPROM.push_back(a);
        guiDebug.add(EEPROM[i]);
    }
    vector<string> EEPROM_saveLoad_names = {"SAVE", "LOAD"};
    /*
     EEPROM_btn = {new ofxButton , new ofxButton}; //TODO problem with implementing ofxButton array
     for(int i=0; i< 2; i++){
     //  ofxButton a;
     //  a.setup(EEPROM_saveLoad_names[i]);
     //  EEPROM_btn.push_back(a);
     guiDebug.add(EEPROM_btn[i].setup("SAVE"+ofToString(i),50,50));
     }
     */
    guiDebug.add(EEPROM_saveBtn.setup(EEPROM_saveLoad_names[0]));
    guiDebug.add(EEPROM_loadBtn.setup(EEPROM_saveLoad_names[1]));
    EEPROM_loadBtn.addListener(this, &ofApp::loadButtonPressed);
    guiDebug.add(textField.setup("Serial:", "0-0-0-0-0"));
    guiDebug.add(currentStyle.set("Style",11,0,NUM_OF_CABLES)); //TODO
    guiDebug.add(style_Btn.setup("Set Position:"));
    guiDebug.add(home_Btn.setup("Home: "));
    guiDebug.add(all_Tog.setup("All Val:"));
    
    guiDebug.add(all_Tog.setup("Set Position:"));
    
    //textField.addListener(this,&ofApp::serialTextInput);
    //--- Cable Position Control ---

    
    parametersCablePos.setName("cablePosition");
    guiCablePosLx.setup("EEPROMReadWrite", "settings.xml", ofGetWidth() - 200, 0);
    guiCablePosLy.setup("EEPROMReadWrite", "settings.xml", ofGetWidth() - 200, 400);
    guiCablePosRx.setup("EEPROMReadWrite", "settings.xml", ofGetWidth() - 100, 0);
    guiCablePosRy.setup("EEPROMReadWrite", "settings.xml", ofGetWidth() - 100, 400);
    for(int i=0; i< NUM_OF_CABLES; i++){
        ofParameter<int> a;
        a.set("P Lx" + ofToString(i),0,0,MAX_X_POS); //lx,ly,rx,ry
        ofParameter<int> b;
        b.set("P Ly" + ofToString(i),0,0,MAX_Y_POS);
        ofParameter<int> c;
        c.set("P Rx" + ofToString(i),0,0,MAX_X_POS);
        ofParameter<int> d;
        d.set("P Ry" + ofToString(i),0,0,MAX_Y_POS);
        cablePosLx.push_back(a);
        cablePosLy.push_back(b);
        cablePosRx.push_back(c);
        cablePosRy.push_back(d);
        guiCablePosLx.add(cablePosLx[i]);
        guiCablePosLy.add(cablePosLy[i]);
        guiCablePosRx.add(cablePosRx[i]);
        guiCablePosRy.add(cablePosRy[i]);
    }
    //--- Cable Accel Control ---
    

    
    parametersCableAccel.setName("cableAccel");
    guiCableAccelLx.setup("EEPROMReadWrite", "settings.xml", ofGetWidth() - 310, 0);
    guiCableAccelLy.setup("EEPROMReadWrite", "settings.xml", ofGetWidth() - 310, 400);
    guiCableAccelRx.setup("EEPROMReadWrite", "settings.xml", ofGetWidth() - 260, 0);
    guiCableAccelRy.setup("EEPROMReadWrite", "settings.xml", ofGetWidth() - 260, 400);
    for(int i=0; i< NUM_OF_CABLES; i++){
        ofParameter<int> a;
        a.set("A" + ofToString(i),1000,0,MAX_X_SPEED); //lx,ly,rx,ry
        ofParameter<int> b;
        b.set("A" + ofToString(i),1000,0,MAX_Y_SPEED);
        ofParameter<int> c;
        c.set("A" + ofToString(i),1000,0,MAX_X_SPEED);
        ofParameter<int> d;
        d.set("A" + ofToString(i),1000,0,MAX_Y_SPEED);
        cableAccelLx.push_back(a);
        cableAccelLy.push_back(b);
        cableAccelRx.push_back(c);
        cableAccelRy.push_back(d);
        guiCableAccelLx.add(cableAccelLx[i]);
        guiCableAccelLy.add(cableAccelLy[i]);
        guiCableAccelRx.add(cableAccelRx[i]);
        guiCableAccelRy.add(cableAccelRy[i]);
    }
    //--- Cable Speed Control ---
    
    parametersCableSpeed.setName("cableSpeed");
    guiCableSpeedLx.setup("EEPROMReadWrite", "settings.xml", ofGetWidth() - 420, 0);
    guiCableSpeedLy.setup("EEPROMReadWrite", "settings.xml", ofGetWidth() - 420, 400);
    guiCableSpeedRx.setup("EEPROMReadWrite", "settings.xml", ofGetWidth() - 370, 0);
    guiCableSpeedRy.setup("EEPROMReadWrite", "settings.xml", ofGetWidth() - 370, 400);
    for(int i=0; i< NUM_OF_CABLES; i++){
        ofParameter<int> a;
        a.set("S" + ofToString(i),1000,0,MAX_X_ACCEL); //lx,ly,rx,ry
        ofParameter<int> b;
        b.set("S" + ofToString(i),1000,0,MAX_Y_ACCEL);
        ofParameter<int> c;
        c.set("S" + ofToString(i),1000,0,MAX_X_ACCEL);
        ofParameter<int> d;
        d.set("S" + ofToString(i),1000,0,MAX_Y_ACCEL);
        cableSpeedLx.push_back(a);
        cableSpeedLy.push_back(b);
        cableSpeedRx.push_back(c);
        cableSpeedRy.push_back(d);
        guiCableSpeedLx.add(cableSpeedLx[i]);
        guiCableSpeedLy.add(cableSpeedLy[i]);
        guiCableSpeedRx.add(cableSpeedRx[i]);
        guiCableSpeedRy.add(cableSpeedRy[i]);
    }
    
    
    int guiPosCableW = 100;
    int guiCableH = 500;
    guiCablePosLx.setSize(guiPosCableW, guiCableH);
    guiCablePosLx.setWidthElements(guiPosCableW);
    guiCablePosLy.setSize(guiPosCableW, guiCableH);
    guiCablePosLy.setWidthElements(guiPosCableW);
    guiCablePosRx.setSize(guiPosCableW, guiCableH);
    guiCablePosRx.setWidthElements(guiPosCableW);
    guiCablePosRy.setSize(guiPosCableW, guiCableH);
    guiCablePosRy.setWidthElements(guiPosCableW);
    
    int guiCableW = 50;
    
    guiCableAccelLx.setSize(guiCableW, guiCableH);
    guiCableAccelLx.setWidthElements(guiCableW);
    guiCableAccelLy.setSize(guiCableW, guiCableH);
    guiCableAccelLy.setWidthElements(guiCableW);
    guiCableAccelRx.setSize(guiCableW, guiCableH);
    guiCableAccelRx.setWidthElements(guiCableW);
    guiCableAccelRy.setSize(guiCableW, guiCableH);
    guiCableAccelRy.setWidthElements(guiCableW);
    
    guiCableSpeedLx.setSize(guiCableW, guiCableH);
    guiCableSpeedLx.setWidthElements(guiCableW);
    guiCableSpeedLy.setSize(guiCableW, guiCableH);
    guiCableSpeedLy.setWidthElements(guiCableW);
    guiCableSpeedRx.setSize(guiCableW, guiCableH);
    guiCableSpeedRx.setWidthElements(guiCableW);
    guiCableSpeedRy.setSize(guiCableW, guiCableH);
    guiCableSpeedRy.setWidthElements(guiCableW);
    
    
    //ofFBO
    kineticVisualizationFbo.allocate(ofGetWidth(),ofGetHeight());
    
    
}


void ofApp::displayLog(string s=""){
    if(s.size() >0){
        currentdisplayLog = s ;
    }
    ofSetColor(255, 255, 255);
    ofDrawBitmapString("Status: " + currentdisplayLog, 10, ofGetHeight()-10);
}


//--------------------------------------------------------------
//--------------------------GUI EVENT -----------------------------
//--------------------------------------------------------------
void ofApp::loadButtonPressed(){
    serialWrite(currentDebugArduinoID, "L");
    serialTrigger = false;
    
}

//--------------------------------------------------------------
//-----------------SERIAL COMMUNICATION ------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------

//--------------------------------------------------------------
void ofApp::checkArduinoIsConnected(){
    
    if(ofGetFrameNum() < 200){
        
        if(300 %10 == 0){
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

string ofApp::serialRead(int a){
    
    string combinedStr = "";
    // for(int i=0; i< arduino.size(); i++){
    
    // The serial device can throw exeptions.
    
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
    
    
    return combinedStr;
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

//--------------------------------------------------------------
//---------------------- OTHER EVENTS --------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
void ofApp::exit()
{
    serialWrite(-1, "V");
    for(int i=0; i< arduino.size(); i++){
        arduino[i].unregisterAllEvents(this);
    }
}

//--------------------------------------------------------------
//--------------------------UNUSED -----------------------------
//--------------------------------------------------------------

//TODO: Unused for now
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
void ofApp::removeSubstrs(string& s, string& p) {
    string::size_type n = p.length();
    for (string::size_type i = s.find(p);
         i != string::npos;
         i = s.find(p))
        s.erase(i, n);
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

