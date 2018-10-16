#include "ofApp.h"

//--------------------------------------------------------------
//------------------------------ Setup -------------------------
//--------------------------------------------------------------
void ofApp::setup(){
    
    ofSetFrameRate(25);
    //================ Scheduler ==============
    scheduler.setup();
    
    ofAddListener(scheduler.lightToggleEvent, this, &ofApp::onSchedulerLightsToggle);
    ofAddListener(scheduler.homeResetEvent, this, &ofApp::onSchedulerHomeResetToggle);
    //================ Movements ==============
    movements.setup();
    ofAddListener(movements.setPointsEvent, this, &ofApp::onSetPoints);
    
    
    //================== Communication ==================
    //------------------- Serial -------------------
    
    isArduinoConnected = serialSetup();
    
    SERIAL_PARAMETERES = {"sa", "lo", "on", "allhm"}; //save, load, online
    serialTrigger = true;
    prevSerialTriggerMillis = ofGetElapsedTimeMillis();
    
    for(int i=0; i < NUM_OF_CABLES; i++){
        isArduinoConnectedBySerial.push_back(false);
    }
    initOnUpdate = true;
    
#ifdef USEOSC
    //------------------- OSC -------------------
    // listen on the given port
    cout << "listening for osc messages on port " << R_PORT << "\n";
    receiver.setup(R_PORT);
    // open an outgoing connection to HOST:PORT
    sender.setup(S_HOST, S_PORT);
#else
    
#endif
    
    //================== debugMode ==================
    
    page = 0;
    numOfPages = 7;
    debugMode = false;
    isEmergencyStop = false;
    for(int i=0; i < NUM_OF_SERIAL_TO_INIT; i++){
        receivedString.push_back("");
        receivedStringBuffer.push_back("");
        prevReceivedString.push_back("");
        updateColor.push_back(255);
    }
    
    for(int i=0; i < NUM_OF_CABLES; i++){
        currDisplayLog.push_back("");
        prevDisplayLog.push_back("old");
        displayLogColor.push_back(255);
    }
    currDisplayLog.push_back("Park Yoho Control Software V0.4");
    
    setupGui();
    
    //================== Timeline/Music Player ==================
    setupMusicPlayerAndTimeline();
    
    //================== Dmx Light ==================
    DmxLight.setup();
    
    //================== Movement Controls ==================
    int mcX = 20;
    int mcY = 0;
    int mcW = 300;
    int mcH = 150;
    int mcHg = 20;
    
    movementController.setup(NUM_OF_CABLES, mcX, mcY, mcW , mcH, MAX_X_POS, MAX_Y_POS,2);
    
    //================== Song 1 ==================
    currMovementStatge = 0;
    
    //================= Settings ==================
    loadSettings();
}


//--------------------------------------------------------------
//------------------------------ Update ------------------------
//--------------------------------------------------------------
void ofApp::update(){
    
    currTime = ofGetElapsedTimeMillis();
    
    ofSetWindowTitle(ofToString(ofGetFrameRate()));
    
    //================ Scheduler ==============
    scheduler.update();
    //================== Communication ==================
    //------------------- OSC -------------------
#ifdef USEOSC
    receivedString = readOSC();
#else
    
    //------------------- Serial -------------------
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
    
#endif
    
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
    
    //Online Cable Checking - TODO
    if(currTime - prevOnlineCheckingMillis < 5000){
        num_of_online = 0;
        for (int i=0; i< NUM_OF_CABLES; i++){
            if(isArduinoConnectedBySerial[i]){
                num_of_online++;
            }
        }
    }
    
    for(int i=0; i< NUM_OF_SERIAL_TO_INIT; i++){
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
    
    for(int i=0; i< NUM_OF_CABLES; i++){
        kinecticVisualisation.set(NUM_OF_CABLES ,i, currStyle ,ofMap(cablePosLx[i],0,MAX_X_POS,0,1) ,ofMap(cablePosLy[i],0,MAX_Y_POS,0,1),ofMap(cablePosRx[i],0,MAX_X_POS,0,1), ofMap(cablePosRy[i],0,MAX_Y_POS,0,1));
    }
    
    kineticVisualizationFbo.begin();
    ofClear(255, 0, 0,0);
    kinecticVisualisation.draw();
    kineticVisualizationFbo.end();
    
    //================== Music/Timeline Player ==================
    musicPlayer.update();
    /*
     if(!showBeginTrigger && isShowReady){
     //timelinePlayer.loadButtonPressed();
     //timelinePlayer.playButtonPressed();
     showBeginTrigger = true;
     ofLog() << " Play Button Triggered ";
     bool t = true;
     playTrack(t);
     }
     */
    //get button status from Timeline player
    setTrackisLoop(timelinePlayer.getLoopButtonStatus());
    timelinePlayer.update();
    
    //================== Dmx Light ==================
    DmxLight.update();
    
    //================== Movement Controls ==================
    movementController.update();
    
    setPoints();
    
    //Safety Check for style if pre style != currstyle //TODO
    if(prevStyle != currStyle){
        serialWrite(-1, "Q");
        waitForStyleChangeMillis = currTime;
        isShowReady = false;
        prevStyle = currStyle;
        
    }
    
    if(currTime - waitForStyleChangeMillis > SHOW_DELAY_TIME){
        isShowReady = true;
    }else{
        currDisplayLog[NUM_OF_CABLES] = "Please wait for " + ofToString((SHOW_DELAY_TIME - (currTime - waitForStyleChangeMillis))/1000) + "s to begin show";
    }
}

//--------------------------------------------------------------
//------------------------------ Draw --------------------------
//--------------------------------------------------------------
void ofApp::draw(){
    
    ss_info.str("");
    
    ss_info << "'space'  --> Emergency Stop : " << endl;
    ss_info << "'d'      --> Debug Mode     : "<< debugMode << endl;
    ss_info << " " << endl;
    ss_info << "'5'      --> power on/off: " << endl;
    ss_info << "'6'      --> lights on/off" << endl;
    ss_info << "'7'      --> Reset and Home"<< endl;
    ss_info << "'8'      --> Mode Selection: " << movementMode << endl;
    ss_info << "'9'      --> Reserved " << endl;
    
    ss_info << " " << endl;
    
    if(isEmergencyStop){
        ofBackground(255, 0, 0);
        ofSetColor(255);
        
        ss_info << "FrameRate : "<< ofGetFrameRate() << endl;
        ss_info << "EMERGENCY STOP - 'r' to relase: " << endl;
        ss_info << "EMERGENCY STOP - 'q' to reset all arduino: " << endl;
        
    } else if(debugMode){
        //================== Debug Mode ==================
        ofBackground(100, 0, 0);
        ofSetColor(255);
        
        ss_info << "MODE : DEBUG MODE" << endl;
        ss_info << "Press '1' for Single Cable Testing Mode :" << endl;
        
        ss_info << "FrameRate : "<< ofGetFrameRate() << endl;
        ss_info << "Page : "<< page << endl;
        
        ss_info << "Num of Arduino: " << arduino.size() << " / " << NUM_OF_CABLES << endl;
        //================== Debug Gui ==================
        
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
                serialWrite(currCableID, toWrite);
                serialWrite(currCableID, "S");
                
                currDisplayLog[NUM_OF_CABLES] = ofToString(currCableID) +" EEPROM SAVED";
                serialTrigger = false;
                prevSerialTriggerMillis =currTime;
                
                
            }
            if(reset_Btn || singleCableResetBtn){
                serialWrite(currCableID, "Q");
            }
            if(singleCableResetAllBtn){
                serialWrite(-1, "Q");
            }
            if(singleCableHomeAllBtn){
                serialWrite(-1, "H");
            }
            if(home_Btn){
                if(all_Tog){
                    serialWrite(-1, "H");
                }else{
                    serialWrite(currCableID, "H");
                }
                
            }
            if(EEPROM_loadBtn){
                
            }
            if(sendStyleBtn_all_same){
                writeStyle(0);//all same = 0, all diff = 1, specific = 2
            }
            
            if(sendStyleBtn_all){
                writeStyle(1);//all same = 0, all diff = 1, specific = 2
            }
            
            if(sendStyleBtn || singleCableSendStyleBtn){
                writeStyle(2);//all same = 0, all diff = 1, specific = 2
            }
            
        }
        if(currTime -  prevSerialTriggerMillis > 200){
            serialTrigger = true;
        }
        
        string t = textField;
        if (t.find('+') != std::string::npos || t.find('=') != std::string::npos)
        {
            t.erase(std::remove(t.begin(), t.end(), '='), t.end());
            t.erase(std::remove(t.begin(), t.end(), '+'), t.end());
            currDisplayLog[currCableID] = t;
            serialWrite(currCableID, t);
            textField = "";
        }
        
        //Ricci Degbug Mode - Single Cable Control
        if(page == 6){
            movementController.setPoints(0,0,36, 23, 1750, 100);
            if(currCableID >= 0 ){
                if(prevCableID != currCableID){
                    singleCablePos[0] = cablePosLx[currCableID];
                    singleCablePos[1] = cablePosLy[currCableID];
                    singleCablePos[2] = cablePosRx[currCableID];
                    singleCablePos[3] = cablePosRy[currCableID];
                }else{
                    cablePosLx[currCableID] = singleCablePos[0];
                    cablePosLy[currCableID] = singleCablePos[1];
                    cablePosRx[currCableID] = singleCablePos[2];
                    cablePosRy[currCableID] = singleCablePos[3];
                }
                prevCableID = currCableID;
            }else{
                currCableID = 0;
            }
            
            if(currTime - prevSingleCableLoopMillis > 10000){
                bool triggerAnyCable = false;
                for(int i=0; i<4; i++){
                    if(singleCablePosLoop[i]){
                        triggerAnyCable = true;
                        if(i== 0 || i== 2){
                            if(singleCablePos[i] < MAX_X_POS /2){
                                singleCablePos[i] = MAX_X_POS/2;
                            }else{
                                singleCablePos[i] = 0;
                            }
                        }
                        else{
                            if(singleCablePos[i] < MAX_Y_POS /2){
                                singleCablePos[i] = MAX_Y_POS/2;
                            }else{
                                singleCablePos[i] = 0;
                            }
                        }
                        ofLog() << "Actived : " << i  << " cm " << currTime << " pm " << prevSingleCableLoopMillis;
                    }else{
                        singleCablePos[i] = 0;
                    }
                    
                }
                if(triggerAnyCable){
                    prevSingleCableLoopMillis = currTime;
                    writeStyle(2);
                }
            }
        }
        
        //END Ricci Mode - Single Cable Control
    }
    else { //Exhibition Mode
        movement(movementMode);
    }
    
    drawGui();
    
    //================== Communication - Serial or OSC ==================
    
    for(int i=0; i < NUM_OF_CABLES; i++){
        std::stringstream ss2;
        // if(isArduinoConnected[i]){
        if(isArduinoConnectedBySerial[i]){
            ofSetColor(0,updateColor[i],updateColor[i]);
#ifdef USEOSC
            ss2 << "Arduino (" << i+1 << ") : " << prevReceivedString[i] << endl;
#else
            ss2 << "Arduino (" << i+1 << ") : " << arduino[i].getPortName() << " : " << prevReceivedString[i] << endl;
#endif
        }else{
            ofSetColor(200,200,200);
            ss2 << "Arduino (" << i+1 << ") : " << "OFFLINE" << endl;
        }
        ofDrawBitmapString(ss2.str(), ofVec2f(20, 30*i + 240));
        
    }
    
    ofSetColor(255);
    ofDrawBitmapString(ss_info.str(), ofVec2f(20, 20));
    
    displayLog(currDisplayLog);
}

