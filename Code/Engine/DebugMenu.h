#pragma once
#include "typedefs_Engine.h"
#ifdef ENGINE_DEBUG
#include <QtGui\qdockwidget.h>
#include "ExportHeader.h"
#include <Qt\qevent.h>

#pragma region Forward Declarations
	namespace Neumont{class Slider;}
	class QVBoxLayout;
	class QHBoxLayout;
	class QLabel;
	class QCheckBox;
	namespace Engine{class Renderer;}
#pragma endregion

namespace Engine
{
	class ENGINE_SHARED DebugMenu : public QDockWidget
	{
	#pragma region Instance Variables
		private:
			QVBoxLayout* mainLayout;
			QHBoxLayout* currentLayout;
			unsigned int nextFloatWatcher;
			unsigned int nextBoolController;
			bool autoHide;
	#pragma endregion

	#pragma region Constructors and Initializers
		public:
			DebugMenu();
	#pragma endregion
		
	#pragma region Internal Structs
		private:
			struct FloatWatchInfo
			{
				QLabel* theLabel;
				Neumont::Slider* theSlider;
				float* theFloat;
				bool slidable;
			} floatWatchers[20];
		
			struct BoolControlInfo
			{
				QLabel* theLabel;
				bool* theBool;
				QCheckBox* theCheckbox;
			} boolControllers[10];
	#pragma endregion

	#pragma region Public Functions
		public:
			void controlBool(const char* description, bool* theBool);
			void slideFloat(const char* description, float* theFloat);
			void slideFloat(const char* description, float* theFloat, float min, float max);
			void watchFloat(const char* description, const float* theFloat);
			void slideVector(const char* description, const float* theVector, int numVectorComponents, float min, float max);
			void update();
			void resizeByMousePosition();
			void newRowWidget();
			void setAutoHide(bool newValue);
	#pragma endregion
	protected:
		virtual void timerEvent(QTimerEvent*);
	};
}
#endif