#include "MusicPlayer.h"

//--------------------------------------------------------------
void MusicPlayer::setup() {

	//-----------------------     MP3 Player      -------------------

	string path = "presetMusic/";
	dir.open(path);
	dir.allowExt("mp3");
	dir.listDir();


	currentTrack = 0;
	//go through and print out all the paths
	for (int i = 0; i < dir.size(); i++) {
		ofLogNotice(dir.getPath(i));
		if (i == currentTrack) {
			mp3.load(dir.getPath(currentTrack));
		}
	}
	lastPauseSoundTime = 0;
	//-----------------------     FFT      -------------------

	// the fft needs to be smoothed out, so we create an array of floats
	// for that purpose:
	fftSmoothed = new float[8192];
	for (int i = 0; i < 8192; i++) {
		fftSmoothed[i] = 0;
	}

	nBandsToGet = 128;
	fftSum = 0.0f;
	fftB1 = 0.0f;
	fftB2 = 0.0f;
	fftB3 = 0.0f;
	fftB4 = 0.0f;

	//-----------------------     Graph      -------------------
	graphScale = 0.05f; // scale = 1/20
	fftHistory.assign(0, 0.0);
	timeHistory.assign(0, 0.0);
	lastMaxSoundTime = 0;
	graphScrollX = 0;
	//-----------------------     Timeline      -------------------
	selectedKeyframe = &nullKeyframe;
	for (int i = 0; i < 20; i++) {
		Timeline timeline;
		timeline.setup();
		ofAddListener(timeline.keyframeSelectedEvent, this, &MusicPlayer::keyFrameSelected);
		ofAddListener(timeline.keyframeDeselectedEvent, this, &MusicPlayer::keyFrameDeselected);
		timelines.push_back(timeline);
	}
	
	
	
	//-----------------------     GUI      -------------------
	playButton.addListener(this, &MusicPlayer::playButtonPressed);
	nextButton.addListener(this, &MusicPlayer::nextButtonPressed);
	addKeyButton.addListener(this, &MusicPlayer::addKeyButtonPressed);
	removeKeyButton.addListener(this, &MusicPlayer::removeKeyButtonPressed);
	selectKeyButton.addListener(this, &MusicPlayer::selectKeyButtonPressed);
	pauseButton.addListener(this, &MusicPlayer::pauseButtonPressed);
	volume.addListener(this, &MusicPlayer::volumeChanged);
	graphSlide.addListener(this, &MusicPlayer::graphScrollChanged);
	keyframeSlider.addListener(this, &MusicPlayer::keyframeSliderChanged);
	saveButton.addListener(this, &MusicPlayer::saveButtonPressed);
	loadButton.addListener(this, &MusicPlayer::loadButtonPressed);

	gui.setup("panel");
	gui.setDefaultWidth(400);
	gui.add(playButton.setup("Play"));
	gui.add(nextButton.setup("Next"));
	gui.add(pauseButton.setup("Pause/Resume"));
	gui.add(snapButtonToggle.setup("Snap graph to center", false));
	gui.add(repeatThisToggle.setup("Repeat Current Track", false));
	gui.add(volume.setup("Volume", 0.8, 0, 1));
	gui.add(graphSlide.setup("Graph Slide", 0, 0, 1));
	gui.add(trackFileName.setup("Now Playing", ""));
	gui.add(saveButton.setup("Save Timeline for this track"));
	gui.add(loadButton.setup("Load Timeline for this track"));
	gui.add(addKeyButton.setup("Add Keyframe Mode"));
	gui.add(removeKeyButton.setup("Remove Keyframe Mode"));
	gui.add(selectKeyButton.setup("Select Keyframe Mode"));
	gui.add(keyframeSlider.set("Keyframe Value", 0, 0, 0));//the slider is disabled at start, enabled when a keyframe is selected
	

	gui.loadFromFile("musicPlayerSettings.xml");
}

