#include "RendererWindow.h"
#include "Renderer/Renderer.h"
#include <iostream>

namespace Engine
{
	RendererWindow::RendererWindow()
	{
		elapsed = 0.0f;
		numFrames = 1;
		initialized = false;
		setCentralWidget(renderer = new Renderer());
		show();
		
		centralWidget()->setMinimumWidth(800);
		centralWidget()->setMinimumHeight(600);
		centralWidget()->show();

		startTimer(0);
		std::cout
				<< "INSTRUCTIONS FOR JAMIE EVEN THOUGH HE PROBABLY WON'T SEE THEM: " << std::endl
				<< "Press ENTER to start" << std::endl
				<< "WASDRF to move camera" << std::endl
				<< "Hold down right click and move mouse to turn" << std::endl;
	}

	void RendererWindow::timerEvent(QTimerEvent*)
	{
		if(!initialized && GetAsyncKeyState(0x0D)) // ENTER
		{
			initialized = true;
			initialize();
			renderer->initializeSize();
		}
		if(initialized)
		{
			renderer->update();
			
			inheritedUpdate();
			renderer->repaint();
			
			if(false) // set to true for FPS tracking
			{
				if(numFrames > 100)
				{
					elapsed = elapsed / numFrames;
					numFrames = 2;
				}

				elapsed += clock.deltaTime();
				if(numFrames%10 == 0)
					std::cout << "average fps: " << 1.0f / (elapsed / numFrames) << std::endl;
				numFrames++;
			}
		}
		clock.frameTick();
	}

	void RendererWindow::initialize()
	{
		
	}

	void RendererWindow::inheritedUpdate()
	{

	}
}