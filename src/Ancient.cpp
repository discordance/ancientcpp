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
    m_swing = 0.;
    m_xor_variation = 0.;
    m_xor_mode = false;
    
    // init eight tracks
    for(int i = 0; i < 8 ; i++)
    {
        Trak tr;
        tr.set_vanilla(Trak::str_to_phr("0000000000000000"));
        m_tracks.push_back(tr);
    }
    
    // test set
    m_tracks[0].set_vanilla(Trak::str_to_phr("f000f000f000f000"),Trak::MODE_LOW_PERC);
    m_tracks[1].set_vanilla(Trak::str_to_phr("00000f000f000060"),Trak::MODE_SNARE);
    m_tracks[3].set_vanilla(Trak::str_to_phr("00f000f000f000f0"),Trak::MODE_OVERHEAD);
    m_tracks[4].set_vanilla(Trak::str_to_phr("6989698969896989"),Trak::MODE_OVERHEAD);
    
    // test
    /*for (int i = 0; i < 3000; i++)
    {
        ofLog(OF_LOG_NOTICE, ofToString(Trak::get_normal(10.,1.)) );
    }*/
    
    
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


void Ancient::set_xor_mode(bool mode)
{
    m_xor_mode = mode;
    // update variation for all tracks
    std::vector<Trak>::iterator track;
    for(track = m_tracks.begin(); track != m_tracks.end(); ++track) 
    {
        track->set_xor_variation(m_xor_variation, m_xor_mode);
    }
    m_seq->update_drum_tracks(&m_tracks);
}

void Ancient::set_xor_variation(float var)
{
    m_xor_variation = var;
    // update variation for all tracks
    std::vector<Trak>::iterator track;
    for(track = m_tracks.begin(); track != m_tracks.end(); ++track) 
    {
        track->set_xor_variation(m_xor_variation, m_xor_mode);
    }
    m_seq->update_drum_tracks(&m_tracks);
}

void Ancient::set_swing(float swg)
{
    m_swing = swg;
    
    // update swing for all tracks
    std::vector<Trak>::iterator track;
    for(track = m_tracks.begin(); track != m_tracks.end(); ++track) 
    {
        track->set_swing(swg);
    }
    m_seq->update_drum_tracks(&m_tracks);
}

void Ancient::set_seq(Seq *seq)
{
    m_seq = seq;
    m_seq->update_drum_tracks(&m_tracks);
}


// protected --------------------------------------
void Ancient::assign_pitchmap(vector<int> pitchmap)
{
    if(m_tracks.size())
    {
        for(vector<Trak>::iterator it = m_tracks.begin(); it != m_tracks.end(); ++it)
        {
            int ct = it - m_tracks.begin();
            if(pitchmap.size()-1 >= ct)
            {
                it->m_pitch = pitchmap[ct];
            }
        }
    }   
}
