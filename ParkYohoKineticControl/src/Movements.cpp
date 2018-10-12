#include "Movements.h"

//--------------------------------------------------------------
void Movements::setup() {
    currShape = 0;
    loadSettings();
}



void Movements::update() {
    currMillis = ofGetElapsedTimeMillis();
    
    if(currMillis>nextTrigger){
      //  setShapes();
        nextTrigger=currMillis+1000*60*60;
    }
    
}

vector<int> Movements::setShapeA() {
    ofLog() << "Setting Shape A" << currShape;
    vector<int> shape;
    shape.push_back(cableOpA[currShape]);
    
    for(int i = 0; i< pointsA1[currShape].size();i++){
        shape.push_back(pointsA1[currShape][i]);
    }
    for(int i = 0; i< pointsA2[currShape].size();i++){
        shape.push_back(pointsA2[currShape][i]);
    }

    
ofLog() << "Shape A Size" << shape.size();
    return shape;
}

vector<int> Movements::setShapeB() {
    ofLog() << "Setting Shape B" << currShape;
    vector<int> shape;
    shape.push_back(cableOpB[currShape]);
    
    for(int i = 0; i< pointsB1[currShape].size();i++){
        shape.push_back(pointsB1[currShape][i]);
    }
    for(int i = 0; i< pointsB2[currShape].size();i++){
        shape.push_back(pointsB2[currShape][i]);
    }
    return shape;
}

void Movements::incrementShape(){
    currShape++;
    if(currShape >= numOfShapes ){
        currShape = 0;
    }
}

int Movements::getCurrShape(){
    return currShape;
}

int Movements::getTotalShapes(){
    return numOfShapes;
}



void Movements::draw() {
    
}


void Movements::exit() {
    
}



//--------------------------------------------------------------
//--------------------------JSON / SETTINGS---------------------
//--------------------------------------------------------------

void Movements::saveSettings() {
    std::string file = "movements.json";
    
    // Now parse the JSON
    bool parsingSuccessful = settings.open(file);
    
    if (parsingSuccessful)
    {
        
        //settings["day"] = day;
        
        // now write pretty print
        if (!settings.save("settings.json", true))
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
//--------------------------------------------------------------
void Movements::loadSettings() {
    
    std::string file = "movements.json";
    
    // Now parse the JSON
    bool parsingSuccessful = settings.open(file);
    
    if (parsingSuccessful)
    {
        ofLogNotice("ofApp::setup") << settings.getRawString();
        
        numOfShapes =  settings["movements"].size();
        
        for(int i = 0 ; i < numOfShapes; i++){
            cableOpA.push_back(settings["movements"][i]["cableOpA"].asInt());
            cableOpB.push_back(settings["movements"][i]["cableOpB"].asInt());
            pointsA1.push_back({0,0,0,0,0});
            pointsA2.push_back({0,0,0,0,0});
            pointsB1.push_back({0,0,0,0,0});
            pointsB2.push_back({0,0,0,0,0});
            for(int j = 0; j < 5; j++){
                pointsA1[i][j] = settings["movements"][i]["pointsA1"][j].asInt();
                pointsA2[i][j] = settings["movements"][i]["pointsA2"][j].asInt();
                pointsB1[i][j] = settings["movements"][i]["pointsB1"][j].asInt();
                pointsB2[i][j] = settings["movements"][i]["pointsB2"][j].asInt();
            }
        }
    }
    else
    {
        ofLogError("ofApp::setup") << "Failed to parse JSON" << endl;
    }

    
}
