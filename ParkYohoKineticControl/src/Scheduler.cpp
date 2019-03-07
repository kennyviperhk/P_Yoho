#include "Scheduler.h"

//--------------------------------------------------------------
void Scheduler::setup() {
    
    panel1.setup();
    panel1.setPosition(400, ofGetHeight()-140);
    panel1.setName("Scheduler, Press OK to confirm");
    
    loadSettings();
    
    panel1.add(beginHour.set("Begin Hour (24)",displayBeginHour, 0, 23));
    panel1.add(beginMin.set("Begin Min",displayBeginMin, 0, 59));
    panel1.add(endHour.set("End Hour (24)",displayEndHour, 0, 23));
    panel1.add(endMin.set("End Min ",displayEndMin, 0, 59));

    save.addListener(this, &Scheduler::okBtnPressed);
    panel1.add(save.setup("OK"));

    int guiW = 300;
    int guiH = 500;
    panel1.setSize(guiW, guiH);
    panel1.setWidthElements(guiW);

    eventIsTriggered = false;

}

void Scheduler::exit() {

}

void Scheduler::update() {
    if(ofGetSeconds() == 1){
        eventIsTriggered = false;
    }
    if(beginHour == ofGetHours() && beginMin == ofGetMinutes() && ofGetSeconds() == 0){
        if(!eventIsTriggered){
            bool t = true;
            ofNotifyEvent(lightToggleEvent, t);
            eventIsTriggered = true;
        }
    }else if(endHour == ofGetHours() && endMin == ofGetMinutes() && ofGetSeconds() == 0){
        if(!eventIsTriggered){
            bool t = false;
            ofNotifyEvent(lightToggleEvent, t);
            eventIsTriggered = true;
        }
    }else if(beginHour == ofGetHours() && (beginMin-4) == ofGetMinutes() && ofGetSeconds() == 0  && (ofGetWeekday() == 5 || ofGetWeekday() == 6 || ofGetWeekday() == 0) ){ //0 reset Home 1 offest Home
        if(!eventIsTriggered){
            int t = 0;
            ofNotifyEvent(homeResetEvent, t);
            eventIsTriggered = true;
        }
    }else if(beginHour == ofGetHours() && (beginMin-2) == ofGetMinutes() && ofGetSeconds() == 0  && (ofGetWeekday() == 5 || ofGetWeekday() == 6 || ofGetWeekday() == 0)  ){//0 reset Home 1 offest Home
        if(!eventIsTriggered){
            int t = 1;
            ofNotifyEvent(homeResetEvent, t);
            eventIsTriggered = true;
        }
    }
    else if(beginHour == ofGetHours() && (beginMin) == ofGetMinutes() && ofGetSeconds() == 0 && (ofGetWeekday() == 5 || ofGetWeekday() == 6 || ofGetWeekday() == 0)  ){
        if(!eventIsTriggered){
            int t = 0;
            ofNotifyEvent(changeFormEvent, t);
            eventIsTriggered = true;
        }
    }else if(beginHour == ofGetHours() && (beginMin) == ofGetMinutes() && ofGetSeconds() == 0 && (ofGetWeekday() == 5 || ofGetWeekday() == 6 || ofGetWeekday() == 0) ){
        if(!eventIsTriggered){
            int t = 1;
            ofNotifyEvent(changeFormEvent, t);
            eventIsTriggered = true;
        }
    }
}

void Scheduler::draw() {
    
    
    std::stringstream ss;
    ss << "Begin Time   : " << displayBeginHour << " : " << displayBeginMin << " || End Time : " << displayEndHour << " : " << endMin << endl;
    ss << "Current Time : " << ofGetHours() << " : " << ofGetMinutes() << " : " << ofGetSeconds() << endl;
    ofPushMatrix();
    ofSetColor(255, 255, 255);
    ofDrawBitmapString(ss.str(), ofVec2f(20, ofGetHeight()-30));
    ofPopMatrix();
    
    panel1.draw();
   
}

void Scheduler::okBtnPressed(){
    displayBeginHour = beginHour;
    displayBeginMin = beginMin;
    displayEndHour = endHour;
    displayEndMin = endMin;
    saveSettings();
}


//--------------------------------------------------------------
//---------------------- SETTINGS / XML-------------------------
//--------------------------------------------------------------

void Scheduler::saveSettings()
{

    XML.setValue("BEGIN_HOUR", ofToString(displayBeginHour));
    XML.setValue("BEGIN_MINUTE", ofToString(displayBeginMin));
    XML.setValue("END_HOUR", ofToString(displayEndHour));
    XML.setValue("END_MINUTE", ofToString(displayEndMin));
    
    XML.save("Scheduler.xml");
    
    ofLog() << "Scheduler.xml Setting Saved";
}
void Scheduler::loadSettings()
{
    XML.load("Scheduler.xml");
    displayBeginHour = XML.getValue("BEGIN_HOUR",0);
    displayBeginMin = XML.getValue("BEGIN_MINUTE",0);
    displayEndHour = XML.getValue("END_HOUR",0);
    displayEndMin = XML.getValue("END_MINUTE",0);
    //ofLog() << "MODE VAL : " << movementMode;
    ofLog() << "Scheduler.xml Setting Loaded";
    
}
