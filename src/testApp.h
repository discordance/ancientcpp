#pragma once

#include "ofxUI.h"
#include "Seq.h"
#include "Ancient.h"

class testApp : public ofBaseApp{
	public:
		void setup();
		void update();
		void draw();
        void exit();
		
		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y);
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
    
        // iface responders
        
        
        // sequencer
        Seq m_seq;
        Ancient m_ancient;
    
        // interface
        ofxUICanvas *gui_g;   	
        void gui_gEvent(ofxUIEventArgs &e);
        bool drawPadding; 
    
    protected:
        float m_view_bpm;
        int   m_view_midi_delay;
        float m_view_swing;
        float m_view_xor_variation;
        bool  m_view_xor_mode;
};
