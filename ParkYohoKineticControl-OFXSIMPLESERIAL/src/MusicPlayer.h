#ifndef __ParkYohoMusicPlayer__
#define __ParkYohoMusicPlayer__

#include <stdio.h>
#include "ofMain.h"
#include "ofxGui.h"

class
	MusicPlayer {
		public:
			void setup();
			void update();
			void draw();


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
	};


#endif
