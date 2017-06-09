#ifndef __ParkYohoDmxLight__
#define __ParkYohoDmxLight__

#include <stdio.h>
#include "ofMain.h"
#include "ofxDmx.h"
#include "ofxGui.h"

class
	DmxLight {
		public:
			void setup();
			void update();
			void draw();
            void exit();

			ofxDmx dmx;
			ofxPanel panel1;
			ofxPanel panel2;
			string port;
			int modules, channelsPerModule;
				ofParameter<bool> moduleNum[80];

				ofParameter<float> red[80], green[80], blue[80], white[80];
				ofParameter<bool> load, save;

	};


#endif
