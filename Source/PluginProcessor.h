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
    decoder inst0;
    decoder inst1;
    decoder inst2;
    decoder inst3;
    decoder inst4;
    decoder inst5;

    juce::AudioParameterFloat* center0; juce::AudioParameterFloat* surround0; 
    juce::AudioParameterFloat* center1; juce::AudioParameterFloat* surround1; 
    juce::AudioParameterFloat* center2; juce::AudioParameterFloat* surround2; 
    juce::AudioParameterFloat* center3; juce::AudioParameterFloat* surround3; 
    juce::AudioParameterFloat* center4; juce::AudioParameterFloat* surround4; 
    juce::AudioParameterFloat* center5; juce::AudioParameterFloat* surround5; 

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