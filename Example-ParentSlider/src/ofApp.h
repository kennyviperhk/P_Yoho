#pragma once

#include "ofMain.h"
#include "ofxGui.h"

class MyButton: public ofxButton
{
public:
    MyButton(): ofxButton()
    {
        
    }
    
    virtual bool mouseReleased(ofMouseEventArgs & args)
    {
        ofxButton::mouseReleased(args);
        
        
        cout << "helllllo" << endl;
    }

};



class MySlider: public ofxFloatSlider
{
public:
    MySlider(): ofxFloatSlider()
    {
        
    }
    
    virtual bool mouseReleased(ofMouseEventArgs & args)
    {
        ofxFloatSlider::mouseReleased(args);
        
//        cout << "slider value: " << value << endl;
        updatedValue = true;
    }
    
    bool isValueChanged()
    {
        return updatedValue;
    }
    
    float getChangedValue()
    {
        updatedValue = false;
        return value;
    }
    
    
    
    bool updatedValue;
    
};

class ofApp : public ofBaseApp{
	
public:
	void setup();
	void update();
	void draw();
	
	void exit();

	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y );
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void mouseEntered(int x, int y);
	void mouseExited(int x, int y);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);		

	void circleResolutionChanged(int & circleResolution);
	void ringButtonPressed();

	bool bHide;

	ofxFloatSlider radius;
	ofxColorSlider color;
	ofxVec2Slider center;
	ofxIntSlider circleResolution;
	ofxToggle filled;
	ofxButton twoCircles;
	ofxButton ringButton;
	ofxLabel screenSize;

	ofxPanel gui;

	ofSoundPlayer ring;
    
    MyButton buttonnn;
    MySlider mySlid;
};

