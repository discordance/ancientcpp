//
//  Seq.cpp
//  ancientSequencer
//
//  Created by Nunja on 7/14/12.
//  Copyright 2012 ISM. All rights reserved.
//


#include "Seq.h"

//--------------------------------------------------------------
Seq::Seq(){
    // value init
    m_started = false;
    m_midi_delay = 4;
    m_ticks = 0;
    m_bpm = 120;
    // synced by default
    m_synced_seq = true;
    m_resolution = 24;
    m_max_ticks = (m_resolution / 4) * 128;
    m_max_steps = 128;
    
    // reset events 
    reset_events();
    
    // midi init
    m_midiIn.openVirtualPort("Ancient Sync In");
    m_midiIn.ignoreTypes(false, false, false);
    
    // virtual port
    m_virtual_midiOut.openVirtualPort("Ancient Notes Out");
	
	// add testApp as a listener
	m_midiIn.addListener(this);
    ofLog(OF_LOG_NOTICE, "initialized Ancient sequencer with a resolution of: "+ofToString(m_max_ticks));
}

float Seq::get_bpm()
{
    return m_bpm;
}

int Seq::get_ticks()
{
    return m_ticks;
}

void Seq::set_midi_delay(int dly)
{
    if(lock())
    {
        m_midi_delay = dly;
        unlock();
    }
}

void Seq::exit()
{
    kill_events(10); // note off on drum events
    m_midiIn.closePort();
    m_midiIn.removeListener(this);
    m_virtual_midiOut.closePort();
}

void Seq::update_drum_tracks(vector<Trak> *tracks) // v1, replace all
{
    if(lock())
    {
        reset_events();
        // update tracks
        // iterator
        std::vector<Trak>::iterator track;
        for(track = tracks->begin(); track != tracks->end(); ++track) 
        {
            // get current
            vector<Step>* current = track->get_current();
            int ps = current->size(); // phrase size
            for(int i = 0; i < m_max_steps; ++i)
            {
                int modi=i%ps;
                Step cstep = current->at(modi);

                if(cstep.vel)
                {
                    int mult = m_max_ticks/m_max_steps; // multiplier to map from 128 to max in res
                    int cstick = i * mult; // current start tick 
                    int drift = mult*(1+cstep.drift) - mult;
                    cstick += drift;
                    int cetick = cstick + ( (cstep.dur*mult) - drift );
                    cetick = (cetick >= m_max_ticks) ? m_max_ticks - 1: cetick; // secure the last dur
                    int vel = ofMap(cstep.vel, 0, 15, 0, 127);
                    
                    Evt on;
                    on.status = 1; // note on on channel 10
                    on.channel = 10;
                    on.pitch = track->m_pitch;
                    on.vel = vel;
                    
                    Evt off;
                    off.status = 0; // note off on channel 10
                    off.channel = 10;
                    off.pitch = track->m_pitch;
                    off.vel = vel;
                    
                    vector<Evt>* event_line_on = &m_events.at(cstick);
                    event_line_on->push_back(on);
                    vector<Evt>* event_line_off = &m_events.at(cetick);
                    event_line_off->push_back(off);
                }
                
            }
        }
        unlock();
    }    
}

//--------------------------------------------------------------

void Seq::reset_events()
{   
        if(m_events.size())
        {
            m_events.clear();
        }
        
        // max ticks at 24 ppqn for 128 squav
        // init the vector event pointers
        for(int i = 0; i < m_max_ticks; ++i)
        {
            m_events.push_back(vector<Evt>(0)); 
        }
    
}

void Seq::newMidiMessage(ofxMidiMessage& msg) {
    
    if( lock() )
    {
        // make a copy of the latest message
        m_midiMessage = msg;
        int at = (m_ticks+m_midi_delay)%m_max_ticks;
        if(at < 0)
        {
            at = 0;
        }
        vector<Evt> *line = &m_events.at(at); // here because of the 0 evtent line must be used
        
        
        // start
        if (msg.status == 250)
        {
            m_started = true;
            m_ticks = 0;
        }
        // stop
        if (msg.status == 252)
        {
            m_started = false;
            m_ticks = 0;
            // send all channels / noteoffs
            kill_events(10);
        }
        // time
        if (msg.status == 248 && m_started)
        {
            m_ticks++;
            // send events
            
            send_events(line);

            // compute the bpm
            if(m_synced_seq)
            {   
                m_bpm = 60000.0/(ofGetElapsedTimeMillis()-m_clock_past_time)/24;
                m_clock_past_time = ofGetElapsedTimeMillis();
            }
            
        }
        
        //ofLog(OF_LOG_NOTICE, ofToString(msg.status) + " ticks: " + ofToString(m_ticks) + " BPM: " + ofToString(m_bpm));
        unlock();
    }
}

void Seq::kill_events(int chan)
{
    for (int i = 0; i < 128; i++) {
        m_virtual_midiOut.sendNoteOff(chan,i,0);
    }
}

void Seq::send_events(vector<Evt>* evts)
{
    vector<Evt>::iterator ev;
    for(ev = evts->begin(); ev != evts->end(); ++ev )
    {
        if(ev->status)
        {
            m_virtual_midiOut.sendNoteOn(ev->channel, ev->pitch, ev->vel); 
        }
        else
        {
            m_virtual_midiOut.sendNoteOff(ev->channel, ev->pitch, ev->vel);
        }
    }
}


