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
        ofxUICanvas *gui_g; // left gui  
        ofxUICanvas *gui_d; // left gui  
        vector< vector<ofxUISlider *> > gui_sliders;
        void gui_gEvent(ofxUIEventArgs &e);
        bool drawPadding; 
    
    protected:
    
        // 
        void update_sliders();    
        // view elems
        ofxUISlider * m_view_xor_slider;
        ofxUISlider * m_view_jacc_slider;
        // view values;
        float m_view_bpm;
        int   m_view_midi_delay;
        float m_view_swing;
        bool  m_view_auto_variation;
        float m_view_xor_variation;
        bool  m_view_xor_mode;
        float m_view_jacc_variation;
};
