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
    m_midi_delay = 0;
    m_ticks = 0;
    m_bpm = 120;
    // synced by default
    m_synced_seq = true;
    m_resolution = 24;
    m_max_ticks = (m_resolution / 4) * 128;
    m_max_steps = 128;
    
    // init mutes to 0
    for(int i = 0; i < 8; ++i){ m_mutes[i] = false; }
    
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

void Seq::toggle_mute(int track, bool status)
{
    m_mutes[track] = status;
    kill_events(10, m_ancient->get_track_pitch(track));
}

float Seq::get_bpm()
{
    return m_bpm;
}

int Seq::get_ticks()
{
    return m_ticks;
}

void Seq::set_ancient(Ancient * anc)
{
    m_ancient = anc;
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


// todo -> groove management is not clean
// todo -> ghost note probz ???
void Seq::update_drum_tracks(vector<Trak> *tracks) // v1, replace all
{
    /***********************************************
     * V2
     *
     ***/
    if(lock())
    {
        reset_events();
        int mult = m_max_ticks/m_max_steps;
        std::vector<Trak>::iterator track;
        for(track = tracks->begin(); track != tracks->end(); ++track)
        {
            // track num
            int tr_num = track - tracks->begin();
            // get current
            vector<Step>* current = track->get_current();
            int ps = current->size(); // phrase size
            vector<Step>::iterator step;
            map<int, vector<int> > evts; // events map for the track to correct it
            
            for(int i = 0; i < m_max_steps; ++i)
            {
                int modi=i%ps;
                Step cstep = current->at(modi);
                if(cstep.vel)
                {
                    vector<int> evt;
                    int t_dur = (cstep.dur*mult)-1;
                    int cstick = i * mult; // current start tick
                    int drift = mult*(1+cstep.drift) - mult;
                    cstick += drift;
                    int cetick = cstick + t_dur;
                    cetick += drift;
                    int vel = ofMap(cstep.vel, 0, 15, 0, 127);
                    evt.push_back(cstick);
                    evt.push_back(cetick);
                    evt.push_back(vel);
                    evts[i] = evt;
                }
                else
                {
                    evts[i] = vector<int>(0);
                }
            }
            // correct the overlaping events and update
        }
        unlock();
    }
    /***********************************************
     * V1
     *
     ***/
    /*
    if(lock())
    {
        reset_events();
        // update tracks
        // iterator
        std::vector<Trak>::iterator track;
        for(track = tracks->begin(); track != tracks->end(); ++track) 
        {
            // track num
            int tr_num = track - tracks->begin();
            
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
                    on.track = tr_num;
                    on.status = 1; // note on on channel 10
                    on.channel = 10;
                    on.pitch = track->get_pitch();
                    on.vel = vel;
                    
                    Evt off;
                    off.track = tr_num;
                    off.status = 0; // note off on channel 10
                    off.channel = 10;
                    off.pitch = track->get_pitch();
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
    */ 
}

//--------------------------------------------------------------

void Seq::correct_and_update(map<int, vector<int> >& evt_map, int track, int pitch)
{
    vector<int>* n_p;
    vector<int>* n_c;
    vector<int>* n_n; // fuck pointers
    map<int, vector<int> >::iterator curr;
    map<int, vector<int> >::iterator prev;
    map<int, vector<int> >::iterator next;
    for(curr = evt_map.begin(); curr != evt_map.end(); curr++)
    {
        n_c = &curr->second;
        
        if(curr != evt_map.begin())
        {
            prev--;
            n_p = &prev->second;
        };
        if(curr != evt_map.end())
        {
            next++;
            n_n = &next->second;
        };
        if(n_p->size() && n_c->size())
        {
            if(n_p->at(1) >= n_c->at(0))
            {
                //n_p->insert(<#iterator __position#>, <#const value_type &__x#>)
                int *trg = &n_p->at(1);
                trg = &n_c->at(0);
            }
        };
    }
}

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
    
    if( (m_ticks+m_midi_delay) % (m_resolution*4) == 0 )
    {
        m_ancient->notify_bar();
    }
    
    if( lock() )
    {
        // make a copy of the latest message
        m_midiMessage = msg;
        int at = (m_ticks+m_midi_delay)%m_max_ticks;
        if(at < 0)
        {
            at = 0;
        }
        vector<Evt> *line = &m_events.at(at); // here because of the 0 event line must be used
        
        
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

void Seq::kill_events(int chan, int pitch)
{
    m_virtual_midiOut.sendNoteOff(chan, pitch ,0);
}

void Seq::send_events(vector<Evt>* evts)
{
    vector<Evt>::iterator ev;
    for(ev = evts->begin(); ev != evts->end(); ++ev )
    {
        if(!m_mutes[ev->track])
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
}


