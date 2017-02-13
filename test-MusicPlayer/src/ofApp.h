#pragma once

#include "ofMain.h"
#include "ofxGui.h"

class ofApp : public ofBaseApp{

	public:

        //-----------------------     MP3 Player      -------------------
    
        ofDirectory dir;
    
		ofSoundPlayer 		mp3;
        int currentTrack;
    
        //-----------------------     FFT      -------------------
        float 				* fftSmoothed;
        int nBandsToGet;
    
        //-----------------------     GUI      -------------------
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
    
        //-----------------------     Other      -------------------

        void exit();
    
    
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
    


};

