#ifndef __ParkYohoMovementController__
#define __ParkYohoMovementController__

#include <stdio.h>
#include "ofMain.h"
#include "ofxGui.h"

class
MovementController {
public:
    
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
    
    int numOfCables;
    
    
    MovementController(){
        
        
    }
    
    void setup(int cableNum, int x1, int y1,int w, int h){
        
        
        curveControls.setup("CurveReadWrite", "settings.xml", x1+w+10, y1);
        for(int i=0; i< 8; i++){
            ofParameter<int> a;
            a.set("C" + ofToString(i),0,0,1000); //lx,ly,rx,ry
            curves.push_back(a);
            curveControls.add(curves[i]);
            
        }
        int eW = 80;
        curveControls.setSize(eW, 100);
        curveControls.setWidthElements(eW);
        
        curveCoord = ofPoint(x1,y1);
        curveW = w;
        curveH = h;
        numOfCables = cableNum;
        
        
        
    }
    
    vector<ofPoint> getPoints(){
        vector<ofPoint> p;
        p.clear();
        for(int i=0; i< numOfCables; i++){
            p.push_back(trail.getPointAtPercent((float)i/numOfCables));
        }
        return p;
        
    }
    
    void update(){
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
    };
    
    void draw(){
        // ofEnableSmoothing();
        
        
        ofSetColor(255,0,0);
        
        ofDrawRectangle(curveCoord.x, curveCoord.y, curveW, curveH);
        ofSetColor(20);
        trail.draw();
        for(int i=0; i< numOfCables; i++){
            ofSetColor(0, 140, 255);
            ofPoint p = getPoints()[i];
            ofDrawCircle(p,5);
        }
        
        // ofDisableSmoothing();
        curveControls.draw();
    };
    
    
    
};


#endif
