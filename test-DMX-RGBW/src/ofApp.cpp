#include "ofApp.h"

void ofApp::setup() {
    ofSetVerticalSync(true);
    
    modules = 20;
    channelsPerModule = 4;
    
    panel1.setup();
    panel1.setPosition(4, 4);
    panel1.setName("settings 0");
    panel1.add(save.set("save", false));
    panel1.add(load.set("load", false));
    
    for(int module = 1; module <= modules; module++) {
        string label = "mod" + ofToString(module);
        panel1.add(red[module].set("red" +ofToString(module), 0, 0, 1));
        panel1.add(green[module].set("green"+ofToString(module), 0, 0, 1));
        panel1.add(blue[module].set("blue"+ofToString(module), 0, 0, 1));
        panel1.add(white[module].set("white"+ofToString(module), 0, 0, 1));
        
        //panel1.add(moduleNum[module].set(label, false));
    }
    
    load = true;
    
    dmx.connect(port, modules * channelsPerModule);
    dmx.update(true); // black on startup
}

void ofApp::exit() {
    dmx.clear();
    dmx.update(true); // black on shutdown
}

void ofApp::update() {
    if(save) {
        panel1.saveToFile("settings.xml");
        save = false;
    }
    if(load) {
        if(ofFile::doesFileExist(ofToDataPath("settings.xml"))) {
            panel1.loadFromFile("settings.xml");
        }
        load = false;
    }
    
    int channel = 1;
    for(int module = 1; module <= modules; module++) {
        dmx.setLevel(channel++, red[module]*255);
        dmx.setLevel(channel++, green[module]*255);
        dmx.setLevel(channel++, blue[module]*255);
        dmx.setLevel(channel++, white[module]*255);
    }
    if(dmx.isConnected()) {
        dmx.update();
    } else {
        ofSetColor(255);
        ofDrawBitmapString("Could not connect to port " + ofToString(port), 250,20);
    }
}

void ofApp::draw() {
    ofBackground(0);
    ofPushMatrix();
    
    ofTranslate(256, 0);
    int channel = 1;
    for(int module = 1; module <= modules; module++) {
        string label = "module " + ofToString(module);
        int rc = channel++;
        int gc = channel++;
        int bc = channel++;
        int wc = channel++;
        int r = dmx.getLevel(rc);
        int g = dmx.getLevel(gc);
        int b = dmx.getLevel(bc);
        int w = dmx.getLevel(wc);
        ofSetColor(r, g, b);
        ofFill();
        ofDrawRectangle(4, module * 16 + 6, 14, 14);
        ofSetColor(255);
        ofNoFill();
        ofDrawRectangle(4, module * 16 + 6, 14, 14);
        string rs = ofToString(rc) + ":" + ofToString(r);
        string gs = ofToString(gc) + ":" + ofToString(g);
        string bs = ofToString(bc) + ":" + ofToString(b);
        string ws = ofToString(wc) + ":" + ofToString(w);
        string text = label + " (" + rs + ", " + gs + ", " + bs + ", " + ws + ")";
        ofDrawBitmapString(text, 24, module * 16 + 16);
    }
    
    ofPopMatrix();
    
    panel1.draw();
}
