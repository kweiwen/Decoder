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

void process(decoder& self, float L, float R);
void update_coeffs(decoder& self, float fc, float sr);

