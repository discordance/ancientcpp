#ifndef STEP
#define STEP

#include <vector>

using namespace std;

struct Step {
    int vel;
    int dur;
    bool lock;
    float drift;
    vector<float> ctrl; 
};

#endif