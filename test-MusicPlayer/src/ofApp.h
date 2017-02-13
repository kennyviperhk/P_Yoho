#pragma once

#include "ofMain.h"
#include "ofxGui.h"

class ofApp : public ofBaseApp{

	public:

		void setup();
		void update();
		void draw();

		void keyPressed  (int key);
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
    
        ofDirectory dir;

		ofSoundPlayer 		mp3;
        int currentTrack;
    
        ofxButton playButton;
        ofxButton nextButton;
    
        ofxLabel trackFileName;
        //ofxToggle repeatAllToggle; //TODO
        ofxToggle repeatThisToggle; //TODO
    
        ofxFloatSlider volume;
    
        ofxPanel gui;
    
        void playButtonPressed();
        void nextButtonPressed();
        void volumeChanged(float &setVolume);
        void exit();

		float 				* fftSmoothed;
		// we will draw a rectangle, bouncing off the wall:
		float 				px, py, vx, vy;

		int nBandsToGet;
		float prevx, prevy;
};

