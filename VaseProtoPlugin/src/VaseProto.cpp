#include "plugin.hpp"
#include "daisysp.h"


struct VaseProto : Module {
	enum ParamIds {
		PITCH_PARAM,
		NUM_PARAMS
	};
	enum InputIds {
		PITCH_INPUT,
		NUM_INPUTS
	};
	enum OutputIds {
		SINE_OUTPUT,
		NUM_OUTPUTS
	};
	enum LightIds {
		BLINK_LIGHT,
		NUM_LIGHTS
	};

	VaseProto() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam(PITCH_PARAM, 0.f, 1.f, 0.f, "");
		// TODO: Figure out if there's a way to do SR check during
		//       construction
        //osc.Init(args.sampleRate);

		// Init the DaisySP Oscillator
		osc.Init(44100.f);
        osc.SetAmp(5.0f);
	}

	void process(const ProcessArgs& args) override {
	    // Get pitch -- taken from plugin guide
	    float pitch = params[PITCH_PARAM].getValue();
	    pitch += inputs[PITCH_INPUT].getVoltage();
	    pitch = clamp(pitch, -4.f, 4.f);
	    // Default pitch is C4 - 261.6256Hz
	    float freq = dsp::FREQ_C4 * std::pow(2.f, pitch);

        // Set Freq, and output.
	    osc.SetFreq(freq);
	    outputs[SINE_OUTPUT].setVoltage(osc.Process());

	    blinkPhase += args.sampleTime;
	    if (blinkPhase >= 1.f)
	        blinkPhase -= 1.f;
	    lights[BLINK_LIGHT].setBrightness(blinkPhase < .5f ? 1.f : 0.f);
	}

	daisysp::Oscillator osc;
	float blinkPhase;
};


struct VaseProtoWidget : ModuleWidget {
	VaseProtoWidget(VaseProto* module) {
		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/VaseProto.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(15.24, 46.063)), module, VaseProto::PITCH_PARAM));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(15.24, 77.478)), module, VaseProto::PITCH_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(15.24, 108.713)), module, VaseProto::SINE_OUTPUT));

		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(15.24, 25.81)), module, VaseProto::BLINK_LIGHT));
	}
};


Model* modelVaseProto = createModel<VaseProto, VaseProtoWidget>("VaseProto");
