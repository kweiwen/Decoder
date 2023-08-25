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
    addParameter(chL = new juce::AudioParameterFloat("L", "L", 0.00f, 1.00f, 0.50f));
    addParameter(chR = new juce::AudioParameterFloat("R", "R", 0.00f, 1.00f, 0.50f));
    addParameter(chC = new juce::AudioParameterFloat("C", "C", 0.00f, 1.00f, 0.50f));
    addParameter(chLC = new juce::AudioParameterFloat("LC", "LC", 0.00f, 1.00f, 0.50f));
    addParameter(chRC = new juce::AudioParameterFloat("RC", "RC", 0.00f, 1.00f, 0.50f));
    addParameter(chLS = new juce::AudioParameterFloat("LS", "LS", 0.00f, 4.00f, 4.00f));
    addParameter(chRS = new juce::AudioParameterFloat("RS", "RS", 0.00f, 4.00f, 4.00f));
    addParameter(pf0 = new juce::AudioParameterFloat("panning0_factor", "panning0_factor", 15.0f, 120.0f, 90.00f));
    addParameter(pf1 = new juce::AudioParameterFloat("panning1_factor", "panning1_factor", 15.0f, 120.0f, 90.00f));
    addParameter(pf2 = new juce::AudioParameterFloat("panning2_factor", "panning2_factor", 15.0f, 120.0f, 90.00f));
    addParameter(pf3 = new juce::AudioParameterFloat("panning3_factor", "panning3_factor", 15.0f, 120.0f, 90.00f));
    addParameter(pf4 = new juce::AudioParameterFloat("panning4_factor", "panning4_factor", 15.0f, 120.0f, 90.00f));
    addParameter(pf5 = new juce::AudioParameterFloat("panning5_factor", "panning5_factor", 15.0f, 120.0f, 90.00f));
    addParameter(band0_level = new juce::AudioParameterFloat("band0_level", "band0_level", 0.00f, 1.0f, 1.00f));
    addParameter(band1_level = new juce::AudioParameterFloat("band1_level", "band1_level", 0.00f, 1.0f, 1.00f));
    addParameter(band2_level = new juce::AudioParameterFloat("band2_level", "band2_level", 0.00f, 1.0f, 1.00f));
    addParameter(band3_level = new juce::AudioParameterFloat("band3_level", "band3_level", 0.00f, 1.0f, 1.00f));
    addParameter(band4_level = new juce::AudioParameterFloat("band4_level", "band4_level", 0.00f, 1.0f, 1.00f));
    addParameter(band5_level = new juce::AudioParameterFloat("band5_level", "band5_level", 0.00f, 1.0f, 1.00f));
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

    F1.init(std::vector<double> { 1.0, -1.9599837715993710940409755494329147040843963623046875, 0.9614960139400998695435873742098920047283172607421875 },
            std::vector<double> { 1.0, -0.961481451595149305688892127363942563533782958984375 });

    F2.init(std::vector<double> { 1.0, -1.9185700325442638369821679589222185313701629638671875, 0.92450263188809345127339156533707864582538604736328125 },
            std::vector<double> { 1.0, -0.9243904916581986430657025266555137932300567626953125 });

    F3.init(std::vector<double> { 1.0, -1.8320767110846463143758455771603621542453765869140625, 0.85491377774159393876374224419123493134975433349609375 },
            std::vector<double> { 1.0, -0.85408068546344428906280654700822196900844573974609375 });
    
    F4.init(std::vector<double> { 1.0, -1.647552215703985023509403617936186492443084716796875, 0.7323389172727996498934999181074090301990509033203125 },
            std::vector<double> { 1.0, -0.726542528005357013398679555393755435943603515625 });
    
    F5.init(std::vector<double> { 1.0, -1.2505164308487393132196530132205225527286529541015625, 0.54572331550945651912343237199820578098297119140625 },
            std::vector<double> { 1.0, -0.5095254494944285728053046113927848637104034423828125 });

    A10.init(std::vector<double> { 1.0, -1.9599837715993710940409755494329147040843963623046875, 0.9614960139400998695435873742098920047283172607421875 });
    A11.init(std::vector<double> { 1.0, -1.9599837715993710940409755494329147040843963623046875, 0.9614960139400998695435873742098920047283172607421875 });

    A30.init(std::vector<double> { 1.0, -1.8320767110846463143758455771603621542453765869140625, 0.85491377774159393876374224419123493134975433349609375 });
    A31.init(std::vector<double> { 1.0, -1.8320767110846463143758455771603621542453765869140625, 0.85491377774159393876374224419123493134975433349609375 });

    A21.init(std::vector<double> { 1.0, -1.9185700325442638369821679589222185313701629638671875, 0.92450263188809345127339156533707864582538604736328125 });

    A50.init(std::vector<double> { 1.0, -1.2505164308487393132196530132205225527286529541015625, 0.54572331550945651912343237199820578098297119140625 });
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


    inst0.update_coeffs(pf0->get(), getSampleRate());
    inst1.update_coeffs(pf1->get(), getSampleRate());
    inst2.update_coeffs(pf2->get(), getSampleRate());
    inst3.update_coeffs(pf3->get(), getSampleRate());
    inst4.update_coeffs(pf4->get(), getSampleRate());
    inst5.update_coeffs(pf5->get(), getSampleRate());

    for (int i = 0; i < blockSize; i++)
    {
        double L = inputL[i];
        double R = inputR[i];

        // Complementary AllPass
        F4.process_sample(L, R);
        A50.process_sample(F4.L.pos, F4.R.pos);
        F2.process_sample(A50.L.filtered, A50.R.filtered);
        A30.process_sample(F2.L.pos, F2.R.pos);
        F1.process_sample(A30.L.filtered, A30.R.filtered);
        A10.process_sample(F2.L.neg, F2.R.neg);
        F3.process_sample(A10.L.filtered, A10.R.filtered);
        A11.process_sample(F4.L.neg, F4.R.neg);
        A21.process_sample(A11.L.filtered, A11.R.filtered);
        A31.process_sample(A21.L.filtered, A21.R.filtered);
        F5.process_sample(A31.L.filtered, A31.R.filtered);

        // decoder
        inst0.process(F1.L.pos * band0_level->get(), F1.R.pos * band0_level->get());
        inst1.process(F1.L.neg * band1_level->get(), F1.R.neg * band1_level->get());
        inst2.process(F3.L.pos * band2_level->get(), F3.R.pos * band2_level->get());
        inst3.process(F3.L.neg * band3_level->get(), F3.R.neg * band3_level->get());
        inst4.process(F5.L.pos * band4_level->get(), F5.R.pos * band4_level->get());
        inst5.process(F5.L.neg * band5_level->get(), F5.R.neg * band5_level->get());

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
        outputC[i] = (inst0.C + inst1.C + inst2.C + inst3.C + inst4.C) * chC->get();
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