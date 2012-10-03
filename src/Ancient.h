//
//  Ancient.h
//  ancientSequencer
//
//  Created by Nunja on 8/12/12.
//

#ifndef ANCIENT
#define ANCIENT

#include <vector>
#include "ofTypes.h"
#include "Gaia.h"
#include "Trak.h"
class Seq; 
#include "Seq.h"

class Ancient : public ofThread
{
    
    
    public:
        Ancient();
        void set_seq(Seq *seq);
        void set_swing(float swg);
        void set_xor_variation(float ratio);
        void set_xor_mode(bool mode);
        void set_jaccard_variation(float thres);
        void assign_pitchmap(vector<int> pitchmap);
        void notify_bar(); // be informed that a bar is going on
        vector<Trak>* get_tracks();
        void ga_test();
    
        bool m_auto_variation;
    
    protected:
        vector<Trak> m_tracks;
        Seq *m_seq;
        float m_swing;
        float m_xor_variation;
        bool  m_xor_mode;
        float m_jacc_variation;
        
        // queue handling
        vector<string> m_tasks;
        
        // thread
        void threadedFunction();
    
};
#endif