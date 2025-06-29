#include "systemclass.h"

//Sets up all outer methods
SystemClass::SystemClass() {
	m_Input = 0;
	m_Application = 0;
}

SystemClass::SystemClass(const SystemClass& other) {
}

SystemClass::~SystemClass() {
}

//Starts the system
bool SystemClass::Initialize() {
	int screenWidth, screenHeight;
	bool result;

	//Initialize the width & height of the screen
	screenWidth = 0;
	screenHeight = 0;

	//Init the windows api
	InitializeWindows(screenWidth, screenHeight);

	//Creates the input object. It will be responsible for handling keyboard input
	m_Input = new InputClass;

	m_Input->Initialize();

	//Creates the application object. It will take care of rendering all the graphics in the app
	m_Application = new ApplicationClass;

	result = m_Application->Initialize(screenWidth, screenHeight, m_hwnd);
	if (!result) {
		return false;
	}
	return true;
}

//Closes the system
void SystemClass::Shutdown() {
	//Releases the application object
	if (m_Application) {
		m_Application->Shutdown();
		delete m_Application;
		m_Application = 0;
	}

	//Releases the input object
	if (m_Input) {
		delete m_Input;
		m_Input = 0;
	}

	//Shutdown the window
	ShutdownWindows();

	return;
}

//Loops the System
void SystemClass::Run() {
	MSG msg;
	bool done, result;

	//Initialize the message structure
	ZeroMemory(&msg, sizeof(MSG));

	//Loop until there is a quit message from the window or the user
	done = false;
	while (!done) {
		//Handle window messages
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessageW(&msg);
		}

		//If window signals to exit app then exit out
		if (msg.message == WM_QUIT) {
			done = true;
		}
		else {
			//Otherwise process the next frame
			result = Frame();
			if (!result) {
				done = true;
			}
		}
	}
	return;
}

//Process for a frame
bool SystemClass::Frame() {
	bool result;

	//Check if the user pressed escape
	if (m_Input->IsKeyDown(VK_ESCAPE)) {
		return false;
	}

	//Do the frame processing for the application object
	result = m_Application->Frame();
	if (!result) {
		return false;
	}
	return true;
}

LRESULT CALLBACK SystemClass::MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam) {
	switch (umsg) {
		//Check if a key has been pressed
		case WM_KEYDOWN: {
			m_Input->KeyDown((unsigned int)wparam);
			return 0;
		}
		//Checks if a key has been released
		case WM_KEYUP: {
			m_Input->KeyUp((unsigned int)wparam);
			return 0;
		}
		default: {
			return DefWindowProc(hwnd, umsg, wparam, lparam);
		}
	}
}

//Initialize the window
void SystemClass::InitializeWindows(int& screenWidth, int& screenHeight) {
	WNDCLASSEX wc;
	DEVMODE dmScreenSettings;
	int posX, posY;

	//Gets an external pointer to this object
	ApplicationHandle = this;

	//Get application instance
	m_hinstance = GetModuleHandle(NULL);

	//Give the application a name
	m_applicationName = L"Engine";

	//Setting up default window settings
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = m_hinstance;
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wc.hIconSm = wc.hIcon;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = m_applicationName;
	wc.cbSize = sizeof(WNDCLASSEX);

	//Register the window class
	RegisterClassEx(&wc);

	//Determine the resolution of the desktop screen
	screenWidth = GetSystemMetrics(SM_CXSCREEN);
	screenHeight = GetSystemMetrics(SM_CXSCREEN);

	//Setup the screen settings depending on whether it is running in full screen or windowed mode
	if (FULL_SCREEN) {
		// if fullscreen set the screen to max size of the desktop & 32bit
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth = (unsigned long)screenWidth;
		dmScreenSettings.dmPelsHeight = (unsigned long)screenHeight;
		dmScreenSettings.dmBitsPerPel = 32;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		//change the display settings to fullscreen
		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

		//set the position of the window to the top left corner
		posX = posY = 0;
	}
	else {
		//if windowed its in 800x600
		screenWidth = 800;
		screenHeight = 600;

		//Place the window in the middle of the screen
		posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
		posY = (GetSystemMetrics(SM_CXSCREEN) - screenHeight) / 5;
	}

	//Create the window with the screen settings and get its handle
	m_hwnd = CreateWindowEx(WS_EX_APPWINDOW, m_applicationName, m_applicationName,
		WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP,
		posX, posY, screenWidth, screenHeight, NULL, NULL, m_hinstance, NULL);

	//Bring the window up on the screen and set it as the main focus
	ShowWindow(m_hwnd, SW_SHOW);
	SetForegroundWindow(m_hwnd);
	SetFocus(m_hwnd);

	//Hide the mouse cursor
	ShowCursor(false);

	return;
}

//Closes the window
void SystemClass::ShutdownWindows() {
	//Show the mouse cursor
	ShowCursor(true);

	//Fix the display settings if left in fullscreen mode
	if (FULL_SCREEN) {
		ChangeDisplaySettings(NULL, 0);
	}

	//Remove the window
	DestroyWindow(m_hwnd);
	m_hwnd = NULL;

	//Remove app instance
	UnregisterClass(m_applicationName, m_hinstance);
	m_hinstance = NULL;

	//Release the pointer to this class
	ApplicationHandle = NULL;

	return;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam) {
	switch (umessage) {
		case WM_DESTROY: {
			PostQuitMessage(0);
			return 0;
		}
		case WM_CLOSE: {
			PostQuitMessage(0);
			return 0;
		}
		default: {
			return ApplicationHandle->MessageHandler(hwnd, umessage, wparam, lparam);
		}
	}
}