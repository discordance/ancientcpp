//
//  Track.cpp
//  ancientSequencer
//
//  Created by Nunja on 8/12/12.
//  Copyright 2012 ISM. All rights reserved.
//

#include "Trak.h"

//--------------------------------------------------------------
Trak::Trak(int mode, int size){
    
    // mode
    m_mode = mode;
    if(mode > 6 || mode < 0)
    {
        m_mode = 0;
    }
    
    // sets the size
    set_size(size);
    
    // init current
    for(int i=0; i < m_size; ++i)
    {
        Step st;
        st.vel = 0;
        st.dur = 1;
        st.lock = FALSE;
        st.ctrl = vector<float>(4,0.);
        st.drift = 0.0;
        m_vanilla.push_back(st);
    }
    m_current = m_vanilla; // copy current to vanilla @TODO best way or matrix 2x0 ?
}

// accessors
void Trak::set_vanilla(vector<Step> phr)
{
    m_vanilla = phr;
    set_size(phr.size());
}

void Trak::set_pitch(int pitch)
{
    m_pitch = pitch;
}

int Trak::get_pitch()
{
    return m_pitch;
}

void Trak::set_vanilla(vector<Step> phr, int mode = -1)
{
    m_vanilla = phr;
    set_size(phr.size());
    if(mode >= 0)
    {
        m_mode = mode;
    }
}

bool Trak::has_events()
{
    vector<Step>::iterator step;
    // iterate
    for(step = m_vanilla.begin(); step != m_vanilla.end(); ++step)
    {
        if(step->vel)
        {
            return true;
        }
    }
    return false;
}

vector<Step>* Trak::get_current()
{
    return &m_current;
}

vector<int> Trak::get_current_vel()
{
    return Gaia::steps_to_vel(&m_current);
}

vector<Step> Trak::get_current_copy()
{
    return m_current;
}

void Trak::set_size(int size)
{
    // size check
    m_size = size;
    if(m_size % 2 != 0)
    {
        m_size = m_size + (m_size % 2);
    }
    if(m_size < 4 || m_size > 128)
    {
        m_size = 16;
    }
    
    // updates the size
    update_size();
}

void Trak::set_euclidian_variation(float thres)
{
    if(!has_events())
    {
        return;
    }
    if(thres > 0.)
    {
        
    }    
}

void Trak::set_jaccard_variation(float thres)
{
    if(!has_events())
    {
        return;
    }
    if(thres > 0.)
    {
        vector<int> vari = Gaia::jaccard_variation(&m_vanilla, thres);
        vector<int>::iterator vel;
        // iterate
        for(vel = vari.begin(); vel != vari.end(); ++vel)
        {
            Step *step = &m_current.at(vel - vari.begin());
            step->vel = *vel;
        }
    }
    else
    {
        // reset vanilla
        m_current = m_vanilla;
    }
}

/**
 * XOR Variation with a bit of Random
 * No on Vanilla
 * @TODO not the coolest thing on earth  for now
 */
void Trak::set_xor_variation(float ratio, bool mode)
{
    if(!has_events())
    {
        return;
    }
    if(ratio > 0)
    {
        // cannot do partial var on 4 squav phrases
        if(m_vanilla.size() <= 4){ mode = 1;}
        
        // we take the quarter of the phrase
        int vlengh = ofNextPow2(m_vanilla.size()/4)/2;
        // we roll a dice to make a start or ending var
        int voffset = 0;
        if(ofRandom(0, 2) > 1)
        {
            int voffset = (m_vanilla.size()/2) - vlengh;
        }    
        
        unsigned char rate = (unsigned char)ofMap(ratio, 0, 1, 0, 255); // ok valid
        vector<unsigned char> cbytes = Gaia::steps_to_bytes(&m_vanilla);
        vector<unsigned char> rbytes;
        vector<unsigned char>::iterator cbyte;
        // iterate
        for(cbyte = cbytes.begin(); cbyte != cbytes.end(); ++cbyte)
        {
            int ct = cbyte - cbytes.begin();
            unsigned char tbyte = *cbyte;
            float modifier = Gaia::type_stats.at(m_mode).at(0);
            unsigned char varbyte = static_cast<unsigned char> (modifier*ofRandom(rate));
            
            if(mode) // full mode
            {
                tbyte = tbyte ^ varbyte;
            }
            else // partial mode
            {
                if(ct >= voffset && ct <= voffset+vlengh)
                {
                    tbyte = tbyte ^ varbyte;
                }
            }
              
            rbytes.push_back(tbyte);
        }
        
        vector<int> res_vels = Gaia::bytes_to_ints(rbytes);
       
        vector<int>::iterator vel;
        for(vel = res_vels.begin(); vel != res_vels.end(); ++vel)
        {
            Step *cstep = &m_current.at(vel - res_vels.begin());
            Step *vstep = &m_vanilla.at(vel - res_vels.begin());
            
            if(Gaia::get_vel_group(*vel) != Gaia::get_vel_group(vstep->vel))
            {
                cstep->vel = *vel;
            }
            else
            {
                cstep->vel = vstep->vel;
            }
        }
    }
    else
    {
        // reset vanilla
        m_current = m_vanilla;
    }
}

/**
 * Impact on Vanilla
 */
void Trak::set_swing(float swg)
{
    // vanilla
    Gaia::swing_phr(&m_vanilla,swg);
    // current
    Gaia::swing_phr(&m_current,swg);
}

int Trak::get_size()
{
    return m_size;
}

// dumps
void Trak::dump_current_vel()
{
    Gaia::dump_vel(&m_current);
}

void Trak::dump_vanilla_vel()
{
    Gaia::dump_vel(&m_vanilla);
}



//protected

//--------------------------------------------------------------
/**
 * Impact on Vanilla
 */
void Trak::update_size()
{
    //updates the size of the current and matrix to fit with the new size
    
    // current phrase and vanilla
    if(m_vanilla.size()) // check if init
    {
        if(m_vanilla.size() > m_size)
        {
            m_vanilla.resize(m_size);// data lost here : TODO non destructive
        }
        if(m_vanilla.size() < m_size)
        {
            //add empty steps
            for(int i = 0; i < m_size - m_vanilla.size(); ++i)
            {
                Step st;
                st.vel = 0;
                st.dur = 1;
                st.lock = FALSE;
                st.ctrl = vector<float>(4,0.);
                m_vanilla.push_back(st);
            }
        }  
        m_current = m_vanilla;
    }
    
    
    // resize matrix
    // ....
}

/*
float Trak::get_variation_rate()
{
    switch (m_mode) {
        case MODE_LOW_PERC:
            return 0.15;
            break;
        case MODE_PERC:
            return 0.45;
            break; 
        case MODE_SNARE:
            return 0.35;
            break;
        case MODE_HITHAT:
            return 0.65;
            break;
        case MODE_OVERHEAD:
            return 0.65;
            break;
        case MODE_ONE_SHOT:
            return 0.8;
            break;
        default:
            return 1.;
            break;
    }
}
 */
