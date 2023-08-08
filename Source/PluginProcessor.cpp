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
                       .withOutput ("Output", juce::AudioChannelSet::create7point0(), true)
                     #endif
                       )
#endif
{
    addParameter(delayTimeSR = new juce::AudioParameterFloat("timeSR", "timeSR", 1.00f, 2048.00f, 2048.00f));
    addParameter(delayTime   = new juce::AudioParameterFloat("time", "time", 1.00f, 1024.00f, 1024.00f));
    addParameter(chL = new juce::AudioParameterFloat("chL", "chL", 0.00f, 1.00f, 0.50f));
    addParameter(chR = new juce::AudioParameterFloat("chR", "chR", 0.00f, 1.00f, 0.50f));
    addParameter(chC = new juce::AudioParameterFloat("chC", "chC", 0.00f, 1.00f, 0.50f));
    addParameter(chLC = new juce::AudioParameterFloat("chLC", "chLC", 0.00f, 1.00f, 0.50f));
    addParameter(chRC = new juce::AudioParameterFloat("chRC", "chRC", 0.00f, 1.00f, 0.50f));
    addParameter(chLS = new juce::AudioParameterFloat("chLS", "chLS", 0.00f, 4.00f, 4.00f));
    addParameter(chRS = new juce::AudioParameterFloat("chRS", "chRS", 0.00f, 4.00f, 4.00f));
    addParameter(fc0 = new juce::AudioParameterFloat("fc0", "fc0", 5.0f, 120.0f, 90.00f));
    addParameter(fc1 = new juce::AudioParameterFloat("fc1", "fc1", 5.0f, 120.0f, 90.00f));
    addParameter(fc2 = new juce::AudioParameterFloat("fc2", "fc2", 5.0f, 120.0f, 90.00f));
    addParameter(fc3 = new juce::AudioParameterFloat("fc3", "fc3", 5.0f, 120.0f, 90.00f));
    addParameter(fc4 = new juce::AudioParameterFloat("fc4", "fc4", 5.0f, 120.0f, 90.00f));
    addParameter(fc5 = new juce::AudioParameterFloat("fc5", "fc5", 5.0f, 120.0f, 90.00f));
    addParameter(bl0 = new juce::AudioParameterFloat("bl0", "bl0", 0.00f, 1.0f, 1.00f));
    addParameter(bl1 = new juce::AudioParameterFloat("bl1", "bl1", 0.00f, 1.0f, 1.00f));
    addParameter(bl2 = new juce::AudioParameterFloat("bl2", "bl2", 0.00f, 1.0f, 1.00f));
    addParameter(bl3 = new juce::AudioParameterFloat("bl3", "bl3", 0.00f, 1.0f, 1.00f));
    addParameter(bl4 = new juce::AudioParameterFloat("bl4", "bl4", 0.00f, 1.0f, 1.00f));
    addParameter(bl5 = new juce::AudioParameterFloat("bl5", "bl5", 0.00f, 1.0f, 1.00f));

    auto b0 = 3.844633506779265e-06;
    auto b1 = 7.68926701355853e-06;
    auto b2 = 3.844633506779265e-06;
    auto a0 = 1.0;
    auto a1 = -1.9944464105419266;
    auto a2 = 0.9944617890759537;

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
    CB_LS.reset(new CircularBuffer<double>);
    CB_RS.reset(new CircularBuffer<double>);
    CB_LC.reset(new CircularBuffer<double>);
    CB_RC.reset(new CircularBuffer<double>);

    CB_LS->createCircularBuffer(8192);
    CB_LS->flushBuffer();
    CB_RS->createCircularBuffer(8192);
    CB_RS->flushBuffer();

    CB_LC->createCircularBuffer(8192);
    CB_LC->flushBuffer();
    CB_RC->createCircularBuffer(8192);
    CB_RC->flushBuffer();
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
    auto* outputC = buffer.getWritePointer(2);
    auto* outputLS = buffer.getWritePointer(3);
    auto* outputRS = buffer.getWritePointer(4);
    auto* outputLC = buffer.getWritePointer(5);
    auto* outputRC = buffer.getWritePointer(6);


    inst0.update_coeffs(fc0->get(), getSampleRate());
    inst1.update_coeffs(fc1->get(), getSampleRate());
    inst2.update_coeffs(fc2->get(), getSampleRate());
    inst3.update_coeffs(fc3->get(), getSampleRate());
    inst4.update_coeffs(fc4->get(), getSampleRate());
    inst5.update_coeffs(fc5->get(), getSampleRate());
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

        inst0.process(b0L, b0R);
        inst1.process(b1L, b1R);
        inst2.process(b2L, b2R);
        inst3.process(b3L, b3R);
        inst4.process(b4L, b4R);
        inst5.process(b5L, b5R);

        auto delayLine_LS = CB_LS->readBuffer(delayTimeSR->get(), false);
        auto delayLine_RS = CB_RS->readBuffer(delayTimeSR->get(), false);
        auto delayLine_LC = CB_LC->readBuffer(delayTime->get(), false);
        auto delayLine_RC = CB_RC->readBuffer(delayTime->get(), false);

        CB_LS->writeBuffer((inst0.LS + inst1.LS + inst2.LS + inst3.LS + inst4.LS + inst5.LS));
        CB_RS->writeBuffer((inst0.RS + inst1.RS + inst2.RS + inst3.RS + inst4.RS + inst5.RS));
        CB_LC->writeBuffer((inst0.LC + inst1.LC + inst2.LC + inst3.LC + inst4.LC + inst5.LC));
        CB_RC->writeBuffer((inst0.RC + inst1.RC + inst2.RC + inst3.RC + inst4.RC + inst5.RC));
        
        outputL[i] = inputL[i] * chL->get();
        outputR[i] = inputR[i] * chR->get();
        outputC[i] = (inst0.C + inst1.C + inst2.C + inst3.C + inst4.C + inst5.C) * chC->get();
        outputLC[i] = delayLine_LC * chLC->get();
        outputRC[i] = delayLine_RC * chRC->get();
        outputLS[i] = delayLine_LS * chLS->get();
        outputRS[i] = delayLine_RS * chRS->get();

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



