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

vector<int> Trak::get_current_vel()
{
    return Trak::steps_to_vel(&m_current);
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
        vector<int> vari = jaccard_variation(&m_vanilla, thres);
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
        vector<unsigned char> cbytes = Trak::steps_to_bytes(&m_vanilla);
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
        
        vector<int> res_vels = Trak::bytes_to_ints(rbytes);
       
        vector<int>::iterator vel;
        for(vel = res_vels.begin(); vel != res_vels.end(); ++vel)
        {
            Step *cstep = &m_current.at(vel - res_vels.begin());
            Step *vstep = &m_vanilla.at(vel - res_vels.begin());
            
            if(Trak::get_vel_group(*vel) != Trak::get_vel_group(vstep->vel))
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

vector<int> Trak::euclidian_variation(vector<Step> *phr, float thres)
{
    int generation = 0;
    vector<int> target = Trak::steps_to_vel(phr);
    
    
}

vector<int> Trak::gauss_variation(vector<Step> *phr, float thres)
{
    vector<int> res;
    
    vector<Step>::iterator step;
    for(step = phr->begin(); step != phr->end(); ++step)
    {
        int stddev = 15 * thres;
        int rvel = ofClamp(Trak::normal( step->vel, stddev),0,15);
        if(Trak::get_vel_group(rvel) != Trak::get_vel_group(step->vel))
        {    
            res.push_back(rvel);
        }
        else
        {
            res.push_back(step->vel);
        }
    }
    return res;
}

// get a variation according to the weighted jaccard method
vector<int> Trak::jaccard_variation(vector<Step> *phr, float thres)
{
    thres = ofClamp(thres, 0.97, 0.99);
    int inc = 4;
    vector<int> target;
    vector<int> goal;
    target = Trak::steps_to_vel(phr);
    for(int i = 0; i < target.size(); i += inc)
    {
        int to = (inc > target.size()-i)? target.size()-i : inc;
        to += i;
        vector<int>::const_iterator beg = target.begin() + i;
        vector<int>::const_iterator end = beg + (to-i);
        vector<int> part(beg,end);
        vector<int> res;
        float score = 1;
        int stedv = 15;
        vector<int> rnd;
        while (score > thres) {
            
            for(int j = i; j < to; ++j)
            {
                rnd.push_back(ofClamp(Trak::normal(target.at(j),stedv),0,15));
            }
            res = rnd;
            score = Trak::wjacc(rnd, part);
            rnd.clear();
            stedv -= 0.1;
        }
        goal.insert(goal.end(), res.begin(), res.end());
    }
    return goal;
}

vector< vector<int> > Trak::generate_pure_randoms(int size)
{
    int st = ofGetElapsedTimeMicros();
    vector< vector<int> > pool;
    int sample_size = 6400;
    float chance = 0.99;
    while (pool.size() < sample_size) 
    {
        vector<int> phr;
        for(int i = 0; i < size; ++i)
        {
            int vel = (int)ofClamp(Trak::normal(12, 12),0,15);
            if(ofRandom(1.) < chance)
            {
                vel = 0;
            }
            phr.push_back(vel);
        }
        pool.push_back(phr);
        //ofLog(OF_LOG_NOTICE, Trak::vel_to_str(phr) + " density: " + ofToString(Trak::get_density(phr)));
        if(pool.size() % 64 == 0)
        {
            chance -= 0.01;
        }
    }
    ofLog(OF_LOG_NOTICE, "time for generation : " + ofToString(ofGetElapsedTimeMicros() - st));
    return pool;
}

vector< vector<int> > Trak::get_vel_groups(vector<int> phr, bool nested = false)
{
    vector< vector<int> > res;
    for(int i = 0; i < 4; ++i)
    {
        vector<int> group;
        vector<int>::iterator vel;
        for(vel = phr.begin(); vel != phr.end(); ++vel)
        {
            int gr = Trak::get_vel_group(*vel);
            if(gr == i)
            {
                group.push_back(*vel);
            }
            else
            {
                if(nested && gr > i)
                {
                    group.push_back(*vel);
                }
                else
                {
                    group.push_back(0);
                }
                
            }
        }
        res.push_back(group);
    }
    return res;
}

// group of a vel
int Trak::get_vel_group(int vel)
{
    return vel / 4;
}

// gaussian random
float Trak::normal(float mean, float stdev)
{
    float rnd = (ofRandom(1) * 2 - 1) + (ofRandom(1) * 2 - 1) + (ofRandom(1) * 2 - 1);
    return rnd * stdev + mean;
}

float Trak::euclidian_distance(vector<int> s1, vector<int> s2)
{
    int l1, l2;
    l1 = s1.size();
    l2 = s2.size();
    int ct = 0;
    float tt = 0.;
    if (l1 == 0 || l1 != l2) // lengh > 0 and same length
    {
        return -1.;
    }
    if(s1 == s2)
    {
        return 0.;
    }
    vector<int>::iterator iter;
    for(iter = s1.begin(); iter != s1.end(); ++iter)
    {
        ct = iter - s1.begin();
        tt += powf((s1.at(ct) - s2.at(ct)), 2);
    }
    return sqrtf(tt) / sqrtf(powf(15, 2)*l1) ;
}

// weighted jaccard distance
float Trak::wjacc(vector<int> s1, vector<int> s2)
{
    int l1, l2;
    l1 = s1.size();
    l2 = s2.size();
    int ct = 0;
    float  a = 0, b = 0 , same = 0, diff = 0 ;
    if (l1 == 0 || l1 != l2) // lengh > 0 and same length
    {
        return -1.;
    }
    if(s1 == s2)
    {
        return 0.;
    }
    while (ct != l1) 
    {
        if (s1.at(ct) == s2.at(ct)) {
            same++;
        } else {
            a = pow((float)s1.at(ct),2);
            b = pow((float)s2.at(ct),2);
            diff += abs(a-b);
        }
        ct++;
    }
    return (1 - (same / (diff + same)));
}

// byte array to vector of int
vector<int> Trak::bytes_to_ints(vector<unsigned char> bytes)
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

vector<int> Trak::steps_to_vel(vector<Step> *phr)
{
    vector<int> res;
    vector<Step>::iterator step;
    for(step = phr->begin(); step != phr->end(); ++step)
    {
        res.push_back(step->vel);
    }    
    return res;
}

// vector of steps to byte array
vector<unsigned char> Trak::steps_to_bytes(vector<Step> *phr)
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


// heuristics tools
//  
//
//
float Trak::get_density(vector<int> phr)
{
    vector< vector<int> > groups = Trak::get_vel_groups(phr);
    vector< vector<int> >::iterator group;
    vector<float> densities;
    float tt_densities = 0.;
    for(group = groups.begin(); group != groups.end(); ++group)
    {
        // group density
        vector<int>::iterator vel;
        float onsets = 0;
        for(vel = group->begin(); vel != group->end(); ++vel)
        {
            if(*vel > 0)
            {
                onsets++;
            }
        }
        densities.push_back(onsets/group->size());
    }
    
    vector<float>::iterator density;
    for(density = densities.begin(); density != densities.end(); ++density)
    {
        int mult = (density - densities.begin() + 1);
        tt_densities += (*density) * mult;
    }
    return tt_densities/4;
} // >1 dense >0 not dense

float Trak::get_syncopation(vector<int> phr)
{
    // according to the LHL syncopation measure
    // phrase must be at least one measure so 16 steps, we duplicate:
    if(phr.size() < 16)
    {
        int rest = 16 - phr.size();
        for (int i = 0; i < rest; ++i)
        {
            phr.push_back(phr.at(rest%phr.size()));
        }
    }

    vector<int> weights = Trak::get_syncopation_weights(phr.size());
    float tt_syncopations = 0.;
    float tt_mult = 0.;
    // get the vel groups:
    vector< vector<int> > groups = Trak::get_vel_groups(phr,true);
    vector< vector<int> >::iterator group;
    vector<float> syncopations;
    for(group = groups.begin(); group != groups.end(); ++group)
    {
        float score = Trak::get_syncopation_score(*group,weights);
        float maximum = Trak::get_max_syncopation(phr.size());
        syncopations.push_back(score/maximum);
    }    
    
    vector<float>::iterator syncopation;
    for(syncopation = syncopations.begin(); syncopation != syncopations.end(); ++syncopation)
    {
        int mult = (syncopation - syncopations.begin() + 1);
        tt_mult += mult;
        tt_syncopations += (*syncopation) * mult;
    }
    return tt_syncopations/tt_mult;

} // >0 repetitive >1 syncopated

/*
 * Inspired from Ianis Lallemand
 */
float Trak::get_repartition(vector<int> phr)
{
    vector< vector<int> > groups = Trak::get_vel_groups(phr);
    vector< vector<int> >::iterator group;
    vector<float> repartitions;
    for(group = groups.begin(); group != groups.end(); ++group)
    {
        vector<int> active;
        float split = 0.;
        float numerator = 0.;
        vector<int>::iterator vel;
        for(vel = group->begin(); vel != group->end(); ++vel)
        {
            if(*vel)
            {
                active.push_back(vel - group->begin());
            }
        }
        
        split = active.size() / 2.;

        if(active.size())
        {
            if(floorf(split) != split)
            {
                numerator =  (active.at(floor(split)) + active.at(ceil(split)))/2;  
            }
            else
            {
                numerator = active.at((int)split);
            }
            repartitions.push_back( numerator / (float)group->size());
        }
        else
        {
            repartitions.push_back(0.5);
        }
    }
    float tt_repartitions = 0.;
    float tt_mult = 0.;
    
    vector<float>::iterator repartition;
    for(repartition = repartitions.begin(); repartition != repartitions.end(); ++repartition)
    {
        int mult = (repartition - repartitions.begin() + 1);
        tt_mult += mult;
        tt_repartitions += (*repartition) * mult;
    }
    return tt_repartitions/tt_mult;
    

}// =0.5 in the middle 0 and 1 are edge

float Trak::get_repetitiveness(vector<int> phr)
{
    //groups of inc, euclidian distances, averaging
    int inc = 4;
    if(floor(phr.size()/(float)inc) != phr.size()/(float)inc)
    {
        inc--;
    }
    
    vector<int> prev_part;
    vector<float> scores;
    for(int i = 0; i < phr.size(); i += inc)
    {
        int to = (inc > phr.size()-i)? phr.size()-i : inc;
        to += i;
        vector<int>::const_iterator beg = phr.begin() + i;
        vector<int>::const_iterator end = beg + (to-i);
        vector<int> part(beg,end);
        if(i)
        {
            scores.push_back(Trak::euclidian_distance(part, prev_part));
        }
        prev_part = part;
        
    }
    
    float avg_score = 0.;
    vector<float>::iterator score;
    for(score = scores.begin(); score != scores.end(); ++score)
    {
        avg_score += *score;
    }
    return avg_score / (float)scores.size();
    
}

int Trak::get_syncopation_score(vector<int> phr, vector<int> weights)
{
    vector<int>::iterator vel;
    float total = 0;
    for(vel = phr.begin(); vel != phr.end(); ++vel)
    {
        int ct = vel - phr.begin();
        if(!*vel)
        {
            int wr = weights[ct%weights.size()];
            int cct = 0;
            while(cct <= phr.size())
            {
                int idx = (ct-cct)%phr.size();
                if(idx < 0){ idx += phr.size(); }
                
                int val = phr.at(idx);
                int wn = weights.at(idx%weights.size());
                
                if(val)
                {
                    if(wn < wr)
                    {
                        total += wr-wn;
                        break;
                    }
                }
                cct++;
            }
        }
    }
    return total;
}


int Trak::get_max_syncopation(int size)
{
    vector<int> test_phr;
    for (int i = 0; i < size; i++)
    {
        if(i%2)
        {
            test_phr.push_back(1);
        }
        else
        {
            test_phr.push_back(0);
        }
    }
    return Trak::get_syncopation_score(test_phr,Trak::get_syncopation_weights(size));
    
}

vector<int> Trak::get_syncopation_weights(int size)
{
    vector<int> res;
    for (int i = 0; i < size; i++)
    {
        res.push_back(0);
    }
    
    int j = size;
    int div = 2;
    
    if((size/2)%div){
        div++;
    }
    
    int weight = 0;
    
    while(j >= 1)
    {
        weight--;
        for(int t = 0; t < size; t++)
        {
            if(t%j != 0)
            {
                res[t] = weight;
            }
        }
        j /= div;
    }
    return res;
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
