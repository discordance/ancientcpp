//
//  Gaia.cpp
//  ancientSequencer
//
//  Created by nunja on 9/30/12.
//
//

#include "Gaia.h"

//Gaia::Gaia(){} // no need only static here

// init weights
map<int, vector<int> >   Gaia::syncopation_weights = Gaia::create_weights_map();
map<int, int>            Gaia::syncopation_maxes = Gaia::create_maxes_map();
map<int, vector<float> > Gaia::type_stats = Gaia::create_type_stats();
//static utilities
//--------------------------------------------------------------
string Gaia::phr_to_str(vector<Step> *phr)
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
vector<Step> Gaia::vel_to_phr(vector<int> ins)
{
    vector<Step> res;
    vector<int>::iterator vel;
    for(vel = ins.begin(); vel != ins.end(); ++vel)
    {
        // create the step
        Step st;
        st.vel = *vel;
        st.dur = 1;
        st.lock = FALSE;
        st.drift = 0;
        st.ctrl = vector<float>(4,0.);
        
        res.push_back(st);
    }
    return res;
}

string Gaia::vel_to_str(vector<int> ins)
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

vector<int> Gaia::str_to_vel(string str)
{
    vector<int> res;
    string::iterator str_iter;
    for(str_iter = str.begin(); str_iter != str.end(); ++str_iter)
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

vector<Step> Gaia::str_to_phr(string str)
{
    vector<Step> res;
    string::iterator str_iter;
    for(str_iter = str.begin(); str_iter != str.end(); ++str_iter)
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

void Gaia::beat_groove_phr(vector<Step> *phr, vector<float> drifts)
{
    if(phr->size()) // check
    {
        // iterate
        vector<Step>::iterator step;
        for(step = phr->begin(); step != phr->end(); ++step)
        {
            int ct = step - phr->begin();
            if(ct % 2 != 0)
            {
                step->drift = drifts.at(ct%4);
            }
        }
    }
}

// swing this phrase
void Gaia::swing_phr(vector<Step> *phr, float swing)
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

vector<int> Gaia::euclidian_variation(vector<Step> *phr, float thres)
{
    int generation = 0;
    vector<int> target = Gaia::steps_to_vel(phr);
    
    
}

vector<int> Gaia::gauss_variation(vector<Step> *phr, float thres)
{
    vector<int> res;
    
    vector<Step>::iterator step;
    for(step = phr->begin(); step != phr->end(); ++step)
    {
        int stddev = 15 * thres;
        int rvel = ofClamp(Gaia::normal( step->vel, stddev),0,15);
        if(Gaia::get_vel_group(rvel) != Gaia::get_vel_group(step->vel))
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
vector<int> Gaia::jaccard_variation(vector<Step> *phr, float thres)
{
    thres = ofClamp(thres, 0.97, 0.99);
    int inc = 4;
    vector<int> target;
    vector<int> goal;
    target = Gaia::steps_to_vel(phr);
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
                rnd.push_back(ofClamp(Gaia::normal(target.at(j),stedv),0,15));
            }
            res = rnd;
            score = Gaia::wjacc(rnd, part);
            rnd.clear();
            stedv -= 0.1;
        }
        goal.insert(goal.end(), res.begin(), res.end());
    }
    return goal;
}

vector<int> Gaia::generate_stochastic_phr(int size, float den)
{
    vector<int> phr;
    for(int i = 0; i < size; ++i)
    {
        int vel = (int)ofClamp(Gaia::normal(12, 12),0,15);
        if(ofRandom(1.) > den)
        {
            vel = 0;
        }
        phr.push_back(vel);
    }
    return phr;
}

vector<int> Gaia::generate_cyclic_phr(int size, int bdiv, int cycle, int offset)
{
    vector<int> res;
    for (int i = 0; i < size; ++i) 
    {   
        float prob, score = 0.;
        int vel = 0;
        int dist = abs(((i+offset) % cycle) - bdiv); // get the dist from modulo
        if(dist)
        {
            prob = 1/(dist+2);
        } 
        else 
        {
            prob=0.98; // almost impossible to miss here, but life is a bitch sometimes
        }
        score=ofRandom(1.);
        if(score < prob)
        {
            vel = floor(Gaia::normal(14*prob, 1)); // using normal function, in relation to the prob
        }
        else
        {
            vel = 0;
        }
        res.push_back(vel);
    }
    return res;
}

vector< vector<int> > Gaia::ga(int size, float den, float rpv, float syn, float rep)
{
    //ofLog(OF_LOG_NOTICE, ofToString(den) + " " + ofToString(rpv) + " " + ofToString(syn) + " " + ofToString(rep) + " <<<<");
    int total_gen = 8;
    int gen_num = total_gen;
    vector< vector<int> > pool = Gaia::generate_stochastic(size, 512, den); // get some randoms
    vector< vector<int> > cyclic_pool = Gaia::generate_cyclic(size, 512); // get cyclic randoms
    pool.insert(pool.end(), cyclic_pool.begin(), cyclic_pool.end()); // merge them
    std::map<float, vector<int> > generation;
    int pop_size = pool.size();
    
    // first generation initiated.
    while(gen_num > 0)
    {
        generation.clear();
        vector< vector<int> >::iterator phr;
        for(phr = pool.begin(); phr != pool.end(); ++phr)
        {
            float fit = Gaia::fitness_score(*phr,den,rpv,syn,rep);
            generation[fit] = *phr;
        }
        
        pool.clear(); // clear pool and re-use;
        std::map<float, vector<int> >::iterator gen;
        int group = 0;
        vector<int> first_phr = generation.begin()->second;

        for(gen = generation.begin(); gen != generation.end(); ++gen)
        {
            int ct = std::distance(generation.begin(), gen) + 1;
            vector<int> rnd;
            switch (group)
            {
                case 0:
                    pool.push_back(gen->second); // keep
                    break;
                case 1:
                    pool.push_back(Gaia::mutate_phr(gen->second, first_phr));
                    break;
                  
                case 2:
                    rnd = Gaia::generate_stochastic_phr(size, den);
                    pool.push_back(Gaia::mutate_phr(gen->second, rnd));
                    break;    
                   
                default:
                    pool.push_back(Gaia::generate_stochastic_phr(size, den));
                    break;
            }
            int modul = (int)floor(generation.size()/4.);
            if(ct % modul == 0 && modul)
            {
                ++group;
            }
        }
        
        // re-do genetics
        
        gen_num--;
    }
    
    // re-ordering
    std::map<float, vector<int> >::iterator gen;
    vector<int> winner = generation.begin()->second;
    map<float, vector<int> > euclidian_ordered;
    for(gen = generation.begin(); gen != generation.end(); ++gen)
    {
        if(gen != generation.begin())
        {
            euclidian_ordered[Gaia::euclidian_distance(winner, gen->second)] = gen->second;
        }
    }
    vector< vector<int> > res;
    res.push_back(winner);
    //ofLog(OF_LOG_NOTICE, Gaia::vel_to_str(winner));
    float thres = 0.05;
    for(gen = euclidian_ordered.begin(); gen != euclidian_ordered.end(); ++gen)
    {
        if(gen->first > thres)
        {
            //ofLog(OF_LOG_NOTICE, Gaia::vel_to_str(gen->second));
            res.push_back(gen->second);
            thres += 0.05;
            if(thres >= 0.25)
            {
                break;
            }
        }
    }
    //ofLog(OF_LOG_NOTICE, ofToString(res.size()));
    return res;
}

vector <int>  Gaia::mutate_phr(vector <int>& va, vector <int>& vb)
{
    vector <int> res;
    if(va.size() != vb.size())
    {
        return res;
    }
    int xover = 1+floor(ofRandom(va.size()-1));
    float direction = ofRandom(1.);
    if(direction < 0.5)
    {
        res.insert(res.end(), va.begin(), va.begin() + xover);
        res.insert(res.end(), vb.begin()+xover, vb.end());
    }
    else
    {
        res.insert(res.end(), vb.begin(), vb.begin() + xover);
        res.insert(res.end(), va.begin()+xover, va.end());
    }
    return res;
}

float Gaia::fitness_score(vector<int>& phr, float den, float rpv, float syn, float rep)
{
    float den_dist, rpv_dist, syn_dist, rep_dist = 0.;
    den_dist = abs(Gaia::get_density(phr)-den);
    rpv_dist = abs(Gaia::get_repetitiveness(phr)-rpv);
    syn_dist = abs(Gaia::get_syncopation(phr)-syn);
    rep_dist = abs(Gaia::get_repartition(phr)-rep);
    return (den_dist+rpv_dist+syn_dist+rep_dist)/4;
}

vector< vector<int> > Gaia::generate_cyclic(int size, int sample_size)
{
    int bdiv = 0;
    int cycle = 2;
    int offset = 0;
    vector< vector<int> > pool;
    while (pool.size() < sample_size)
    {
        vector<int> phr = Gaia::generate_cyclic_phr(size, bdiv, cycle, offset);
        pool.push_back(phr);
        
        if(pool.size() % 2 == 0)
        {
            cycle = (cycle+1)%size;
            if(cycle < 1)
            {
                cycle = 2;
            }
            bdiv = ((int)ofRandom(0., size)) % cycle;
            offset = ((int)ofRandom(0., size)) % cycle;
        }
        
    }
    return pool;
}

vector< vector<int> > Gaia::generate_stochastic(int size, int sample_size, float den)
{
    vector< vector<int> > pool;
    float chance = den;
    while (pool.size() < sample_size) 
    {
        vector<int> phr = Gaia::generate_stochastic_phr(size, den);
        if(accumulate(phr.begin(),phr.end(),0))
        {
            pool.push_back(phr);
        }
        
        if(pool.size() % 2 == 0)
        {
            chance = ofClamp(Gaia::normal(den, 0.4),0.05, 0.95);
        }
    }
    
    return pool;
}

vector< vector<int> > Gaia::get_vel_groups(vector<int>& phr, bool nested = false)
{
    vector< vector<int> > res;
    for(int i = 0; i < 4; ++i)
    {
        vector<int> group;
        vector<int>::iterator vel;
        for(vel = phr.begin(); vel != phr.end(); ++vel)
        {
            int gr = Gaia::get_vel_group(*vel);
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
int Gaia::get_vel_group(int vel)
{
    return vel / 4;
}

// gaussian random
float Gaia::normal(float mean, float stdev)
{
    float rnd = (ofRandom(1) * 2 - 1) + (ofRandom(1) * 2 - 1) + (ofRandom(1) * 2 - 1);
    return rnd * stdev + mean;
}

float Gaia::euclidian_distance(vector<int>& s1, vector<int>& s2)
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
float Gaia::wjacc(vector<int>& s1, vector<int>& s2)
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
            ++same;
        } else {
            a = pow((float)s1.at(ct),2);
            b = pow((float)s2.at(ct),2);
            diff += abs(a-b);
        }
        ++ct;
    }
    return (1 - (same / (diff + same)));
}

// byte array to vector of int
vector<int> Gaia::bytes_to_ints(vector<unsigned char> bytes)
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

vector<int> Gaia::steps_to_vel(vector<Step> *phr)
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
vector<unsigned char> Gaia::steps_to_bytes(vector<Step> *phr)
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

void Gaia::dump_vel(vector<Step> *phr)
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
float Gaia::get_density(vector<int>& phr)
{
    vector< vector<int> > groups = Gaia::get_vel_groups(phr);
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
                ++onsets;
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

float Gaia::get_syncopation(vector<int>& phr)
{
   
    vector<int> phr_a = phr;// copy because
    // according to the LHL syncopation measure
    // phrase must be at least one measure so 16 steps, we duplicate:
    if(phr_a.size() < 16)
    {
        int rest = 16 - phr_a.size();
        for (int i = 0; i < rest; ++i)
        {
            phr_a.push_back(phr_a.at(rest%phr_a.size()));
        }
    }

    vector<int> weights = Gaia::syncopation_weights[phr_a.size()];
    float tt_syncopations = 0.;
    float tt_mult = 0.;

    vector< vector<int> > groups = Gaia::get_vel_groups(phr_a,true);
    vector< vector<int> >::iterator group;
    vector<float> syncopations;

    for(group = groups.begin(); group != groups.end(); ++group)
    {
          
       
        float score = Gaia::get_syncopation_score(*group,weights);
        float maximum = Gaia::syncopation_maxes[phr_a.size()];
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
float Gaia::get_repartition(vector<int>& phr)
{
    vector< vector<int> > groups = Gaia::get_vel_groups(phr);
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
            if(floorf(split) != split && active.size() > 1)
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

float Gaia::get_repetitiveness(vector<int>& phr)
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
            scores.push_back(Gaia::euclidian_distance(part, prev_part));
        }
        //else
        //{
           prev_part = part; 
        //}
    }
    
    float avg_score = 0.;
    vector<float>::iterator score;
    for(score = scores.begin(); score != scores.end(); ++score)
    {
        avg_score += *score;
    }
    return 1-(avg_score / (float)scores.size());
    
}

int Gaia::get_syncopation_score(vector<int>& phr, vector<int>& weights)
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
                ++cct;
            }
        }
    }
    return (int)total;
}


