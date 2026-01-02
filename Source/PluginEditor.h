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
        g.setFont(11.0f);
        g.drawText("Distortion Type", 20, 30, 200, 40, Justification::centredLeft, false);
        g.drawText("Oversampling", 240, 30, 150, 40, Justification::centredLeft, false);
        g.drawText("Drive", 20, 190, 100, 20, Justification::centred, false);
        g.drawText("Range", 140, 190, 100, 20, Justification::centred, false);
        g.drawText("Blend", 260, 190, 100, 20, Justification::centred, false);
        g.drawText("Volume", 80, 320, 100, 20, Justification::centred, false);
        g.drawText("Reverb", 240, 320, 100, 20, Justification::centred, false);
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
        g.drawText("60Hz", 20, 220, 40, 20, Justification::centred, false);
        g.drawText("250Hz", 90, 220, 40, 20, Justification::centred, false);
        g.drawText("1kHz", 160, 220, 40, 20, Justification::centred, false);
        g.drawText("4kHz", 230, 220, 40, 20, Justification::centred, false);
        g.drawText("16kHz", 300, 220, 40, 20, Justification::centred, false);
        
        // Legend on the left showing dB scale
        g.setFont(10.0f);
        g.drawText("+12dB", 2, 80, 12, 15, Justification::centred, false);
        g.drawText("0dB", 2, 145, 12, 15, Justification::centred, false);
        g.drawText("-12dB", 2, 210, 12, 15, Justification::centred, false);
        
        // Draw reference lines on the legend
        g.setColour(Colours::grey);
        g.drawLine(15, 88, 18, 88, 1.0f);   // +12dB line
        g.drawLine(15, 153, 18, 153, 1.0f); // 0dB line
        g.drawLine(15, 218, 18, 218, 1.0f); // -12dB line
    }
    
    void resized() override
    {
        resetButton.setBounds(380, 80, 80, 40);
    }
    
    TextButton resetButton;
};

// Custom tab component for Noise Gate
class NoiseGateTabComponent : public Component
{
public:
    void paint(Graphics& g) override
    {
        g.fillAll(getLookAndFeel().findColour(ResizableWindow::backgroundColourId));
        g.setColour(Colours::white);
        g.setFont(14.0f);
        g.drawText("Gate Enable", 20, 30, 150, 25, Justification::centred, false);
        g.drawText("Threshold", 20, 130, 150, 25, Justification::centred, false);
        g.drawText("Attack (ms)", 20, 150, 150, 25, Justification::centred, false);
        g.drawText("Release (ms)", 20, 210, 150, 25, Justification::centred, false);
    }
};

// Custom tab component for Impulse Response
class IRTabComponent : public Component
{
public:
    IRTabComponent() : irCombo("IR Selection"), irMixSlider("IR Mix")
    {
        irCombo.setColour(ComboBox::backgroundColourId, Colours::darkgrey);
        irCombo.setColour(ComboBox::textColourId, Colours::white);
        addAndMakeVisible(irCombo);
        
        irMixSlider.setColour(Slider::textBoxOutlineColourId, Colours::grey);
        addAndMakeVisible(irMixSlider);
    }
    
    void paint(Graphics& g) override
    {
        g.fillAll(getLookAndFeel().findColour(ResizableWindow::backgroundColourId));
        g.setColour(Colours::white);
        g.setFont(12.0f);
        g.drawText("Impulse Response", 20, 30, 200, 25, Justification::centredLeft, false);
        
        // Draw folder path info
        g.setFont(9.0f);
        g.setColour(Colours::lightgrey);
        g.drawText("IRs folder: ~/Documents/DistortionVST/IRs", 20, 70, 450, 20, Justification::centredLeft, false);
        
        // Draw dropdown label
        g.setColour(Colours::white);
        g.setFont(11.0f);
        g.drawText("Select IR:", 20, 110, 100, 20, Justification::centredLeft, false);
        
        // Draw IR mix label
        g.setFont(11.0f);
        g.drawText("IR Mix", 20, 200, 100, 20, Justification::centredLeft, false);
    }
    
    void resized() override
    {
        irCombo.setBounds(130, 105, 370, 30);
        irMixSlider.setBounds(20, 230, 450, 60);
    }
    
    ComboBox irCombo;
    Slider irMixSlider;

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
    
    // Distortion Type Selector
    std::unique_ptr<ComboBox> distortionTypeCombo;
    
    // Oversampling Rate Selector
    std::unique_ptr<ComboBox> oversamplingRateCombo;
    
    // EQ Sliders
    std::unique_ptr<Slider> lowGainSlider;
    std::unique_ptr<Slider> lowMidGainSlider;
    std::unique_ptr<Slider> midGainSlider;
    std::unique_ptr<Slider> highMidGainSlider;
    std::unique_ptr<Slider> highGainSlider;
    
    // Noise Gate Controls
    std::unique_ptr<ToggleButton> gateEnabledButton;
    std::unique_ptr<Slider> gateThresholdSlider;
    std::unique_ptr<Slider> gateAttackSlider;
    std::unique_ptr<Slider> gateReleaseSlider;
    
    // Tabbed interface components
    std::unique_ptr<DistortionTabComponent> distortionTab;
    std::unique_ptr<EQTabComponent> eqTab;
    std::unique_ptr<NoiseGateTabComponent> noiseGateTab;
    std::unique_ptr<IRTabComponent> irTab;
    std::unique_ptr<juce::TabbedComponent> tabbedComponent;

    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> driveAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> rangeAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> blendAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> volumeAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> reverbAttachment;
    
    // Distortion Type Attachment
    std::unique_ptr<AudioProcessorValueTreeState::ComboBoxAttachment> distortionTypeAttachment;
    
    // Oversampling Rate Attachment
    std::unique_ptr<AudioProcessorValueTreeState::ComboBoxAttachment> oversamplingRateAttachment;
    
    // EQ Attachments
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> lowGainAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> lowMidGainAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> midGainAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> highMidGainAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> highGainAttachment;
    
    // Noise Gate Attachments
    std::unique_ptr<AudioProcessorValueTreeState::ButtonAttachment> gateEnabledAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> gateThresholdAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> gateAttackAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> gateReleaseAttachment;
    
    // IR Attachments
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> irMixAttachment;

    DistortionVSTAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DistortionVSTAudioProcessorEditor)
};
