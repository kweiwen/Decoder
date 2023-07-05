/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
DecoderAudioProcessor::DecoderAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
    addParameter(center0 = new juce::AudioParameterFloat("center0", "center0", 0.00f, 1.00f, 0.50f));
    addParameter(center1 = new juce::AudioParameterFloat("center1", "center1", 0.00f, 1.00f, 0.50f));
    addParameter(center2 = new juce::AudioParameterFloat("center2", "center2", 0.00f, 1.00f, 0.50f));
    addParameter(center3 = new juce::AudioParameterFloat("center3", "center3", 0.00f, 1.00f, 0.50f));
    addParameter(center4 = new juce::AudioParameterFloat("center4", "center4", 0.00f, 1.00f, 0.50f));
    addParameter(center5 = new juce::AudioParameterFloat("center5", "center5", 0.00f, 1.00f, 0.50f));
    addParameter(surround0 = new juce::AudioParameterFloat("surround0", "surround0", 0.00f, 1.00f, 0.50f));
    addParameter(surround1 = new juce::AudioParameterFloat("surround1", "surround1", 0.00f, 1.00f, 0.50f));
    addParameter(surround2 = new juce::AudioParameterFloat("surround2", "surround2", 0.00f, 1.00f, 0.50f));
    addParameter(surround3 = new juce::AudioParameterFloat("surround3", "surround3", 0.00f, 1.00f, 0.50f));
    addParameter(surround4 = new juce::AudioParameterFloat("surround4", "surround4", 0.00f, 1.00f, 0.50f));
    addParameter(surround5 = new juce::AudioParameterFloat("surround5", "surround5", 0.00f, 1.00f, 0.50f));
    addParameter(fc0 = new juce::AudioParameterFloat("fc0", "fc0", 0.01f, 60.0f, 5.00f));
    addParameter(fc1 = new juce::AudioParameterFloat("fc1", "fc1", 0.01f, 60.0f, 5.00f));
    addParameter(fc2 = new juce::AudioParameterFloat("fc2", "fc2", 0.01f, 60.0f, 5.00f));
    addParameter(fc3 = new juce::AudioParameterFloat("fc3", "fc3", 0.01f, 60.0f, 5.00f));
    addParameter(fc4 = new juce::AudioParameterFloat("fc4", "fc4", 0.01f, 60.0f, 5.00f));
    addParameter(fc5 = new juce::AudioParameterFloat("fc5", "fc5", 0.01f, 60.0f, 5.00f));
    addParameter(bl0 = new juce::AudioParameterFloat("bl0", "bl0", 0.01f, 2.0f, 1.00f));
    addParameter(bl1 = new juce::AudioParameterFloat("bl1", "bl1", 0.01f, 2.0f, 1.00f));
    addParameter(bl2 = new juce::AudioParameterFloat("bl2", "bl2", 0.01f, 2.0f, 1.00f));
    addParameter(bl3 = new juce::AudioParameterFloat("bl3", "bl3", 0.01f, 2.0f, 1.00f));
    addParameter(bl4 = new juce::AudioParameterFloat("bl4", "bl4", 0.01f, 2.0f, 1.00f));
    addParameter(bl5 = new juce::AudioParameterFloat("bl5", "bl5",  0.01f, 2.0f,  1.00f));

    auto b0 = 3.844633506779265e-06;
    auto b1 = 7.68926701355853e-06;
    auto b2 = 3.844633506779265e-06;
    auto a0 = 1.0;
    auto a1 = -1.9944464105419266;
    auto a2 = 0.9944617890759537;

    //juce::IIRCoefficients coeffs(b0, b1, b2, a0, a1, a2);
    //inst0.smoother.setCoefficients(coeffs);
    //inst0.C = 0.0f;
    //inst0.S = 0.0f;
    inst0.factor = 0.5f;
    inst1.factor = 0.5f;
    inst2.factor = 0.5f;
    inst3.factor = 0.5f;
    inst4.factor = 0.5f;
    inst5.factor = 0.5f;

    double vec1[] = { -3.49091142e-08, 2.46292042e-09, 3.88926097e-07, 5.38842464e-07, -4.32830436e-07, -1.24023767e-06, -4.32830436e-07, 5.38842464e-07, 3.88926097e-07, 2.46292042e-09, -3.49091142e-08 };
    double vec2[] = { 1.00000000e+00, -6.85241024e+00, 2.08375308e+01, -3.69741767e+01, 4.23197284e+01, -3.25792598e+01, 1.70414550e+01, -5.96309296e+00, 1.33137023e+00, -1.70651331e-01, 9.50672270e-03 };
    double vec3[] = { 3.25740238e-05, -1.32594269e-04, -9.31257227e-05, 5.39569774e-04, 6.05516989e-05, -8.13951010e-04, 6.05516989e-05, 5.39569774e-04, -9.31257227e-05, -1.32594269e-04, 3.25740238e-05 };
    double vec4[] = { 1.00000000e+00, -6.85241024e+00, 2.08375308e+01, -3.69741767e+01, 4.23197284e+01, -3.25792598e+01, 1.70414550e+01, -5.96309296e+00, 1.33137023e+00, -1.70651331e-01, 9.50672270e-03 };
    double vec5[] = { 0.0001336, -0.00053496, -0.00039965, 0.00214215, 0.00026605, -0.00321437, 0.00026605, 0.00214215, -0.00039965, -0.00053496, 0.0001336 };
    double vec6[] = { 1.00000000e+00, -6.85241024e+00, 2.08375308e+01, -3.69741767e+01, 4.23197284e+01, -3.25792598e+01, 1.70414550e+01, -5.96309296e+00, 1.33137023e+00, -1.70651331e-01, 9.50672270e-03 };
    double vec7[] = { -0.00770798, 0.06169688, -0.16206595, 0.12359223, 0.16977392, -0.37057822, 0.16977392, 0.12359223, -0.16206595, 0.06169688, -0.00770798 };
    double vec8[] = { 1.00000000e+00, -6.85241024e+00, 2.08375308e+01, -3.69741767e+01, 4.23197284e+01, -3.25792598e+01, 1.70414550e+01, -5.96309296e+00, 1.33137023e+00, -1.70651331e-01, 9.50672270e-03 };
    double vec9[] = { -0.05252213, 0.31997016, -0.70222094, 0.4396943, 0.75474308, -1.51932893, 0.75474308, 0.4396943, -0.70222094, 0.31997016, -0.05252213 };
    double vec10[] = { 1.00000000e+00, -6.85241024e+00, 2.08375308e+01, -3.69741767e+01, 4.23197284e+01, -3.25792598e+01, 1.70414550e+01, -5.96309296e+00, 1.33137023e+00, -1.70651331e-01, 9.50672270e-03 };
    double vec11[] = { 0.15756639, -1.59017606, 7.20683593, -19.31581487, 33.90539323, -40.72760926, 33.90539323, -19.31581487, 7.20683593, -1.59017606, 0.15756639 };
    double vec12[] = { 1.00000000e+00, -6.85241024e+00, 2.08375308e+01, -3.69741767e+01, 4.23197284e+01, -3.25792598e+01, 1.70414550e+01, -5.96309296e+00, 1.33137023e+00, -1.70651331e-01, 9.50672270e-03 };
    
    B0.push_back(std::make_unique<IIRFilter>(vec1,  vec2));
    B0.push_back(std::make_unique<IIRFilter>(vec1,  vec2));
    B1.push_back(std::make_unique<IIRFilter>(vec3,  vec4));
    B1.push_back(std::make_unique<IIRFilter>(vec3,  vec4));
    B2.push_back(std::make_unique<IIRFilter>(vec5,  vec6));
    B2.push_back(std::make_unique<IIRFilter>(vec5,  vec6));
    B3.push_back(std::make_unique<IIRFilter>(vec7,  vec8));
    B3.push_back(std::make_unique<IIRFilter>(vec7,  vec8));
    B4.push_back(std::make_unique<IIRFilter>(vec9,  vec10));
    B4.push_back(std::make_unique<IIRFilter>(vec9,  vec10));
    B5.push_back(std::make_unique<IIRFilter>(vec11, vec12));
    B5.push_back(std::make_unique<IIRFilter>(vec11, vec12));
}

