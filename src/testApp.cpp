#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup()
{
    // init framerate
    ofSetFrameRate(30);
    m_ancient.set_seq(&m_seq);
    m_view_bpm = 120;
    m_view_midi_delay = 4;
    m_view_swing = 0.;
    m_view_auto_variation = false;
    m_view_xor_mode = false;
    m_view_xor_variation =0.;    
    m_view_jacc_variation = 0.;
    // heuristics 
    m_rpv = 0.;    
    m_syn = 0.;
    m_rep = 0.;
    
    // INTERFACE
    float xInit = OFX_UI_GLOBAL_WIDGET_SPACING;
    float length = 190-xInit; 
    float dim = 6;
    ofxUIWidget *w;
    
    gui_g = new ofxUICanvas(0, 0, length+xInit, ofGetHeight());
    gui_g->addWidgetDown(new ofxUILabel("ANCIENT SEQ :::::::", OFX_UI_FONT_LARGE));  
    gui_g->addWidgetDown(new ofxUISpacer(length-xInit, 1));
    gui_g->addWidgetDown(new ofxUILabel("TRANSPORT .................", OFX_UI_FONT_MEDIUM));
    gui_g->addWidgetDown(new ofxUISpacer(length-xInit, 1));
    gui_g->addWidgetDown(new ofxUILabel("GLOBAL ..........................", OFX_UI_FONT_MEDIUM));
    gui_g->addWidgetDown(new ofxUISpacer(length-xInit, 1)); 
    w = gui_g->addWidgetDown(new ofxUISlider(length-xInit,dim, -24, +24, m_view_midi_delay, "live_midi_delay")); 
    w->setColorBack(ofColor(255,128,0,128));
    gui_g->addWidgetDown(new ofxUISpacer(48-xInit, 1)); 
    w = gui_g->addWidgetDown(new ofxUISlider(length-xInit,dim, -0.99, 0.99, m_view_swing, "swing")); 
    w->setColorBack(ofColor(255,128,0,128));
    w = gui_g->addWidgetDown(new ofxUIToggle(dim*3, dim*3, m_view_auto_variation, "auto_variation")); 
    w->setColorBack(ofColor(255,128,0,128));
    gui_g->addWidgetDown(new ofxUILabel("XOR VARIATOR ...........", OFX_UI_FONT_MEDIUM));
    gui_g->addWidgetDown(new ofxUISpacer(length-xInit, 1)); 
    m_view_xor_slider = (ofxUISlider *) gui_g->addWidgetDown(new ofxUISlider(length-xInit,dim*2, 0., 1., m_view_xor_variation, "xor_variation"));  
    m_view_xor_slider->setColorBack(ofColor(255,128,0,128));
    
    // XOR var modes
    vector<string> xor_modes; 
	xor_modes.push_back("part");
	xor_modes.push_back("full");
    ofxUIRadio *xor_modes_gui = (ofxUIRadio *)gui_g->addWidgetDown(new ofxUIRadio( dim*2, dim*2, "xor_mode", xor_modes, OFX_UI_ORIENTATION_HORIZONTAL)); 
    vector<ofxUIToggle *> tggls = xor_modes_gui->getToggles();
    tggls.at(0)->setColorBack(ofColor(255,128,0,128));
    tggls.at(1)->setColorBack(ofColor(255,128,0,128));
    xor_modes_gui->activateToggle(xor_modes[0]);
    gui_g->addWidgetDown(new ofxUILabel("JAC VARIATOR ...........", OFX_UI_FONT_MEDIUM));
    m_view_jacc_slider = (ofxUISlider *) gui_g->addWidgetDown(new ofxUISlider(length-xInit,dim*2, 0., 1., m_view_jacc_variation, "jacc_variation"));  
    m_view_jacc_slider->setColorBack(ofColor(255,128,0,128));
    gui_g->addWidgetDown(new ofxUISpacer(length-xInit, 1));
    w = gui_g->addWidgetDown(new ofxUIButton(dim*3, dim*3, false, "smooth_jacc"));  
    w->setColorBack(ofColor(255,128,0,128));
    w = gui_g->addWidgetDown(new ofxUIButton(dim*3, dim*3, false, "hard_jacc")); 
    w->setColorBack(ofColor(255,128,0,128));
    w = gui_g->addWidgetDown(new ofxUIButton(dim*3, dim*3, false, "vanilla")); 
    w->setColorBack(ofColor(255,128,0));
    
    ofAddListener(gui_g->newGUIEvent,this,&testApp::gui_gEvent);
    
    // widgets d
    gui_d = new ofxUICanvas(ofGetWidth() - (length+xInit), 0, length+xInit, ofGetHeight());
    gui_d->addWidgetDown(new ofxUISpacer(length-xInit, 1));
    gui_d->addWidgetDown(new ofxUILabel("GROOVE ..........................", OFX_UI_FONT_MEDIUM));
    gui_d->addWidgetDown(new ofxUISpacer(length-xInit, 1));
    gui_d->addWidgetDown(new ofxUILabel("GA .....................................", OFX_UI_FONT_MEDIUM));
    w = gui_d->addWidgetDown(new ofxUISlider(length-xInit,dim*2, 0., 1., m_rpv, "rpv"));
    w = gui_d->addWidgetDown(new ofxUISlider(length-xInit,dim*2, 0., 1., m_syn, "syn"));
    w = gui_d->addWidgetDown(new ofxUISlider(length-xInit,dim*2, 0., 1., m_rep, "rep"));
    //w = gui_d->addWidgetDown(new ofxUIButton(dim*4, dim*4, false, "generate"));
    //w->setColorBack(ofColor(255,128,0));
    
    ofAddListener(gui_d->newGUIEvent,this,&testApp::gui_gEvent);
    
    int w_offset = 4;
    int tr_height = ofGetHeight();
    int trashed = 2 * (length+xInit);
    int tr_width = (int)((ofGetWidth() - trashed)-(w_offset*8)) /8.; // @TODO a revoir moin defonced
    int ct = 0;
    
    //m_ancient.ga(1, 0.5, 0.5, 0.5, 0.5);
    // tracks
    
    for(int i = 0 ; i < 8 ; ++i)
    {
       ofxUIWidget *iw;
       int x = (length+xInit+w_offset)+((w_offset+tr_width)*i); // @TODO a revoir moin defonced
       ofxUICanvas * tr_gui = new ofxUICanvas(x, 0 , tr_width, tr_height);
       iw = tr_gui->addWidgetDown(new ofxUIToggle(dim*3, dim*3, true, "mt"+ofToString(i)));
       iw->setColorBack(ofColor(255, 0, 0)); 
       iw = tr_gui->addWidgetDown(new ofxUIButton(dim*2, dim*2, false, "ga"+ofToString(i)));
       iw->setColorBack(ofColor(255,128,0));
       ofAddListener(tr_gui->newGUIEvent,this,&testApp::gui_gEvent);
    }
}