//--------------------------------------------------------------
//------------------- Keyboard Command -------------------------
//--------------------------------------------------------------

void ofApp::keyReleased(int key){
    switch (key){
        case 'a':
            ofLog() << "Hello";
#ifdef USEOSC
            sendOSC(currCableID, "Testing");
#else
#endif
            break;
        case 's':
            saveSettings();
            break;
            
        case 'd':
            debugMode = !debugMode;
            
            if(debugMode){
                for(int i=0; i < NUM_OF_CABLES; i++){
                    cableTimeLx[i] = TEST_X_TIME;
                    cableTimeLy[i] = TEST_Y_TIME;
                    cableTimeRx[i] = TEST_X_TIME;
                    cableTimeRy[i] = TEST_Y_TIME;
                }
            }
            else{
                for(int i=0; i < NUM_OF_CABLES; i++){
                    cableTimeLx[i] = DEFAULT_X_TIME;
                    cableTimeLy[i] = DEFAULT_Y_TIME;
                    cableTimeRx[i] = DEFAULT_X_TIME;
                    cableTimeRy[i] = DEFAULT_Y_TIME;
                }
            }
            
            break;
            
        case 'c': //check if arduino online
            serialWrite(-1, "C");
            break;
            
        case 'p': //dialog for serial writing
            serialCommander();
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
            
        case '1': //Ricci Mode - Single Cable Control
            if(debugMode){
                currStyle == 11;
                page = 6;
                
                for(int i=0; i < NUM_OF_CABLES; i++){
                    cablePosLx[i] = 0;
                    cablePosLy[i] = 0;
                    cablePosRx[i] = 0;
                    cablePosRy[i] = 0;
                }
            }
            break;
            
        case '=': //Ricci Mode - Single Cable Control
            if(debugMode && page == 6){
                if(currCableID<NUM_OF_CABLES-1){
                    currCableID++;
                }else{
                    currCableID = NUM_OF_CABLES-1;
                }
            }
            break;
            
        case '-': //Ricci Mode - Single Cable Control
            if(debugMode && page == 6){
                if(currCableID>0){
                    currCableID--;
                }else{
                    currCableID=0;
                }
            }
            break;
            
        case 'e':
            movementMode++;
            if(movementMode > 4){
                movementMode = 0;
            }
            XML.setValue("MODE", ofToString(movementMode));
            settingsMode["mode"] = (int)movementMode;
            saveSettings();
            break;
            
        case '3':
            if(page == 6){
                writeStyle(2);
            }
            break;
        case '5': //power on/off
            movementMode = 0;
            break;
        case '6': //lights on/off
            if(movementMode == 1){
                movementMode = 0;
            }else{
                movementMode = 1;
            }
            break;
        case '7': //Reset and Home
            serialWrite(-1, "Q");
            movementMode = 2;
            break;
        case '8': //Mode Selection
            if(movementMode>= 2){
                movementMode++;
            }else{
                movementMode = 2;
            }
            if(movementMode > 4){
                movementMode = 2;
            }
            break;
            
        case '9': //Reserved
            break;
            
        case 'h': //to zero points
            offsetHome();
            break;
            
        case 'l': //to zero points
            movements.loadSettings();
            break;
        case 'k':
            prevTime = currTime;
            break;
            
            
        default:
            break;
            
    }
    ofLog() << "page " << page;
}



//--------------------------------------------------------------
//----------------------- Mouse Command-------------------------
//--------------------------------------------------------------

//-------------------------Timeline Player----------------------
void ofApp::mouseDragged(int x, int y, int button){
    timelinePlayer.mouseDragged(x, y, button);
}

void ofApp::mousePressed(int x, int y, int button){
    timelinePlayer.mousePressed(x, y, button);
}

void ofApp::mouseReleased(int x, int y, int button){
    timelinePlayer.mouseReleased(x, y, button);
}

//--------------------------------------------------------------
//-------------------------- OSC -------------------------------
//--------------------------------------------------------------
#ifdef USEOSC

//--------------------------------------------------------------
void ofApp::sendOSC(int ar, string s){
    ofxOscMessage m;
    m.setAddress("/serial/");
    m.addIntArg(ar);
    m.addStringArg(s);
    sender.sendMessage(m, false);
    
    if(debugMode){
        ofLog() << "sending OSC : " << ar << " : " << s;
    }
}

vector<string> ofApp::readOSC(){
    
    vector<string> read;
    
    for(int i=0; i< NUM_OF_SERIAL_TO_INIT; i++){
        read.push_back("");
    }
    
    int currentArduinoID = -1;
    
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
            
            receivedString[0] = mouseButtonState;
            
        }
        // check for an image being sent (note: the size of the image depends greatly on your network buffer sizes - if an image is too big the message won't come through )
        else{
            // unrecognized message: display on the bottom of the screen
            string msg_string;
            string getString = "";
            msg_string = m.getAddress();
            msg_string += ": ";
            for(int i = 0; i < m.getNumArgs(); i++){
                // get the argument type
                msg_string += m.getArgTypeName(i);
                msg_string += ":";
                // display the argument - make sure we get the right type
                if(m.getArgType(i) == OFXOSC_TYPE_INT32){
                    //msg_string += ofToString(m.getArgAsInt32(i));
                    currentArduinoID = m.getArgAsInt32(i);
                }
                else if(m.getArgType(i) == OFXOSC_TYPE_FLOAT){
                    msg_string += ofToString(m.getArgAsFloat(i));
                }
                else if(m.getArgType(i) == OFXOSC_TYPE_STRING){
                    msg_string += m.getArgAsString(i);
                    getString+= m.getArgAsString(i);
                }
                else{
                    msg_string += "unknown";
                }
                
            }
            //  ofLog() << " currentArduinoID:  "<< currentArduinoID << " String: " << getString;
            for(int i=0; i< NUM_OF_SERIAL_TO_INIT; i++){
                if(arduino[i] == currentArduinoID){
                    read[i] = getString;
                }
            }
        }
    }
    return read;
    
}
#else
#endif


//--------------------------------------------------------------
//-------------------------- Utils -----------------------------
//--------------------------------------------------------------

