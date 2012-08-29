//
//  Seq.h
//  ancientSequencer
//
//  Created by Nunja on 7/14/12.
//  Copyright 2012 ISM. All rights reserved.
//
#ifndef SEQ
#define SEQ

#include <vector>
#include "ofxMidi.h"
#include "utils/ofLog.h"
#include "utils/ofUtils.h"
#include "Trak.h"

using namespace std;

class Seq : public ofxMidiListener, public ofThread
{
    
    public:
        Seq();
        float get_bpm();
        int get_ticks();
        void set_midi_delay(int dly);
        void exit();
        void update_drum_tracks(vector<Trak> *tracks);
        
    
    protected:
        // evt
        struct Evt 
        {
            int status; // note on or off
            int channel; // channel
            int pitch;
            int vel;
        };
    
        void newMidiMessage(ofxMidiMessage& eventArgs);
        void reset_events();
        void send_events(vector<Evt> *evts);
        void kill_events(int chan);
        
        // midi
        ofxMidiIn	m_midiIn;
        ofxMidiOut  m_virtual_midiOut; // for internal routing
        ofxMidiOut  m_hard_midiOut; // for hardware routing
        ofxMidiMessage m_midiMessage;
        
        // seq mode/ res
        bool m_synced_seq; // tells if the sequencer is sync to an external clock or not (use an internal clock thread)
        int  m_resolution; // 96 for self clock, synced must be 24
        int  m_max_ticks; // size of the event grid, depends on the resolution
        int  m_max_steps; // max steps of the step sequencer;
        // seq status
        int   m_midi_delay; // delay of the clock
        bool  m_started;
        int   m_ticks;
        float m_bpm;
        
        // helpers
        int m_clock_past_time;
        
        // seq data
        vector< vector<Evt> > m_events; // max ticks at 96 ppqn for 128 squav
};


#endif