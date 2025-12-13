/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
using namespace juce;

// Custom tab component for Distortion controls
class DistortionTabComponent : public Component
{
public:
    void paint(Graphics& g) override
    {
        g.fillAll(getLookAndFeel().findColour(ResizableWindow::backgroundColourId));
        g.setColour(Colours::white);
        g.setFont(12.0f);
        g.drawText("Drive", 20, 130, 100, 20, Justification::centred, false);
        g.drawText("Range", 140, 130, 100, 20, Justification::centred, false);
        g.drawText("Blend", 260, 130, 100, 20, Justification::centred, false);
        g.drawText("Volume", 80, 260, 100, 20, Justification::centred, false);
        g.drawText("Reverb", 240, 260, 100, 20, Justification::centred, false);
    }
};

// Custom tab component for EQ
class EQTabComponent : public Component
{
public:
    EQTabComponent() : resetButton("Reset")
    {
        resetButton.setColour(TextButton::buttonColourId, Colours::darkgrey);
        resetButton.setColour(TextButton::textColourOffId, Colours::white);
        addAndMakeVisible(resetButton);
    }
    
    void paint(Graphics& g) override
    {
        g.fillAll(getLookAndFeel().findColour(ResizableWindow::backgroundColourId));
        g.setColour(Colours::white);
        g.setFont(12.0f);
        
        // Frequency labels at bottom
        g.drawText("60Hz", 20, 200, 40, 20, Justification::centred, false);
        g.drawText("250Hz", 90, 200, 40, 20, Justification::centred, false);
        g.drawText("1kHz", 160, 200, 40, 20, Justification::centred, false);
        g.drawText("4kHz", 230, 200, 40, 20, Justification::centred, false);
        g.drawText("16kHz", 300, 200, 40, 20, Justification::centred, false);
        
        // Legend on the left showing dB scale
        g.setFont(10.0f);
        g.drawText("+12dB", 2, 40, 12, 15, Justification::centred, false);
        g.drawText("0dB", 2, 105, 12, 15, Justification::centred, false);
        g.drawText("-12dB", 2, 170, 12, 15, Justification::centred, false);
        
        // Draw reference lines on the legend
        g.setColour(Colours::grey);
        g.drawLine(15, 47, 18, 47, 1.0f);   // +12dB line
        g.drawLine(15, 112, 18, 112, 1.0f); // 0dB line
        g.drawLine(15, 177, 18, 177, 1.0f); // -12dB line
    }
    
    void resized() override
    {
        resetButton.setBounds(380, 80, 80, 40);
    }
    
    TextButton resetButton;
};

//==============================================================================
/**
*/
class DistortionVSTAudioProcessorEditor  : public AudioProcessorEditor
{
public:
    DistortionVSTAudioProcessorEditor (DistortionVSTAudioProcessor&);
    ~DistortionVSTAudioProcessorEditor() override;

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;

private:
    std::unique_ptr<Slider> driveKnob;
    std::unique_ptr<Slider> rangeKnob;
    std::unique_ptr<Slider> blendKnob;
    std::unique_ptr<Slider> volumeKnob;
    std::unique_ptr<Slider> reverbKnob;
    
    // EQ Sliders
    std::unique_ptr<Slider> lowGainSlider;
    std::unique_ptr<Slider> lowMidGainSlider;
    std::unique_ptr<Slider> midGainSlider;
    std::unique_ptr<Slider> highMidGainSlider;
    std::unique_ptr<Slider> highGainSlider;
    
    // Tabbed interface components
    std::unique_ptr<DistortionTabComponent> distortionTab;
    std::unique_ptr<EQTabComponent> eqTab;
    std::unique_ptr<juce::TabbedComponent> tabbedComponent;

    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> driveAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> rangeAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> blendAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> volumeAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> reverbAttachment;
    
    // EQ Attachments
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> lowGainAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> lowMidGainAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> midGainAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> highMidGainAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> highGainAttachment;

    DistortionVSTAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DistortionVSTAudioProcessorEditor)
};