bool ofApp::is_number(const std::string& s)
{
    std::string::const_iterator it = s.begin();
    while (it != s.end() && std::isdigit(*it)) ++it;
    return !s.empty() && it == s.end();
}

void ofApp::serialCommander(){
    string txt = ofSystemTextBoxDialog("Serial Command", txt);
    serialWrite(currCableID, txt);
    
}

//--------------------------------------------------------------
//-------------------------- GUI -------------------------------
//--------------------------------------------------------------

void ofApp::setupGui(){
    drawDebugGui = {false,false,false};
    
    //--- EEPROM ---
    parametersDebug.setName("settings");
    guiDebug.setup("EEPROMReadWrite", "settings.xml", ofGetWidth() - 630, 0);
    
    guiDebug.setSize(80, 1000);
    guiDebug.setWidthElements(80);
    
    guiDebug2.setup("EEPROMReadWrite", "settings.xml", ofGetWidth() - 730, 0);
    guiDebug2.setSize(60, 1000);
    guiDebug2.setWidthElements(60);
    
    guiDebugCableOption.setup("EEPROMReadWrite", "settings.xml", 600 , 0);
    guiDebugCableOption.setSize(60, 1000);
    guiDebugCableOption.setWidthElements(60);
    
    vector<string> EEPROM_names = {"HOME_MAXSPEED", "MAX_SPEED_X", "MAX_ACCELERATION_X", "MAX_SPEED_Y","MAX_ACCELERATION_Y","MAX_POSITION_LX","MAX_POSITION_LY","MAX_POSITION_RX","MAX_POSITION_RY","INVERT_DIR_LX","INVERT_DIR_LY","INVERT_DIR_RX","INVERT_DIR_RY"};
    
    vector<int> EEPROM_min = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    vector<int> EEPROM_max = {500, 1000, 5000, 1000, 5000, 1000, 1000, 1000, 1000, 1, 1, 1, 1}; //Todo Transfer definition /variables to xml
    
    guiDebug.add(currCableID.set("Cable No + 1",0,0,NUM_OF_CABLES-1));
    guiDebug.add(movementMode.set("Movement Mode",movementMode,0,NUM_OF_CABLES));
    
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
     guiDebug.add(EEPROM_btn[i].setup("SAVE"+ofToString(i+1),50,50));
     }
     */
    guiDebug.add(EEPROM_saveBtn.setup(EEPROM_saveLoad_names[0]));
    guiDebug.add(EEPROM_loadBtn.setup(EEPROM_saveLoad_names[1]));
    EEPROM_loadBtn.addListener(this, &ofApp::loadEEPROMButtonPressed);
    guiDebug.add(textField.setup("Serial:", "0-0-0-0-0"));
    guiDebug.add(currStyle.set("Style",12,0,NUM_OF_CABLES)); //TODO
    guiDebug.add(sendStyleBtn.setup("Set Pos:"));
    guiDebug.add(sendStyleBtn_all_same.setup("Set Pos ALL SAME:"));
    guiDebug.add(sendStyleBtn_all.setup("Set Pos ALL:"));
    guiDebug.add(reset_Btn.setup("Reset:"));
    guiDebug.add(home_Btn.setup("Home: "));
    guiDebug.add(all_Tog.setup("ALL TOGGLE:",false));
    
    prevStyle = currStyle;
    
    all_Tog = false;
    int numOfWaveForm = 4;
    guiDebugCableOption.add(cableOp.set("Cable Option",5, 0,9));
    for(int i=0; i< 8; i++){
        ofParameter<bool> a;
        a.set("input " + ofToString(i+1),false);
        ofParameter<bool> b;
        string c = "";
        if(i==0){
            c+= "Lx";
        }
        if(i==1){
            c+= "Ly";
        }
        if(i==2){
            c+= "Rx";
        }
        if(i==3){
            c+= "Ry";
        }
        if(i==4){
            c+= "Lx2";
        }
        if(i==5){
            c+= "Ly2";
        }
        if(i==6){
            c+= "Rx2";
        }
        if(i==7){
            c+= "Ry2";
        }
        
        b.set(c,false);
        
        input_pts.push_back(a);
        output_pts.push_back(b);
        //guiDebug.add(input_pts[i]);
        guiDebugCableOption.add(output_pts[i]);
    }
    
    for(int i=0; i< NUM_OF_CABLES; i++){
        ofParameter<bool> a;
        a.set("On " + ofToString(i+1),true);
        
        working_cable.push_back(a);
        guiDebug2.add(working_cable[i]);
    }
    
    guiDebugSingleCableCtrl.setup("EEPROMReadWrite", "settings.xml", ofGetWidth() - 500, 0);
    guiDebugSingleCableCtrl.setSize(30, 1000);
    guiDebugSingleCableCtrl.setWidthElements(30);
    
    guiDebugSingleCableCtrl.add(currCableID.set("Cable No (0-19)",0,0,NUM_OF_CABLES-1));
    
    for(int i=0; i< 4; i++){
        ofParameter<int> scp;
        ofParameter<bool> b;
        vector<string> t = {"Lx", "Ly", "Rx", "Ry"};
        
        if(i==0 || i==2){
            b.set(t[i] +" loop",false);
            scp.set(t[i],0,0,MAX_X_POS);
        }else{
            b.set(t[i] +" loop",false);
            scp.set(t[i],0,0,MAX_Y_POS);
        }
        
        singleCablePos.push_back(scp);
        singleCablePosLoop.push_back(b);
        guiDebugSingleCableCtrl.add(singleCablePos[i]);
        guiDebugSingleCableCtrl.add(singleCablePosLoop[i]);
    }
    
    guiDebugSingleCableCtrl.add(singleCableSendStyleBtn.setup("(3)Send Command"));
    guiDebugSingleCableCtrl.add(singleCableResetBtn.setup("Reset"));
    guiDebugSingleCableCtrl.add(singleCableResetAllBtn.setup("Reset all"));
    guiDebugSingleCableCtrl.add(singleCableHomeAllBtn.setup("HOME all"));
    
    // guiDebug.add(all_Tog.setup("SET ALL:"));
    
    //textField.addListener(this,&ofApp::serialTextInput);
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
    
    //--- Cable Position Offset Control ---
    drawPosOffset = false;
    
    parametersCablePosOffset.setName("cablePositionOffset");
    guiCablePosLxOffset.setup("EEPROMReadWrite", "settings.xml",  100, 0);
    guiCablePosLyOffset.setup("EEPROMReadWrite", "settings.xml",  100, 400);
    guiCablePosRxOffset.setup("EEPROMReadWrite", "settings.xml",  200, 0);
    guiCablePosRyOffset.setup("EEPROMReadWrite", "settings.xml",  200, 400);
    for(int i=0; i< NUM_OF_CABLES; i++){
        ofParameter<int> a;
        a.set("OffS Lx" + ofToString(i+1),0,0,MAX_X_POS*0.1);
        ofParameter<int> b;
        b.set("OffS Ly" + ofToString(i+1),0,0,MAX_Y_POS*0.1);
        ofParameter<int> c;
        c.set("OffS Rx" + ofToString(i+1),0,0,MAX_X_POS*0.1);
        ofParameter<int> d;
        d.set("OffS Ry" + ofToString(i+1),0,0,MAX_Y_POS*0.1);
        cablePosLxOffset.push_back(a);
        cablePosLyOffset.push_back(b);
        cablePosRxOffset.push_back(c);
        cablePosRyOffset.push_back(d);
        guiCablePosLxOffset.add(cablePosLxOffset[i]);
        guiCablePosLyOffset.add(cablePosLyOffset[i]);
        guiCablePosRxOffset.add(cablePosRxOffset[i]);
        guiCablePosRyOffset.add(cablePosRyOffset[i]);
    }
    
    //--- Cable Distance Calculation ---
    for(int i=0; i< NUM_OF_CABLES; i++){
        prevCablePosLx.push_back(0);
        prevCablePosLy.push_back(0);
        prevCablePosRx.push_back(0);
        prevCablePosRy.push_back(0);
        cableLxDist.push_back(0);
        cableLyDist.push_back(0);
        cableRxDist.push_back(0);
        cableRyDist.push_back(0);
    }
    
    
    //--- Cable Time Control ---
    parametersCableTime.setName("cableTime");
    guiCableTimeLx.setup("EEPROMReadWrite", "settings.xml", ofGetWidth() - 410, 0);
    guiCableTimeLy.setup("EEPROMReadWrite", "settings.xml", ofGetWidth() - 410, 400);
    guiCableTimeRx.setup("EEPROMReadWrite", "settings.xml", ofGetWidth() - 300, 0);
    guiCableTimeRy.setup("EEPROMReadWrite", "settings.xml", ofGetWidth() - 300, 400);
    for(int i=0; i< NUM_OF_CABLES; i++){
        ofParameter<int> a;
        a.set("T" + ofToString(i+1),DEFAULT_X_TIME,10000,MAX_X_TIME); //lx,ly,rx,ry
        ofParameter<int> b;
        b.set("T" + ofToString(i+1),DEFAULT_Y_TIME,10000,MAX_Y_TIME);
        ofParameter<int> c;
        c.set("T" + ofToString(i+1),DEFAULT_X_TIME,10000,MAX_X_TIME);
        ofParameter<int> d;
        d.set("T" + ofToString(i+1),DEFAULT_Y_TIME,10000,MAX_Y_TIME);
        cableTimeLx.push_back(a);
        cableTimeLy.push_back(b);
        cableTimeRx.push_back(c);
        cableTimeRy.push_back(d);
        guiCableTimeLx.add(cableTimeLx[i]);
        guiCableTimeLy.add(cableTimeLy[i]);
        guiCableTimeRx.add(cableTimeRx[i]);
        guiCableTimeRy.add(cableTimeRy[i]);
    }
    
    //--- Cable Accel Control ---
    
    parametersCableAccel.setName("cableAccel");
    guiCableAccelLx.setup("EEPROMReadWrite", "settings.xml", ofGetWidth() - 310, 0);
    guiCableAccelLy.setup("EEPROMReadWrite", "settings.xml", ofGetWidth() - 310, 400);
    guiCableAccelRx.setup("EEPROMReadWrite", "settings.xml", ofGetWidth() - 260, 0);
    guiCableAccelRy.setup("EEPROMReadWrite", "settings.xml", ofGetWidth() - 260, 400);
    for(int i=0; i< NUM_OF_CABLES; i++){
        ofParameter<int> a;
        a.set("A" + ofToString(i+1),DEFAULT_X_SPEED,MIN_X_SPEED,MAX_X_SPEED); //lx,ly,rx,ry
        ofParameter<int> b;
        b.set("A" + ofToString(i+1),DEFAULT_Y_SPEED,MIN_Y_SPEED,MAX_Y_SPEED);
        ofParameter<int> c;
        c.set("A" + ofToString(i+1),DEFAULT_X_SPEED,MIN_X_SPEED,MAX_X_SPEED);
        ofParameter<int> d;
        d.set("A" + ofToString(i+1),DEFAULT_Y_SPEED,MIN_Y_SPEED,MAX_Y_SPEED);
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
        a.set("S" + ofToString(i+1),DEFAULT_X_ACCEL,0,MAX_X_ACCEL); //lx,ly,rx,ry
        ofParameter<int> b;
        b.set("S" + ofToString(i+1),DEFAULT_Y_ACCEL,0,MAX_Y_ACCEL);
        ofParameter<int> c;
        c.set("S" + ofToString(i+1),DEFAULT_X_ACCEL,0,MAX_X_ACCEL);
        ofParameter<int> d;
        d.set("S" + ofToString(i+1),DEFAULT_Y_ACCEL,0,MAX_Y_ACCEL);
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
    
    int guiTimeCableW = 100;
    guiCableTimeLx.setSize(guiTimeCableW, guiCableH);
    guiCableTimeLx.setWidthElements(guiTimeCableW);
    guiCableTimeLy.setSize(guiTimeCableW, guiCableH);
    guiCableTimeLy.setWidthElements(guiTimeCableW);
    guiCableTimeRx.setSize(guiTimeCableW, guiCableH);
    guiCableTimeRx.setWidthElements(guiTimeCableW);
    guiCableTimeRy.setSize(guiTimeCableW, guiCableH);
    guiCableTimeRy.setWidthElements(guiTimeCableW);
    
    guiCablePosLxOffset.setSize(guiPosCableW, guiCableH);
    guiCablePosLxOffset.setWidthElements(guiPosCableW);
    guiCablePosLyOffset.setSize(guiPosCableW, guiCableH);
    guiCablePosLyOffset.setWidthElements(guiPosCableW);
    guiCablePosRxOffset.setSize(guiPosCableW, guiCableH);
    guiCablePosRxOffset.setWidthElements(guiPosCableW);
    guiCablePosRyOffset.setSize(guiPosCableW, guiCableH);
    guiCablePosRyOffset.setWidthElements(guiPosCableW);
    
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
    
    //==== Style 2 =====
    
    guiCablePosLx2.setup("EEPROMReadWrite", "settings.xml", 100, 0);
    guiCablePosLy2.setup("EEPROMReadWrite", "settings.xml", 100, 400);
    guiCablePosRx2.setup("EEPROMReadWrite", "settings.xml", 200, 0);
    guiCablePosRy2.setup("EEPROMReadWrite", "settings.xml", 200, 400);
    for(int i=0; i< NUM_OF_CABLES; i++){
        ofParameter<int> a;
        a.set("P2 Lx" + ofToString(i+1),1500,0,MAX_X_POS); //lx,ly,rx,ry
        ofParameter<int> b;
        b.set("P2 Ly" + ofToString(i+1),1500,0,MAX_Y_POS);
        ofParameter<int> c;
        c.set("P2 Rx" + ofToString(i+1),1500,0,MAX_X_POS);
        ofParameter<int> d;
        d.set("P2 Ry" + ofToString(i+1),1500,0,MAX_Y_POS);
        cablePosLx2.push_back(a);
        cablePosLy2.push_back(b);
        cablePosRx2.push_back(c);
        cablePosRy2.push_back(d);
        guiCablePosLx2.add(cablePosLx2[i]);
        guiCablePosLy2.add(cablePosLy2[i]);
        guiCablePosRx2.add(cablePosRx2[i]);
        guiCablePosRy2.add(cablePosRy2[i]);
    }
    
    guiCableSpeedAccelAll.setup("EEPROMReadWrite", "settings.xml", 400, 0);
    
    cableSpeedX.set("S2 X ",270,0,MAX_X_SPEED);
    cableAccelX.set("A2 X " ,140,0,MAX_X_ACCEL);
    
    cableSpeedY.set("S2 Y ",280,0,MAX_Y_SPEED);
    cableAccelY.set("A2 Y ",240,0,MAX_Y_ACCEL);
    
    guiCableSpeedAccelAll.add(cableSpeedX);
    guiCableSpeedAccelAll.add(cableAccelX);
    guiCableSpeedAccelAll.add(cableSpeedY);
    guiCableSpeedAccelAll.add(cableAccelY);
    
    guiCablePosLx2.setSize(guiPosCableW, guiCableH);
    guiCablePosLx2.setWidthElements(guiPosCableW);
    guiCablePosLy2.setSize(guiPosCableW, guiCableH);
    guiCablePosLy2.setWidthElements(guiPosCableW);
    guiCablePosRx2.setSize(guiPosCableW, guiCableH);
    guiCablePosRx2.setWidthElements(guiPosCableW);
    guiCablePosRy2.setSize(guiPosCableW, guiCableH);
    guiCablePosRy2.setWidthElements(guiPosCableW);
    
    guiCableSpeedAccelAll.setSize(guiPosCableW, guiCableH);
    guiCableSpeedAccelAll.setWidthElements(guiPosCableW);
    
    
}


