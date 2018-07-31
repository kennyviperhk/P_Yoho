#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    
    ofSetFrameRate(25);
    
    //================== Serial ==================
    
    isArduinoConnected = serialSetup();
    
    SERIAL_PARAMETERES = {"sa", "lo","on", "allhm"}; //save, load, online
    serialTrigger = true;
    prevSerialTriggerMillis = ofGetElapsedTimeMillis();
    
    for(int i=0; i < NUM_OF_CABLES; i++){
        isArduinoConnectedBySerial.push_back(false);
    }
    initOnUpdate = true;
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
    
    currentdisplayLog = "Park Yoho Control Software V1";
    
    loadSettings();
    guiSetup();
    
    //================== Music Player ==================
    
    drawMusicPlayer = false;
    musicPlayer.setup();
    
    //================== Dmx Light ==================
    
    DmxLight.setup();
    
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
    MovementController.setup(NUM_OF_CABLES,mcX,mcY,mcW,mcH,MAX_X_POS,MAX_Y_POS);
    drawMovementController = false;
    
    //================== Song 1 ==================
    songStage = 0;
    
#ifdef USEOSC
    //================== OSC ==================
    // listen on the given port
    cout << "listening for osc messages on port " << R_PORT << "\n";
    receiver.setup(R_PORT);
    // open an outgoing connection to HOST:PORT
    sender.setup(S_HOST, S_PORT);
#else
    
#endif
}


//--------------------------------------------------------------
void ofApp::update(){
    
    currTime = ofGetElapsedTimeMillis();
    
    ofSetWindowTitle(ofToString(ofGetFrameRate()));
    
    currMillis = ofGetElapsedTimeMillis();
    
    //OSC
#ifdef USEOSC
    receivedString = readOSC();
#else
    
    
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
    if(currMillis - prevOnlineCheckingMillis < 5000){
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
        
        kinecticVisualisation.set(NUM_OF_CABLES ,i, currentStyle ,ofMap(cablePosLx[i],0,MAX_X_POS,0,2) ,ofMap(cablePosLy[i],0,MAX_Y_POS,0,1),ofMap(cablePosRx[i],0,MAX_X_POS,0,2), ofMap(cablePosRy[i],0,MAX_Y_POS,0,1));
        
    }
    
    
    kineticVisualizationFbo.begin();
    ofClear(255, 0, 0,0);
    kinecticVisualisation.draw();
    kineticVisualizationFbo.end();
    
    
    //================== Music Player ==================
    musicPlayer.update();
    
    //================== Dmx Light ==================
    DmxLight.update();
    
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
    
    ss_info.str("");
    
    ss_info << "'space'  --> Emergency Stop : " << endl;
    ss_info << "'d'      --> Debug Mode     : "<< debugMode << endl;
    ss_info << "'e'      --> Movement Mode: "<< movementMode << endl;
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
        ss_info << "MODE : DEBUG MODE" << endl;
        
        ofSetColor(255);
        
        //================== Debug Gui ==================
        
        ss_info << "FrameRate : "<< ofGetFrameRate() << endl;
        ss_info << "Page : "<< page << endl;
        
        ss_info << "Num of Arduino: " << arduino.size() << " / " << NUM_OF_CABLES << endl;

        
        if(page == 0){
            drawDebugGui = {true,true,false};
            drawPosGui = true;
            drawSpeedAccelGui = true;
            drawKineticVisualizationFbo = true;
            drawMusicPlayer = false;
            drawMovementController = false;
            showOffset = false;
            drawDmx = false;
        }else if(page == 1){
            drawDebugGui = {true,true,false};
            drawPosGui = true;
            drawSpeedAccelGui = true;
            drawKineticVisualizationFbo = false;
            drawMusicPlayer = false;
            drawMovementController = false;
            showOffset = true;
            drawDmx = false;
        }else if(page == 2){
            drawDebugGui = {false,false,false};
            drawPosGui = true;
            drawSpeedAccelGui = true;
            drawKineticVisualizationFbo = false;
            drawMusicPlayer = true;
            drawMovementController = false;
            showOffset = false;
            drawDmx = false;
        }else if(page == 3){
            drawDebugGui = {true,true,false};
            drawPosGui = true;
            drawSpeedAccelGui = true;
            drawKineticVisualizationFbo = true;
            drawMusicPlayer = false;
            drawMovementController = true;
            showOffset = false;
            drawDmx = false;
        }else if(page == 4){
            
            drawDebugGui = {true,true,false};
            drawPosGui = true;
            drawSpeedAccelGui = true;
            drawKineticVisualizationFbo = true;
            drawMusicPlayer = false;
            drawMovementController = true;
            showOffset = false;
            drawDmx = false;
            
            guiCablePosLx2.draw();
            guiCablePosLy2.draw();
            guiCablePosRx2.draw();
            guiCablePosRy2.draw();
            guiCableSpeedAccelAll.draw();
            
        }else if(page == 5){
            //================== Dmx Light ==================
            drawDebugGui = {false,false,false};
            drawSpeedAccelGui = true;
            drawDmx = true;
            
        }else{ //Page = 6 // ricci mode
            movementMode = 4;
            drawDebugGui = {false,false,true};
            drawPosGui = true;
            drawSpeedAccelGui = false;
            drawKineticVisualizationFbo = true;
            drawMusicPlayer = false;
            drawMovementController = false;
            showOffset = false;
            drawDmx = false;
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
                serialWrite(currCableID, toWrite);
                serialWrite(currCableID, "S");
                
                currentdisplayLog = ofToString(currCableID) +" EEPROM SAVED";
                serialTrigger = false;
                prevSerialTriggerMillis =currMillis;
                
                
            }
            if(reset_Btn || singleCableResetBtn){
                serialWrite(currCableID, "Q");
            }
            if(singleCableResetAllBtn){
                serialWrite(-1, "Q");
            }
            if(singleCableHomeAllBtn){
                serialWrite(currCableID, "H");
            }
            if(home_Btn){
                if(all_Tog){
                    serialWrite(-1, "H");
                }else{
                    serialWrite(currCableID, "H");
                }
                //manual home "G"
                //serialWrite(currCableID, 1-1-1-1);
                //serialWrite(currCableID, "G");
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
        if(currMillis -  prevSerialTriggerMillis > 200){
            serialTrigger = true;
        }
        
        string t = textField;
        if (t.find('+') != std::string::npos || t.find('=') != std::string::npos)
        {
            t.erase(std::remove(t.begin(), t.end(), '='), t.end());
            t.erase(std::remove(t.begin(), t.end(), '+'), t.end());
            currentdisplayLog = t;
            serialWrite(currCableID, t);
            textField = "";
        }
        
        //Ricci Mode - Single Cable Control
        
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
        
        if(currMillis - prevSingleCableLoopMillis > 10000){
            for(int i=0; i<4; i++){
                if(singleCablePosLoop[i]){
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
                    ofLog() << "Actived : " << i  << " cm " << currMillis << " pm " << prevSingleCableLoopMillis;
                }
            }
            prevSingleCableLoopMillis = currMillis;
            writeStyle(2);
        }
        
        //END Ricci Mode - Single Cable Control
        
        
        
    }
    
    else { //Exhibition Mode

        movement(movementMode);

    }
    
    guiDraw();
    setPoints();
    
    
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
        ofDrawBitmapString(ss2.str(), ofVec2f(20, 20*i + 150));
        
    }
    
    
    if(drawKineticVisualizationFbo){
        ofSetColor(255);
        kineticVisualizationFbo.draw(0,100,ofGetWidth()*0.7,ofGetHeight()*0.7);
    }
    
    ofSetColor(255);
    ofDrawBitmapString(ss_info.str(), ofVec2f(20, 20));
    
    displayLog(currentdisplayLog);
}

