#ifndef __ParkYohoMusicPlayer__
#define __ParkYohoMusicPlayer__

#include <stdio.h>
#include "ofMain.h"
#include "ofxGui.h"
#include "Timeline.h"
#include "ofxXmlSettings.h"


// A class that wrap the music player, fft graph, timeline
// set the KEYFRAME_MIN_VALUE, KEYFRAME_MAX_VALUE for the keyframe's max/min value
// "getTimelineTweenValues()" will return all the timelines's values in a vector
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
			int duration;

			//-----------------------     FFT      -------------------
			float 				* fftSmoothed;
			int nBandsToGet;

			float fftSum;
			float fftB1; //the sum of the first 1/4 of the bands (not used yet)
			float fftB2; //the sum of the second 1/4 of the bands (not used yet)
			float fftB3;  //the sum of the third 1/4 of the bands (not used yet)
			float fftB4; //the sum of the fourth 1/4 of the bands (not used yet)

			//-----------------------     Graph      -------------------
			float graphScrollX; //the current page of the graph
			vector <float> fftHistory; //history for fftSum
			vector <float> timeHistory; //history for the time of sound
			float graphScale; //draw scale for the graph 


			//-----------------------     Timeline      -------------------
			vector <Timeline> timelines;


			//-----------------------     GUI      -------------------
			ofxButton playButton;
			ofxButton nextButton;
			ofxButton pauseButton;
			ofxButton saveButton;
			ofxButton loadButton;

			ofxLabel trackFileName;
			//ofxToggle repeatAllToggle; //TODO
			ofxToggle repeatThisToggle; //TODO
			ofxToggle snapButtonToggle;

			ofxFloatSlider volume;
			ofxFloatSlider graphSlide;
			ofxButton addKeyButton;
			ofxButton removeKeyButton;
			ofxButton selectKeyButton;
			ofParameter<float> keyframeSlider;

			ofxPanel gui;


			void playButtonPressed();
			void nextButtonPressed();
			void pauseButtonPressed();
			void saveButtonPressed();
			void loadButtonPressed();
			void addKeyButtonPressed();
			void removeKeyButtonPressed();
			void selectKeyButtonPressed();
			void volumeChanged(float &setVolume);
			void graphScrollChanged(float &setScroll);
			void keyframeSliderChanged(float &val);
			void keyFrameSelected(Keyframe &kf);
			void keyFrameDeselected(int &i);

			//Use this function to get the tween value of timeline
			vector<float> getTimelineTweenValues();

			void mousePressed(int x, int y, int button);

		private:
			int lastMaxSoundTime; //record the last max sound time, used for prevent redraw the graph when song looped
			int lastPauseSoundTime; //record the time when the sound is paused
			void computeSoundDuration(); //calculate the sound's duration when playing a new sound
			void resetGraph(); //reset the fft sum graph and the timelines when playing a new sound
			Keyframe *selectedKeyframe; //selected keyframe for editing its value
			Keyframe nullKeyframe; //a keyframe that is null, i.e. no selected key frame
			const float KEYFRAME_MIN_VALUE = 0.0f; //THe minimum value for a keyframe in slider
			const float KEYFRAME_MAX_VALUE = 20.0f; //the maximum value for a keyframe in slider

			void reloadTimelineFromSave();//load the timeline save file

	};


#endif