void ofApp::displayLog(vector<string> s){
    
    
    
    for(int i=0; i<currDisplayLog.size(); i++){
        if(prevDisplayLog[i] != currDisplayLog[i]){
            displayLogColor[i] = 255;
            prevDisplayLog[i] = currDisplayLog[i];
        }
        if(displayLogColor[i] >255/3){
            displayLogColor[i]--;
        }
        ofSetColor(displayLogColor[i], displayLogColor[i],displayLogColor[i]);
        ofDrawBitmapString("        (" + ofToString(i+1) + ") : " + currDisplayLog[i], 20, 30*i + 255);
    }
    
}


void ofApp::drawGui(){
    
    //================ Scheduler ==============
    if(drawScheduler){
        scheduler.draw();
    }
    
    //================== General Gui ==================
    if(drawDebugGui[0]){
        guiDebug.draw();
    }
    if(drawDebugGui[1]){ //working cable
        guiDebug2.draw();
    }
    if(drawDebugGui[2]){
        guiDebugSingleCableCtrl.draw();
    }
    
    if(drawSpeedAccelGui){
        guiCableAccelLx.draw();
        guiCableAccelLy.draw();
        guiCableAccelRx.draw();
        guiCableAccelRy.draw();
        guiCableSpeedLx.draw();
        guiCableSpeedLy.draw();
        guiCableSpeedRx.draw();
        guiCableSpeedRy.draw();
    }
    //================== Position Gui ==================
    if(drawPosGui){
        guiCablePosLx.draw();
        guiCablePosLy.draw();
        guiCablePosRx.draw();
        guiCablePosRy.draw();
    }
    if(drawTimeGui){
        guiCableTimeLx.draw();
        guiCableTimeLy.draw();
        guiCableTimeRx.draw();
        guiCableTimeRy.draw();
    }
    
    //================== Dmx Light ==================
    if(drawDmx){
        DmxLight.draw();
    }
    
    //================== Movement Controller ==================
    
    if(drawMovementController){
        movementController.draw();
        guiDebugCableOption.draw();
    }
    
    if(drawPosOffset){
        guiCablePosLxOffset.draw();
        guiCablePosLyOffset.draw();
        guiCablePosRxOffset.draw();
        guiCablePosRyOffset.draw();
    }
    
    if(drawKineticVisualizationFbo){
        ofSetColor(255);
        ofPushMatrix();
        kineticVisualizationFbo.draw(200,-50,ofGetWidth()*0.5,ofGetHeight()*0.5);
        ofPopMatrix();
    }
    
    
    //================== Music Player ==================
    if(drawMusicPlayer){
        musicPlayer.draw();
        timelinePlayer.draw(ofGetScreenHeight()/2);
    }
    if(debugMode){
        
        
        if(page == 0){ //MoveTo Mode (style 11)
            drawDebugGui = {true,true,false};
            drawSpeedAccelGui = true;
            drawPosGui = true;
            drawTimeGui = false;
            drawDmx = false;
            drawMovementController = false;
            drawPosOffset = false;
            drawKineticVisualizationFbo = true;
            drawMusicPlayer = false;
            drawScheduler = false;
        }
        else if(page == 1){ //2 pos (style 2)
            drawDebugGui = {true,true,false};
            drawSpeedAccelGui = true;
            drawPosGui = true;
            drawTimeGui = false;
            drawDmx = false;
            drawMovementController = true;
            drawPosOffset = false;
            drawKineticVisualizationFbo = true;
            drawMusicPlayer = false;
            drawScheduler = false;
            
            guiCablePosLx2.draw();
            guiCablePosLy2.draw();
            guiCablePosRx2.draw();
            guiCablePosRy2.draw();
            guiCableSpeedAccelAll.draw();
        }
        else if(page == 2){ //time ctrl mode
            drawDebugGui = {true,true,false};
            drawSpeedAccelGui = false;
            drawPosGui = true;
            drawTimeGui = true;
            drawDmx = false;
            drawMovementController = false;
            drawPosOffset = false;
            drawKineticVisualizationFbo = true;
            drawMusicPlayer = false;
            drawScheduler = false;
        }
        else if(page == 3){ //pos by wave curve
            drawDebugGui = {true,true,false};
            drawSpeedAccelGui = true;
            drawPosGui = true;
            drawTimeGui = false;
            drawDmx = false;
            drawMovementController = true;
            drawPosOffset = false;
            drawKineticVisualizationFbo = true;
            drawMusicPlayer = false;
            drawScheduler = false;
        }
        else if(page == 4){ //pos offset
            drawDebugGui = {true,true,false};
            drawSpeedAccelGui = true;
            drawPosGui = true;
            drawTimeGui = false;
            drawDmx = false;
            drawMovementController = false;
            drawPosOffset = true;
            drawKineticVisualizationFbo = true;
            drawMusicPlayer = false;
            drawScheduler = false;
        }
        else if(page == 5){//================== Dmx Light ==================
            drawDebugGui = {false,false,false};
            drawSpeedAccelGui = false;
            drawPosGui = false;
            drawTimeGui = false;
            drawDmx = true;
            drawMovementController = true;
            drawPosOffset = false;
            drawKineticVisualizationFbo = true;
            drawMusicPlayer = false;
            drawScheduler = false;
        }
        else if(page == 6){ //Page = 6 // ricci mode
            drawDebugGui = {false,false,true};
            drawSpeedAccelGui = false;
            drawPosGui = true;
            drawTimeGui = false;
            drawDmx = false;
            drawMovementController = false;
            drawPosOffset = false;
            drawKineticVisualizationFbo = true;
            drawMusicPlayer = false;
            drawScheduler = false;
        }
        else{ //Page = 7 // mp3
            movementMode = 4;
            drawDebugGui = {false,false,true};
            drawSpeedAccelGui = false;
            drawPosGui = true;
            drawTimeGui = true;
            drawDmx = false;
            drawMovementController = false;
            drawPosOffset = false;
            drawKineticVisualizationFbo = true;
            drawMusicPlayer = false;
            drawScheduler = false;
        }
    } //debug mode end
    else{ //exhibition mode
        
        
        if(movementMode == 0){
            
            drawDebugGui = {false,false,false};
            drawSpeedAccelGui = false;
            drawPosGui = false;
            drawTimeGui = false;
            drawDmx = false;
            drawMovementController = false;
            drawPosOffset = false;
            drawKineticVisualizationFbo = true;
            drawMusicPlayer = false;
            drawScheduler = true;
            
        }else if(movementMode == 1){
            
            drawDebugGui = {false,false,false};
            drawSpeedAccelGui = false;
            drawPosGui = false;
            drawTimeGui = false;
            drawDmx = true;
            drawMovementController = false;
            drawPosOffset = false;
            drawKineticVisualizationFbo = true;
            drawMusicPlayer = false;
            drawScheduler = true;
            
        }else if(movementMode == 2){
            
            drawDebugGui = {false,false,false};
            drawSpeedAccelGui = false;
            drawPosGui = true;
            drawTimeGui = false;
            drawDmx = false;
            drawMovementController = true;
            drawPosOffset = false;
            drawKineticVisualizationFbo = true;
            drawMusicPlayer = false;
            drawScheduler = true;
            
        }else if(movementMode == 3){
            
            drawDebugGui = {false,false,false};
            drawSpeedAccelGui = false;
            drawPosGui = true;
            drawTimeGui = false;
            drawDmx = false;
            drawMovementController = true;
            drawPosOffset = false;
            drawKineticVisualizationFbo = true;
            drawMusicPlayer = false;
            drawScheduler = false;
        }else if(movementMode == 4){
            
            drawDebugGui = {false,false,false};
            drawSpeedAccelGui = false;
            drawPosGui = true;
            drawTimeGui = true;
            drawDmx = false;
            drawMovementController = true;
            drawPosOffset = false;
            drawKineticVisualizationFbo = true;
            drawMusicPlayer = true;
            drawScheduler = false;
        }
        else if(movementMode == 5){
            
            drawDebugGui = {false,false,false};
            drawSpeedAccelGui = false;
            drawPosGui = true;
            drawTimeGui = true;
            drawDmx = false;
            drawMovementController = true;
            drawPosOffset = false;
            drawKineticVisualizationFbo = true;
            drawMusicPlayer = true;
            drawScheduler = false;
        }
    }
    
    
}


