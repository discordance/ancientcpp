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
    
    // init mode
    m_mode = mode;
    if(mode > 6 || mode < 0)
    {
        m_mode = 0;
    }
    
    // init groove
    for(int g = 0; g < 4; ++g)
    {
        m_groove.push_back(0.);
    }
    
    // init levels and variat
    m_level = 2;
    m_variat = 0;
    
    // init pitch
    m_pitch = 0;
    
    vector<Step> zero_phr;
    // init current
    for(int i=0; i < size; ++i)
    {
        Step st;
        st.vel = 0;
        st.dur = 1;
        st.lock = FALSE;
        st.ctrl = vector<float>(4,0.);
        st.drift = 0.0;
        zero_phr.push_back(st);
    }
    // init the matrix
    for (int m = 0; m < 5; ++m)
    {
        vector< vector<Step> > line;
        m_matrix.push_back(line);
        for (int n = 0; n < 5; ++n)
        {
            m_matrix.at(m).push_back(zero_phr);
        }
    }
    m_vanilla = &m_matrix.at(2).at(0); // update vanilla pointer
    m_current = *m_vanilla; // copy vanilla to current;
    
    // update and check the size
    set_size(size);
}

// accessors
void Trak::set_vanilla(vector<Step> phr)
{
    m_matrix.at(2).at(0) = phr;
    m_vanilla = &m_matrix.at(2).at(0);
    set_size(phr.size());
    update_groove();
    update_current();
}

void Trak::set_matrix(vector< vector < vector<Step> > > matrix)
{
    m_matrix = matrix;
    m_vanilla = &m_matrix.at(2).at(0);
    set_size(m_vanilla->size());
    update_groove();
    update_current();
}

void Trak::set_level_variat(int level, int variat)
{
    m_level = level;
    m_variat = variat;
    update_current();
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
    m_matrix.at(2).at(0) = phr;
    m_vanilla = &m_matrix.at(2).at(0);
    set_size(phr.size());
    if(mode >= 0)
    {
        m_mode = mode;
    }
    update_groove();
    update_current();
}

bool Trak::has_events()
{
    // get the current level / variat
    vector<Step>::iterator step;
    // iterate
    for(step = m_current.begin(); step != m_current.end(); ++step)
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
    
    // updates the size/the groove
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
        vector<int> vari = Gaia::euclidian_variation(&*m_vanilla, thres);
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
        update_current();
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
        vector<int> vari = Gaia::jaccard_variation(&m_matrix.at(m_level).at(m_variat), thres);
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
        update_current();
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
        if(m_size <= 4){ mode = 1;}
        
        // we take the quarter of the phrase
        int div = 4;
        
        if((m_size/2)%div){
            --div;
        }
        int vlengh = (m_size/div)/2;
        // we roll a dice to make a start or ending var
        int voffset = 0;
        if(ofRandom(0, 2) > 1.)
        {
            voffset = (m_size/2) - vlengh;
        }    
        
        unsigned char rate = (unsigned char)ofMap(ratio, 0, 1, 0, 255); // ok valid
        vector<unsigned char> cbytes = Gaia::steps_to_bytes(&get_matrix_target());
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
            Step *vstep = &get_matrix_target().at(vel - res_vels.begin());
            
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
        update_current();
    }
}

/**
 * Impact on Vanilla
 */
void Trak::set_swing(float swg)
{
    // check
    if(swg >= 1){ swg = 0.99; }
    if(swg <= -1){ swg = -0.99; }
    vector<float> groove;
    for(int i = 0; i < 4 ; ++i)
    {
        if(i % 2 != 0)
        {
           groove.push_back(swg);
        }
        else
        {
            groove.push_back(0);
        }
    }
    m_groove = groove;
    update_groove();
    update_current();
}

void Trak::set_beat_groove(vector<float> drifts)
{
    m_groove = drifts;
    update_groove();
    update_current();
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
    Gaia::dump_vel(&*m_vanilla);
}



//protected

//--------------------------------------------------------------

vector<Step>& Trak::get_matrix_target()
{
    return m_matrix.at(m_level).at(m_variat);
}

void Trak::update_current()
{
    m_current = m_matrix.at(m_level).at(m_variat);
}
/**
 * Impact on matrix
 */
void Trak::update_size()
{
    //updates the size of the current and matrix to fit with the new size
    // resize matrix
    vector < vector< vector<Step> > >::iterator level;
    for(level = m_matrix.begin(); level != m_matrix.end(); ++level)
    {
        vector< vector<Step> >::iterator variat;
        for(variat = level->begin(); variat != level->end(); ++variat)
        {
            if(variat->size() > m_size)
            {
                variat->resize(m_size); // data lost here : TODO non destructive
            }
            if(variat->size() < m_size)
            {
                //add empty steps
                for(int i = 0; i < m_size - variat->size(); ++i)
                {
                    Step st;
                    st.vel = 0;
                    st.dur = 1;
                    st.lock = FALSE;
                    st.ctrl = vector<float>(4,0.);
                    variat->push_back(st);
                }
            }
        }
    }
}

/**
 * Impact on matrix
 */
void Trak::update_groove()
{
    // update matrix
    vector < vector< vector<Step> > >::iterator level;
    for(level = m_matrix.begin(); level != m_matrix.end(); ++level)
    {
        vector< vector<Step> >::iterator variat;
        for(variat = level->begin(); variat != level->end(); ++variat)
        {
            vector<Step>::iterator step;
            for(step = variat->begin(); step != variat->end(); ++step)
            {
                int ct = step - variat->begin();
                step->drift = m_groove.at(ct%m_groove.size());
            }
        }
    }
}
