#pragma once

#include "ofMain.h"
#include "ofxGui.h"

#define NUM_OF_CABLES 20 //Todo Transfer definition /variables to xml


class ofApp : public ofBaseApp{
	public:
		void setup();
		void update();
		void draw();
		
		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y);
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
    
    ofxPanel curveControls;
    ofParameterGroup parametersDebug;
    ofXml settings;
    vector<ofParameter<int>> curves;
    
    ofPolyline trail;
    float x,y;
    float increment;
    float angle;
    
    ofPoint curveCoord;
    int curveW;
    int curveH;
    void setupCurve(int x1, int y1,int w, int h);
    void updateCurve();
    void drawCurve();
    vector<ofPoint> getPoints();
    
    double GetFloatPrecision(double value, double precision);
    
    //Cable Pos
    ofxPanel guiCablePosLx;
    ofxPanel guiCablePosLy;
    ofxPanel guiCablePosRx;
    ofxPanel guiCablePosRy;
    ofParameterGroup parametersCablePos;
    vector<ofParameter<int>> cablePosLx; //lx,ly,rx,ry
    vector<ofParameter<int>> cablePosLy; //lx,ly,rx,ry
    vector<ofParameter<int>> cablePosRx; //lx,ly,rx,ry
    vector<ofParameter<int>> cablePosRy; //lx,ly,rx,ry
};
