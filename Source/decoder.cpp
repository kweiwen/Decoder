#include "decoder.h"

//void process(decoder& self, float L, float R)
//{
//    auto VCA_L = L * sin(self.factor * M_PI / 2);
//    auto VCA_R = R * sin((1 - self.factor) * M_PI / 2);
//
//    self.C = VCA_L + VCA_R;
//    self.S = VCA_L - VCA_R;
//
//    double FWR_L = fabs(VCA_L);
//    double FWR_R = fabs(VCA_R);
//
//    double E = FWR_L + FWR_R;
//
//    bool flag = (FWR_L * FWR_R > 0) && !isnan(FWR_R / E);
//
//    self.factor = self.smoother.processSingleSampleRaw(fmin(flag, (FWR_R / E)) + (1 - flag) * self.factor);
//}
//
//void update_coeffs(decoder& self, float fc, float sr)
//{
//    double q = sqrt(2) / 2;
//    float gain = 1.0;
//    double omega = 2 * M_PI * fc / sr;
//    double sineOmega = sin(omega);
//    double cosineOmega = cos(omega);
//
//    double alpha = sineOmega / (2 * q);
//    auto a0_temp = 1 + alpha;
//    auto a1 = (-2 * cosineOmega) / a0_temp;
//    auto a2 = (1 - alpha) / a0_temp;
//
//    auto b0 = (1 - cosineOmega) * gain / 2 / a0_temp;
//    auto b1 = (1 - cosineOmega) * gain / a0_temp;
//    auto b2 = (1 - cosineOmega) * gain / 2 / a0_temp;
//
//    auto a0 = 1.0;
//
//    juce::IIRCoefficients new_coeffs(b0, b1, b2, a0, a1, a2);
//    self.smoother.setCoefficients(new_coeffs);
//}

surround::surround()
{
    unit_main.factor = 0.5f;
    unit_L.factor = 0.5f;
    unit_R.factor = 0.5f;
}

surround::~surround()
{

}

void surround::process(float L, float R)
{
    process_(&unit_main, L, R);
    process_(&unit_L, L, C);
    process_(&unit_R, R, C);

    C = unit_main.C;
    S = unit_main.S;

    LC = unit_L.C;
    RC = unit_R.C;

    LS = unit_L.S;
    RS = unit_R.S;
}

void surround::update_coeffs(float fc, float sr)
{
    update_coeffs_(&unit_main, fc, sr);
    update_coeffs_(&unit_L, fc, sr);
    update_coeffs_(&unit_R, fc, sr);
}

void surround::process_(decoder* self, float L, float R)
{
    auto VCA_L = L * sin(self->factor * M_PI / 2);
    auto VCA_R = R * sin((1 - self->factor) * M_PI / 2);

    self->C = VCA_L + VCA_R;
    self->S = VCA_L - VCA_R;

    double FWR_L = fabs(VCA_L);
    double FWR_R = fabs(VCA_R);

    double E = FWR_L + FWR_R;

    bool flag = (FWR_R > 0) && (FWR_L > 0) && !isnan(1 / E);

    self->factor = self->smoother.processSingleSampleRaw(fmin(flag, (FWR_R / E)) + (1 - flag) * self->factor);
}

void surround::update_coeffs_(decoder* self, float fc, float sr)
{
    double q = sqrt(2) / 2;
    double omega = 2 * M_PI * fc / sr;
    double sine_omega = sin(omega);
    double cosine_omega = cos(omega);

    double alpha = sine_omega / (2 * q);

    auto a0 = (1 + alpha);
    auto a1 = (-2 * cosine_omega);
    auto a2 = (1 - alpha);

    auto b0 = (1 - cosine_omega) / 2;
    auto b1 = (1 - cosine_omega);
    auto b2 = (1 - cosine_omega) / 2;

    juce::IIRCoefficients new_coeffs(b0, b1, b2, a0, a1, a2);
    self->smoother.setCoefficients(new_coeffs);
}