//--------------------------------------------------------------
void MusicPlayer::update() {

	ofBackground(80, 80, 20);

	//-----------------------     MP3 Player      -------------------

	// update the sound playing system:
	ofSoundUpdate();



	//-----------------------     FFT      -------------------

	// (5) grab the fft, and put in into a "smoothed" array,
	//		by taking maximums, as peaks and then smoothing downward
	float * val = ofSoundGetSpectrum(nBandsToGet);		// request 128 values for fft
	fftSum = 0.0f;
	fftB1 = 0.0f;
	fftB2 = 0.0f;
	fftB3 = 0.0f;
	fftB4 = 0.0f;
	for (int i = 0; i < nBandsToGet; i++) {

		// let the smoothed calue sink to zero:
		fftSmoothed[i] *= 0.96f;

		// take the max, either the smoothed or the incoming:
		if (fftSmoothed[i] < val[i]) fftSmoothed[i] = val[i];

		//sum of all bands
		fftSum += fftSmoothed[i];

		//sum for 4-sub bands
		if (i < nBandsToGet / 4) {
			fftB1 += fftSmoothed[i];
		}else if (i < 2*nBandsToGet / 4) {
			fftB2 += fftSmoothed[i];
		}else if (i < 3*nBandsToGet / 4) {
			fftB3 += fftSmoothed[i];
		}else{
			fftB4 += fftSmoothed[i];
		}
	}

	//record the history if sound is playig. Skip drawing if the sound is looped ( where the currnetTime < lastMaxSoundTime)
	if (mp3.getIsPlaying() && mp3.getPositionMS()>= lastMaxSoundTime) {
		fftHistory.push_back(fftSum);
		timeHistory.push_back(mp3.getPositionMS());
		if (mp3.getPositionMS() > lastMaxSoundTime)
			lastMaxSoundTime = mp3.getPositionMS();
	}
	
	//-----------------------     GUI      -------------------

	mp3.setLoop(repeatThisToggle);

	//-----------------------     Timeline      -------------------
	for (int i = 0; i < 20; i++) {
		timelines[i].update();
	}
	
}

//--------------------------------------------------------------
void MusicPlayer::draw() {

	//-----------------------     FFT      -------------------
	ofPushStyle();
	ofEnableAlphaBlending();
	ofSetColor(255, 255, 255, 100);
	ofDrawRectangle(100, ofGetHeight() - 300, 5 * 128, 200);
	ofDisableAlphaBlending();

	// draw the fft resutls to bar chart:
	ofSetColor(255, 255, 255, 255);
	float width = (float)(5 * 128) / nBandsToGet;
	for (int i = 0; i < nBandsToGet; i++) {
		// (we use negative height here, because we want to flip them
		// because the top corner is 0,0)
		ofDrawRectangle(100 + i*width, ofGetHeight() - 100, width, -(fftSmoothed[i] * 200));
	}
	
	//calculate the data for drawing graphs
	float graphWidth = duration * 1.0f * graphScale;
	float playheadPos = 0.0f;
	if (mp3.getIsPlaying()) {
		playheadPos = graphWidth*mp3.getPosition();
	}else if (duration >0) {
		playheadPos = graphWidth*(lastPauseSoundTime*1.0f / duration);
	}

	if (snapButtonToggle && duration >0) {
		graphScrollX = mp3.getPosition() - (0.5f * ofGetWidth() / graphWidth);
	}
	ofPushMatrix();
	ofPoint guiPos = gui.getPosition();
	ofTranslate(-graphScrollX * graphWidth , 0, 0);//translate graph to current scroll

	//draw the fft history as a line graph
	ofSetColor(225);
	ofSetLineWidth(1);
	ofFill();
	ofSetColor(125);
	ofDrawRectangle(0, 400, graphWidth, 20);
	ofSetColor(225);
	if (fftHistory.size() > 0) {
		ofBeginShape();

		for (unsigned int i = 0; i < fftHistory.size(); i++) {
			if (i == 0) ofVertex(i, 400);
			float t = timeHistory[i];

			ofVertex(t*graphScale, 400 - fftHistory[i] * 10);

			if (i == fftHistory.size() - 1) ofVertex(t*graphScale, 400);
		}
		ofEndShape(false);
	}

	//draw the timeline
	for (int i = 0; i < 20; i++) {
		timelines[i].setPos(0, 420 + i * 20, graphWidth, graphScrollX * graphWidth, i);
		timelines[i].draw();
	}

	//draw playhead
	ofSetColor(100, 255, 100);
	ofDrawRectangle(playheadPos - 1, 320, 2, 500);
	
	ofPopMatrix();

	//print out the useful values , timeline's value : timeline.getValueAtPos(playheadPos) or getTimelineTweenValues()[0]
	ofSetColor(255, 255, 255);
	float currentPos = 0.0f;
	if (mp3.isPlaying()) {
		currentPos = mp3.getPositionMS() / 1000.0f;
	}else if(duration >0){
		currentPos = lastPauseSoundTime / 1000.0f;
	}

	string reportString = "fftSum: " + ofToString(fftSum) + "\ntime: " + ofToString(currentPos) + " / " + ofToString(duration / 1000.0f) + "\nTimeline: " + ofToString(getTimelineTweenValues());
	ofDrawBitmapString(reportString, 100, ofGetHeight() - 60);

	ofPopStyle();
	//-----------------------     GUI      -------------------

	gui.draw();

}



