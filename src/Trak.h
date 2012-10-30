//
//  Track.h
//  ancientSequencer
//
//  Created by Nunja on 8/12/12.
#ifndef TRAK
#define TRAK

#include <vector>
#include "Step.h"
#include "Gaia.h"
//#include "utils/ofLog.h"
//#include "utils/ofUtils.h"

using namespace std;

class Trak {
    public:
        Trak(int mode = 0, int size = 16);
        
        // accessors
        void set_vanilla(vector<Step> phr);
        void set_vanilla(vector<Step> phr, int mode);
        void set_matrix(vector< vector < vector<Step> > > matrix);
        void set_level_variat(int level, int variat);
        bool has_events();
        
        // current accessors
        vector<Step>* get_current(); 
        vector<int>   get_current_vel();
        vector<Step>  get_current_copy();
    
        void set_size(int size);
        void set_swing(float swg);
        void set_beat_groove(vector<float> drifts);
        void set_xor_variation(float ratio, bool mode);
        void set_jaccard_variation(float thres);
        void set_euclidian_variation(float thres);
        void set_pitch(int pitch);
        int  get_size();
        int  get_pitch();
        // public dump
        void dump_current_vel();
        void dump_vanilla_vel();
  
        // 
        /*
         * 0 -> low perc (kicks), 
         * 1 -> perc, 
         * 2 -> snare, 
         * 3 -> hh, 
         * 4 -> ovh,
         * 5 -> one shot, 
         */
        int m_mode; 
        
    protected:
        int m_pitch; // pitch of the current track
        int m_size; // max steps in the Trak
        int m_level;
        int m_variat;
        vector<float> m_groove;
        vector<Step> m_current; // current form of the track after lot of transforms
        vector<Step>* m_vanilla; // original form of the track that is written, links to the matrix[2][0];
        vector< vector < vector<Step> > > m_matrix; // 5x5 matrix y-> complexity, x -> variation
        vector<Step>& get_matrix_target();
    
        // utilities
        void update_current();
        void update_size();
        void update_groove();
        //float get_variation_rate();
    
};

#endif