int Gaia::get_max_syncopation(int size)
{
    vector<int> test_phr;
    for (int i = 0; i < size; ++i)
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
    vector<int> weights = Gaia::syncopation_weights[size];//Gaia::get_syncopation_weights(size);
    return Gaia::get_syncopation_score(test_phr, weights);
}

map<int, vector<int> > Gaia::create_weights_map()
{
    map<int, vector<int> > res;
    for(int i = 4; i < 129; i += 2)
    {
        res[i] =  Gaia::get_syncopation_weights(i);
    }
    return res;
}

map <int,int> Gaia::create_maxes_map()
{
    map<int, int> res;
    for(int i = 4; i < 129; i += 2)
    {
        res[i] =  Gaia::get_max_syncopation(i);
    }
    return res;
}

map<int, vector<float> > Gaia::create_type_stats()
{
    map<int, vector<float> > res;
    for (int i = 0; i < 5; ++i)
    {
        vector<float> stats;
        float vrate = 0.;
        float den = 0.;
        switch (i) {
            case MODE_LOW_PERC:
                vrate = 0.15;
                den = 0.25;
                break;
            case MODE_PERC:
                vrate = 0.45;
                den = 0.15;
                break;
            case MODE_SNARE:
                vrate = 0.35;
                den = 0.125;
                break;
            case MODE_HITHAT:
                vrate = 0.65;
                den = 0.25;
                break;
            case MODE_OVERHEAD:
                vrate = 0.65;
                den = 0.5;
                break;
            case MODE_ONE_SHOT:
                vrate = 0.8;
                den = 0.07;
                break;
            default:
                vrate = 0.5;
                den = 0.5;
                break;
        }
        stats.push_back(vrate);
        stats.push_back(den);
        res[i] = stats;
    }
    
    return res;
}

vector<int> Gaia::get_syncopation_weights(int size)
{
    vector<int> res;
    for (int i = 0; i < size; ++i)
    {
        res.push_back(0);
    }
    
    int j = size;
    int div = 2;
    
    if((size/2)%div){
        ++div;
    }
    
    int weight = 0;
    
    while(j >= 1)
    {
        weight--;
        for(int t = 0; t < size; ++t)
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