//---------------- Timeline functions ----------------

//function for other classes to get the timeline's keyframes tweening value
vector<float> MusicPlayer::getTimelineTweenValues() {
	float graphWidth = duration * 1.0f * graphScale;
	float playheadPos = 0.0f;
	if (mp3.getIsPlaying()) {
		playheadPos = graphWidth*mp3.getPosition();
	}
	else if (duration >0) {
		playheadPos = graphWidth*(lastPauseSoundTime*1.0f / duration);
	}

	vector<float> rslt;
	for (int i = 0; i < 20; i++) {
		float val = timelines[i].getValueAtPos(playheadPos);
		rslt.push_back(val);
	}

	return rslt;
}

void MusicPlayer::keyframeSliderChanged(float &val) {
	if (selectedKeyframe->x != nullKeyframe.x) {
		selectedKeyframe->val = val;
	}
}

void MusicPlayer::keyFrameSelected(Keyframe &kf) {
	//ofLog() << "select keyframe : " << kf.x;

	//deselect all timeline's keyframes
	for (int i = 0; i < 20; i++) {
		timelines[i].deselectKeyframes();
	}
	
	//re-select the kf
	kf.selected = true;
	selectedKeyframe = &kf;

	//ofLog() << keyframeSlider.getParameter().toString();// .setup("Keyframe Value", kf.val, 0, 20);
	keyframeSlider.setMax(KEYFRAME_MAX_VALUE);
	keyframeSlider.setMin(KEYFRAME_MIN_VALUE);
	keyframeSlider.set(kf.val);

}

void MusicPlayer::keyFrameDeselected(int &i) {
	selectedKeyframe = &nullKeyframe;

	keyframeSlider.setMax(0);
	keyframeSlider.setMin(0);
	keyframeSlider.set(0);
}

void MusicPlayer::addKeyButtonPressed() {
	for (int i = 0; i < 20; i++) {
		timelines[i].addKeyframeOnClick();
	}
}

void MusicPlayer::removeKeyButtonPressed() {
	for (int i = 0; i < 20; i++) {
		timelines[i].removeKeyframeOnClick();
	}
}

void MusicPlayer::selectKeyButtonPressed() {
	for (int i = 0; i < 20; i++) {
		timelines[i].selectKeyframeOnClick();
	}
}

//--------------------------------------------------------------
//-----------------------     GUI EVENTS     -------------------
//--------------------------------------------------------------

//--------------------------------------------------------------
void MusicPlayer::playButtonPressed() {
	computeSoundDuration();
	resetGraph();
	reloadTimelineFromSave();
	mp3.play();
	trackFileName.setup("Now Playing", dir.getName(currentTrack));
}

