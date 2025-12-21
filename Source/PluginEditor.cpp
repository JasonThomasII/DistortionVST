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
    noiseGateTab = std::make_unique<NoiseGateTabComponent>();
    
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
    tabbedComponent->addTab("Noise Gate", juce::Colours::darkgrey, noiseGateTab.get(), false);
    addAndMakeVisible(*tabbedComponent);

    // === DISTORTION TAB SETUP ===
    
    // Distortion Type ComboBox
    distortionTypeCombo = std::make_unique<ComboBox>("Distortion Type");
    distortionTypeCombo->addItem("Atan", 1);
    distortionTypeCombo->addItem("Tanh", 2);
    distortionTypeCombo->addItem("Soft Knee", 3);
    distortionTypeCombo->addItem("Asymmetric", 4);
    distortionTypeCombo->setSelectedItemIndex(0);
    distortionTypeCombo->setColour(ComboBox::backgroundColourId, Colours::darkgrey);
    distortionTypeCombo->setColour(ComboBox::textColourId, Colours::white);
    distortionTab->addAndMakeVisible(*distortionTypeCombo);
    
    // Oversampling Rate ComboBox
    oversamplingRateCombo = std::make_unique<ComboBox>("Oversampling Rate");
    oversamplingRateCombo->addItem("None (1x)", 1);
    oversamplingRateCombo->addItem("2x", 2);
    oversamplingRateCombo->addItem("4x", 3);
    oversamplingRateCombo->addItem("8x", 4);
    oversamplingRateCombo->setSelectedItemIndex(2);  // Default to 4x
    oversamplingRateCombo->setColour(ComboBox::backgroundColourId, Colours::darkgrey);
    oversamplingRateCombo->setColour(ComboBox::textColourId, Colours::white);
    distortionTab->addAndMakeVisible(*oversamplingRateCombo);
    
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
    lowGainSlider->setTextBoxStyle(Slider::TextBoxBelow, false, 50, 20);
    lowGainSlider->setRange(-12.0f, 12.0f, 0.1f);
    eqTab->addAndMakeVisible(*lowGainSlider);
    
    lowMidGainSlider = std::make_unique<Slider>("Low Mid Gain");
    lowMidGainSlider->setSliderStyle(Slider::LinearVertical);
    lowMidGainSlider->setTextBoxStyle(Slider::TextBoxBelow, false, 50, 20);
    lowMidGainSlider->setRange(-12.0f, 12.0f, 0.1f);
    eqTab->addAndMakeVisible(*lowMidGainSlider);
    
    midGainSlider = std::make_unique<Slider>("Mid Gain");
    midGainSlider->setSliderStyle(Slider::LinearVertical);
    midGainSlider->setTextBoxStyle(Slider::TextBoxBelow, false, 50, 20);
    midGainSlider->setRange(-12.0f, 12.0f, 0.1f);
    eqTab->addAndMakeVisible(*midGainSlider);
    
    highMidGainSlider = std::make_unique<Slider>("High Mid Gain");
    highMidGainSlider->setSliderStyle(Slider::LinearVertical);
    highMidGainSlider->setTextBoxStyle(Slider::TextBoxBelow, false, 50, 20);
    highMidGainSlider->setRange(-12.0f, 12.0f, 0.1f);
    eqTab->addAndMakeVisible(*highMidGainSlider);
    
    highGainSlider = std::make_unique<Slider>("High Gain");
    highGainSlider->setSliderStyle(Slider::LinearVertical);
    highGainSlider->setTextBoxStyle(Slider::TextBoxBelow, false, 50, 20);
    highGainSlider->setRange(-12.0f, 12.0f, 0.1f);
    eqTab->addAndMakeVisible(*highGainSlider);

    // === NOISE GATE TAB SETUP ===
    
    // Gate Enable Toggle Button
    gateEnabledButton = std::make_unique<ToggleButton>("Enable");
    noiseGateTab->addAndMakeVisible(*gateEnabledButton);
    
    // Gate Threshold Slider
    gateThresholdSlider = std::make_unique<Slider>("Gate Threshold");
    gateThresholdSlider->setSliderStyle(Slider::LinearHorizontal);
    gateThresholdSlider->setTextBoxStyle(Slider::TextBoxBelow, false, 50, 20);
    gateThresholdSlider->setRange(-80.0f, 0.0f, 0.1f);
    noiseGateTab->addAndMakeVisible(*gateThresholdSlider);
    
    // Gate Attack Slider
    gateAttackSlider = std::make_unique<Slider>("Gate Attack");
    gateAttackSlider->setSliderStyle(Slider::LinearHorizontal);
    gateAttackSlider->setTextBoxStyle(Slider::TextBoxBelow, false, 50, 20);
    gateAttackSlider->setRange(0.1f, 100.0f, 0.1f);
    noiseGateTab->addAndMakeVisible(*gateAttackSlider);
    
    // Gate Release Slider
    gateReleaseSlider = std::make_unique<Slider>("Gate Release");
    gateReleaseSlider->setSliderStyle(Slider::LinearHorizontal);
    gateReleaseSlider->setTextBoxStyle(Slider::TextBoxBelow, false, 50, 20);
    gateReleaseSlider->setRange(10.0f, 1000.0f, 1.0f);
    noiseGateTab->addAndMakeVisible(*gateReleaseSlider);

    // Attachments
    driveAttachment = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(p.getState(), "drive", *driveKnob);
    rangeAttachment = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(p.getState(), "range", *rangeKnob);
    blendAttachment = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(p.getState(), "blend", *blendKnob);
    volumeAttachment = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(p.getState(), "volume", *volumeKnob);
    reverbAttachment = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(p.getState(), "reverb", *reverbKnob);
    
    // Distortion Type Attachment
    distortionTypeAttachment = std::make_unique<AudioProcessorValueTreeState::ComboBoxAttachment>(p.getState(), "distortionType", *distortionTypeCombo);
    
    // Oversampling Rate Attachment
    oversamplingRateAttachment = std::make_unique<AudioProcessorValueTreeState::ComboBoxAttachment>(p.getState(), "oversamplingRate", *oversamplingRateCombo);
    
    // EQ Attachments
    lowGainAttachment = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(p.getState(), "lowGain", *lowGainSlider);
    lowMidGainAttachment = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(p.getState(), "lowMidGain", *lowMidGainSlider);
    midGainAttachment = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(p.getState(), "midGain", *midGainSlider);
    highMidGainAttachment = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(p.getState(), "highMidGain", *highMidGainSlider);
    highGainAttachment = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(p.getState(), "highGain", *highGainSlider);
    
    // Noise Gate Attachments
    gateEnabledAttachment = std::make_unique<AudioProcessorValueTreeState::ButtonAttachment>(p.getState(), "gateEnabled", *gateEnabledButton);
    gateThresholdAttachment = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(p.getState(), "gateThreshold", *gateThresholdSlider);
    gateAttackAttachment = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(p.getState(), "gateAttack", *gateAttackSlider);
    gateReleaseAttachment = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(p.getState(), "gateRelease", *gateReleaseSlider);

    //Canvas
    setSize (500, 400);
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
    // Position distortion type selector
    distortionTypeCombo->setBounds(contentBounds.withX(20).withY(60).withWidth(200).withHeight(25));
    
    // Position oversampling rate selector to the right of distortion type
    oversamplingRateCombo->setBounds(contentBounds.withX(240).withY(60).withWidth(150).withHeight(25));
    
    // Position knobs on distortion tab
    driveKnob->setBounds(contentBounds.withX(20).withY(110).withWidth(100).withHeight(100));
    rangeKnob->setBounds(contentBounds.withX(140).withY(110).withWidth(100).withHeight(100));
    blendKnob->setBounds(contentBounds.withX(260).withY(110).withWidth(100).withHeight(100));
    volumeKnob->setBounds(contentBounds.withX(80).withY(240).withWidth(100).withHeight(100));
    reverbKnob->setBounds(contentBounds.withX(240).withY(240).withWidth(100).withHeight(100));
    
    // === EQ TAB LAYOUT ===
    // Position EQ sliders on EQ tab with better spacing (taller to accommodate text boxes)
    lowGainSlider->setBounds(contentBounds.withX(20).withY(90).withWidth(40).withHeight(120));
    lowMidGainSlider->setBounds(contentBounds.withX(90).withY(90).withWidth(40).withHeight(120));
    midGainSlider->setBounds(contentBounds.withX(160).withY(90).withWidth(40).withHeight(120));
    highMidGainSlider->setBounds(contentBounds.withX(230).withY(90).withWidth(40).withHeight(120));
    highGainSlider->setBounds(contentBounds.withX(300).withY(90).withWidth(40).withHeight(120));
    
    // === NOISE GATE TAB LAYOUT ===
    // Position noise gate controls
    gateEnabledButton->setBounds(contentBounds.withX(70).withY(50).withWidth(150).withHeight(30));
    gateThresholdSlider->setBounds(contentBounds.withX(50).withY(170).withWidth(250).withHeight(60));
    gateAttackSlider->setBounds(contentBounds.withX(50).withY(230).withWidth(250).withHeight(60));
    gateReleaseSlider->setBounds(contentBounds.withX(50).withY(290).withWidth(250).withHeight(60));
}