DecoderAudioProcessor::~DecoderAudioProcessor()
{
}

//==============================================================================
const juce::String DecoderAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool DecoderAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool DecoderAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool DecoderAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double DecoderAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int DecoderAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int DecoderAudioProcessor::getCurrentProgram()
{
    return 0;
}

void DecoderAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String DecoderAudioProcessor::getProgramName (int index)
{
    return {};
}

void DecoderAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void DecoderAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
}

void DecoderAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool DecoderAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void DecoderAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    auto blockSize = buffer.getNumSamples();

    auto* inputL = buffer.getReadPointer(0);
    auto* inputR = buffer.getReadPointer(1);
    auto* outputL = buffer.getWritePointer(0);
    auto* outputR = buffer.getWritePointer(1);
    update_coeffs(inst0, fc0->get(), getSampleRate());
    update_coeffs(inst1, fc1->get(), getSampleRate());
    update_coeffs(inst2, fc2->get(), getSampleRate());
    update_coeffs(inst3, fc3->get(), getSampleRate());
    update_coeffs(inst4, fc4->get(), getSampleRate());
    update_coeffs(inst5, fc5->get(), getSampleRate());
    for (int i = 0; i < blockSize; i++)
    {
        auto L = inputL[i];
        auto R = inputR[i];

        auto b0L = B0[0]->process_sample(L) * bl0->get();
        auto b1L = B1[0]->process_sample(L) * bl1->get();
        auto b2L = B2[0]->process_sample(L) * bl2->get();
        auto b3L = B3[0]->process_sample(L) * bl3->get();
        auto b4L = B4[0]->process_sample(L) * bl4->get();
        auto b5L = B5[0]->process_sample(L) * bl5->get();

        auto b0R = B0[1]->process_sample(R) * bl0->get();
        auto b1R = B1[1]->process_sample(R) * bl1->get();
        auto b2R = B2[1]->process_sample(R) * bl2->get();
        auto b3R = B3[1]->process_sample(R) * bl3->get();
        auto b4R = B4[1]->process_sample(R) * bl4->get();
        auto b5R = B5[1]->process_sample(R) * bl5->get();

        process(inst0, b0L, b0R);
        process(inst1, b1L, b1R);
        process(inst2, b2L, b2R);
        process(inst3, b3L, b3R);
        process(inst4, b4L, b4R);
        process(inst5, b5L, b5R);

        auto l0 = inst0.C * center0->get() - inst0.S * surround0->get();
        auto l1 = inst1.C * center1->get() - inst1.S * surround1->get();
        auto l2 = inst2.C * center2->get() - inst2.S * surround2->get();
        auto l3 = inst3.C * center3->get() - inst3.S * surround3->get();
        auto l4 = inst4.C * center4->get() - inst4.S * surround4->get();
        auto l5 = inst5.C * center5->get() - inst5.S * surround5->get();

        auto r0 = inst0.C * center0->get() + inst0.S * surround0->get();
        auto r1 = inst1.C * center1->get() + inst1.S * surround1->get();
        auto r2 = inst2.C * center2->get() + inst2.S * surround2->get();
        auto r3 = inst3.C * center3->get() + inst3.S * surround3->get();
        auto r4 = inst4.C * center4->get() + inst4.S * surround4->get();
        auto r5 = inst5.C * center5->get() + inst5.S * surround5->get();

        //outputL[i] = instance.C * level1->get() - instance.S * level2->get();
        //outputR[i] = instance.C * level1->get() + instance.S * level2->get();

        outputL[i] = l0+l1+l2+l3+l4+l5;
        outputR[i] = r0+r1+r2+r3+r4+r5;
    }
}

//==============================================================================
bool DecoderAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* DecoderAudioProcessor::createEditor()
{
    return new DecoderAudioProcessorEditor (*this);
}

//==============================================================================
void DecoderAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void DecoderAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new DecoderAudioProcessor();
}



