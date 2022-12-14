#include "MainLoop.h"
#include "TopDown.h"
int main()
{
	MainLoop::Get()->SetRunCondition([]() {return Acousent::isRunning(); });
	MainLoop::Get()->AddToOnBegin([]() {Acousent::processMapChanges(); Acousent::init(); });
	MainLoop::Get()->AddToOnUpdate([](float dt) {Acousent::processPlayer(dt); /*TTD::processAI(dt);*/ });
	MainLoop::Get()->AddToOnPostUpdate([]() {Acousent::renderScene(); });
	return MainLoop::Get()->Run();
}

