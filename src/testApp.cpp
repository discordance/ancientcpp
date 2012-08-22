#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup()
{
    // init framerate
    ofSetFrameRate(60);
    m_ancient.set_seq(&m_seq);
    m_view_bpm = 120;
    m_view_midi_delay = 4;
    m_view_swing = 0.;
    m_view_xor_mode = false;
    
    // INTERFACE
    float xInit = OFX_UI_GLOBAL_WIDGET_SPACING;
    float length = 190-xInit; 
    float dim = 6;
    
    gui_g = new ofxUICanvas(0, 0, length+xInit, ofGetHeight());
    gui_g->addWidgetDown(new ofxUILabel("ANCIENT SEQ :::::::", OFX_UI_FONT_LARGE));  
    gui_g->addWidgetDown(new ofxUISpacer(length-xInit, 1));
    gui_g->addWidgetDown(new ofxUILabel("GLOBAL ..........................", OFX_UI_FONT_MEDIUM));
    gui_g->addWidgetDown(new ofxUISpacer(length-xInit, 1)); 
    gui_g->addWidgetDown(new ofxUISlider(length-xInit,dim, -24, +24, m_view_midi_delay, "live_midi_delay")); 
    gui_g->addWidgetDown(new ofxUISpacer(48-xInit, 1)); 
    gui_g->addWidgetDown(new ofxUISlider(length-xInit,dim, -0.99, 0.99, m_view_swing, "swing")); 
    gui_g->addWidgetDown(new ofxUILabel("XOR VARIATOR ...........", OFX_UI_FONT_MEDIUM));
    gui_g->addWidgetDown(new ofxUISpacer(length-xInit, 1)); 
    gui_g->addWidgetDown(new ofxUISlider(length-xInit,dim*2, 0., 1., m_view_xor_variation, "xor_variation")); 
    // XOR var modes
    vector<string> xor_modes; 
	xor_modes.push_back("part");
	xor_modes.push_back("full");
    ofxUIRadio *xor_modes_gui = (ofxUIRadio *)gui_g->addWidgetDown(new ofxUIRadio( dim*2, dim*2, "xor_mode", xor_modes, OFX_UI_ORIENTATION_HORIZONTAL)); 
    xor_modes_gui->activateToggle(xor_modes[0]);
    ofAddListener(gui_g->newGUIEvent,this,&testApp::gui_gEvent);
    
}

void testApp::gui_gEvent(ofxUIEventArgs &e)
{
    string name = e.widget->getName(); 
	int kind = e.widget->getKind(); 
    
    if(name == "live_midi_delay")
	{
		ofxUISlider *slider = (ofxUISlider *) e.widget; 
		m_view_midi_delay = slider->getScaledValue(); 
        m_seq.set_midi_delay(m_view_midi_delay);
    }
    else if(name == "swing")
    {
        ofxUISlider *slider = (ofxUISlider *) e.widget; 
		m_view_swing = slider->getScaledValue(); 
        m_ancient.set_swing(m_view_swing);
    }
    else if(name == "xor_variation")
    {
        ofxUISlider *slider = (ofxUISlider *) e.widget; 
		m_view_xor_variation = slider->getScaledValue(); 
        m_ancient.set_xor_variation(m_view_xor_variation);
    } 
    else if(name == "part")
    {
        m_view_xor_mode = false;
        m_ancient.set_xor_mode(m_view_xor_mode);
    } 
    else if(name == "full")
    {
        m_view_xor_mode = true;
        m_ancient.set_xor_mode(m_view_xor_mode);
    }
    
}

//--------------------------------------------------------------
void testApp::update()
{
    if(ofGetFrameNum() % 12 == 0) // refresh slower
    {
        m_view_bpm = m_seq.get_bpm();
    }
    
    
}

//--------------------------------------------------------------
void testApp::exit()
{
    // cleanup
}

//--------------------------------------------------------------
void testApp::draw(){
    
    ofPushStyle(); 
	ofEnableBlendMode(OF_BLENDMODE_ALPHA);     
	ofPopStyle(); 
    
    // graphic background
    ofBackground(33, 33, 33);
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){

}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}