void testApp::gui_gEvent(ofxUIEventArgs &e)
{
    string name = e.widget->getName(); 
	int kind = e.widget->getKind(); 
    
    // trick to avoid double triggers
    if(!m_trigg[name])
    {
        m_trigg[name] = true;
    }
    else
    {
        m_trigg[name] = false;
        return;
    }
    
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
    else if(name == "auto_variation")
    {
        ofxUIToggle *butt = (ofxUIToggle *) e.widget;
        m_view_auto_variation = butt->getValue();
        m_ancient.m_auto_variation = m_view_auto_variation;
    }
    else if(name == "xor_variation")
    {
        ofxUISlider *slider = (ofxUISlider *) e.widget; 
		m_view_xor_variation = slider->getScaledValue(); 
        m_view_jacc_variation = 0.;
        m_view_jacc_slider->setValue(0.);
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
    else if(name == "jacc_variation")
    {
        ofxUISlider *slider = (ofxUISlider *) e.widget; 
		m_view_jacc_variation = ofMap(slider->getScaledValue(), 0., 1., 0.97, 0.99); 
        if(m_view_jacc_variation < 0.9701) // hack do not why
        {
            m_view_jacc_variation = 0.;
        }
        m_view_xor_variation = 0.;
        m_view_xor_slider->setValue(0.);
        m_ancient.set_jaccard_variation(m_view_jacc_variation);
    }
    else if(name == "smooth_jacc")
    {
        ofxUIButton *butt = (ofxUIButton *) e.widget;
        bool val = butt->getValue();
        m_view_jacc_variation = 0.978;
        m_view_xor_variation = 0.;
        m_view_xor_slider->setValue(0.);
        m_view_jacc_slider->setValue(ofMap(m_view_jacc_variation, 0.97,0.99,0.,1.));
        m_ancient.set_jaccard_variation(m_view_jacc_variation);
    }
    else if(name == "hard_jacc")
    {
        ofxUIButton *butt = (ofxUIButton *) e.widget;
        bool val = butt->getValue(); 
        m_view_jacc_variation = 0.99;
        m_view_xor_variation = 0.;
        m_view_xor_slider->setValue(0.);
        m_view_jacc_slider->setValue(ofMap(m_view_jacc_variation, 0.97,0.99,0.,1.));
        m_ancient.set_jaccard_variation(m_view_jacc_variation);
    }
    else if(name == "vanilla")
    {
        ofxUIButton *butt = (ofxUIButton *) e.widget;
        bool val = butt->getValue(); 
        m_view_jacc_variation = 0;
        m_view_xor_variation = 0.;
        m_view_xor_slider->setValue(0.);
        m_view_jacc_slider->setValue(0.);
        m_ancient.set_jaccard_variation(0);
    }

    string::iterator nmchar;
    string res = "";
    int tr;
    for (nmchar = name.begin(); nmchar != name.end(); ++nmchar)
    {		
        res += *nmchar;
        if(res == "ga")
        {
            stringstream ss;
            string s;
            ss << *name.rbegin();
            ss >> s;
            tr = ofToInt(s);
        }
        else if(res == "mt")
        {
            stringstream ss;
            string s;
            ss << *name.rbegin();
            ss >> s;
            tr = ofToInt(s); 
        } 
    }
    
}

//--------------------------------------------------------------
void testApp::update()
{
    if(ofGetFrameNum() % 8 == 0) // refresh slower
    {
        m_view_bpm = m_seq.get_bpm();
       // update_sliders();
    }
    
}

void testApp::update_sliders()
{
    vector<Trak> * tracks = m_ancient.get_tracks();
    std::vector<Trak>::iterator track;
    for(track = tracks->begin(); track != tracks->end(); ++track) 
    {
        int cta = track - tracks->begin();
        vector<Step> * steps;
        steps = track->get_current();
        std::vector<Step>::iterator step;
        for(step = steps->begin(); step != steps->end(); ++step) 
        {
            
            int ctb = step - steps->begin();
            vector<ofxUISlider* > * sliders = &gui_sliders.at(cta);
            ofxUISlider* slider = sliders->at(ctb);
            slider->setValue(ofMap(step->vel, 0, 15, 0., 1.));
            
        }
    }
}

//--------------------------------------------------------------
void testApp::exit()
{
    // cleanup
    m_seq.exit();
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