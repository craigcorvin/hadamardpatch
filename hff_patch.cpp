#include "daisy_patch.h"
#include "daisysp.h"
#include "hff.cpp"

using namespace daisy;
using namespace daisysp;

DaisyPatch hw;

Parameter firstKnob, secondKnob, thirdKnob, fourthKnob;

int selected_function = 0;
int previous_selected_function = 0;
int selected_samples = 0;
int previous_selected_samples = 0;
int selected_amplitude = 0;
int selected_feedback = 0;

HadamardFunctionFilter hff;

// The length of the Hadamard Matrix, which is a value that is a power of 2
// 1, 2, 4, 8, 16, 32, 64, 128
#define N_Value 64

void AudioCallback(AudioHandle::InputBuffer in, AudioHandle::OutputBuffer out, size_t size)
{
	hw.ProcessAllControls();

   float first = firstKnob.Process();
   float second = secondKnob.Process();
   float third = thirdKnob.Process();
   float fourth = fourthKnob.Process();

	selected_function = static_cast<int>(first * (N_Value - 1));

	if (selected_function != previous_selected_function) 
	{
		hff.SetCurrentFunction(selected_function);
	}

	selected_samples = (second * N_Value) + 1;

	if (selected_samples != previous_selected_samples)
	{
		hff.SetSamplesPerCell(selected_samples);
	}

	selected_amplitude = static_cast<int>(third * 100.0f);

	hff.SetAmplitude(third);

	selected_feedback = static_cast<int>(fourth * 100.0f);

	hff.SetFeedback(fourth);

	for (size_t i = 0; i < size; i++)
	{
		float output = hff.Process(in[0][i]);

		out[0][i] = out[1][i] = output;
	}
}

void UpdateOled();

int main(void)
{
	hw.Init();

	hff.Init(N_Value);

	firstKnob.Init(hw.controls[0], 0.0, 1.0f, Parameter::LINEAR);
	secondKnob.Init(hw.controls[1], 0.0, 1.0f, Parameter::LINEAR);
	thirdKnob.Init(hw.controls[2], 0.0, 3.0f, Parameter::LINEAR);
	fourthKnob.Init(hw.controls[3], 0.0, 1.0f, Parameter::LINEAR);

	hw.StartAdc();
	hw.StartAudio(AudioCallback);

	while(1) {
		UpdateOled();
	}
}

void UpdateOled()
{
	hw.display.Fill(false);
	hw.display.SetCursor(0, 0);
	std::string str  = "Sequency: ";
	char *      cstr = &str[0];
	hw.display.WriteString(cstr, Font_7x10, true);
	
	hw.display.SetCursor(75, 0);
	str = std::to_string(selected_function);
	hw.display.WriteString(cstr, Font_7x10, true);
	
	hw.display.SetCursor(0, 15);
	str = "Samples: ";
	hw.display.WriteString(cstr, Font_7x10, true);
	
	hw.display.SetCursor(75, 15);
	str = std::to_string(selected_samples);;
	hw.display.WriteString(cstr, Font_7x10, true);
	
	hw.display.SetCursor(0, 30);
	str = "Amplitude: ";
	hw.display.WriteString(cstr, Font_7x10, true);
	
	hw.display.SetCursor(75, 30);
	str = std::to_string(selected_amplitude);
	hw.display.WriteString(cstr, Font_7x10, true);
	
	hw.display.SetCursor(0, 45);
	str = "Feedback: ";
	hw.display.WriteString(cstr, Font_7x10, true);
	
	hw.display.SetCursor(75, 45);
	str = std::to_string(selected_feedback);
	hw.display.WriteString(cstr, Font_7x10, true);
	
	hw.display.Update();
}