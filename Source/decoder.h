#pragma once

#include <JuceHeader.h>
#include <math.h>
#define M_PI       3.14159265358979323846

struct decoder
{
    float factor;
    float C;
    float S;
    juce::IIRFilter smoother;
};

//void process(decoder& self, float L, float R);
//void update_coeffs(decoder& self, float fc, float sr);

class surround
{
public:
    surround();
    ~surround();

    float LC;
    float LS;
    float RC;
    float RS;
    float C;
    float S;

    decoder unit_main;
    decoder unit_L;
    decoder unit_R;

    void process(float  L, float R);
    void update_coeffs(float fc, float sr);

private:
    void process_(decoder* self, float  L, float R);
    void update_coeffs_(decoder* self, float fc, float sr);
};