//--------------------------------------------------------------
//--------------------- Cable Control Commands -----------------
//--------------------------------------------------------------

//============================ Set Position ====================
void ofApp::setPoints(){
    cableOption();
    vector<ofPoint> mcPoints;
    mcPoints = movementController.getPoints();
    
    
    for(int i=0; i < NUM_OF_CABLES; i++){
        for(int j=0; j < 8; j++){
            if(output_pts[j]){
                if(j==0){
                    cablePosLx[i] = mcPoints[i].y;
                    if (cablePosLx[i]<= 0){
                        cablePosLx[i] = 0;
                    }
                    cablePosLx[i]+= cablePosLxOffset[i];
                    if(cablePosLx[i] >= MAX_X_POS){
                        cablePosLx[i] = MAX_X_POS;
                    }
                }else if(j==1){
                    cablePosLy[i] = mcPoints[i].y*(MAX_Y_POS/MAX_X_POS);
                    if (cablePosLy[i]<= 0){
                        cablePosLy[i] = 0 ;
                    }
                    cablePosLy[i] += cablePosLyOffset[i];
                    if(cablePosLy[i] >= MAX_Y_POS){
                        cablePosLy[i] = MAX_Y_POS;
                    }
                }else if(j==2){
                    cablePosRx[i] = mcPoints[i].y ;
                    if (cablePosRx[i]<= 0){
                        cablePosRx[i] = 0;
                    }
                    cablePosRx[i] += cablePosRxOffset[i];
                    if(cablePosRx[i] >= MAX_X_POS){
                        cablePosRx[i] = MAX_X_POS;
                    }
                }else if(j==3){
                    cablePosRy[i] = mcPoints[i].y*(MAX_Y_POS/MAX_X_POS);
                    if (cablePosRy[i]<= 0){
                        cablePosRy[i] = 0;
                    }
                    cablePosRy[i] += cablePosRyOffset[i];
                    if(cablePosRy[i] >= MAX_Y_POS){
                        cablePosRy[i] = MAX_Y_POS;
                    }
                }
                else  if(j==4){
                    cablePosLx2[i] = mcPoints[i].y;
                    if (cablePosLx2[i]<= 0){
                        cablePosLx2[i] = 0;
                    }
                    cablePosLx2[i]+= cablePosLxOffset[i];
                    if(cablePosLx2[i] >= MAX_X_POS){
                        cablePosLx2[i] = MAX_X_POS;
                    }
                }else if(j==5){
                    cablePosLy2[i] = mcPoints[i].y*(MAX_Y_POS/MAX_X_POS);
                    if (cablePosLy2[i]<= 0){
                        cablePosLy2[i] = 0;
                    }
                    cablePosLy2[i] += cablePosLyOffset[i];
                    if(cablePosLy2[i] >= MAX_Y_POS){
                        cablePosLy2[i] = MAX_Y_POS;
                    }
                }else if(j==6){
                    cablePosRx2[i] = mcPoints[i].y;
                    if (cablePosRx2[i]<= 0){
                        cablePosRx2[i] = 0;
                    }
                    cablePosRx2[i] += cablePosRxOffset[i];
                    if(cablePosRx2[i] >= MAX_X_POS){
                        cablePosRx2[i] = MAX_X_POS;
                    }
                }else if(j==7){
                    cablePosRy2[i] = mcPoints[i].y*(MAX_Y_POS/MAX_X_POS) + cablePosRyOffset[i];
                    if (cablePosRy2[i]<= 0){
                        cablePosRy2[i] = 0;
                    }
                    cablePosRy2[i] += cablePosRyOffset[i];
                    if(cablePosRy2[i] >= MAX_Y_POS){
                        cablePosRy2[i] = MAX_Y_POS;
                    }
                }
            }
        }
    }
}



