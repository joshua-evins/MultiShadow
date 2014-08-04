#include "OldDebugMenu.h"
#include <QtGui\qboxlayout>
#include <QtGui\qlabel.h>
#include "Slider.h"
#include <QtGui\qcheckbox.h>
#include "Renderer.h"

namespace Engine
{
	DebugMenu DebugMenu::theInstance;

#pragma region Constructors and Initializers
	// singleton, no constructors
	
	void DebugMenu::initialize(QVBoxLayout* debugLayout, Renderer* renderer)
	{
#ifdef ENGINE_DEBUG
		targetRenderer = renderer;
		targetRenderer->debug_setCamera(&debugCamera);
		mainLayout = debugLayout;
		setCameraToRendererCamera();
#endif
	}
#pragma endregion

#pragma region Public Functions

	void DebugMenu::controlBool(const char* description, bool* theBool)
	{
#ifdef ENGINE_DEBUG
		QHBoxLayout* thisLayout;
		mainLayout->addLayout(thisLayout = new QHBoxLayout);
		thisLayout->addWidget(new QLabel(description));
		BoolControlInfo& w = boolControllers[nextBoolController++];
		thisLayout->addWidget(w.theCheckbox = new QCheckBox);
		w.theBool = theBool;
#endif
	}

	void DebugMenu::slideFloat(const char* description, float* theFloat)
	{
#ifdef ENGINE_DEBUG
		QHBoxLayout* thisLayout;
		mainLayout->addLayout(thisLayout = new QHBoxLayout);
		thisLayout->addWidget(new QLabel(description));
		FloatWatchInfo& w = floatWatchers[nextFloatWatcher++];
		w.slidable = true;
		thisLayout->addWidget(w.theSlider = new Neumont::Slider);
		w.theFloat = theFloat;
#endif
	}

	void DebugMenu::slideFloat(const char* description, float* theFloat, float min, float max)
	{
#ifdef ENGINE_DEBUG
		QHBoxLayout* thisLayout;
		mainLayout->addLayout(thisLayout = new QHBoxLayout);
		thisLayout->addWidget(new QLabel(description));
		FloatWatchInfo& w = floatWatchers[nextFloatWatcher++];
		w.slidable = true;
		thisLayout->addWidget(w.theSlider = new Neumont::Slider(min, max));
		w.theFloat = theFloat;
#endif
	}

	void DebugMenu::watchFloat(const char* description, const float* theFloat)
	{
#ifdef ENGINE_DEBUG
		QHBoxLayout* thisLayout;
		mainLayout->addLayout(thisLayout = new QHBoxLayout);
		thisLayout->addWidget(new QLabel(description));
		FloatWatchInfo& w = floatWatchers[nextFloatWatcher++];
		thisLayout->addWidget(w.theLabel = new QLabel);
		w.theFloat = const_cast<float*>(theFloat);
#endif
	}

	void DebugMenu::setCameraToRendererCamera()
	{
#ifdef ENGINE_DEBUG
		Camera& c = targetRenderer->camera;
		debugCamera.initialize(c.position, c.facing, c.up);
#endif
	}
	
	void DebugMenu::update()
	{
#ifdef ENGINE_DEBUG
		for(unsigned int i=0; i<nextFloatWatcher; i++)
		{
			const FloatWatchInfo&f = floatWatchers[i];
			
			if(f.slidable)
			{
				*(f.theFloat) = f.theSlider->value();
			}
			else
			{
				QString temp;
				temp.setNum(*(f.theFloat));
				f.theLabel->setText(temp);
			}
		}

		for(unsigned int i=0; i<nextBoolController; i++)
		{
			const BoolControlInfo& b = boolControllers[i];
			*(b.theBool) = b.theCheckbox->isChecked();	
		}
#endif
	}
#pragma endregion
}