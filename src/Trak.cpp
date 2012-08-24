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
    for(int i=0; i < m_size; i++)
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

void Trak::set_jaccard_variation(float thres)
{
    if(!has_events())
    {
        return;
    }
    if(thres > 0.)
    {
        vector<int> vari = get_jaccard_variation(&m_vanilla, thres);
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
        vector<unsigned char> cbytes = Trak::get_steps2bytes(&m_vanilla);
        vector<unsigned char> rbytes;
        vector<unsigned char>::iterator cbyte;
        // iterate
        for(cbyte = cbytes.begin(); cbyte != cbytes.end(); ++cbyte)
        {
            int ct = cbyte - cbytes.begin();
            unsigned char tbyte = *cbyte;
            unsigned char varbyte = static_cast<unsigned char> (get_variation_rate()*ofRandom(rate));
            
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
        
        vector<int> res_vels = Trak::get_bytes2ints(rbytes);
       
        vector<int>::iterator vel;
        for(vel = res_vels.begin(); vel != res_vels.end(); ++vel)
        {
            Step *cstep = &m_current.at(vel - res_vels.begin());
            Step *vstep = &m_vanilla.at(vel - res_vels.begin());
      
            cstep->vel = max(*vel,vstep->vel); // we take the max
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
    Trak::swing_phr(&m_vanilla,swg);
    // current
    Trak::swing_phr(&m_current,swg);
}

int Trak::get_size()
{
    return m_size;
}

// dumps
void Trak::dump_current_vel()
{
    Trak::dump_vel(&m_current);
}

void Trak::dump_vanilla_vel()
{
    Trak::dump_vel(&m_vanilla);
}

//static utilities
//--------------------------------------------------------------
string Trak::phr_to_str(vector<Step> *phr)
{
    string res;
    vector<Step>::iterator step;
    for(step = phr->begin(); step != phr->end(); ++step)
    {
        string a;
        std::stringstream ss1;
        ss1 << std::hex << step->vel;
        ss1 >> a;
        res += a;
    }
    return res;
}
string Trak::vel_to_str(vector<int> ins)
{
    string res;
    vector<int>::iterator num;
    for(num = ins.begin(); num != ins.end(); ++num)
    {
        string a;
        std::stringstream ss1;
        ss1 << std::hex << *num;
        ss1 >> a;
        res += a;
    }
    return res;
}    

vector<int> Trak::str_to_vel(string str)
{
    vector<int> res;
    string::iterator str_iter;
    for(str_iter = str.begin(); str_iter != str.end(); str_iter++)
    {
        // cast hex str to int
        int x;   
        std::stringstream ss;
        ss << std::hex << *str_iter;
        ss >> x;
        
        res.push_back(x);
    }
    return res;
}

vector<Step> Trak::str_to_phr(string str)
{
    vector<Step> res;
    string::iterator str_iter;
    for(str_iter = str.begin(); str_iter != str.end(); str_iter++)
    {
        // cast hex str to int
        int x;   
        std::stringstream ss;
        ss << std::hex << *str_iter;
        ss >> x;
        
        // create the step
        Step st;
        st.vel = x;
        st.dur = 1;
        st.lock = FALSE;
        st.drift = 0;
        st.ctrl = vector<float>(4,0.);
        
        res.push_back(st);
    }
    return res;
}

// swing this phrase
void Trak::swing_phr(vector<Step> *phr, float swing)
{
    // check
    if(swing >= 1){ swing = 0.99; }
    if(swing <= -1){ swing = -0.99; }
    // 
    if(phr->size()) // check
    {
        // iterate
        vector<Step>::iterator step;
        for(step = phr->begin(); step != phr->end(); ++step)
        {
            int ct = step - phr->begin();
            if(ct % 2 != 0)
            {
                step->drift = swing; 
            }
        }
    }    
}

vector<int> Trak::get_jaccard_variation(vector<Step> *phr, float thres)
{
    
    thres = ofClamp(thres, 0.97, 0.99);
    string goal;
    string target = phr_to_str(phr);
    int inc = 4;
    
    for(int i = 0; i < target.length(); i += inc)
    {
        int to = (inc > target.length()-i)? target.length()-i : inc;
        to += i;
        string part = target.substr(i,to-i);
        string res;
        float score = 1;

        while (score > thres) {
            vector<int> rnd;
            for(int j = i; j < to; ++j)
            {
                //ofLog(OF_LOG_NOTICE, ofToString(j) + " " + ofToString(i) + " " + ofToString(to));
                int rvel = ofClamp(Trak::normal( phr->at(j).vel, 12.),0,15);
                rnd.push_back(rvel);
            }
            res = Trak::vel_to_str(rnd);
            score = Trak::wjaccard(part, res);
            //ofLog(OF_LOG_NOTICE, "res " + res + " part " + part + " score " + ofToString(score));
        }
        goal += res;
    }
   // ofLog(OF_LOG_NOTICE, goal);
    return Trak::str_to_vel(goal);
}

// gaussian random
float Trak::normal(float mean, float stdev)
{
    float rnd = (ofRandom(1) * 2 - 1) + (ofRandom(1) * 2 - 1) + (ofRandom(1) * 2 - 1);
    return rnd * stdev + mean;
}

// weighted jaccard
float Trak::wjaccard(string s1, string s2)
{
    int l1, l2;
    l1 = s1.length();
    l2 = s2.length();
    int ct = 0;
    float  a = 0, b = 0 , same = 0, diff = 0 ;
    /* strings must be of equal size and non-zero length */
    if (l1 == 0 || l1 != l2) {
        return (-1.0);
    }
    if(s1.compare(s2) == 0)
    {
        return 0.;
    }
    while (ct != l1) {
        
        if (s1.at(ct) == s2.at(ct)) {
            same++;
        } else {
            int aa,bb;
            std::stringstream ss1, ss2;
            ss1 << std::hex << s1.at(ct);
            ss1 >> aa;
       
            ss2 << std::hex << s2.at(ct);
            ss2 >> bb;
            
            a = pow((float)aa,2);
            b = pow((float)bb,2);
            diff += abs(a-b);
            
        }
        ct++;
    }
    return (1 - (same / (diff + same)));
}



// byte array to vector of int
vector<int> Trak::get_bytes2ints(vector<unsigned char> bytes)
{
    vector<int> res;
    vector<unsigned char>::iterator cbyte;
    for(cbyte = bytes.begin(); cbyte != bytes.end(); ++cbyte)
    {
        int a = (*cbyte & 0x0f);
        int b = *cbyte >> 4;
        res.push_back(a);
        res.push_back(b);
    }
    return res;
}

// vector of steps to byte array
vector<unsigned char> Trak::get_steps2bytes(vector<Step> *phr)
{
    vector<unsigned char> res;
    for(int i = 0; i < phr->size(); i+=2) 
    {
        unsigned char cbyte;
        cbyte = (cbyte & 0xF0) | (phr->at(i).vel & 0xF); // write low quartet
        cbyte = (cbyte & 0x0F) | ((phr->at(i+1).vel & 0xF) << 4); 
        res.push_back(cbyte);
    }
    return res;
}

void Trak::dump_vel(vector<Step> *phr)
{
    string dumped = "";    
    for(std::vector<Step>::iterator it = phr->begin(); it != phr->end(); ++it) 
    {
        int ct = it - phr->begin();
        if(ct % 4 == 0)
        {
            dumped += " ";
        }
        dumped += "[";
        string velstr = ofToString(it->vel);
        if(velstr.size() < 2)
        {
            dumped += "0";
        }
        dumped += velstr;
        dumped += "]";
    }
    ofLog(OF_LOG_NOTICE, dumped);
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
            for(int i = 0; i < m_size - m_vanilla.size(); i++)
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
        case MODE_OVERHEAD:
            return 0.65;
            break;
        case MODE_FREE_MIND:
            return 0.45;
            break;    
        case MODE_ONE_SHOT:
            return 0.8;
            break;
        case MODE_BASS_NOTE:
            return 0.1; // tbd
            break;
        case MODE_NOTE:
            return 0.1; // tbd
            break;
        default:
            return 1.;
            break;
    }
}
