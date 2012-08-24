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
        tr.set_vanilla(Trak::str_to_phr("00000000000000000000000000000000"));
        m_tracks.push_back(tr);
    }
    
    // test set
    
    m_tracks[0].set_vanilla(Trak::str_to_phr("f000f000f00f0000f000f000f00f0000"),Trak::MODE_LOW_PERC);
    m_tracks[1].set_vanilla(Trak::str_to_phr("05000f000f00006000000f000f000060"),Trak::MODE_SNARE);
    m_tracks[3].set_vanilla(Trak::str_to_phr("00f000f6 00f000f000f000f000f000f"),Trak::MODE_OVERHEAD);
    m_tracks[4].set_vanilla(Trak::str_to_phr("69896989698969896989698969896989"),Trak::MODE_OVERHEAD);
    
    
    
    //ofLog(OF_LOG_NOTICE, ofToString(Trak::get_jaccard_variation(m_tracks[0].get_current(),0.98).size()));
    //int st = ofGetElapsedTimeMicros();
    //for (int i = 0; i < 1; i++)
    //{
    //      Trak::get_jaccard_variation(m_tracks[3].get_current(),0.99);
    //}
   // ofLog(OF_LOG_NOTICE, "time: "+ofToString(ofGetElapsedTimeMicros() - st));
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

vector<Trak>* Ancient::get_tracks()
{
    return &m_tracks;
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

void Ancient::set_jaccard_variation(float thres)
{
    m_jacc_variation = thres;
    // update variation for all tracks
    std::vector<Trak>::iterator track;
    for(track = m_tracks.begin(); track != m_tracks.end(); ++track) 
    {
        track->set_jaccard_variation(m_jacc_variation);
    }
    m_seq->update_drum_tracks(&m_tracks);
} 

void Ancient::set_xor_variation(float ratio)
{
    m_xor_variation = ratio;
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
