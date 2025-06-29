#include "systemclass.h"

int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow) {
	SystemClass* System;
	bool result;

	//makes the system object
	System = new SystemClass;

	//Initalize and runs the system object
	result = System->Initialize();
	if (result) {
		System->Run();
	}

	//Shutdown and release the System Object
	System->Shutdown();
	delete System;
	System = 0;

	return 0;
}