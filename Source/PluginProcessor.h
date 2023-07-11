/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "decoder.h"
#include "IIRFilter.h"

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
    surround inst5;

    juce::AudioParameterFloat* chLC;
    juce::AudioParameterFloat* chRC;
    juce::AudioParameterFloat* chLS;
    juce::AudioParameterFloat* chRS;
    juce::AudioParameterFloat* chC;
    juce::AudioParameterFloat* chL;
    juce::AudioParameterFloat* chR;

    juce::AudioParameterFloat* fc0;
    juce::AudioParameterFloat* fc1;
    juce::AudioParameterFloat* fc2;
    juce::AudioParameterFloat* fc3;
    juce::AudioParameterFloat* fc4;
    juce::AudioParameterFloat* fc5;

    juce::AudioParameterFloat* bl0;
    juce::AudioParameterFloat* bl1;
    juce::AudioParameterFloat* bl2;
    juce::AudioParameterFloat* bl3;
    juce::AudioParameterFloat* bl4;
    juce::AudioParameterFloat* bl5;

    std::vector<std::unique_ptr<IIRFilter>> B0;
    std::vector<std::unique_ptr<IIRFilter>> B1;
    std::vector<std::unique_ptr<IIRFilter>> B2;
    std::vector<std::unique_ptr<IIRFilter>> B3;
    std::vector<std::unique_ptr<IIRFilter>> B4;
    std::vector<std::unique_ptr<IIRFilter>> B5;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DecoderAudioProcessor)
};