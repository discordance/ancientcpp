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
        void set_vanilla(vector<Step> phr);
        void set_vanilla(vector<Step> phr, int mode);
        bool has_events();
        
        // current accessors
        vector<Step>* get_current(); 
        vector<int>   get_current_vel();
        vector<Step>  get_current_copy();
    
        void set_size(int size);
        void set_swing(float swg);
        void set_xor_variation(float ratio, bool mode);
        void set_jaccard_variation(float thres);
        void set_euclidian_variation(float thres);
    
        int  get_size();
        // public dump
        void dump_current_vel();
        void dump_vanilla_vel();
        
        
        /**** utilities ****/
        // dump
        static void dump_vel(vector<Step> *phr);
        // converters
        static vector<Step> str_to_phr(string str);
        static vector<int> str_to_vel(string str);
        static string vel_to_str(vector<int> ins);
        static string phr_to_str(vector<Step> *phr);
        static vector<unsigned char> steps_to_bytes(vector<Step> *phr);
        static vector<int> steps_to_vel(vector<Step> *phr);
        static vector<int> bytes_to_ints(vector<unsigned char> bytes);
    
        // swing
        static void swing_phr(vector<Step> *phr, float swing);
        
        // variators
        static vector<int> jaccard_variation(vector<Step> *phr, float thres);
        static vector<int> gauss_variation(vector<Step> *phr, float thres);
        static vector<int> euclidian_variation(vector<Step> *phr, float thres);
        
        // generators;
        static vector< vector<int> > generate_pure_randoms(int size);
        
        // weigthed jaccard
        static float wjacc(vector<int> s1, vector<int> s2);
    
        // euclidian
        static float euclidian_distance(vector<int> s1, vector<int> s2);
    
        // normal gaussian rand
        static float normal(float mean, float stdev);
    
        // groups of vel
        static int get_vel_group(int vel);
        static vector< vector<int> > get_vel_groups(vector<int> phr, bool nested);
    
        // heuristics tools
        static float get_density(vector<int> phr); // >1 dense >0 not dense
        static float get_syncopation(vector<int> phr); // >0 repetitive >1 syncopated
        static float get_repartition(vector<int> phr); // =0.5 in the middle -1 and 1 are edge
        static float get_repetitiveness(vector<int> phr); // = 1 repetitive, 0 chaotic
        
        // utils 2
        static vector<int> get_syncopation_weights(int size);
        static int get_syncopation_score(vector<int> phr, vector<int> weights);
        static int get_max_syncopation(int size);
        
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