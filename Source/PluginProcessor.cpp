/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
using namespace juce;

//==============================================================================
DistortionVSTAudioProcessor::DistortionVSTAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
    state = new AudioProcessorValueTreeState(*this, nullptr);
    
    auto floatToString = [](float value) { return String(value, 2); };
    auto stringToFloat = [](const String& str) { return str.getFloatValue(); };
        
    state->createAndAddParameter("drive", "Drive","Drive",NormalisableRange<float>(0.0f, 1.0f, 0.001f),1,floatToString,stringToFloat);
    state->createAndAddParameter("range", "Range", "Range", NormalisableRange<float>(0.0f, 3000.0f, 0.001f), 1, floatToString, stringToFloat);
    state->createAndAddParameter("blend", "Blend", "Blend", NormalisableRange<float>(0.0f, 1.0f, 0.001f), 1, floatToString, stringToFloat);
    state->createAndAddParameter("volume", "Volume", "Volume", NormalisableRange<float>(0.0f, 3.0f, 0.001f), 1, floatToString, stringToFloat);
    state->createAndAddParameter("reverb", "Reverb", "Reverb", NormalisableRange<float>(0.0f, 1.0f, 0.001f), 0.2f, floatToString, stringToFloat);
    
    // EQ Parameters
    state->createAndAddParameter("lowGain", "Low Gain", "Low Gain", NormalisableRange<float>(-12.0f, 12.0f, 0.1f), 0.0f, floatToString, stringToFloat);
    state->createAndAddParameter("lowMidGain", "Low Mid Gain", "Low Mid Gain", NormalisableRange<float>(-12.0f, 12.0f, 0.1f), 0.0f, floatToString, stringToFloat);
    state->createAndAddParameter("midGain", "Mid Gain", "Mid Gain", NormalisableRange<float>(-12.0f, 12.0f, 0.1f), 0.0f, floatToString, stringToFloat);
    state->createAndAddParameter("highMidGain", "High Mid Gain", "High Mid Gain", NormalisableRange<float>(-12.0f, 12.0f, 0.1f), 0.0f, floatToString, stringToFloat);
    state->createAndAddParameter("highGain", "High Gain", "High Gain", NormalisableRange<float>(-12.0f, 12.0f, 0.1f), 0.0f, floatToString, stringToFloat);
    
    // Noise Gate Parameters
    state->createAndAddParameter("gateEnabled", "Gate Enabled", "Gate Enabled", NormalisableRange<float>(0.0f, 1.0f, 1.0f), 0.0f, floatToString, stringToFloat);
    state->createAndAddParameter("gateThreshold", "Gate Threshold", "Gate Threshold", NormalisableRange<float>(-80.0f, 0.0f, 0.1f), -40.0f, floatToString, stringToFloat);
    state->createAndAddParameter("gateAttack", "Gate Attack", "Gate Attack", NormalisableRange<float>(0.1f, 100.0f, 0.1f), 1.0f, floatToString, stringToFloat);
    state->createAndAddParameter("gateRelease", "Gate Release", "Gate Release", NormalisableRange<float>(10.0f, 1000.0f, 1.0f), 100.0f, floatToString, stringToFloat);

    state->state = ValueTree("drive");
    state->state = ValueTree("range");
    state->state = ValueTree("blend");
    state->state = ValueTree("volume");
    state->state = ValueTree("reverb");


}

DistortionVSTAudioProcessor::~DistortionVSTAudioProcessor()
{
}

//==============================================================================
const String DistortionVSTAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool DistortionVSTAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool DistortionVSTAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool DistortionVSTAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double DistortionVSTAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int DistortionVSTAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int DistortionVSTAudioProcessor::getCurrentProgram()
{
    return 0;
}

void DistortionVSTAudioProcessor::setCurrentProgram (int index)
{
}

const String DistortionVSTAudioProcessor::getProgramName (int index)
{
    return {};
}

void DistortionVSTAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void DistortionVSTAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Reverb setup
    reverbParams.roomSize  = 0.6f;
    reverbParams.damping   = 0.4f;
    reverbParams.width     = 1.0f;
    reverbParams.wetLevel  = 0.3f;   // starting wet amount
    reverbParams.dryLevel  = 0.7f;

    reverb.setParameters(reverbParams);
    reverb.setSampleRate(sampleRate);
    
    // EQ setup
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = getTotalNumInputChannels();
    
    eqChain.prepare(spec);
    
    // Initialize filters with neutral settings
    updateEQFilters(sampleRate);
    
    // Initialize noise gate envelopes
    gateEnvelope.clear();
    gateEnvelope.resize(getTotalNumInputChannels(), 0.0f);
}

void DistortionVSTAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool DistortionVSTAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
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

void DistortionVSTAudioProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    auto reverbParams = reverb.getParameters();


    // In case we have more outputs than inputs, this code clears any output
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());
    float drive = *state->getRawParameterValue("drive");
    float range = *state->getRawParameterValue("range");
    float blend = *state->getRawParameterValue("blend");
    float volume = *state->getRawParameterValue("volume");
    float reverbAmount = *state->getRawParameterValue("reverb");
    
    // Noise Gate parameters
    bool gateEnabled = *state->getRawParameterValue("gateEnabled") > 0.5f;
    float gateThreshold = *state->getRawParameterValue("gateThreshold");
    float gateAttackMs = *state->getRawParameterValue("gateAttack");
    float gateReleaseMs = *state->getRawParameterValue("gateRelease");
    
    float thresholdLinear = juce::Decibels::decibelsToGain(gateThreshold);
    
    // Calculate attack and release coefficients (smoother envelope)
    double sampleRate = getSampleRate();
    float attackCoeff = std::exp(-2.0f * juce::MathConstants<float>::pi / (gateAttackMs * sampleRate * 0.001f));
    float releaseCoeff = std::exp(-2.0f * juce::MathConstants<float>::pi / (gateReleaseMs * sampleRate * 0.001f));

    reverbParams.wetLevel = reverbAmount;
    reverbParams.dryLevel = 1.0f - reverbAmount;
    reverb.setParameters(reverbParams);
    
    // Update EQ settings
    updateEQFilters(getSampleRate());

    // Apply distortion effect
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);
        for (int sample = 0; sample < buffer.getNumSamples(); sample++) {
            
            float inputSample = *channelData;
            
            // Noise Gate processing
            if (gateEnabled && channel < (int)gateEnvelope.size())
            {
                float inputLevel = std::abs(inputSample);
                
                // Envelope follower with attack/release
                if (inputLevel > gateEnvelope[channel])
                {
                    gateEnvelope[channel] = attackCoeff * gateEnvelope[channel] + (1.0f - attackCoeff) * inputLevel;
                }
                else
                {
                    gateEnvelope[channel] = releaseCoeff * gateEnvelope[channel] + (1.0f - releaseCoeff) * inputLevel;
                }
                
                // Gate logic - mute if below threshold
                if (gateEnvelope[channel] < thresholdLinear)
                {
                    inputSample = 0.0f;
                }
            }
            
            float cleanSig = inputSample;
            
            *channelData = inputSample * drive * range;
            
            // Clamp blend to avoid division by zero
            float clampedBlend = juce::jlimit(0.001f, 0.999f, blend);
            
            //Distortion formula - mix between distorted and clean signal
            *channelData = ((((2.f / juce::MathConstants<float>::pi) * atan(*channelData) * clampedBlend) + (cleanSig * (1.f - clampedBlend))) / 2.f) * volume;
            
            channelData++;
        }
        
    }
    
    // Apply EQ
    juce::dsp::AudioBlock<float> block(buffer);
    eqChain.process(juce::dsp::ProcessContextReplacing<float>(block));
    
if (buffer.getNumChannels() >= 2)
    {
        reverb.processStereo(buffer.getWritePointer(0),
                            buffer.getWritePointer(1),
                            buffer.getNumSamples());
    }
else
    {
        reverb.processMono(buffer.getWritePointer(0),
                        buffer.getNumSamples());
    }
}

//GetState
AudioProcessorValueTreeState& DistortionVSTAudioProcessor::getState() {
    return *state;
}

void DistortionVSTAudioProcessor::updateEQFilters(double sampleRate)
{
    float lowGain = state->getRawParameterValue("lowGain")->load();
    float lowMidGain = state->getRawParameterValue("lowMidGain")->load();
    float midGain = state->getRawParameterValue("midGain")->load();
    float highMidGain = state->getRawParameterValue("highMidGain")->load();
    float highGain = state->getRawParameterValue("highGain")->load();
    
    // Convert dB to linear
    float lowGainLinear = juce::Decibels::decibelsToGain(lowGain);
    float lowMidGainLinear = juce::Decibels::decibelsToGain(lowMidGain);
    float midGainLinear = juce::Decibels::decibelsToGain(midGain);
    float highMidGainLinear = juce::Decibels::decibelsToGain(highMidGain);
    float highGainLinear = juce::Decibels::decibelsToGain(highGain);
    
    // Create filter coefficients for 5-band EQ
    // Low-shelf filter at 60 Hz
    auto& lowFilter = eqChain.get<0>();
    lowFilter.coefficients = juce::dsp::IIR::Coefficients<float>::makeLowShelf(sampleRate, 60.0f, 0.707f, lowGainLinear);
    
    // Peaking filter at 250 Hz (low-mid)
    auto& lowMidFilter = eqChain.get<1>();
    lowMidFilter.coefficients = juce::dsp::IIR::Coefficients<float>::makePeakFilter(sampleRate, 250.0f, 0.707f, lowMidGainLinear);
    
    // Peaking filter at 1000 Hz (mid)
    auto& midFilter = eqChain.get<2>();
    midFilter.coefficients = juce::dsp::IIR::Coefficients<float>::makePeakFilter(sampleRate, 1000.0f, 0.707f, midGainLinear);
    
    // Peaking filter at 4000 Hz (high-mid)
    auto& highMidFilter = eqChain.get<3>();
    highMidFilter.coefficients = juce::dsp::IIR::Coefficients<float>::makePeakFilter(sampleRate, 4000.0f, 0.707f, highMidGainLinear);
    
    // High-shelf filter at 16000 Hz
    auto& highFilter = eqChain.get<4>();
    highFilter.coefficients = juce::dsp::IIR::Coefficients<float>::makeHighShelf(sampleRate, 16000.0f, 0.707f, highGainLinear);
}



//==============================================================================
bool DistortionVSTAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* DistortionVSTAudioProcessor::createEditor()
{
    return new DistortionVSTAudioProcessorEditor (*this);
}

//==============================================================================
void DistortionVSTAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    MemoryOutputStream stream(destData, false);
    state->state.writeToStream(stream);

}

void DistortionVSTAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    ValueTree tree = ValueTree::readFromData(data, sizeInBytes);
    if (tree.isValid()) {
        state->state = tree;
    }
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new DistortionVSTAudioProcessor();
}