//--------------------------------------------------------------
//------------------- Keyboard Command -------------------------
//--------------------------------------------------------------
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
            
        case 'm': //drawMusicPlayer
            drawMusicPlayer = !drawMusicPlayer;
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
                page = 6;
            }
            
            break;
            
        case 'e':
            movementMode++;
            if(movementMode > 4){
                movementMode = 0;
            }
            XML.setValue("MODE", ofToString(movementMode));
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
            
        default:
            break;
            
    }
    ofLog() << "page " << page;
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
    // ofLog() << "sending OSC : " << ar << " : " << s;
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
    serialWrite(currCableID, txt);
    
}
//--------------------------------------------------------------
//-------------------------- GUI -------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
void ofApp::guiSetup(){
    drawDebugGui = {false,false,false};
    
    //--- EEPROM ---
    parametersDebug.setName("settings");
    guiDebug.setup("EEPROMReadWrite", "settings.xml", ofGetWidth() - 630, 0);
    
    guiDebug.setSize(80, 1000);
    guiDebug.setWidthElements(80);
    
    guiDebug2.setup("EEPROMReadWrite", "settings.xml", ofGetWidth() - 730, 0);
    guiDebug2.setSize(60, 1000);
    guiDebug2.setWidthElements(60);
    
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
    EEPROM_loadBtn.addListener(this, &ofApp::loadButtonPressed);
    guiDebug.add(textField.setup("Serial:", "0-0-0-0-0"));
    guiDebug.add(currentStyle.set("Style",11,0,NUM_OF_CABLES)); //TODO
    guiDebug.add(sendStyleBtn.setup("Set Pos:"));
    guiDebug.add(sendStyleBtn_all_same.setup("Set Pos ALL SAME:"));
    guiDebug.add(sendStyleBtn_all.setup("Set Pos ALL:"));
    guiDebug.add(reset_Btn.setup("Reset:"));
    guiDebug.add(home_Btn.setup("Home: "));
    guiDebug.add(all_Tog.setup("ALL TOGGLE:",false));
    
    all_Tog = false;
    int numOfWaveForm = 4;
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
        guiDebug.add(input_pts[i]);
        guiDebug.add(output_pts[i]);
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
    showOffset = false;
    
    parametersCablePosOffset.setName("cablePositionOffset");
    guiCablePosLxOffset.setup("EEPROMReadWrite", "settings.xml",  100, 0);
    guiCablePosLyOffset.setup("EEPROMReadWrite", "settings.xml",  100, 400);
    guiCablePosRxOffset.setup("EEPROMReadWrite", "settings.xml",  200, 0);
    guiCablePosRyOffset.setup("EEPROMReadWrite", "settings.xml",  200, 400);
    for(int i=0; i< NUM_OF_CABLES; i++){
        ofParameter<int> a;
        a.set("OffS Lx" + ofToString(i+1),0,0,MAX_X_POS*0.1); //lx,ly,rx,ry
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
    
    
    //--- Cable Time Control ---
    parametersCableTime.setName("cableTime");
    guiCableTimeLx.setup("EEPROMReadWrite", "settings.xml", ofGetWidth() - 410, 0);
    guiCableTimeLy.setup("EEPROMReadWrite", "settings.xml", ofGetWidth() - 410, 400);
    guiCableTimeRx.setup("EEPROMReadWrite", "settings.xml", ofGetWidth() - 300, 0);
    guiCableTimeRy.setup("EEPROMReadWrite", "settings.xml", ofGetWidth() - 300, 400);
    for(int i=0; i< NUM_OF_CABLES; i++){
        ofParameter<int> a;
        a.set("T" + ofToString(i+1),2000,0,MAX_X_TIME); //lx,ly,rx,ry
        ofParameter<int> b;
        b.set("T" + ofToString(i+1),2000,0,MAX_Y_TIME);
        ofParameter<int> c;
        c.set("T" + ofToString(i+1),2000,0,MAX_X_TIME);
        ofParameter<int> d;
        d.set("T" + ofToString(i+1),2000,0,MAX_Y_TIME);
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
        a.set("A" + ofToString(i+1),200,0,MAX_X_SPEED); //lx,ly,rx,ry
        ofParameter<int> b;
        b.set("A" + ofToString(i+1),500,0,MAX_Y_SPEED);
        ofParameter<int> c;
        c.set("A" + ofToString(i+1),200,0,MAX_X_SPEED);
        ofParameter<int> d;
        d.set("A" + ofToString(i+1),500,0,MAX_Y_SPEED);
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
        a.set("S" + ofToString(i+1),200,0,MAX_X_ACCEL); //lx,ly,rx,ry
        ofParameter<int> b;
        b.set("S" + ofToString(i+1),400,0,MAX_Y_ACCEL);
        ofParameter<int> c;
        c.set("S" + ofToString(i+1),200,0,MAX_X_ACCEL);
        ofParameter<int> d;
        d.set("S" + ofToString(i+1),400,0,MAX_Y_ACCEL);
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


void ofApp::displayLog(string s=""){
    if(s.size() >0){
        currentdisplayLog = s ;
    }
    ofSetColor(255, 255, 255);
    ofDrawBitmapString("Status: " + currentdisplayLog, 10, ofGetHeight()-10);
}


void ofApp::guiDraw(){
    //================== Gui ==================
    if(drawDebugGui[0]){
        guiDebug.draw();
    }
    if(drawDebugGui[1]){
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
    //================== Music Player ==================
    if(drawMusicPlayer){
        musicPlayer.draw();
    }
    
    //================== Dmx Light ==================
    if(drawDmx){
        DmxLight.draw();
    }
    
    //================== Movement Controller ==================
    /*
     for(int i=0; i< MovementControllers.size(); i++){
     MovementControllers[i].draw();
     }
     */
    if(drawMovementController){
        MovementController.draw();
    }
    
    if(showOffset){
        guiCablePosLxOffset.draw();
        guiCablePosLyOffset.draw();
        guiCablePosRxOffset.draw();
        guiCablePosRyOffset.draw();
    }
    
}



//--------------------------------------------------------------
//--------------------------STYLE -----------------------------
//--------------------------------------------------------------

void ofApp::setPoints(){
    vector<ofPoint> mcPoints = MovementController.getPoints();
    
    for(int i=0; i < NUM_OF_CABLES; i++){
        for(int j=0; j < 8; j++){
            if(output_pts[j]){
                if(j==0){
                    cablePosLx[i] = mcPoints[i].y;
                    if(cablePosLx[i] >= MAX_X_POS){
                        cablePosLx[i] = MAX_X_POS;
                    }else if (cablePosLx[i]<= 0){
                        cablePosLx[i] = 0;
                    }
                }else if(j==1){
                    cablePosLy[i] = mcPoints[i].y*(MAX_Y_POS/MAX_X_POS);
                    
                    if(cablePosLy[i] >= MAX_Y_POS){
                        cablePosLy[i] = MAX_Y_POS;
                    }else if (cablePosLy[i]<= 0){
                        cablePosLy[i] = 0;
                    }
                }else if(j==2){
                    cablePosRx[i] = mcPoints[i].y;
                    
                    if(cablePosRx[i] >= MAX_X_POS){
                        cablePosRx[i] = MAX_X_POS;
                    }else if (cablePosRx[i]<= 0){
                        cablePosRx[i] = 0;
                    }
                }else if(j==3){
                    cablePosRy[i] = mcPoints[i].y*(MAX_Y_POS/MAX_X_POS);
                    
                    if(cablePosRy[i] >= MAX_Y_POS){
                        cablePosRy[i] = MAX_Y_POS;
                    }else if (cablePosRy[i]<= 0){
                        cablePosRy[i] = 0;
                    }
                }
                else  if(j==4){
                    cablePosLx2[i] = mcPoints[i].y;
                    if(cablePosLx2[i] >= MAX_X_POS){
                        cablePosLx2[i] = MAX_X_POS;
                    }else if (cablePosLx2[i]<= 0){
                        cablePosLx2[i] = 0;
                    }
                }else if(j==5){
                    cablePosLy2[i] = mcPoints[i].y*(MAX_Y_POS/MAX_X_POS);
                    
                    if(cablePosLy2[i] >= MAX_Y_POS){
                        cablePosLy2[i] = MAX_Y_POS;
                    }else if (cablePosLy2[i]<= 0){
                        cablePosLy2[i] = 0;
                    }
                }else if(j==6){
                    cablePosRx2[i] = mcPoints[i].y;
                    
                    if(cablePosRx2[i] >= MAX_X_POS){
                        cablePosRx2[i] = MAX_X_POS;
                    }else if (cablePosRx2[i]<= 0){
                        cablePosRx2[i] = 0;
                    }
                }else if(j==7){
                    cablePosRy2[i] = mcPoints[i].y*(MAX_Y_POS/MAX_X_POS);
                    
                    if(cablePosRy2[i] >= MAX_Y_POS){
                        cablePosRy2[i] = MAX_Y_POS;
                    }else if (cablePosRy2[i]<= 0){
                        cablePosRy2[i] = 0;
                    }
                }
            }
        }
    }
}

//--------------------------------------------------------------

void ofApp::movement(int s){
    
    if(prevSong != s){
        currCableID =-1;
        songStage = 0;
        prevSong = s;
        timeDiff = 0;
        ofLog() << "Song Reset";
    }
    
    if(s == 0){ // ALL OFF
        //----- INFO -----
        ofBackground(0, 0, 100);
        ss_info << "MODE : ALL OFF" << endl;
        
        drawDebugGui = {false,false,false};
        drawPosGui = false;
        drawSpeedAccelGui = false;
        drawKineticVisualizationFbo = true;
        drawMusicPlayer = false;
        drawMovementController = false;
        showOffset = false;
        drawDmx = false;
        
        //---- BEGIN -----
        if(currTime - prevTime >timeDiff){
            
            prevTime = currTime;
            setPattern = true;
            
            timeDiff = 600;
        }
        if(setPattern){
            if(songStage == 0){
                ofLog() << "ALL OFF : " << songStage;
                DmxLight.setAll((float)0.0, (float)0.0, (float)0.0, (float)0.0);
                setPattern = false;
                songStage++;
            }
            else if(songStage == 1){
                //rest Here do nth.
            }
        }
    }
    else if(s == 1){ // Light ON Mode Only
        //----- INFO -----
        ofBackground(0, 0, 100);
        ss_info << "MODE : LIGHT ONLY (No Shape)" << endl;
        
        drawPosGui = false;
        drawTimeGui = false;
        drawDmx = true;
        drawMovementController = false;
        
        DmxLight.setAll((float)1.0, (float)1.0, (float)1.0, (float)1.0);
        
        //---- BEGIN -----
        if(currTime - prevTime >timeDiff){
            
            prevTime = currTime;
            setPattern = true;
            
            timeDiff = 600;
            
        }
        if(setPattern){
            if(songStage == 0){
                ofLog() << "LIGHTS ON Only : " << songStage;
                DmxLight.setAll((float)1.0, (float)1.0, (float)1.0, (float)1.0);
                setPattern = false;
                songStage++;
            }
            else if(songStage == 1){
                //rest Here do nth.
            }
        }
        
    }else if(s == 2){ // Light ON With One Shape
        //----- INFO -----
        ofBackground(0, 0, 100);
        ss_info << "MODE : LIGHT ONLY (With Static Shape)" << endl;
        
        drawPosGui = false;
        drawTimeGui = false;
        drawDmx = true;
        drawMovementController = false;
        
        DmxLight.setAll((float)1.0, (float)1.0, (float)1.0, (float)1.0);
        
        //---- BEGIN -----
        if(currTime - prevTime >timeDiff){
            
            prevTime = currTime;
            setPattern = true;
            
            timeDiff = 6000;
            
        }
        if(setPattern){
            if(songStage == 0){
                ofLog() << "LIGHTS ON with One Static Shape : " << songStage;
                DmxLight.setAll((float)1.0, (float)1.0, (float)1.0, (float)1.0);
                
                setPattern = false;
                songStage++;
            }
            if(songStage == 1){
                
                //set Y
                output_pts[0] = false;
                output_pts[2] = false;
                
                output_pts[1] = true;
                output_pts[3] = true;
                
                MovementController.setPoints((int)ofRandom(45,46), ofRandom(28,29),(int)ofRandom(1125,1126),(int)ofRandom(525,526));
                
                
                writeStyle(1);
                setPattern = false;
                songStage++;
            }
            else if(songStage == 2){
                ofLog() << "LIGHTS ON with One Static Shape <<< Setting: " << songStage;
                
                //set X
                output_pts[0] = true;
                output_pts[2] = true;
                
                output_pts[1] = false;
                output_pts[3] = false;
                
                MovementController.setPoints((int)ofRandom(50,51), ofRandom(25,26),(int)ofRandom(875,876),(int)ofRandom(50,51));
                
                setPattern = false;
                writeStyle(1);
                
                songStage++;
            }else if(songStage == 3){
                //rest Here do nth.
            }
        }
        
    }else if(s == 3){ // Light ON and 3 Shapes In LOOP
        //----- INFO -----
        ofBackground(0, 0, 110);
        ss_info << "MODE : LIGHT and 3 SHAPES : " << endl;
        
        drawPosGui = true;
        drawTimeGui = false;
        drawMovementController = true;
        drawDmx = false;
        
        //---- BEGIN -----
        if(currTime - prevTime >timeDiff){
            
            //ofLog() << "timeDiff ; "<< timeDiff;
            prevTime = currTime;
            setPattern = true;
            
            if(currCableID >= NUM_OF_CABLES ){
                //timeDiff = ofRandom(60000,90000);
                
                ofLog() << "reach cable 19";
                currCableID = -1;
                
            }else{
                timeDiff = 600;
                currCableID++;
            }
            ofLog() << "songStage : " << songStage;
            
            
        }
        
        if(setPattern){
            if(songStage == 0){
                DmxLight.setAll((float)1.0, (float)1.0, (float)1.0, (float)1.0);
                
                //set Y
                output_pts[0] = false;
                output_pts[2] = false;
                
                output_pts[1] = true;
                output_pts[3] = true;
                
                MovementController.setPoints((int)ofRandom(45,46), ofRandom(28,29),(int)ofRandom(1125,1126),(int)ofRandom(525,526));
                
                
                if(currCableID == NUM_OF_CABLES ){
                    songStage++;
                }else{
                    writeStyle(2);
                }
                
                setPattern = false;
                
            }else if(songStage == 1){
                
                //set X
                output_pts[0] = true;
                output_pts[2] = true;
                
                output_pts[1] = false;
                output_pts[3] = false;
                
                MovementController.setPoints((int)ofRandom(0,0), ofRandom(0,0),(int)ofRandom(68,69),(int)ofRandom(0,1000));
                
                if(currCableID == NUM_OF_CABLES ){
                    
                    songStage++;
                    timeDiff = ofRandom(25000,40000);
                }else{
                    writeStyle(2);
                }
                
                setPattern = false;
                
            }else if(songStage == 2){
                
                //set Y
                output_pts[0] = false;
                output_pts[2] = false;
                
                output_pts[1] = true;
                output_pts[3] = true;
                
                MovementController.setPoints((int)ofRandom(45,46), ofRandom(28,29),(int)ofRandom(1125,1126),(int)ofRandom(525,526));
                
                if(currCableID == NUM_OF_CABLES ){
                    songStage++;
                }else{
                    writeStyle(2);
                }
                
                setPattern = false;
                
            }else if(songStage == 3){
                
                //set X
                output_pts[0] = true;
                output_pts[2] = true;
                
                output_pts[1] = false;
                output_pts[3] = false;
                
                MovementController.setPoints((int)ofRandom(50,51), ofRandom(25,26),(int)ofRandom(875,876),(int)ofRandom(50,51));
                
                if(currCableID == NUM_OF_CABLES ){
                    
                    songStage++;
                    timeDiff = ofRandom(25000,40000);
                }else{
                    writeStyle(2);
                }
                
                setPattern = false;
                
            }else if(songStage == 4){
                
                //set Y
                output_pts[0] = false;
                output_pts[2] = false;
                
                output_pts[1] = true;
                output_pts[3] = true;
                
                MovementController.setPoints((int)ofRandom(50,51), ofRandom(25,26),(int)ofRandom(875,876),(int)ofRandom(50,51));
                
                if(currCableID == NUM_OF_CABLES ){
                    songStage++;
                }else{
                    writeStyle(2);
                }
                
                setPattern = false;
                
            }else if(songStage == 5){
                
                //set X
                output_pts[0] = true;
                output_pts[2] = true;
                
                output_pts[1] = false;
                output_pts[3] = false;
                
                MovementController.setPoints((int)ofRandom(45,46), ofRandom(28,29),(int)ofRandom(1125,1126),(int)ofRandom(525,526));
                
                if(currCableID == NUM_OF_CABLES ){
                    
                    songStage++;
                    timeDiff = ofRandom(25000,40000);
                }else{
                    writeStyle(2);
                }
                
                setPattern = false;
                
            }else if (songStage == 6){
                setPattern = false;
                songStage=0;
                
            }
        }
        
        
    }else if(s == 4){ //one by one
        
        //----- INFO -----
        ofBackground(0, 0, 120);
        ss_info << "MODE : MP3 : " << endl;
        
        drawPosGui = true;
        drawTimeGui = true;
        drawMovementController = true;
        drawDmx = false;
        
        //---- BEGIN -----
        if(currTime - prevTime >timeDiff){
            
            ofLog() << "timeDiff ; "<< timeDiff;
            prevTime = currTime;
            setPattern = true;
            
            if(currCableID > NUM_OF_CABLES-1){
                timeDiff = ofRandom(60000,90000);
                ofLog() << "reach cable 19";
                currCableID = -1;
            }else{
                timeDiff = 600;
            }
        }
        
        if(setPattern){
            if(songStage == 0){
                int option = (int)ofRandom(4);
                if(option ==3){//TFTF , FTTF, TFTF, FTFT
                    output_pts[1] = true;
                    output_pts[3] = false;
                    
                    output_pts[0] = true;
                    output_pts[2] = false;
                }else if(option ==2)
                {
                    output_pts[1] = false;
                    output_pts[3] = true;
                    
                    output_pts[0] = true;
                    output_pts[2] = false;
                    
                }
                else if(option ==1){
                    output_pts[1] = true;
                    output_pts[3] = false;
                    
                    output_pts[0] = false;
                    output_pts[2] = true;
                }
                else {
                    output_pts[1] = false;
                    output_pts[3] = true;
                    
                    output_pts[0] = false;
                    output_pts[2] = true;
                }
                MovementController.setPoints((int)ofRandom(30,90), ofRandom(35,37),(int)ofRandom(0,187),(int)ofRandom(0,1000));
                
                
                songStage++;
                
            }else if (songStage == 1){
                
                writeStyle(2);
                
                if(currCableID >= NUM_OF_CABLES ){
                    ofLog() << "STAGE END " << currCableID;
                    songStage++;
                }
                
                setPattern = false;
            }else if (songStage == 2){
                setPattern = false;
                songStage=0;
                currCableID = 0;
            }
            
        }
        
    }else if(s == 5){  // all move at the same time
        
        if(songStage == 0){
            if(currTime - prevTime >timeDiff){
                timeDiff = ofRandom(60000,90000);
                ofLog() << "timeDiff ; "<< timeDiff;
                prevTime = currTime;
                setPattern = true;
                
                //DmxLight.setAll((float)1.0, (float)1.0, (float)1.0, (float)1.0);
                // DmxLight.setAll((float)ofRandom(0,1), (float)ofRandom(0,1), (float)ofRandom(0,1), (float)ofRandom(0,1));
            }
            if(setPattern){
                
                int option = (int)ofRandom(4);
                if(option ==3){//TFTF , FTTF, TFTF, FTFT
                    output_pts[1] = true;
                    output_pts[3] = false;
                    
                    output_pts[0] = true;
                    output_pts[2] = false;
                }else if(option ==2)
                {
                    output_pts[1] = false;
                    output_pts[3] = true;
                    
                    output_pts[0] = true;
                    output_pts[2] = false;
                    
                }
                else if(option ==1){
                    output_pts[1] = true;
                    output_pts[3] = false;
                    
                    output_pts[0] = false;
                    output_pts[2] = true;
                }
                else {
                    output_pts[1] = false;
                    output_pts[3] = true;
                    
                    output_pts[0] = false;
                    output_pts[2] = true;
                }
                MovementController.setPoints((int)ofRandom(30,90), ofRandom(35,37),(int)ofRandom(0,187),(int)ofRandom(0,1000));
                setPattern = false;
                ofLog() << "Set Pattern";
                
                writeStyle(1);
                
            }
            
        }
    }else if(s == 6){ //ricci mode, debug one by one
        currentStyle = 11;
        if(songStage == 0){
            output_pts[0] = true;
            output_pts[2] = true;
            
            output_pts[1] = true;
            output_pts[3] = true;
            MovementController.setPoints((int)ofRandom(0,0), ofRandom(0,0),(int)ofRandom(68,69),(int)ofRandom(0,1000));
            currCableID = 0;
            songStage++;
        }else{
            output_pts[0] = false;
            output_pts[2] = false;
            
            output_pts[1] = false;
            output_pts[3] = false;
            
        }
        
        if(setPattern){
            setPattern = false;
        }
    }
}


void ofApp::writeStyle(int s){
    ofLog() << "write Style " << s  << " current arduino ID : " << currCableID;
    
    if (s==0){
        
        if(currentStyle == 11){
            
            string writeInTotal = "LX : ";
            
            string toWrite = "";
            
            toWrite+= ofToString(currentStyle);
            toWrite+= "-";
            
            toWrite+= ofToString((int)cableSpeedLx[currCableID]);
            toWrite+= "-";
            toWrite+= ofToString((int)cableAccelLx[currCableID]);
            toWrite+= "-";
            toWrite+= ofToString((int)cablePosLx[currCableID]);
            toWrite+= "-";
            
            writeInTotal=toWrite + " LY:";
            
            toWrite+= ofToString((int)cableSpeedLy[currCableID]);
            toWrite+= "-";
            toWrite+= ofToString((int)cableAccelLy[currCableID]);
            toWrite+= "-";
            toWrite+= ofToString((int)cablePosLy[currCableID]);
            toWrite+= "-";
            
            writeInTotal=toWrite +" RX: ";
            
            toWrite+= ofToString((int)cableSpeedRx[currCableID]);
            toWrite+= "-";
            toWrite+= ofToString((int)cableAccelRx[currCableID]);
            toWrite+= "-";
            toWrite+= ofToString((int)cablePosRx[currCableID]);
            toWrite+= "-";
            
            writeInTotal=toWrite +" RY: ";;
            
            toWrite+= ofToString((int)cableSpeedRy[currCableID]);
            toWrite+= "-";
            toWrite+= ofToString((int)cableAccelRy[currCableID]);
            toWrite+= "-";
            toWrite+= ofToString((int)cablePosRy[currCableID]);
            
            writeInTotal=toWrite;
            
            serialWrite(-1, toWrite);
            currentdisplayLog = writeInTotal;
            
            //MovementController
            //MovementController.getPoints();
            
        }
        
        if(currentStyle == 2){
            string writeInTotal = "SPEED - ACCEL - : ";
            
            string toWrite = "";
            
            toWrite+= ofToString(currentStyle);
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
            
            toWrite+= ofToString((int)cablePosLx[currCableID]);
            toWrite+= "-";
            toWrite+= ofToString((int)cablePosLx2[currCableID]);
            toWrite+= "-";
            
            writeInTotal=toWrite + " LY:";
            
            toWrite+= ofToString((int)cablePosLy[currCableID]);
            toWrite+= "-";
            toWrite+= ofToString((int)cablePosLy2[currCableID]);
            toWrite+= "-";
            
            writeInTotal=toWrite +" RX: ";
            
            toWrite+= ofToString((int)cablePosRx[currCableID]);
            toWrite+= "-";
            toWrite+= ofToString((int)cablePosRx2[currCableID]);
            toWrite+= "-";
            
            writeInTotal=toWrite +" RY: ";;
            
            toWrite+= ofToString((int)cablePosRy[currCableID]);
            toWrite+= "-";
            toWrite+= ofToString((int)cablePosRy2[currCableID]);
            
            writeInTotal=toWrite;
            
            serialWrite(-1, toWrite);
            currentdisplayLog = writeInTotal;
            
        }
        
    }
    else if (s ==1){
        for(int i=0; i< NUM_OF_CABLES; i++){
            if(currentStyle == 12){ //By Time   //STYLE - POS - TIME - POS - TIME - POS - TIME - POS - TIME
                
                string writeInTotal = "LX : ";
                
                string toWrite = "";
                
                toWrite+= ofToString(currentStyle);
                toWrite+= "-";
                
                toWrite+= ofToString((int)cablePosLx[i]);
                toWrite+= "-";
                toWrite+= ofToString((int)cableTimeLx[i]);
                toWrite+= "-";
                
                writeInTotal=toWrite + " LY:";
                
                toWrite+= ofToString((int)cablePosLy[i]);
                toWrite+= "-";
                toWrite+= ofToString((int)cableTimeLy[i]);
                toWrite+= "-";
                
                writeInTotal=toWrite +" RX: ";
                
                toWrite+= ofToString((int)cablePosRx[i]);
                toWrite+= "-";
                toWrite+= ofToString((int)cableTimeRx[i]);
                toWrite+= "-";
                
                writeInTotal=toWrite +" RY: ";;
                
                toWrite+= ofToString((int)cablePosRy[i]);
                toWrite+= "-";
                toWrite+= ofToString((int)cableTimeRy[i]);
                toWrite+= "-";
                
                writeInTotal=toWrite;
                
                serialWrite(i, toWrite);
                currentdisplayLog = writeInTotal;
                
                //MovementController
                
                //MovementController.getPoints();
                
            }
            
            
            if(currentStyle == 11){
                
                string writeInTotal = "LX : ";
                
                string toWrite = "";
                
                toWrite+= ofToString(currentStyle);
                toWrite+= "-";
                
                toWrite+= ofToString((int)cableSpeedLx[i]);
                toWrite+= "-";
                toWrite+= ofToString((int)cableAccelLx[i]);
                toWrite+= "-";
                toWrite+= ofToString((int)cablePosLx[i]);
                toWrite+= "-";
                
                writeInTotal=toWrite + " LY:";
                
                toWrite+= ofToString((int)cableSpeedLy[i]);
                toWrite+= "-";
                toWrite+= ofToString((int)cableAccelLy[i]);
                toWrite+= "-";
                toWrite+= ofToString((int)cablePosLy[i]);
                toWrite+= "-";
                
                writeInTotal=toWrite +" RX: ";
                
                toWrite+= ofToString((int)cableSpeedRx[i]);
                toWrite+= "-";
                toWrite+= ofToString((int)cableAccelRx[i]);
                toWrite+= "-";
                toWrite+= ofToString((int)cablePosRx[i]);
                toWrite+= "-";
                
                writeInTotal=toWrite +" RY: ";;
                
                toWrite+= ofToString((int)cableSpeedRy[i]);
                toWrite+= "-";
                toWrite+= ofToString((int)cableAccelRy[i]);
                toWrite+= "-";
                toWrite+= ofToString((int)cablePosRy[i]);
                
                writeInTotal=toWrite;
                
                serialWrite(i, toWrite);
                currentdisplayLog = writeInTotal;
                
                //MovementController
                
                //MovementController.getPoints();
                
            }
            
            if(currentStyle == 2){
                string writeInTotal = "SPEED - ACCEL - : ";
                
                string toWrite = "";
                
                toWrite+= ofToString(currentStyle);
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
                
                toWrite+= ofToString((int)cablePosLx[i]);
                toWrite+= "-";
                toWrite+= ofToString((int)cablePosLx2[i]);
                toWrite+= "-";
                
                writeInTotal=toWrite + " LY:";
                
                toWrite+= ofToString((int)cablePosLy[i]);
                toWrite+= "-";
                toWrite+= ofToString((int)cablePosLy2[i]);
                toWrite+= "-";
                
                writeInTotal=toWrite +" RX: ";
                
                toWrite+= ofToString((int)cablePosRx[i]);
                toWrite+= "-";
                toWrite+= ofToString((int)cablePosRx2[i]);
                toWrite+= "-";
                
                writeInTotal=toWrite +" RY: ";;
                
                toWrite+= ofToString((int)cablePosRy[i]);
                toWrite+= "-";
                toWrite+= ofToString((int)cablePosRy2[i]);
                
                writeInTotal=toWrite;
                
                serialWrite(i, toWrite);
                currentdisplayLog = writeInTotal;
                
            }
        }
        
    }else if (s ==2){
        if(currCableID <= NUM_OF_CABLES-1 && currCableID >= 0){
            if(currentStyle == 11){
                
                string writeInTotal = "LX : ";
                
                string toWrite = "";
                
                toWrite+= ofToString(currentStyle);
                toWrite+= "-";
                
                toWrite+= ofToString((int)cableSpeedLx[currCableID]);
                toWrite+= "-";
                toWrite+= ofToString((int)cableAccelLx[currCableID]);
                toWrite+= "-";
                toWrite+= ofToString((int)cablePosLx[currCableID]);
                toWrite+= "-";
                
                writeInTotal=toWrite + " LY:";
                
                toWrite+= ofToString((int)cableSpeedLy[currCableID]);
                toWrite+= "-";
                toWrite+= ofToString((int)cableAccelLy[currCableID]);
                toWrite+= "-";
                toWrite+= ofToString((int)cablePosLy[currCableID]);
                toWrite+= "-";
                
                writeInTotal=toWrite +" RX: ";
                
                toWrite+= ofToString((int)cableSpeedRx[currCableID]);
                toWrite+= "-";
                toWrite+= ofToString((int)cableAccelRx[currCableID]);
                toWrite+= "-";
                toWrite+= ofToString((int)cablePosRx[currCableID]);
                toWrite+= "-";
                
                writeInTotal=toWrite +" RY: ";;
                
                toWrite+= ofToString((int)cableSpeedRy[currCableID]);
                toWrite+= "-";
                toWrite+= ofToString((int)cableAccelRy[currCableID]);
                toWrite+= "-";
                toWrite+= ofToString((int)cablePosRy[currCableID]);
                
                writeInTotal=toWrite;
                
                serialWrite(currCableID, toWrite);
                currentdisplayLog = writeInTotal;
                
                //MovementController
                
                //MovementController.getPoints();
                
            }
            
            if(currentStyle == 2){
                
                string writeInTotal = "SPEED - ACCEL - : ";
                
                string toWrite = "";
                
                toWrite+= ofToString(currentStyle);
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
                
                toWrite+= ofToString((int)cablePosLx[currCableID]);
                toWrite+= "-";
                toWrite+= ofToString((int)cablePosLx2[currCableID]);
                toWrite+= "-";
                
                writeInTotal=toWrite + " LY:";
                
                toWrite+= ofToString((int)cablePosLy[currCableID]);
                toWrite+= "-";
                toWrite+= ofToString((int)cablePosLy2[currCableID]);
                toWrite+= "-";
                
                writeInTotal=toWrite +" RX: ";
                
                toWrite+= ofToString((int)cablePosRx[currCableID]);
                toWrite+= "-";
                toWrite+= ofToString((int)cablePosRx2[currCableID]);
                toWrite+= "-";
                
                writeInTotal=toWrite +" RY: ";;
                
                toWrite+= ofToString((int)cablePosRy[currCableID]);
                toWrite+= "-";
                toWrite+= ofToString((int)cablePosRy2[currCableID]);
                
                writeInTotal=toWrite;
                
                serialWrite(currCableID, toWrite);
                currentdisplayLog = writeInTotal;
                
                
            }
        }
    }
    
}



//--------------------------------------------------------------
//--------------------------GUI EVENT -----------------------------
//--------------------------------------------------------------
void ofApp::loadButtonPressed(){
    serialWrite(currCableID, "L");
    serialTrigger = false;
    
}

//--------------------------------------------------------------
//-----------------SERIAL COMMUNICATION ------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
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
vector<bool> ofApp::serialSetup(){ //int give the connection status of each cables
    
    
    
    vector<bool> connectionStatus;
    
    
    
    for(int i=0; i< NUM_OF_CABLES; i++){
        connectionStatus.push_back(0);
    }
#ifdef USEOSC
    
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
        arduino.push_back(true);
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
        }
    }else if (arduinoID >= 0 && working_cable[arduinoID] && arduinoID<arduino.size()){
        sendOSC(arduino[arduinoID], sw);
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
    else{ ofLog() << "Arduino: " <<arduinoID << "OFFLINE ";} // todo put in gui
    
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
        currentdisplayLog = ofToString(currCableID) +" EEPROM LOADED";
        return sToIntArray;
    }
    else{
        // vector<int> sToIntArray;
        return sToIntArray;
    }
    
}

//--------------------------------------------------------------
//---------------------- SETTINGS / XML--------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
void ofApp::saveSettings()
{
    XML.save("XMLSettings.xml");
    
    ofLog() << "XML Setting Saved";
}
void ofApp::loadSettings()
{
    XML.load("XMLSettings.xml");
    movementMode = XML.getValue<int>("MODE");
    ofLog() << "MODE VAL : " << movementMode;
    ofLog() << "XML Setting Loaded";
    
}

//--------------------------------------------------------------
//---------------------- OTHER EVENTS --------------------------
//--------------------------------------------------------------
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