//============================ Cable Option ====================
void ofApp::cableOption(){
    /*
     0: LX
     1: LY
     2: RX
     3: RY
     4: X (L+R)
     5: Y (L+R)
     6: LX + LY
     7: RX + RY
     8: TFTF
     9: FTFT
     */
    if(cableOp == 0){
        //set LX
        output_pts[0] = true;
        output_pts[2] = false;
        
        output_pts[1] = false;
        output_pts[3] = false;
    }else if(cableOp == 1){
        //set LY
        output_pts[0] = false;
        output_pts[2] = false;
        
        output_pts[1] = true;
        output_pts[3] = false;
    }else if(cableOp == 2){
        //set RX
        output_pts[0] = false;
        output_pts[2] = true;
        
        output_pts[1] = false;
        output_pts[3] = false;
    }else if(cableOp == 3){
        //set RY
        output_pts[0] = false;
        output_pts[2] = false;
        
        output_pts[1] = false;
        output_pts[3] = true;
    }else if(cableOp == 4){
        //set X (L+R)
        output_pts[0] = true;
        output_pts[2] = true;
        
        output_pts[1] = false;
        output_pts[3] = false;
    }else if(cableOp == 5){
        //set Y (L+R)
        output_pts[0] = false;
        output_pts[2] = false;
        
        output_pts[1] = true;
        output_pts[3] = true;
    }else if(cableOp == 6){
        output_pts[1] = true;
        output_pts[3] = false;
        
        output_pts[0] = true;
        output_pts[2] = false;
    }else if(cableOp == 7){
        output_pts[1] = false;
        output_pts[3] = true;
        
        output_pts[0] = false;
        output_pts[2] = true;
    }else if(cableOp == 8){
        output_pts[1] = false;
        output_pts[3] = true;
        
        output_pts[0] = true;
        output_pts[2] = false;
    }else if(cableOp == 9){
        output_pts[1] = true;
        output_pts[3] = false;
        
        output_pts[0] = false;
        output_pts[2] = true;
    }
    
}

//============================ movement ============================
void ofApp::movement(int s){
    
    if(prevMovement != s){
        currCableID =-1;
        currMovementStatge = 0;
        prevMovement = s;
        timeDiff = 0;
        prevTime = currTime;
        ofLog() << "Song Reset";
        triggerTime = 0;
        sendTrigger = false;
    }
    
    if(s == 0){ // ALL OFF
        //----- INFO -----
        ofBackground(0, 0, 100);
        ss_info << "MODE : ALL OFF" << endl;
        
        //---- BEGIN -----
        if(currTime - prevTime >timeDiff){
            
            prevTime = currTime;
            setPattern = true;
            
            timeDiff = 600;
        }
        if(setPattern){
            if(currMovementStatge == 0){
                ofLog() << "ALL OFF : " << currMovementStatge;
                DmxLight.setAll((float)0.0, (float)0.0, (float)0.0, (float)0.0);
                setPattern = false;
                currMovementStatge++;
            }
            else if(currMovementStatge == 1){
                //reset Here do nth.
            }
        }
    }
    else if(s == 1){ // Light ON Mode Only
        //----- INFO -----
        ofBackground(0, 0, 100);
        ss_info << "MODE : LIGHT ONLY (No Shape)" << endl;
        
        DmxLight.setAll((float)1.0, (float)1.0, (float)1.0, (float)1.0);
        
        //---- BEGIN -----
        if(currTime - prevTime >timeDiff){
            
            prevTime = currTime;
            setPattern = true;
            
            timeDiff = 600;
            
        }
        if(setPattern){
            if(currMovementStatge == 0){
                ofLog() << "LIGHTS ON Only : " << currMovementStatge;
                DmxLight.setAll((float)1.0, (float)1.0, (float)1.0, (float)1.0);
                setPattern = false;
                currMovementStatge++;
            }
            else if(currMovementStatge == 1){
                //rest Here do nth.
            }
        }
        
    }else if(s == 2){ // Light ON With One Shape
        //----- INFO -----
        ofBackground(0, 0, 100);
        currStyle = 12;
        ss_info << "MODE : LIGHT ONLY (With Static Shape), " << " Stage 4 of "<< currMovementStatge <<endl;
        
        DmxLight.setAll((float)1.0, (float)1.0, (float)1.0, (float)1.0);
        
        //---- BEGIN -----
        if(currTime - prevTime >timeDiff){
            
            prevTime = currTime;
            setPattern = true;
            
            timeDiff = 30000;
            
        }
        if(setPattern){
            if(currMovementStatge == 0){
                DmxLight.setAll((float)1.0, (float)1.0, (float)1.0, (float)1.0);
                
                setPattern = false;
                currMovementStatge++;
                
            }
            if(currMovementStatge == 1){
                
                cableOp = 4;
                movementController.setOption(0, 0);
                movementController.setPoints(1,1,0,0,0,0);
                setPoints();
                
                setPattern = false;
                currMovementStatge++;
                
                
            }
            else if(currMovementStatge == 2){
                
                cableOp = 5;
                movementController.setOption(0, 1);
                movementController.setPoints(0,1,40,56,1208,208);
                setPoints();
                
                ofLog() << "stage 2";
                setPattern = false;
                currMovementStatge++;
                
            }else if(currMovementStatge == 3){
                writeStyle(1);
                currMovementStatge++;
            }else if(currMovementStatge == 4){
                
            }
        }
        
    }else if(s == 3){ // Light ON and Many Shapes In LOOP
        //----- INFO -----
        ofBackground(0, 0, 150);
        currStyle = 12;
        vector<int> mA, mB;
        int trigger = false;
        
        currMovementStatge = movements.getCurrShape();
        ss_info << "MODE : LIGHT and Many SHAPES : " << " Stage "<< movements.getTotalShapes()-1 <<" of "<<  currMovementStatge << endl;
        
        //---- BEGIN -----
        timeGap = 300000;
        
        if(currTime>prevTime){
            setPattern = true;
            prevTime = currTime + timeGap;
        }
        
        
        if(setPattern){
            
            DmxLight.setAll((float)1.0, (float)1.0, (float)1.0, (float)1.0);
            
            mA = movements.setShapeA();
            
            for(int i = 0 ; i < mA.size(); i++){
                ofLog() << "mA " << mA[i];
            }
            cableOp = mA[0];
            movementController.setPoints(0,mA[1],mA[2],mA[3],mA[4],mA[5]);
            movementController.setPoints(1,mA[6],mA[7],mA[8],mA[9],mA[10]);
            setPoints();
            
            mB = movements.setShapeB();
            cableOp = mB[0];
            for(int i = 0 ; i < mB.size(); i++){
                ofLog() << "mB " << mB[i];
            }
            movementController.setPoints(0,mB[1],mB[2],mB[3],mB[4],mB[5]);
            movementController.setPoints(1,mB[6],mB[7],mB[8],mB[9],mB[10]);
            setPoints();
            
            triggerTime = currTime + 500;
            sendTrigger = true;
            
            movements.incrementShape();
            
            setPattern = false;
            
        }
        
        if(currTime>triggerTime && sendTrigger){
            writeStyle(1);
            sendTrigger = false;
        }
        
    }else if(s == 4){ //MP3
        
        //----- INFO -----
        ofBackground(120, 0, 120);
        currStyle = 12;
        
        ss_info << "MODE : MP3 : " << endl;
        
        DmxLight.setAll((float)1.0, (float)1.0, (float)1.0, (float)1.0);
        
        if(currMovementStatge == 0){
            
            //int option = 0;
            cableOp = 5;
            
            movementController.setPoints(0,1,(int)ofRandom(30,90), ofRandom(35,37),(int)ofRandom(0,187),(int)ofRandom(0,1000));
            
            currMovementStatge++;
            
        }else if (currMovementStatge == 1){
            
            timelinePlayer.setCurrentTime((int)(musicPlayer.getCurrentPos() * (float)currMusicDuration));
            ss_info << "Current MP3 Pos : " << (int)(musicPlayer.getCurrentPos() * (float)currMusicDuration)  << endl;
            
        }else{
            
        }
        
    }else if(s == 5){  // ALL SAME
        
        if(currMovementStatge == 0){
            if(currTime - prevTime >timeDiff){
                timeDiff = ofRandom(60000,90000);
                ofLog() << "timeDiff ; "<< timeDiff;
                prevTime = currTime;
                setPattern = true;
                
                //DmxLight.setAll((float)1.0, (float)1.0, (float)1.0, (float)1.0);
                
            }
            if(setPattern){
                
                int option = (int)ofRandom(4);
                cableOp = option;
                
                movementController.setPoints(0,1,(int)ofRandom(30,90), ofRandom(35,37),(int)ofRandom(0,187),(int)ofRandom(0,1000));
                
                setPattern = false;
                ofLog() << "Set Pattern";
                
                writeStyle(1);
            }
        }
    }else if(s == 6){ //ricci mode, debug one by one
        
        currStyle = 11;
        
        if(currMovementStatge == 0){
            output_pts[0] = true;
            output_pts[2] = true;
            
            output_pts[1] = true;
            output_pts[3] = true;
            
            movementController.setPoints(0,1,(int)ofRandom(0,0), ofRandom(0,0),(int)ofRandom(68,69),(int)ofRandom(0,1000));
            
            currCableID = 0;
            currMovementStatge++;
        }else{
            output_pts[0] = false;
            output_pts[2] = false;
            
            output_pts[1] = false;
            output_pts[3] = false;
        }
        
        if(setPattern){
            setPattern = false;
        }
    }else{
        
    }
}


