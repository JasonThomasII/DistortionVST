/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/**
*/
class DistortionVSTAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    DistortionVSTAudioProcessor();
    ~DistortionVSTAudioProcessor() override;

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

    //Getters
    juce::AudioProcessorValueTreeState& getState();
    
    void updateEQFilters(double sampleRate);
    
    // Distortion type enum
    enum DistortionType {
        AtanClip = 0,
        Tanh = 1,
        SoftKnee = 2,
        AsymmetricClip = 3
    };
    
    // Distortion function dispatcher
    float applyDistortion(float input, DistortionType type, float drive, float range);
    float atanDistortion(float x);
    float tanhDistortion(float x);
    float softKneeDistortion(float x);
    float asymmetricDistortion(float x);
    
    // IR loading and processing
    void loadIRsFromFolder();
    void selectIR(int index);
    juce::StringArray getIRFileList() const;
    void processIRBuffer(juce::AudioBuffer<float>& buffer);


private:

    juce::ScopedPointer<juce::AudioProcessorValueTreeState> state;
    juce::Reverb reverb;
    juce::Reverb::Parameters reverbParams;
    
    // Noise Gate state (per channel)
    std::vector<float> gateEnvelope;
    
    // IR Convolution
    juce::File irFolder;
    juce::StringArray irFileList;
    int currentIRIndex = -1;
    juce::AudioBuffer<float> irBuffer;
    std::unique_ptr<juce::dsp::Convolution> convolver;
    int currentIRLength = 0;
    
    // 5-Band EQ Filters
    juce::dsp::ProcessorChain<
        juce::dsp::IIR::Filter<float>,  // Low-shelf (60 Hz)
        juce::dsp::IIR::Filter<float>,  // Peaking (250 Hz)
        juce::dsp::IIR::Filter<float>,  // Peaking (1000 Hz)
        juce::dsp::IIR::Filter<float>,  // Peaking (4000 Hz)
        juce::dsp::IIR::Filter<float>   // High-shelf (16000 Hz)
    > eqChain;
    
    // Oversampling for distortion (configurable rate for cleaner harmonics)
    std::unique_ptr<juce::dsp::Oversampling<float>> oversampler;
    int currentOversamplingFactor = 2;  // Current oversampling factor (0, 1, 2, or 3 = 1x, 2x, 4x, 8x)
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DistortionVSTAudioProcessor)
};
