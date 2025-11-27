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
    //images



    //Drive Knob
    driveKnob = std::make_unique<Slider>("Drive");
    driveKnob->setSliderStyle(Slider::Rotary);
    driveKnob->setTextBoxStyle(Slider::NoTextBox, false, 50, 50);
    addAndMakeVisible(*driveKnob);

    //Range Knob
    rangeKnob = std::make_unique<Slider>("Range");
    rangeKnob->setSliderStyle(Slider::Rotary);
    rangeKnob->setRange(0.0f, 1.0f);
    rangeKnob->setTextBoxStyle(Slider::NoTextBox, false, 50,50);
    addAndMakeVisible(*rangeKnob);

    //Blend Knob
    blendKnob = std::make_unique<Slider>("Blend");
    blendKnob->setSliderStyle(Slider::Rotary);
    blendKnob->setTextBoxStyle(Slider::NoTextBox, false, 50,50);
    addAndMakeVisible(*blendKnob);

    //Volume Knob
    volumeKnob = std::make_unique<Slider>("Volume");
    volumeKnob->setSliderStyle(Slider::Rotary);
    volumeKnob->setTextBoxStyle(Slider::NoTextBox, false, 50, 50);
    addAndMakeVisible(*volumeKnob);

    // Attachments
    driveAttachment = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(p.getState(), "drive", *driveKnob);
    rangeAttachment = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(p.getState(), "range", *rangeKnob);
    blendAttachment = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(p.getState(), "blend", *blendKnob);
    volumeAttachment = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(p.getState(), "volume", *volumeKnob);



    //Canvus
    setSize (500 , 200);
}

DistortionVSTAudioProcessorEditor::~DistortionVSTAudioProcessorEditor()
{
}

//==============================================================================
void DistortionVSTAudioProcessorEditor::paint (Graphics& g)
{

    g.fillAll(getLookAndFeel().findColour(ResizableWindow::backgroundColourId));

    g.setColour(Colours::white);
    g.setFont(15.0f);
   
    g.drawText("Drive", ((getWidth() / 5) * 1) - (100 / 2), (getHeight() / 2) +5, 100 , 100, Justification::centred, false );
    g.drawText("Range", ((getWidth() / 5) * 2) - (100 / 2), (getHeight() / 2) + 5, 100, 100, Justification::centred, false);
    g.drawText("Blend", ((getWidth() / 5) * 3) - (100 / 2), (getHeight() / 2) + 5, 100, 100, Justification::centred, false);
    g.drawText("Volume", ((getWidth() / 5) * 4) - (100 / 2), (getHeight() / 2) + 5, 100, 100, Justification::centred, false);


}

void DistortionVSTAudioProcessorEditor::resized()
{
    driveKnob->setBounds(((getWidth() / 5) * 1) - (100 / 2), (getHeight()/2) - (100/2), 100,100);
    rangeKnob->setBounds(((getWidth() / 5) * 2) - (100 / 2), (getHeight() / 2) - (100 / 2), 100, 100);
    blendKnob->setBounds(((getWidth() / 5) * 3) - (100 / 2), (getHeight() / 2) - (100 / 2), 100, 100);
    volumeKnob->setBounds(((getWidth() / 5) * 4) - (100 / 2), (getHeight() / 2) - (100 / 2), 100, 100);

}
