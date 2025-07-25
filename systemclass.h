#ifndef _SYSTEMCLASS_H_
#define _SYSTEMCLASS_H_

//Pre-processing directives
#define WIN32_LEAN_AND_MEAN

//Includes
#include <Windows.h>

//My Class Includes
#include "inputclass.h"
#include "applicationclass.h"

//Class definition
class SystemClass {
public:
	SystemClass();
	SystemClass(const SystemClass&);
	~SystemClass();

	bool Initialize();
	void Shutdown();
	void Run();

	LRESULT CALLBACK MessageHandler(HWND, UINT, WPARAM, LPARAM);

private:
	bool Frame();
	void InitializeWindows(int&, int&);
	void ShutdownWindows();

private:
	LPCWSTR m_applicationName;
	HINSTANCE m_hinstance;
	HWND m_hwnd;

	InputClass* m_Input;
	ApplicationClass* m_Application;
};

//Function Prototypes
static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

//GLobals
static SystemClass* ApplicationHandle = 0;

#endif