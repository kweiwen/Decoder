/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "decoder.h"
#include "IIRFilter.h"
#include "CircularBuffer.h"

class DecoderAudioProcessor  : public juce::AudioProcessor
                            #if JucePlugin_Enable_ARA
                             , public juce::AudioProcessorARAExtension
                            #endif
{
public:
    //==============================================================================
    DecoderAudioProcessor();
    ~DecoderAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

private:
    juce::AudioChannelSet channelSet;

    surround inst0;
    surround inst1;
    surround inst2;
    surround inst3;
    surround inst4;

    juce::AudioParameterFloat* chLC;
    juce::AudioParameterFloat* chRC;
    juce::AudioParameterFloat* chLS;
    juce::AudioParameterFloat* chRS;
    juce::AudioParameterFloat* chC;
    juce::AudioParameterFloat* chL;
    juce::AudioParameterFloat* chR;
    juce::AudioParameterFloat* delayTimeSR;
    juce::AudioParameterFloat* delayTime;

    juce::AudioParameterFloat* fc0;
    juce::AudioParameterFloat* fc1;
    juce::AudioParameterFloat* fc2;
    juce::AudioParameterFloat* fc3;
    juce::AudioParameterFloat* fc4;

    juce::AudioParameterFloat* bl0;
    juce::AudioParameterFloat* bl1;
    juce::AudioParameterFloat* bl2;
    juce::AudioParameterFloat* bl3;
    juce::AudioParameterFloat* bl4;

    std::unique_ptr<CircularBuffer<double>> CB_LS;
    std::unique_ptr<CircularBuffer<double>> CB_RS;
    std::unique_ptr<CircularBuffer<double>> CB_LC;
    std::unique_ptr<CircularBuffer<double>> CB_RC;

    std::vector<std::unique_ptr<IIRFilter>> A11;
    std::vector<std::unique_ptr<IIRFilter>> A21;
    std::vector<std::unique_ptr<IIRFilter>> A31;

    std::vector<std::unique_ptr<IIRFilter>> A1;
    std::vector<std::unique_ptr<IIRFilter>> A3;

    std::vector<std::unique_ptr<IIRFilter>> F1;
    std::vector<std::unique_ptr<IIRFilter>> F1_;

    std::vector<std::unique_ptr<IIRFilter>> F2;
    std::vector<std::unique_ptr<IIRFilter>> F2_;

    std::vector<std::unique_ptr<IIRFilter>> F3;
    std::vector<std::unique_ptr<IIRFilter>> F3_;

    std::vector<std::unique_ptr<IIRFilter>> F4;
    std::vector<std::unique_ptr<IIRFilter>> F4_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DecoderAudioProcessor)
};