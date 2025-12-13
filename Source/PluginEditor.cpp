/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
using namespace juce;

//==============================================================================
DistortionVSTAudioProcessorEditor::DistortionVSTAudioProcessorEditor (DistortionVSTAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{   
    // Create custom tab components
    distortionTab = std::make_unique<DistortionTabComponent>();
    eqTab = std::make_unique<EQTabComponent>();
    
    // Add reset button listener
    eqTab->resetButton.onClick = [this]()
    {
        // Reset all EQ sliders to 0 (midpoint)
        audioProcessor.getState().getParameter("lowGain")->setValueNotifyingHost(0.5f);
        audioProcessor.getState().getParameter("lowMidGain")->setValueNotifyingHost(0.5f);
        audioProcessor.getState().getParameter("midGain")->setValueNotifyingHost(0.5f);
        audioProcessor.getState().getParameter("highMidGain")->setValueNotifyingHost(0.5f);
        audioProcessor.getState().getParameter("highGain")->setValueNotifyingHost(0.5f);
    };
    
    // Create tabbed component
    tabbedComponent = std::make_unique<juce::TabbedComponent>(juce::TabbedButtonBar::TabsAtTop);
    tabbedComponent->addTab("Distortion", juce::Colours::darkgrey, distortionTab.get(), false);
    tabbedComponent->addTab("EQ", juce::Colours::darkgrey, eqTab.get(), false);
    addAndMakeVisible(*tabbedComponent);

    // === DISTORTION TAB SETUP ===
    
    //Drive Knob
    driveKnob = std::make_unique<Slider>("Drive");
    driveKnob->setSliderStyle(Slider::Rotary);
    driveKnob->setTextBoxStyle(Slider::NoTextBox, false, 50, 50);
    distortionTab->addAndMakeVisible(*driveKnob);

    //Range Knob
    rangeKnob = std::make_unique<Slider>("Range");
    rangeKnob->setSliderStyle(Slider::Rotary);
    rangeKnob->setRange(0.0f, 1.0f);
    rangeKnob->setTextBoxStyle(Slider::NoTextBox, false, 50,50);
    distortionTab->addAndMakeVisible(*rangeKnob);

    //Blend Knob
    blendKnob = std::make_unique<Slider>("Blend");
    blendKnob->setSliderStyle(Slider::Rotary);
    blendKnob->setTextBoxStyle(Slider::NoTextBox, false, 50,50);
    distortionTab->addAndMakeVisible(*blendKnob);

    //Volume Knob
    volumeKnob = std::make_unique<Slider>("Volume");
    volumeKnob->setSliderStyle(Slider::Rotary);
    volumeKnob->setTextBoxStyle(Slider::NoTextBox, false, 50, 50);
    distortionTab->addAndMakeVisible(*volumeKnob);

    //Reverb Knob
    reverbKnob = std::make_unique<Slider>("Reverb");
    reverbKnob->setSliderStyle(Slider::Rotary);
    reverbKnob->setTextBoxStyle(Slider::NoTextBox, false, 50, 50);
    distortionTab->addAndMakeVisible(*reverbKnob);
    
    // === EQ TAB SETUP ===
    
    // EQ Sliders (5-Band EQ)
    lowGainSlider = std::make_unique<Slider>("Low Gain");
    lowGainSlider->setSliderStyle(Slider::LinearVertical);
    lowGainSlider->setTextBoxStyle(Slider::NoTextBox, false, 50, 50);
    lowGainSlider->setRange(-12.0f, 12.0f, 0.1f);
    eqTab->addAndMakeVisible(*lowGainSlider);
    
    lowMidGainSlider = std::make_unique<Slider>("Low Mid Gain");
    lowMidGainSlider->setSliderStyle(Slider::LinearVertical);
    lowMidGainSlider->setTextBoxStyle(Slider::NoTextBox, false, 50, 50);
    lowMidGainSlider->setRange(-12.0f, 12.0f, 0.1f);
    eqTab->addAndMakeVisible(*lowMidGainSlider);
    
    midGainSlider = std::make_unique<Slider>("Mid Gain");
    midGainSlider->setSliderStyle(Slider::LinearVertical);
    midGainSlider->setTextBoxStyle(Slider::NoTextBox, false, 50, 50);
    midGainSlider->setRange(-12.0f, 12.0f, 0.1f);
    eqTab->addAndMakeVisible(*midGainSlider);
    
    highMidGainSlider = std::make_unique<Slider>("High Mid Gain");
    highMidGainSlider->setSliderStyle(Slider::LinearVertical);
    highMidGainSlider->setTextBoxStyle(Slider::NoTextBox, false, 50, 50);
    highMidGainSlider->setRange(-12.0f, 12.0f, 0.1f);
    eqTab->addAndMakeVisible(*highMidGainSlider);
    
    highGainSlider = std::make_unique<Slider>("High Gain");
    highGainSlider->setSliderStyle(Slider::LinearVertical);
    highGainSlider->setTextBoxStyle(Slider::NoTextBox, false, 50, 50);
    highGainSlider->setRange(-12.0f, 12.0f, 0.1f);
    eqTab->addAndMakeVisible(*highGainSlider);

    // Attachments
    driveAttachment = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(p.getState(), "drive", *driveKnob);
    rangeAttachment = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(p.getState(), "range", *rangeKnob);
    blendAttachment = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(p.getState(), "blend", *blendKnob);
    volumeAttachment = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(p.getState(), "volume", *volumeKnob);
    reverbAttachment = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(p.getState(), "reverb", *reverbKnob);
    
    // EQ Attachments
    lowGainAttachment = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(p.getState(), "lowGain", *lowGainSlider);
    lowMidGainAttachment = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(p.getState(), "lowMidGain", *lowMidGainSlider);
    midGainAttachment = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(p.getState(), "midGain", *midGainSlider);
    highMidGainAttachment = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(p.getState(), "highMidGain", *highMidGainSlider);
    highGainAttachment = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(p.getState(), "highGain", *highGainSlider);

    //Canvas
    setSize (500 , 350);
}

DistortionVSTAudioProcessorEditor::~DistortionVSTAudioProcessorEditor()
{
}

//==============================================================================
void DistortionVSTAudioProcessorEditor::paint (Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour(ResizableWindow::backgroundColourId));
}

