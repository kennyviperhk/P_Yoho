#include "ofApp.h"


//--------------------------------------------------------------
void ofApp::setup(){	 

    //-----------------------     MP3 Player      -------------------
    
    string path = "presetMusic/";
    dir.open(path);
    dir.allowExt("mp3");
    dir.listDir();
    
    currentTrack = 0;
    //go through and print out all the paths
    for(int i = 0; i < dir.size(); i++){
        ofLogNotice(dir.getPath(i));
        if(i==currentTrack){
            mp3.load(dir.getPath(currentTrack));
        }
    }
    
    //-----------------------     FFT      -------------------
    
	// the fft needs to be smoothed out, so we create an array of floats
	// for that purpose:
	fftSmoothed = new float[8192];
	for (int i = 0; i < 8192; i++){
		fftSmoothed[i] = 0;
	}
	
	nBandsToGet = 128;
    
    //-----------------------     GUI      -------------------
    
    playButton.addListener(this,&ofApp::playButtonPressed);
    nextButton.addListener(this,&ofApp::nextButtonPressed);
    volume.addListener(this, &ofApp::volumeChanged);
    
    gui.setup("panel");
    gui.setDefaultWidth(400);
    gui.add(playButton.setup("Play"));
    gui.add(nextButton.setup("Next"));
    gui.add(repeatThisToggle.setup("Repeat Current Track", false));
    gui.add(volume.setup("Volume", 0.8, 0, 1));
    gui.add(trackFileName.setup("Now Playing", ""));
    
    gui.loadFromFile("musicPlayerSettings.xml");
}

//--------------------------------------------------------------
void ofApp::update(){
	
	ofBackground(80,80,20);

    //-----------------------     MP3 Player      -------------------
    
	// update the sound playing system:
	ofSoundUpdate();
    

    
    //-----------------------     FFT      -------------------

	// (5) grab the fft, and put in into a "smoothed" array,
	//		by taking maximums, as peaks and then smoothing downward
	float * val = ofSoundGetSpectrum(nBandsToGet);		// request 128 values for fft
	for (int i = 0;i < nBandsToGet; i++){
		
		// let the smoothed calue sink to zero:
		fftSmoothed[i] *= 0.96f;
		
		// take the max, either the smoothed or the incoming:
		if (fftSmoothed[i] < val[i]) fftSmoothed[i] = val[i];
		
	}

    
    //-----------------------     GUI      -------------------
    
    mp3.setLoop(repeatThisToggle);
    
}

//--------------------------------------------------------------
void ofApp::draw(){


    
    //-----------------------     FFT      -------------------
    
	ofEnableAlphaBlending();
		ofSetColor(255,255,255,100);
		ofDrawRectangle(100,ofGetHeight()-300,5*128,200);
	ofDisableAlphaBlending();
	
	// draw the fft resutls:
	ofSetColor(255,255,255,255);
	
	float width = (float)(5*128) / nBandsToGet;
	for (int i = 0;i < nBandsToGet; i++){
		// (we use negative height here, because we want to flip them
		// because the top corner is 0,0)
		ofDrawRectangle(100+i*width,ofGetHeight()-100,width,-(fftSmoothed[i] * 200));
	}
    
    
    //-----------------------     GUI      -------------------
    
    gui.draw();

}

//--------------------------------------------------------------
//-----------------------     GUI EVENTS     -------------------
//--------------------------------------------------------------

//--------------------------------------------------------------
void ofApp::playButtonPressed(){
    mp3.play();
    trackFileName.setup("Now Playing", dir.getName(currentTrack));
}

//--------------------------------------------------------------
void ofApp::nextButtonPressed(){
    
    if(currentTrack < dir.size() -1)
    {
        currentTrack++;
    }else{
        currentTrack=0;
    }
    mp3.load(dir.getPath(currentTrack));
    mp3.play();
    trackFileName.setup("Now Playing", dir.getName(currentTrack));
    
    ofLog() << "CurrentTrack : " << currentTrack;
}


//--------------------------------------------------------------
void ofApp::volumeChanged(float &setVolume){
    mp3.setVolume(setVolume);
}


//--------------------------------------------------------------
//-----------------------     ON EXIT        -------------------
//--------------------------------------------------------------

//--------------------------------------------------------------
void ofApp::exit(){
    playButton.removeListener(this,&ofApp::playButtonPressed);
    trackFileName.setup("Now Playing", "");
    gui.saveToFile("musicPlayerSettings.xml");
}


//--------------------------------------------------------------
//-----------------------     UNUSED     -----------------------
//--------------------------------------------------------------

//--------------------------------------------------------------
void ofApp::keyPressed  (int key){ 
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){ 
	
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
	
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

