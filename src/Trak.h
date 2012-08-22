//
//  Track.h
//  ancientSequencer
//
//  Created by Nunja on 8/12/12.
#ifndef TRACK
#define TRACK

#include <vector>
#include "Step.h"
#include "utils/ofLog.h"
#include "utils/ofUtils.h"

using namespace std;

class Trak {
    public:
        Trak(int mode = 0, int size = 16);
        
        // accessors
        void set_current(vector<Step> phr);
        void set_current(vector<Step> phr, int mode);
        // current
        vector<Step>* get_current(); 
        vector<Step> get_current_copy();
    
        void set_size(int size);
        void set_swing(float swg);
        void set_xor_variation(float ratio, bool mode);
    
        int  get_size();
        // public dump
        void dump_current_vel();
        void dump_vanilla_vel();
        
        // utilities
        static vector<Step> str_to_phr(string str);
        static void dump_vel(vector<Step> *phr);
        static vector<unsigned char> get_steps2bytes(vector<Step> *phr);
        static vector<int> get_bytes2ints(vector<unsigned char> bytes);
        static void swing_phr(vector<Step> *phr, float swing);
        // jaccard
        static float get_wjaccard(string s1, string s2);
        // normal rand
        static float get_normal(float mean, float stdev);
    
        
        // analysis tools
        static float get_density_ratio(vector<Step> *phr); // >1 dense >0 not dense
        static float get_repetition_ratio(vector<Step> *phr); // >1 repetitive >0 chaotic
        static float get_repartition_ratio(vector<Step> *phr); // =0.5 in the middle -1 and 1 are edge
        
        // static const
        static const int MODE_LOW_PERC = 0;
        static const int MODE_PERC = 1;
        static const int MODE_SNARE = 2;
        static const int MODE_OVERHEAD = 3;
        static const int MODE_FREE_MIND = 4;
        static const int MODE_ONE_SHOT = 5;
        static const int MODE_BASS_NOTE = 6;
        static const int MODE_NOTE = 7;
    
        // 
        /*
         * 0 -> low perc (kicks), 
         * 1 -> perc, 
         * 2 -> snare, 
         * 3 -> overhead(hh), 
         * 4 -> free-mind,
         * 5 -> one shot, 
         * 6 -> bass-note, 
         * 7 -> note
         */
        int m_mode; 
        int m_pitch; // pitch of the current track
        
    protected:
        int m_size; // max steps in the Trak
        vector<Step> m_current; // current form of the track after lot of transforms
        vector<Step> m_vanilla; // original form of the track that is written
        vector< vector < vector<Step> > > m_matrix; // 5x5 matrix y-> complexity, x -> variation
        
        // utilities
        void update_size();
        float get_variation_rate();
};

#endif