void DistortionVSTAudioProcessorEditor::resized()
{
    // TabbedComponent takes up entire editor area
    tabbedComponent->setBounds(0, 0, getWidth(), getHeight());
    
    // Get the content area bounds (inside the tabs)
    auto contentBounds = tabbedComponent->getLocalBounds();
    
    // === DISTORTION TAB LAYOUT ===
    // Position knobs on distortion tab
    driveKnob->setBounds(contentBounds.withX(20).withY(20).withWidth(100).withHeight(100));
    rangeKnob->setBounds(contentBounds.withX(140).withY(20).withWidth(100).withHeight(100));
    blendKnob->setBounds(contentBounds.withX(260).withY(20).withWidth(100).withHeight(100));
    volumeKnob->setBounds(contentBounds.withX(80).withY(150).withWidth(100).withHeight(100));
    reverbKnob->setBounds(contentBounds.withX(240).withY(150).withWidth(100).withHeight(100));
    
    // === EQ TAB LAYOUT ===
    // Position EQ sliders on EQ tab with better spacing
    lowGainSlider->setBounds(contentBounds.withX(20).withY(40).withWidth(40).withHeight(150));
    lowMidGainSlider->setBounds(contentBounds.withX(90).withY(40).withWidth(40).withHeight(150));
    midGainSlider->setBounds(contentBounds.withX(160).withY(40).withWidth(40).withHeight(150));
    highMidGainSlider->setBounds(contentBounds.withX(230).withY(40).withWidth(40).withHeight(150));
    highGainSlider->setBounds(contentBounds.withX(300).withY(40).withWidth(40).withHeight(150));
}
