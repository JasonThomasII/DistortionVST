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
        
    state->createAndAddParameter("drive", "Drive","Drive",NormalisableRange<float>(0.0f, 1.0f, 0.001f),1,nullptr,nullptr);
    state->createAndAddParameter("range", "Range", "Range", NormalisableRange<float>(0.0f, 3000.0f, 0.001f), 1, nullptr, nullptr);
    state->createAndAddParameter("blend", "Blend", "Blend", NormalisableRange<float>(0.0f, 1.0f, 0.001f), 1, nullptr, nullptr);
    state->createAndAddParameter("volume", "Volume", "Volume", NormalisableRange<float>(0.0f, 3.0f, 0.001f), 1, nullptr, nullptr);
    state->createAndAddParameter("reverb", "Reverb", "Reverb", NormalisableRange<float>(0.0f, 1.0f, 0.001f), 0.2f, nullptr, nullptr);

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

    reverbParams.wetLevel = reverbAmount;
    reverbParams.dryLevel = 1.0f - reverbAmount;
    reverb.setParameters(reverbParams);

    // Apply distortion effect
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);
        for (int sample = 0; sample < buffer.getNumSamples(); sample++) {
            
            float cleanSig = *channelData;
            
            *channelData *= drive * range;
            
            //Distortion formula                                              //Read clean single
            *channelData = ((((2.f / float_Pi) * atan(*channelData) * blend) + (cleanSig * (1.f/ blend))) /2) * volume;
            
            channelData++;
        }
        
    }
    
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
