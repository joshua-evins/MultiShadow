#ifndef DEBUGMENU_H
#define DEBUGMENU_H
#include "typedefs_Engine.h"
#include "Camera.h"
#include "ExportHeader.h"

#pragma region Forward Declarations
	namespace Neumont{class Slider;}
	class QVBoxLayout;
	class QLabel;
	class QCheckBox;
	namespace Engine{class Renderer;}
#pragma endregion

namespace Engine
{
	class ENGINE_SHARED DebugMenu
	{
		//singleton class
		static DebugMenu theInstance;

	#pragma region Instance Variables
		private:
			Camera debugCamera;
			Renderer* targetRenderer;

			QVBoxLayout* mainLayout;
			unsigned int nextFloatWatcher;
			unsigned int nextBoolController;
	#pragma endregion

	#pragma region Constructors and Initializers
		private:
			DebugMenu() : nextFloatWatcher(0), nextBoolController(0) {}
			DebugMenu(const DebugMenu&);
			DebugMenu& operator=(const DebugMenu&);
		public:
			void initialize(QVBoxLayout* debugLayout, Renderer* renderer);
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
			static DebugMenu& getInstance() {return theInstance;}

			void controlBool(const char* description, bool* theBool);
			void slideFloat(const char* description, float* theFloat);
			void slideFloat(const char* description, float* theFloat, float min, float max);
			void watchFloat(const char* description, const float* theFloat);
		
			void setCameraToRendererCamera();
			void update();
	#pragma endregion

	};
}
#endif