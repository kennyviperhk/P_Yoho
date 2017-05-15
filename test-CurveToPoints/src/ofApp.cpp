#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    
    //Cable GUI
    
    //--- Cable Position Control ---
    
    parametersCablePos.setName("cablePosition");
    guiCablePosLx.setup("EEPROMReadWrite", "settings.xml", ofGetWidth() - 200, 0);
    guiCablePosLy.setup("EEPROMReadWrite", "settings.xml", ofGetWidth() - 200, 400);
    guiCablePosRx.setup("EEPROMReadWrite", "settings.xml", ofGetWidth() - 100, 0);
    guiCablePosRy.setup("EEPROMReadWrite", "settings.xml", ofGetWidth() - 100, 400);
    for(int i=0; i< NUM_OF_CABLES; i++){
        ofParameter<int> a;
        a.set("P Lx" + ofToString(i),0,0,1000); //lx,ly,rx,ry
        ofParameter<int> b;
        b.set("P Ly" + ofToString(i),0,0,1000);
        ofParameter<int> c;
        c.set("P Rx" + ofToString(i),0,0,1000);
        ofParameter<int> d;
        d.set("P Ry" + ofToString(i),0,0,1000);
        cablePosLx.push_back(a);
        cablePosLy.push_back(b);
        cablePosRx.push_back(c);
        cablePosRy.push_back(d);
        guiCablePosLx.add(cablePosLx[i]);
        guiCablePosLy.add(cablePosLy[i]);
        guiCablePosRx.add(cablePosRx[i]);
        guiCablePosRy.add(cablePosRy[i]);
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
    
    
    //Animation GUI
    
    //--- Curve Control ---
    curveControls.setup("CurveReadWrite", "settings.xml", ofGetWidth()/2, 0);
    for(int i=0; i< 8; i++){
        ofParameter<int> a;
        a.set("Lx Curve" + ofToString(i),0,0,1000); //lx,ly,rx,ry
        curves.push_back(a);
        curveControls.add(curves[i]);
    }
    setupCurve(10,10,500,200);
}

//--------------------------------------------------------------
void ofApp::setupCurve(int x1, int y1,int w, int h){
    curveCoord = ofPoint(x1,y1);
    curveW = w;
    curveH = h;
}

//--------------------------------------------------------------
vector<ofPoint> ofApp::getPoints(){
    vector<ofPoint> p;
    p.clear();
    for(int i=0; i< NUM_OF_CABLES; i++){
        p.push_back(trail.getPointAtPercent((float)i/NUM_OF_CABLES));
    }
    return p;
}

//--------------------------------------------------------------
void ofApp::updateCurve(){
    
    x=0;
    
    
    trail.clear();
    angle = 0;
    increment = (float)curves[0]/1000/TWO_PI;
    for(int i=0; i< curveW;i++){
        x++;
        angle+=increment;
        if (angle>=TWO_PI) { //if the angle is more than or equal to two PI (a full rotation measured in Radians) then make it zero.
            angle=0;
        }
        y=(curveH/2)+ (curveH/2)*sin(angle);
        trail.addVertex(ofPoint(curveCoord.x+ x,curveCoord.y+ y));
    }
}


//--------------------------------------------------------------
void ofApp::drawCurve(){
    
    // ofEnableSmoothing();
    
    
    ofSetColor(255,0,0);
    
    ofDrawRectangle(curveCoord.x, curveCoord.y, curveW, curveH);
    ofSetColor(20);
    trail.draw();
    for(int i=0; i< NUM_OF_CABLES; i++){
        ofSetColor(0, 140, 255);
        ofPoint p = getPoints()[i];
        ofDrawCircle(p,5);
        cablePosLx[i] = p.x;
        cablePosLy[i] = p.y;
    }
    
    // ofDisableSmoothing();
}

//--------------------------------------------------------------

double ofApp::GetFloatPrecision(double value, double precision)
{
    return (floor((value * pow(10, precision) + 0.5)) / pow(10, precision));
}

//--------------------------------------------------------------
void ofApp::update(){
    updateCurve();
    
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofBackgroundGradient(ofColor(240),ofColor(210),OF_GRADIENT_BAR);
    guiCablePosLx.draw();
    guiCablePosLy.draw();
    guiCablePosRx.draw();
    guiCablePosRy.draw();
    
    curveControls.draw();
    
    drawCurve();
    
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y){
    
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
