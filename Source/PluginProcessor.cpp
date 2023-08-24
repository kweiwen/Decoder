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
                       //.withOutput ("Output", juce::AudioChannelSet::create7point0(), true)
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


    //F3.init(std::vector<double> { 1.0, -1.895426574914740758259767972049303352832794189453125, 0.89949353638218720874419886968098580837249755859375 },
    //        std::vector<double> { 1.0, -2.89278428755276362238646470359526574611663818359375, 2.79254385729267351479165881755761802196502685546875, -0.8994934687432796227568587710266001522541046142578125 });
    //F4.init(std::vector<double> { 1.0, -1.59737821583015460191745660267770290374755859375, 0.6537276179548758836546085149166174232959747314453125 },
    //        std::vector<double> { 1.0, -2.5561124994299166957034685765393078327178955078125, 2.224470430509824847575828243861906230449676513671875, -0.65367591026143345711574283996014855802059173583984375 });
    //F5.init(std::vector<double> { 1.0, -0.5880176344709477920247309157275594770908355712890625, 0.17603526894189702733939384415862150490283966064453125 },
    //        std::vector<double> { 1.0, -1.056831276945417119605963307549245655536651611328125, 0.7891444864975742934376512494054622948169708251953125, -0.1548114425068488897441199014792800880968570709228515625 });

    //A10.init(std::vector<double> {0.99366138261047354252042396183242090046405792236328125, -1.9936459871909999463923668372444808483123779296875, 1.0}, std::vector<double> {1.0, -1.9936459871909999463923668372444808483123779296875, 0.99366138261047354252042396183242090046405792236328125});
    //A11.init(std::vector<double> {0.99366138261047354252042396183242090046405792236328125, -1.9936459871909999463923668372444808483123779296875, 1.0}, std::vector<double> {1.0, -1.9936459871909999463923668372444808483123779296875, 0.99366138261047354252042396183242090046405792236328125});
    //
    //A30.init(std::vector<double> {0.89949353638218720874419886968098580837249755859375, -1.895426574914740758259767972049303352832794189453125, 1.0}, std::vector<double> {1.0, -1.895426574914740758259767972049303352832794189453125, 0.89949353638218720874419886968098580837249755859375});
    //A31.init(std::vector<double> {0.89949353638218720874419886968098580837249755859375, -1.895426574914740758259767972049303352832794189453125, 1.0}, std::vector<double> {1.0, -1.895426574914740758259767972049303352832794189453125, 0.89949353638218720874419886968098580837249755859375});
    //
    //A21.init(std::vector<double> {0.97387202264802230633478075105813331902027130126953125, -1.973607796148709514483243765425868332386016845703125, 1.0},  std::vector<double> {1.0, -1.973607796148709514483243765425868332386016845703125, 0.97387202264802230633478075105813331902027130126953125});
    //A50.init(std::vector<double> {0.17603526894189702733939384415862150490283966064453125, -0.5880176344709477920247309157275594770908355712890625, 1.0}, std::vector<double> {1.0, -0.5880176344709477920247309157275594770908355712890625, 0.17603526894189702733939384415862150490283966064453125});
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

    std::vector<double> f1d1{ 1.0, -1.9936459871909999463923668372444808483123779296875, 0.99366138261047354252042396183242090046405792236328125 };
    std::vector<double> f1d2{ 1.0, -2.9936364418694285660649256897158920764923095703125, 2.98729788498444737143699967418797314167022705078125, -0.99366138261041225820946465319138951599597930908203125 };
    F1.init(f1d1, f1d2);

    F2.init(std::vector<double> { 1.0, -1.973607796148709514483243765425868332386016845703125, 0.97387202264802230633478075105813331902027130126953125 },
        std::vector<double> { 1.0, -2.97344235114248522933166896109469234943389892578125, 2.9473186970317701849353397847153246402740478515625, -0.9738720225766270832679083468974567949771881103515625 });
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
    //auto* outputC = buffer.getWritePointer(2);
    //auto* outputLS = buffer.getWritePointer(3);
    //auto* outputRS = buffer.getWritePointer(4);
    //auto* outputLC = buffer.getWritePointer(5);
    //auto* outputRC = buffer.getWritePointer(6);


    inst0.update_coeffs(fc0->get(), getSampleRate());
    inst1.update_coeffs(fc1->get(), getSampleRate());
    inst2.update_coeffs(fc2->get(), getSampleRate());
    inst3.update_coeffs(fc3->get(), getSampleRate());
    inst4.update_coeffs(fc4->get(), getSampleRate());

    for (int i = 0; i < blockSize; i++)
    {
        double L = inputL[i];
        double R = inputR[i];

        //// L Channel
        //auto F4L = F4[0]->process_sample(L);
        //auto F4_L = F4_[0]->process_sample(L);
        //
        //auto F2L = F2[0]->process_sample(F4_L);
        //auto F2_L = F2_[0]->process_sample(F4_L);

        //auto A3_L = A3[0]->process_sample(F2L);
        //auto A1_L = A1[0]->process_sample(F2_L);
   
        //auto b0L = A11[0]->process_sample(A21[0]->process_sample(A31[0]->process_sample(F4L))) * bl0->get();
        //auto b1L = F1[0]->process_sample(A3_L) * bl1->get();
        //auto b2L = F1_[0]->process_sample(A3_L) * bl2->get();
        //auto b3L = F3[0]->process_sample(A1_L) * bl3->get();
        //auto b4L = F3_[0]->process_sample(A1_L) * bl4->get();

        //// R Channel
        //auto F4R = F4[1]->process_sample(R);
        //auto F4_R = F4_[1]->process_sample(R);

        //auto F2R = F2[1]->process_sample(F4_R);
        //auto F2_R = F2_[1]->process_sample(F4_R);

        //auto A3_R = A3[1]->process_sample(F2R);
        //auto A1_R = A1[1]->process_sample(F2_R);

        //auto b0R = A11[1]->process_sample(A21[1]->process_sample(A31[1]->process_sample(F4R))) * bl0->get();
        //auto b1R = F1[1]->process_sample(A3_R) * bl1->get();
        //auto b2R = F1_[1]->process_sample(A3_R) * bl2->get();
        //auto b3R = F3[1]->process_sample(A1_R) * bl3->get();
        //auto b4R = F3_[1]->process_sample(A1_R) * bl4->get();

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
        //DBG("b0L: " << b0L);
        //DBG("b1L: " << b1L);
        //DBG("b2L: " << b2L);
        //DBG("b3L: " << b3L);
        //DBG("b4L: " << b4L);

        F1.process_sample(L);
        F2.process_sample(L);
        outputL[i] = F1.neg;
        outputR[i] = F2.neg;
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