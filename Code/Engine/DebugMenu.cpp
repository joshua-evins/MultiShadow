#include "DebugMenu.h"
#ifdef ENGINE_DEBUG
#include <QtGui\qboxlayout>
#include <QtGui\qlabel.h>
#include "Slider.h"
#include <QtGui\qcheckbox.h>
#include <QtGui\qapplication.h>

namespace Engine
{
	DebugMenu::DebugMenu()
	{
		nextFloatWatcher = 0;
		nextBoolController = 0;
		autoHide = false;
		QWidget* widget;
		setWidget(widget=new QWidget);
		widget->setLayout(mainLayout = new QVBoxLayout);
		startTimer(10);
		mainLayout->addLayout(currentLayout = new QHBoxLayout);
	}

	void DebugMenu::timerEvent(QTimerEvent*)
	{
		update();
	}

	void DebugMenu::controlBool(const char* description, bool* theBool)
	{
		currentLayout->addWidget(new QLabel(description));
		BoolControlInfo& w = boolControllers[nextBoolController++];
		w.theBool = theBool;
		*w.theBool = true;
		currentLayout->addWidget(w.theCheckbox = new QCheckBox);
		w.theCheckbox->setChecked(true);
	}

	void DebugMenu::slideFloat(const char* description, float* theFloat)
	{
		currentLayout->addWidget(new QLabel(description));
		FloatWatchInfo& w = floatWatchers[nextFloatWatcher++];
		w.theFloat = theFloat;
		w.slidable = true;
		currentLayout->addWidget(w.theSlider = new Neumont::Slider);
		w.theSlider->setValue(1.0f);
	}

	void DebugMenu::slideFloat(const char* description, float* theFloat, float min, float max)
	{
		currentLayout->addWidget(new QLabel(description));
		FloatWatchInfo& w = floatWatchers[nextFloatWatcher++];
		w.theFloat = theFloat;
		w.slidable = true;
		currentLayout->addWidget(w.theSlider = new Neumont::Slider(min, max));
		w.theSlider->setValue(*theFloat);
	}

	void DebugMenu::slideVector(const char* description, const float* theVector, int numVectorComponents, float min, float max)
	{
		currentLayout->addWidget(new QLabel(description));
		
		for(int i=0; i<numVectorComponents; i++)
		{
			FloatWatchInfo& w = floatWatchers[nextFloatWatcher++];
			w.slidable = true;
			currentLayout->addWidget(w.theSlider = new Neumont::Slider(min, max));
			w.theFloat = const_cast<float*>(theVector + i);
			w.theSlider->setValue(*w.theFloat);
		}
	}

	void DebugMenu::watchFloat(const char* description, const float* theFloat)
	{
		currentLayout->addWidget(new QLabel(description));
		FloatWatchInfo& w = floatWatchers[nextFloatWatcher++];
		currentLayout->addWidget(w.theLabel = new QLabel);
		w.theFloat = const_cast<float*>(theFloat);
		w.slidable = false;
	}

	void DebugMenu::newRowWidget()
	{
		mainLayout->addLayout(currentLayout = new QHBoxLayout);
	}

	void DebugMenu::update()
	{
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

		resizeByMousePosition();
	}

	void DebugMenu::setAutoHide(bool newValue)
	{
		autoHide = newValue;
	}

	void DebugMenu::resizeByMousePosition()
	{
		if(autoHide)
		{
			QPoint relMousePos( mapFromGlobal(QCursor::pos()) );
			if(relMousePos.y() < height())
				widget()->show();
			else if(relMousePos.y() > height() && !(QApplication::mouseButtons() & 1))
				widget()->hide();
		}
	}
}
#endif