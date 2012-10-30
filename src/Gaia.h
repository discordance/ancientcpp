//
//  Gaia.h
//  ancientSequencer
//
//  Created by nunja on 9/30/12.
//
//

#ifndef GAIA
#define GAIA

#include <iostream>
#include <vector>
#include <map>
#include <numeric> 
#include "Step.h"
#include "utils/ofLog.h"
#include "utils/ofUtils.h"

using namespace std;

class Gaia {
    
    public:
        Gaia();
        ~Gaia();
        
        // modes
        static const int MODE_LOW_PERC = 0;
        static const int MODE_PERC = 1;
        static const int MODE_SNARE = 2;
        static const int MODE_HITHAT = 3;
        static const int MODE_OVERHEAD = 4;
        static const int MODE_ONE_SHOT = 5;
        
        // GA
        static const int GA_POPULATION = 1024;
        
        // lookup tables
        static std::map<int, vector<int> > syncopation_weights;
        static std::map<int, int> syncopation_maxes;
        static std::map<int, vector<float> > type_stats;
        
        /**** utilities ****/
    
        // compressor
        static void upper_compressor(vector<int> *vels);
        
        // dump
        static void dump_vel(vector<Step> *phr);
        // converters
        static vector<Step> str_to_phr(string str);
        static vector<int> str_to_vel(string str);
        static vector<Step> vel_to_phr(vector<int> ins);
        static string vel_to_str(vector<int> ins);
        static string phr_to_str(vector<Step> *phr);
        static vector<unsigned char> steps_to_bytes(vector<Step> *phr);
        static vector<int> steps_to_vel(vector<Step> *phr);
        static vector<int> bytes_to_ints(vector<unsigned char> bytes);
        
        // swing
        static void swing_phr(vector<Step> *phr, float swing);
        static void beat_groove_phr(vector<Step> *phr, vector<float> drifts);
        
        // variators
        static vector<int> jaccard_variation(vector<Step> *phr, float thres);
        static vector<int> gauss_variation(vector<Step> *phr, float thres);
        static vector<int> euclidian_variation(vector<Step> *phr, float thres);
        
        // generators;
        inline static vector<int> generate_cyclic_phr(int size, int bdiv, int cycle, int offset);
        inline static vector<int> generate_stochastic_phr(int size, float den);
        inline static vector< vector<int> > generate_stochastic(int size, int sample_size, float den);
        inline static vector< vector<int> > generate_cyclic(int size, int sample_size);
        static vector< vector<int> > ga(int size, float den, float rpv, float syn, float rep);
        
        // weigthed jaccard
        static float wjacc(vector<int>& s1, vector<int>& s2);
        
        // euclidian
        static float euclidian_distance(vector<int>& s1, vector<int>& s2);
        
        // normal gaussian rand
        static float normal(float mean, float stdev);
        
        // groups of vel
        static int get_vel_group(int vel);
        static vector< vector<int> > get_vel_groups(vector<int>& phr, bool nested);
        
        // heuristics tools
        inline static float get_density(vector<int>& phr); // >1 dense >0 not dense
        inline static float get_syncopation(vector<int>& phr); // >0 repetitive >1 syncopated
        inline static float get_repartition(vector<int>& phr); // =0.5 in the middle -1 and 1 are edge
        inline static float get_repetitiveness(vector<int>& phr); // = 1 repetitive, 0 chaotic
        
        // utils syncopation
        static vector<int> get_syncopation_weights(int size);
        inline static int get_syncopation_score(vector<int>& phr, vector<int>& weights);
        static int get_max_syncopation(int size);
    
        static map<int, vector<int> > create_weights_map();
        static map<int, int> create_maxes_map();
        static map<int, vector<float> > create_type_stats();
    
        // utils GA
        inline static float fitness_score(vector<int>& phr, float den, float rpv, float syn, float rep);
        inline static vector <int> mutate_phr(vector <int>& va, vector <int>& vb);
    
};


#endif 
