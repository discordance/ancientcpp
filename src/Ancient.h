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
        void set_level_variat(int level, int variat);
        void set_swing(float swg);
        void set_groove(vector<float> groove);
        void set_xor_variation(float ratio);
        void set_xor_mode(bool mode);
        void set_jaccard_variation(float thres);
        bool is_processing();
        int get_track_pitch(int track);
        void assign_pitchmap(vector<int> pitchmap);
        void assign_typemap(vector<int> typemap);
        void notify_bar(); // be informed that a bar is going on
        vector<Trak>* get_tracks();
        void ga(int track, int size, float den, float rpv, float syn, float rep);
        
        bool m_auto_variation;
    
    protected:
        bool m_processing;
        vector<Trak> m_tracks;
        Seq *m_seq;
        float m_swing;
        vector<float> m_groove;
        float m_xor_variation;
        bool  m_xor_mode;
        float m_jacc_variation;
        float m_level;
        float m_variat;
        
        // queue handling
        vector<string> m_tasks;
        // GA
        map<int, vector<float> > m_ga_tasks;
        
        // thread
        void threadedFunction();
    
};
#endif