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
    addParameter(bl0 = new juce::AudioParameterFloat("bl0", "bl0", 0.00f, 1.0f, 1.00f));
    addParameter(bl1 = new juce::AudioParameterFloat("bl1", "bl1", 0.00f, 1.0f, 1.00f));
    addParameter(bl2 = new juce::AudioParameterFloat("bl2", "bl2", 0.00f, 1.0f, 1.00f));
    addParameter(bl3 = new juce::AudioParameterFloat("bl3", "bl3", 0.00f, 1.0f, 1.00f));
    addParameter(bl4 = new juce::AudioParameterFloat("bl4", "bl4", 0.00f, 1.0f, 1.00f));

    //auto b0 = 3.844633506779265e-06;
    //auto b1 = 7.68926701355853e-06;
    //auto b2 = 3.844633506779265e-06;
    //auto a0 = 1.0;
    //auto a1 = -1.9944464105419266;
    //auto a2 = 0.9944617890759537;

    double a1[] = { 0.96779909, -1.93663508,  0.96779909 };
    double f1[] = { -0.00025922, -0.00051845, -0.00025922 };
    double f1_[] = { 0.96805831, -1.93611663,  0.96805831 };
    double denominator1[] = { 1.0, -1.93559818,  0.93663508 };

    double a2[] = { 0.87697646, -1.76908772,  0.87697646 };
    double f2[] = { -0.0037837, -0.0075674, -0.0037837 };
    double f2_[] = { 0.88076016, -1.76152032,  0.88076016 };
    double denominator2[] = { 1.0 , -1.75395293,  0.76908772 };

    double a3[] = { 0.57735027, -1.33333333,  0.57735027 };
    double f3[] = { -0.0446582, -0.0893164, -0.0446582 };
    double f3_[] = { 0.62200847, -1.24401694,  0.62200847 };
    double denominator3[] = { 1.0 , -1.15470054,  0.33333333 };

    double f4[] = { 2.35193153e-10, 9.40772611e-10, 1.41115892e-09, 9.40772611e-10, 2.35193153e-10 };
    double f4_[] = { 0.98895425, -3.955817,  5.9337255, -3.955817,  0.98895425 };
    double denominator4[] = { 1.0 , -3.97778581,  5.93360349, -3.93384818,  0.97803051 };

    A1.push_back(std::make_unique<IIRFilter>(a1, denominator1));
    A1.push_back(std::make_unique<IIRFilter>(a1, denominator1));
    A11.push_back(std::make_unique<IIRFilter>(a1, denominator1));
    A11.push_back(std::make_unique<IIRFilter>(a1, denominator1));
    F1.push_back(std::make_unique<IIRFilter>(f1, denominator1));
    F1.push_back(std::make_unique<IIRFilter>(f1, denominator1));
    F1_.push_back(std::make_unique<IIRFilter>(f1_, denominator1));
    F1_.push_back(std::make_unique<IIRFilter>(f1_, denominator1));

    A21.push_back(std::make_unique<IIRFilter>(a2, denominator2));
    A21.push_back(std::make_unique<IIRFilter>(a2, denominator2));
    F2.push_back(std::make_unique<IIRFilter>(f2, denominator2));
    F2.push_back(std::make_unique<IIRFilter>(f2, denominator2));
    F2_.push_back(std::make_unique<IIRFilter>(f2_, denominator2));
    F2_.push_back(std::make_unique<IIRFilter>(f2_, denominator2));
    
    A3.push_back(std::make_unique<IIRFilter>(a3, denominator3));
    A3.push_back(std::make_unique<IIRFilter>(a3, denominator3));
    A31.push_back(std::make_unique<IIRFilter>(a3, denominator3));
    A31.push_back(std::make_unique<IIRFilter>(a3, denominator3));
    F3.push_back(std::make_unique<IIRFilter>(f3, denominator3));
    F3.push_back(std::make_unique<IIRFilter>(f3, denominator3));
    F3_.push_back(std::make_unique<IIRFilter>(f3_, denominator3));
    F3_.push_back(std::make_unique<IIRFilter>(f3_, denominator3));

    F4.push_back(std::make_unique<IIRFilter>(f4, denominator4));
    F4.push_back(std::make_unique<IIRFilter>(f4, denominator4));
    F4_.push_back(std::make_unique<IIRFilter>(f4_, denominator4));
    F4_.push_back(std::make_unique<IIRFilter>(f4_, denominator4));
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

    for (int i = 0; i < blockSize; i++)
    {
        auto L = inputL[i];
        auto R = inputR[i];

        // L Channel
        auto F4L = F4[0]->process_sample(L);
        auto F4_L = F4_[0]->process_sample(L);
        
        auto F2L = F2[0]->process_sample(F4_L);
        auto F2_L = F2_[0]->process_sample(F4_L);

        auto A3_L = A3[0]->process_sample(F2L);
        auto A1_L = A1[0]->process_sample(F2_L);
   
        auto b0L = A11[0]->process_sample(A21[0]->process_sample(A31[0]->process_sample(F4L))) * bl0->get();
        auto b1L = F1[0]->process_sample(A3_L) * bl1->get();
        auto b2L = F1_[0]->process_sample(A3_L) * bl2->get();
        auto b3L = F3[0]->process_sample(A1_L) * bl3->get();
        auto b4L = F3_[0]->process_sample(A1_L) * bl4->get();

        // R Channel
        auto F4R = F4[1]->process_sample(R);
        auto F4_R = F4_[1]->process_sample(R);

        auto F2R = F2[1]->process_sample(F4_R);
        auto F2_R = F2_[1]->process_sample(F4_R);

        auto A3_R = A3[1]->process_sample(F2R);
        auto A1_R = A1[1]->process_sample(F2_R);

        auto b0R = A11[1]->process_sample(A21[1]->process_sample(A31[1]->process_sample(F4R))) * bl0->get();
        auto b1R = F1[1]->process_sample(A3_R) * bl1->get();
        auto b2R = F1_[1]->process_sample(A3_R) * bl2->get();
        auto b3R = F3[1]->process_sample(A1_R) * bl3->get();
        auto b4R = F3_[1]->process_sample(A1_R) * bl4->get();

        //inst0.process(b0L, b0R);
        //inst1.process(b1L, b1R);
        //inst2.process(b2L, b2R);
        //inst3.process(b3L, b3R);
        //inst4.process(b4L, b4R);

        //auto delayLine_LS = CB_LS->readBuffer(delayTimeSR->get(), false);
        //auto delayLine_RS = CB_RS->readBuffer(delayTimeSR->get(), false);
        //auto delayLine_LC = CB_LC->readBuffer(delayTime->get(), false);
        //auto delayLine_RC = CB_RC->readBuffer(delayTime->get(), false);

        //CB_LS->writeBuffer((inst0.LS + inst1.LS + inst2.LS + inst3.LS + inst4.LS));
        //CB_RS->writeBuffer((inst0.RS + inst1.RS + inst2.RS + inst3.RS + inst4.RS));
        //CB_LC->writeBuffer((inst0.LC + inst1.LC + inst2.LC + inst3.LC + inst4.LC));
        //CB_RC->writeBuffer((inst0.RC + inst1.RC + inst2.RC + inst3.RC + inst4.RC));
        
        //outputL[i] = inputL[i] * chL->get();
        //outputR[i] = inputR[i] * chR->get();
        //outputC[i] = (inst0.C + inst1.C + inst2.C + inst3.C + inst4.C) * chC->get();
        //outputLC[i] = delayLine_LC * chLC->get();
        //outputRC[i] = delayLine_RC * chRC->get();
        //outputLS[i] = delayLine_LS * chLS->get();
        //outputRS[i] = delayLine_RS * chRS->get();
        outputL[i] = b0L + b1L + b2L + b3L + b4L;
        outputR[i] = b0R + b1R + b2R + b3R + b4R;

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



