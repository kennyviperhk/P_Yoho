#include "DmxLight.h"

//--------------------------------------------------------------
void DmxLight::setup() {
    ofSetVerticalSync(true);

    modules = NUM_OF_DMX;
    
    panel1.setup();
    panel1.setPosition(4, 4);
    panel1.setName("dmx0");
    panel1.add(save.set("save", false));
    panel1.add(load.set("load", false));

    panel2.setup();
    panel2.setPosition(400, 4);
    panel2.setName("dmx1");
    panel2.add(save.set("save", false));
    panel2.add(load.set("load", false));

    for(int module = 1; module <= modules; module++) {
        string label = "mod" + ofToString(module);
            if(module<10){

                panel1.add(red[module].set("red" +ofToString(module), 1, 0, 1));
                panel1.add(green[module].set("green"+ofToString(module), 1, 0, 1));
                panel1.add(blue[module].set("blue"+ofToString(module), 1, 0, 1));
                panel1.add(white[module].set("white"+ofToString(module), 1, 0, 1));
        }
        else{

            panel2.add(red[module].set("red" +ofToString(module), 1, 0, 1));
            panel2.add(green[module].set("green"+ofToString(module), 1, 0, 1));
            panel2.add(blue[module].set("blue"+ofToString(module), 1, 0, 1));
            panel2.add(white[module].set("white"+ofToString(module), 1, 0, 1));
        }
    }

    load = true;
    
    dmx.connect(0, 80);  //Set Module ID
    dmx.update(true); // black on startup
}

void DmxLight::exit() {
    dmx.clear();
    dmx.update(true); // black on shutdown
}

void DmxLight::update() {
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
  //  for(int i = 1; i <= modules; i++) {
  //      ofLog() << ofLerp(red[i], pred[i], 0.1);
  //  }
}

void DmxLight::set(int id, int r, int g, int b, int w) {
    if(id < NUM_OF_DMX){
        red[id] = r;
        green[id] = g;
        blue[id] = b;
        white[id] = w;
        
    }
}

void DmxLight::setAll(int r, int g, int b, int w) {
    for(int i=0; i < NUM_OF_DMX; i++){
        red[i] = r;
        green[i] = g;
        blue[i] = b;
        white[i] = w;
    }
}

void DmxLight::draw() {
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
    panel2.draw();
}