//--------------------------------------------------------------
void MusicPlayer::nextButtonPressed() {

	if (currentTrack < dir.size() - 1)
	{
		currentTrack++;
	}
	else {
		currentTrack = 0;
	}
	mp3.load(dir.getPath(currentTrack));
	computeSoundDuration();
	resetGraph();
	reloadTimelineFromSave();
	mp3.play();
	trackFileName.setup("Now Playing", dir.getName(currentTrack));

	ofLog() << "CurrentTrack : " << currentTrack;
}

void MusicPlayer::pauseButtonPressed() {
	if (mp3.isPlaying()) {
		lastPauseSoundTime = mp3.getPositionMS();
		mp3.stop();
	} else {
		mp3.play();
		mp3.setPositionMS(lastPauseSoundTime);
	}
}

void MusicPlayer::mousePressed(int x, int y, int button) {
	//hit the timeline of the graph
	if (y > 400 && y < 420) {
		float graphWidth = duration * 1.0f * graphScale;
		float time = duration * (((x*1.0 )/graphWidth)+graphScrollX);
		if (time > duration)
			time = duration;
		if (time < 0.0f)
			time = 0.0f;
		ofLog() << "time : " << time;
		mp3.setPositionMS(time);
		if (!mp3.getIsPlaying()) {
			lastPauseSoundTime = time;
		}
	}
	for (int i = 0; i < 20; i++) {
		timelines[i].mousePressed(x, y, button);
	}
}

void MusicPlayer::saveButtonPressed() {
	ofxXmlSettings track;
	track.addTag("track");
	track.pushTag("track");
	//volHistory is we want to save to a file
	for (int i = 0; i < timelines.size(); i++) {
		track.addTag("timeline");
		track.pushTag("timeline", i);
		Timeline timeline = timelines[i];
		for (int j = 0; j < timeline.frames.size(); j++) {
			//each position tag represents one point
			track.addTag("snd");
			track.pushTag("snd", j);
			//so set the three values in the file
			track.addValue("val", timeline.frames[j].val);
			track.addValue("x", timeline.frames[j].x);
			track.popTag();
		}
		track.popTag();
	}
	track.popTag(); //pop position
	track.saveFile("track_"+ ofToString(currentTrack) + ofToString(".xml"));
}

void MusicPlayer::loadButtonPressed() {
	reloadTimelineFromSave();
}

void MusicPlayer::reloadTimelineFromSave() {
	//reset the timeline previous values
	for (int i = 0; i < 20; i++) {
		timelines[i].reset();
	}

	//This is how you would load that very same file    
	ofxXmlSettings track;
	if (track.loadFile("track_" + ofToString(currentTrack) + ofToString(".xml"))) {
		track.pushTag("track");
		int numTimeline = track.getNumTags("timeline");
		for (int i = 0; i < numTimeline; i++) {
			track.pushTag("timeline", i);

			int numSnd = track.getNumTags("snd");
			for (int j = 0; j < numSnd; j++) {
				track.pushTag("snd", j);
				timelines[i].addKeyframeByVal(track.getValue("val", 0.0f), track.getValue("x", 0.0f));
				track.popTag();
			}
			track.popTag();
		}

		track.popTag(); //pop position
	}
	else {
		ofLogError("Track Save file did not load!");
	}
}

//--------------------------------------------------------------
void MusicPlayer::volumeChanged(float &setVolume) {
	mp3.setVolume(setVolume);
}

void MusicPlayer::graphScrollChanged(float &setScroll) {
	graphScrollX = setScroll;
}

void MusicPlayer::computeSoundDuration() {
	//set to half position and get the millisecond there
	mp3.play();
	mp3.setPosition(0.5f);
	duration = mp3.getPositionMS()*2 -1;
	mp3.setPosition(0);
	mp3.stop();
	printf("SOUND LENGTH: %i\n\n", duration);
}

void MusicPlayer::resetGraph() {
	fftHistory.clear();
	timeHistory.clear();
	lastMaxSoundTime = 0;
	lastPauseSoundTime = 0;

	//reset timeline
	for (int i = 0; i < 20; i++) {
		timelines[i].reset();
	}
	selectedKeyframe = &nullKeyframe;
}