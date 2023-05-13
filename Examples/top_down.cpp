#include "MainLoop.h"
#include "Core/TopDown.h"
#include <iostream>

using namespace std;
int main()
{
	MainLoop::Get()->SetRunCondition([]() {return Acousent::isRunning(); });
	MainLoop::Get()->AddToOnBegin([]() {Acousent::processMapChanges(); Acousent::init(); Acousent::initializeDearImGui();  });
	MainLoop::Get()->AddToOnUpdate([](float dt) {Acousent::processPlayer(dt); /*TTD::processAI(dt);*/ });
	MainLoop::Get()->AddToOnPostUpdate([]() {Acousent::renderScene(); });
	return MainLoop::Get()->Run();
}

