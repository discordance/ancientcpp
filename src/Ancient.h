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
#include "Trak.h"
#include "Seq.h"

class Ancient 
{
    
    
    public:
        Ancient();
        void set_seq(Seq *seq);
        void set_swing(float swg);
        void set_xor_variation(float var);
        void set_xor_mode(bool mode);
        void assign_pitchmap(vector<int> pitchmap);
    
    protected:
        vector<Trak> m_tracks;
        Seq *m_seq;
        float m_swing;
        float m_xor_variation;
        bool  m_xor_mode;
    
};
#endif