void ofApp::setShapes(int op0, vector<int> a, vector<int> b, int op1, vector<int> c, vector<int> d){
    
    
}

void ofApp::moveCommandMethod(int method, int c, int whichCurrentCable){
    string writeInTotal = "";
    string toWrite = "";
    
    if(method == 11){ //moveTo with speed and accel control
        writeInTotal = "LX : ";
        
        toWrite = "";
        
        toWrite+= ofToString(method);
        toWrite+= "-";
        
        toWrite+= ofToString((int)cableSpeedLx[c]);
        toWrite+= "-";
        toWrite+= ofToString((int)cableAccelLx[c]);
        toWrite+= "-";
        toWrite+= ofToString((int)cablePosLx[c]);
        toWrite+= "-";
        
        writeInTotal=toWrite + " LY:";
        
        toWrite+= ofToString((int)cableSpeedLy[c]);
        toWrite+= "-";
        toWrite+= ofToString((int)cableAccelLy[c]);
        toWrite+= "-";
        toWrite+= ofToString((int)cablePosLy[c]);
        toWrite+= "-";
        
        writeInTotal=toWrite +" RX: ";
        
        toWrite+= ofToString((int)cableSpeedRx[c]);
        toWrite+= "-";
        toWrite+= ofToString((int)cableAccelRx[c]);
        toWrite+= "-";
        toWrite+= ofToString((int)cablePosRx[c]);
        toWrite+= "-";
        
        writeInTotal=toWrite +" RY: ";;
        
        toWrite+= ofToString((int)cableSpeedRy[c]);
        toWrite+= "-";
        toWrite+= ofToString((int)cableAccelRy[c]);
        toWrite+= "-";
        toWrite+= ofToString((int)cablePosRy[c]);
    }
    else if (method == 12){//moveTo with time
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
    else if (method == 2)//moveTo within 2 position with speed and accel control
    {
        writeInTotal = "LX : ";
        
        toWrite = "";
        
        toWrite+= ofToString(method);
        toWrite+= "-";
        
        toWrite+= ofToString((int)cableSpeedX);
        toWrite+= "-";
        toWrite+= ofToString((int)cableAccelX);
        toWrite+= "-";
        toWrite+= ofToString((int)cableSpeedY);
        toWrite+= "-";
        toWrite+= ofToString((int)cableAccelY);
        toWrite+= "-";
        
        writeInTotal=toWrite + " LX:";
        
        toWrite+= ofToString((int)cablePosLx[c]);
        toWrite+= "-";
        toWrite+= ofToString((int)cablePosLx2[c]);
        toWrite+= "-";
        
        writeInTotal=toWrite + " LY:";
        
        toWrite+= ofToString((int)cablePosLy[c]);
        toWrite+= "-";
        toWrite+= ofToString((int)cablePosLy2[c]);
        toWrite+= "-";
        
        writeInTotal=toWrite +" RX: ";
        
        toWrite+= ofToString((int)cablePosRx[c]);
        toWrite+= "-";
        toWrite+= ofToString((int)cablePosRx2[c]);
        toWrite+= "-";
        
        writeInTotal=toWrite +" RY: ";;
        
        toWrite+= ofToString((int)cablePosRy[c]);
        toWrite+= "-";
        toWrite+= ofToString((int)cablePosRy2[c]);
        
        
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
    currDisplayLog[NUM_OF_CABLES] = "write Style : " + ofToString(s) + " currCableID : " + ofToString(currCableID);
    for(int i=0; i< NUM_OF_CABLES; i++){
        if(prevCablePosLx[i] != cablePosLx[i]){
            cableLxDist[i] = abs(prevCablePosLx[i] - cablePosLx[i]);
            prevCablePosLx[i] = cablePosLx[i];
        }
        if(prevCablePosLy[i] != cablePosLy[i]){
            cableLyDist[i] = abs(prevCablePosLy[i] - cablePosLy[i]);
            prevCablePosLy[i] = cablePosLy[i];
        }
        if(prevCablePosRx[i] != cablePosRx[i]){
            cableRxDist[i] = abs(prevCablePosRx[i] - cablePosRx[i]);
            prevCablePosRx[i] = cablePosRx[i];
        }
        if(prevCablePosRy[i] != cablePosRy[i]){
            cableRyDist[i] = abs(prevCablePosRy[i] - cablePosRy[i]);
            prevCablePosRy[i] = cablePosRy[i];
        }
    }
    
    for(int i=0; i< NUM_OF_CABLES; i++){
        int xFactor = 1;
        int yFactor = 1;
        cableSpeedLx[i] = ofMap(prevCablePosLx[i],0,MAX_X_POS,MIN_X_SPEED,MAX_X_SPEED);
        if (cableSpeedLx[i]< MIN_X_SPEED){
            cableSpeedLx[i] = MIN_X_SPEED;
        }
        cableSpeedLy[i] = ofMap(prevCablePosLy[i],0,MAX_Y_POS,MIN_Y_SPEED,MAX_Y_SPEED);
        if (cableSpeedLy[i]< MIN_Y_SPEED){
            cableSpeedLy[i] = MIN_Y_SPEED;
        }
        cableSpeedRx[i] = ofMap(prevCablePosRx[i],0,MAX_X_POS,MIN_X_SPEED,MAX_X_SPEED);
        if (cableSpeedRx[i]< MIN_X_SPEED){
            cableSpeedRx[i] = MIN_X_SPEED;
        }
        cableSpeedRy[i] = ofMap(prevCablePosRy[i],0,MAX_Y_POS,MIN_Y_SPEED,MAX_Y_SPEED);
        if (cableSpeedRy[i]< MIN_Y_SPEED){
            cableSpeedRy[i] = MIN_Y_SPEED;
        }
    }
    
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


void ofApp::offsetHome(){
    cableOp = 5;
    movementController.setPoints(0, 1, 0, 0, 0, 0);
    setPoints();
    
    cableOp = 4;
    movementController.setPoints(0, 1, 0, 0, 0, 0);
    setPoints();
    
    writeStyle(1);
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
            if(working_cable[i]){
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
            }
        }
    }
    else if (isArduinoConnected[arduinoID]==TRUE && arduinoID >= 0 && working_cable[arduinoID])
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

void ofApp::loadEEPROMButtonPressed(){
    serialWrite(currCableID, "L");
    serialTrigger = false;
}

void ofApp::checkArduinoIsConnected(){
    
    if(ofGetFrameNum() < 200){
        
        if(ofGetFrameNum() %30 == 0){
            // for(int i=0; i< arduino.size(); i++){
            // serialWrite(i, "C");
            //      ofSleepMillis(100);
            //  }
            serialWrite(-1, "C");
            ofLog() << "hello";
        }
    }
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
        currDisplayLog[NUM_OF_CABLES] = ofToString(currCableID) +" EEPROM LOADED";
        return sToIntArray;
    }
    else{
        // vector<int> sToIntArray;
        return sToIntArray;
    }
    
}

//--------------------------------------------------------------
//---------------------- Music Timeline Player------------------
//--------------------------------------------------------------

//====================== Show Control ==========================
void ofApp::setupMusicPlayerAndTimeline(){
    drawMusicPlayer = false;
    musicPlayer.setup();
    
    timelinePlayer.setup(NUM_TIMELINE);
    ofAddListener(timelinePlayer.onKeyFrameEntered, this, &ofApp::onKeyframe);
    timelinePlayer.loadButtonPressed();  // load keyframe once;
    
    isExhibitionMode = true;
    timelinePlayer.exhibitionMode(isExhibitionMode); //TODO
    
    // ofLog() << "ffMovie.getDuration() : " << ffmovie.getDuration();
    // timelinePlayer.setDuration(ffMovie.getDuration()*1000);
    
    showBeginTrigger = false;
    
    ofAddListener(timelinePlayer.onPlayStatus, this, &ofApp::playTrack);
    ofAddListener(timelinePlayer.onPauseStatus, this, &ofApp::pauseTrack);
    ofAddListener(timelinePlayer.onSliderChange, this, &ofApp::changeTrackPos);
    ofAddListener(timelinePlayer.onVolumeChange, this, &ofApp::changeVolume);
}

void ofApp::isShowBegin(bool sb){
    if(sb){ //begin
        prevShowBeginMillis = currTime;
        showBeginTrigger = true;
        page = 7;
        currStyle = 12;
    }
    else{ //reset
        timelinePlayer.stopButtonPressed();
        serialWrite(-1, "Q"); //Reset and Home All
    }
}

void ofApp::onKeyframe(Keyframe &kf){
    
    //0: option
    //1: C0
    //2: C1
    //3: D2
    //4: C3
    
    
    if(kf.timelineId  == 0 || kf.timelineId == 7){
        currDisplayLog[NUM_OF_CABLES] = "KeyFrame At " + ofToString(kf.timelineId);
        vector<float> a;
        a = timelinePlayer.getTimelineTweenValues();
        
        if(kf.timelineId  == 0){
            cableOp = a[0];
            movementController.setOption(1,0);
            movementController.setPoints(0, 1, a[1], a[2], a[3], a[4]);
        }else if(kf.timelineId  == 7){
            cableOp = a[7];
            movementController.setOption(0,0);
            movementController.setPoints(1, 1, a[8], a[9], a[10], a[11]);
        }
        setPoints();
        writeStyle(1);
    }
    
    
    /*
     if(kf.timelineId < NUM_OF_CABLES *2  && kf.timelineId%2==0){ //LY
     ofLog() << "LY HAS KEYFRAME : " << kf.timelineId << " " << kf.val << " " << kf.x;
     currCableID = kf.timelineId /2 ;
     cableTimeLy[currentArduinoID] = timelinePlayer.getTimelineValue(kf.timelineId + 1, kf.x);
     cableTimeRy[currentArduinoID] = 0; //means no input
     currentMotor = 0;
     writeStyleMode(2);
     
     }else if(kf.timelineId >= NUM_OF_CABLES *2  && kf.timelineId%2==0 && kf.timelineId < NUM_OF_CABLES *4){ //RY
     currCableID = kf.timelineId /2 - NUM_OF_CABLES;
     currentMotor = 1;
     cableTimeLy[currentArduinoID] = 0; //means no input
     cableTimeRy[currentArduinoID] = timelinePlayer.getTimelineValue(kf.timelineId + 1, kf.x);
     writeStyleMode(2);
     ofLog() << "RY HAS KEYFRAME : " << kf.timelineId << " " << kf.val << " " << kf.x;
     }else{
     
     // currentArduinoID = kf.timelineId /2 - NUM_OF_CABLES;
     //currentMotor = 1;
     currentArduinoID = 0;
     cableTimeLy[currentArduinoID] = 0; //means no input
     cableTimeRy[currentArduinoID] = 0;//means no input
     writeStyleMode(0);
     ofLog() << "LED HAS KEYFRAME : " << kf.timelineId << " " << kf.val << " " << kf.x;
     //ofLog() << "LED " << LEDStyle;
     // writeStyleMode(2);
     }
     */
}

void ofApp::changeTrackPos(float & pos){
    musicPlayer.setPos(pos);
}

void ofApp::changeVolume(float & vol){
    musicPlayer.setVol(vol);
}

void ofApp::playTrack(bool & t){
    currMusicDuration = musicPlayer.getDuration();
    timelinePlayer.loadButtonPressed();
    // timelinePlayer.playButtonPressed();
    timelinePlayer.setDuration(currMusicDuration);
    musicPlayer.play();
    
    currDisplayLog[NUM_OF_CABLES] = "Playing Timeline";
}

void ofApp::pauseTrack(bool & t){
    musicPlayer.pause(t);
    ofLog() << "is paused : ";
}

void ofApp::setTrackisLoop(bool t){
    timelinePlayer.setLoop(t);
}

//--------------------------------------------------------------
//------------------------- Movements --------------------------
//--------------------------------------------------------------
void ofApp::onSetPoints(bool & t){
    setPoints();
}


//--------------------------------------------------------------
//------------------------- Scheduler --------------------------
//--------------------------------------------------------------
void ofApp::onSchedulerLightsToggle(bool & t){
    if(t){
        currDisplayLog[NUM_OF_CABLES] = "Lights On";
        movementMode = defaultMovementMode;
    }else{
        currDisplayLog[NUM_OF_CABLES] = "Lights Off, Static Mode";
        movementMode = 0;
    }
    ofLog() << currDisplayLog[NUM_OF_CABLES];
}

void ofApp::onSchedulerHomeResetToggle(int & t){
    if(t == 0 ){
        currDisplayLog[NUM_OF_CABLES] = "Reset Home";
        serialWrite(-1, "Q");
    }else{
        currDisplayLog[NUM_OF_CABLES] = "Offset Home";
        offsetHome();
    }
    ofLog() << currDisplayLog[NUM_OF_CABLES];
}



//--------------------------------------------------------------
//---------------------- SETTINGS / XML-------------------------
//--------------------------------------------------------------

void ofApp::saveSettings()
{
    for(int i=0; i < NUM_OF_CABLES; i++){
        cableXML.setValue("LX"+ ofToString(i),ofToString(cablePosLxOffset[i]));
        cableXML.setValue("LY"+ ofToString(i),ofToString(cablePosLyOffset[i]));
        cableXML.setValue("RX"+ ofToString(i),ofToString(cablePosRxOffset[i]));
        cableXML.setValue("RY"+ ofToString(i),ofToString(cablePosRyOffset[i]));
    }
    cableXML.save("cableSettings.xml");
    
    XML.save("XMLSettings.xml");
    
    ofLog() << "XML Setting Saved";
    
    
    std::string file = "settingsMode.json";
    
    // Now parse the JSON
    bool parsingSuccessful = settingsMode.open(file);
    
    if (parsingSuccessful)
    {
        
        //settings["day"] = day;
        
        // now write pretty print
        if (!settingsMode.save(file, true))
        {
            ofLogNotice("ofApp::setup") << "settings.json written unsuccessfully.";
        }
        else
        {
            ofLogNotice("ofApp::setup") << "settings.json written successfully.";
        }
    }
    else
    {
        ofLogError("ofApp::setup") << "Failed to parse JSON" << endl;
    }
    
    
    
    
}
void ofApp::loadSettings()
{
    cableXML.load("cableSettings.xml");
    for(int i=0; i < NUM_OF_CABLES; i++){
        cablePosLxOffset[i] = cableXML.getValue("LX"+ ofToString(i),0);
        cablePosLyOffset[i] = cableXML.getValue("LY"+ ofToString(i),0);
        cablePosRxOffset[i] = cableXML.getValue("RX"+ ofToString(i),0);
        cablePosRyOffset[i] = cableXML.getValue("RY"+ ofToString(i),0);
    }
    XML.load("XMLSettings.xml");
    movementMode = XML.getValue<int>("MODE");
    ofLog() << "MODE VAL : " << movementMode;
    ofLog() << "XML Setting Loaded";
    
    
    
    std::string file = "settingsMode.json";
    
    // Now parse the JSON
    bool parsingSuccessful = settingsMode.open(file);
    
    if (parsingSuccessful)
    {
        ofLogNotice("ofApp::setup") << settingsMode.getRawString();
        //movementMode =  settingsMode["mode"].asInt();
        defaultMovementMode = settingsMode["mode"].asInt();
        movementMode = defaultMovementMode;
    }
    else
    {
        ofLogError("ofApp::setup") << "Failed to parse JSON" << endl;
    }
    
    
}

//--------------------------------------------------------------
//---------------------- OTHER EVENTS --------------------------
//--------------------------------------------------------------
void ofApp::exit()
{
    serialWrite(-1, "V");
#ifdef USEOSC
#else
    for(int i=0; i< arduino.size(); i++){
        arduino[i].unregisterAllEvents(this);
    }
#endif
    DmxLight.exit();
}

//--------------------------------------------------------------
//--------------------------UNUSED -----------------------------
//--------------------------------------------------------------

//TODO: Unused for now

//--------------------------------------------------------------
void ofApp::removeSubstrs(string& s, string& p) {
    string::size_type n = p.length();
    for (string::size_type i = s.find(p);
         i != string::npos;
         i = s.find(p))
        s.erase(i, n);
}




