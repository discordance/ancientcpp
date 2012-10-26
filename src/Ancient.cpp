//
//  Ancient.cpp
//  ancientSequencer
//
//  Created by Nunja on 8/12/12.
//  Copyright 2012 ISM. All rights reserved.
//

#include "Ancient.h"

Ancient::Ancient()
{
    m_auto_variation = false;
    m_swing = 0.;
    m_xor_variation = 0.;
    m_xor_mode = false;
    m_jacc_variation = 0;
    m_processing = false;
    
    // init eight tracks
    for(int i = 0; i < 8 ; i++)
    {
        Trak tr;
        m_tracks.push_back(tr);
    }
    
    // pitch map stuff
    static const int arr[] = {36,// kick
                              38,// snare1
                              40,// snare2
                              42,// chh
                              46,// ohh
                              47,// perc3   
                              48,// perc2
                              49 // crash
                              };
    vector<int> pitchmap (arr, arr + sizeof(arr) / sizeof(arr[0]) );
    assign_pitchmap(pitchmap);
    
}

vector<Trak>* Ancient::get_tracks()
{
    return &m_tracks;
}

bool Ancient::is_processing()
{
    return m_processing;
}

void Ancient::notify_bar()
{
    if(m_auto_variation)
    {
        if(m_xor_variation > 0.)
        {
            set_xor_variation(m_xor_variation);
            return;
        }
        
        if(m_jacc_variation > 0.)
        {
            set_jaccard_variation(m_jacc_variation);
            return;
        }
    }
}

void Ancient::ga(int track, float den, float rpv, float syn, float rep)
{
    // get size
    int size = m_tracks.at(track).get_size();
    vector<float> stats;
    stats.push_back((float)size);
    stats.push_back(den);
    stats.push_back(rpv);
    stats.push_back(syn);
    stats.push_back(rep);
    m_ga_tasks[track] = stats;
    startThread();
}

void Ancient::set_xor_mode(bool mode)
{
    m_xor_mode = mode;
    m_jacc_variation = 0.; // not two at the same time
    m_tasks.push_back("xor_var");
    startThread(); // threaded calculus
}

void Ancient::set_jaccard_variation(float thres)
{
    m_jacc_variation = thres;
    m_xor_variation = 0.; // not two at the same time
    m_tasks.push_back("jacc_var");
    startThread();
} 

void Ancient::set_xor_variation(float ratio)
{
    m_xor_variation = ratio;
    m_jacc_variation = 0.; // not two at the same time
    m_tasks.push_back("xor_var");
    startThread();
}

void Ancient::set_swing(float swg)
{
    m_swing = swg;
    m_tasks.push_back("swing");
    startThread();
}
void Ancient::set_groove(vector<float> groove)
{
    m_groove = groove;
    m_tasks.push_back("groove");
    startThread();
}

void Ancient::set_seq(Seq *seq)
{
    m_seq = seq;
    m_seq->set_ancient(this);
    m_seq->update_drum_tracks(&m_tracks);
}

int Ancient::get_track_pitch(int track)
{
    return m_tracks.at(track).get_pitch();
}

// protected --------------------------------------
void Ancient::assign_pitchmap(vector<int> pitchmap)
{
    if(m_tracks.size())
    {
        for(vector<Trak>::iterator track = m_tracks.begin(); track != m_tracks.end(); ++track)
        {
            int ct = track - m_tracks.begin();
            if(pitchmap.size()-1 >= ct)
            {
                track->set_pitch(pitchmap[ct]);
            }
        }
    }   
}

//--------------------------
void Ancient::threadedFunction()
{
    while( isThreadRunning() != 0 )
    {
        m_processing = true;
        if( lock() )
        {
            if(m_tasks.size())
            {
                string task = m_tasks[0];
                m_tasks.erase(m_tasks.begin());

                // update variation for all tracks
                std::vector<Trak>::iterator track;
                for(track = m_tracks.begin(); track != m_tracks.end(); ++track) 
                {
                    if(task == "jacc_var")
                    {    
                        track->set_jaccard_variation(m_jacc_variation);
                    }
                    else if(task == "xor_var")
                    {
                        track->set_xor_variation(m_xor_variation, m_xor_mode);
                    }
                    else if(task == "swing")
                    {
                        track->set_swing(m_swing);
                    }
                    else if(task == "groove")
                    {
                        track->set_beat_groove(m_groove);
                    }
                }
            }
  
            //ga
            if(m_ga_tasks.size())
            {
                vector<float> gas = m_ga_tasks.begin()->second;
                int track = m_ga_tasks.begin()->first;
                int size = (int)gas.at(0);
                float den = gas.at(1);
                float rpv = gas.at(2);
                float syn = gas.at(3);
                float rep = gas.at(4);
                m_ga_tasks.erase(m_ga_tasks.begin());
                vector< vector<int> > res = Gaia::ga(size, den, rpv, syn, rep);
                m_tracks[track].set_vanilla(Gaia::vel_to_phr(*res.begin()));
                
            }
            unlock();
        }
        m_seq->update_drum_tracks(&m_tracks);
        m_processing = false;
        stopThread();
    } 
}
    
