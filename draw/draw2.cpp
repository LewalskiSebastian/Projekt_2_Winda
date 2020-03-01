// draw.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "Resource.h"
#include <cstdio>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>
#include <cmath>

#define MAX_LOADSTRING 100
#define TMR_1 1

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

const int MAX_TABLE = 3000;
double main_table[11][MAX_TABLE];
int roll = 0;
int pitch = 1;
int yaw = 2;
int x = 3;
int y = 4;
int z = 5;
int n_x = 6;
int n_y = 7;
int n_z = 8;
int g = 9;
int H = 10;
double PI = 3.14159265358979323846;
bool diagramG = FALSE;
bool constantG = FALSE;
bool diagramH = FALSE;
int omittedSamples = 0;
int time;
int translation;
double constantGravity;
double Z;
double maxH = 0;
double axisG = 100;
double axisH = 10;
double axisT = 1;
int window_width = 1300;

INT value;

// buttons
HWND hwndButton;

HWND hText;

// sent data
int col = 0;
RECT drawArea2 = { 50, 400, 650, 422 };
RECT drawAreaG = { 0, 0, window_width, 200 };
RECT drawAreaH = { 0, 200, window_width, 800 };
RECT drawAreaWsjo = { 0, 0, window_width, 800 };
RECT drawAreaAverageG = { window_width, 550, 1600, 800 };

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	Buttons(HWND, UINT, WPARAM, LPARAM);

int GetText();
void writeG(HDC hdc);
void writeH(HDC hdc);
void countZ();
void writeT(HDC hdc);

void MyOnPaint(HDC hdc)
{
	Graphics graphics(hdc);
	Pen pen(Color(255, 0, 0, 0));
	Pen pen1(Color(255, 255, 0, 0));
	Pen pen2(Color(255, 255, 128, 192));
	Pen pen3(Color(255, 0, 230, 60));

	TextOut(hdc, 1300, 435, (LPCWSTR)L"Liczba pominiêtych próbek:", 26);
	writeG(hdc);
	writeH(hdc);
	writeT(hdc);

	graphics.DrawLine(&pen, 0, 199, window_width, 199);		//reference value (0) for Gravity
	graphics.DrawLine(&pen, 0, 600, window_width, 600);		//reference value (0) for height

	if (constantG) {
		graphics.DrawLine(&pen2, 0, 200 - constantGravity * axisG, window_width, 200 - constantGravity * axisG);			// drawing a diagram of constant of gravity
	}

	if (diagramG) {
		for (int i = omittedSamples + 1; i < window_width + omittedSamples; i++)
			graphics.DrawLine(&pen1, i - 1 - omittedSamples, 200 - main_table[g][(int)((i - 1 + translation)*axisT)] * axisG, i - omittedSamples, 200 - main_table[g][(int)((i + translation)*axisT)] * axisG);		// drawing a diagram of gravity
	}

	if (diagramH) {
		for (int i = omittedSamples + 1; i < (window_width + omittedSamples); i++)
			graphics.DrawLine(&pen3, i - 1 - omittedSamples, 600 - main_table[H][(int)((i - 1 + translation)*axisT)] * -axisH, i - omittedSamples, 600 - main_table[H][(int)((i + translation)*axisT)] * -axisH);	// drawing a diagram of height
	}
}

void RotationMatrix() {				// we have to rotate the axis X Y Z to normal position so we can simply count the height which the robot came up
	for (int i = 0; i < time; i++) {
		double r_yaw = main_table[yaw][i] * PI / 180;
		double r_pitch = main_table[pitch][i] * PI / 180;
		double r_roll = main_table[roll][i] * PI / 180;
		double yawMatrix[3][3] = { { cos(-r_yaw),-sin(-r_yaw),0 },{ sin(-r_yaw),cos(-r_yaw),0 },{ 0,0,1 } };
		double pitchMatrix[3][3] = { { cos(-r_pitch),0,sin(-r_pitch) },{ 0,1,0 },{ -sin(-r_pitch),0,cos(-r_pitch) } };
		double rollMatrix[3][3] = { { 1,0,0 },{ 0,cos(-r_roll),-sin(-r_roll) },{ 0,sin(-r_roll),cos(-r_roll) } };
		double xyzMatrix[3][3] = { { main_table[x][i] },{ main_table[y][i] },{ main_table[z][i] } };
		double resultMatrix1[3][3] = { { 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 } };
		double resultMatrix2[3][3] = { { 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 } };
		double resultMatrix3[3][3] = { { 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 } };

		for (int row = 0; row < 3; row++) {
			for (int col = 0; col < 3; col++) {
				for (int k = 0; k < 3; k++) {
					resultMatrix1[row][col] = resultMatrix1[row][col] + yawMatrix[row][k] * pitchMatrix[k][col];
				}
			}
		}
		for (int row = 0; row < 3; row++) {
			for (int col = 0; col < 3; col++) {
				for (int k = 0; k < 3; k++) {
					resultMatrix2[row][col] = resultMatrix2[row][col] + resultMatrix1[row][k] * rollMatrix[k][col];
				}
			}
		}
		for (int row = 0; row < 3; row++) {
			for (int col = 0; col < 1; col++) {
				for (int k = 0; k < 3; k++) {
					resultMatrix3[row][col] = resultMatrix3[row][col] + resultMatrix2[row][k] * xyzMatrix[k][col];
				}
			}
		}
		// new components [n_x(X), n_y(Y), n_z(Z)] of gravity after rotation
		main_table[n_x][i] = -resultMatrix3[0][0];
		main_table[n_y][i] = -resultMatrix3[1][0];
		main_table[n_z][i] = -resultMatrix3[2][0];
	}
}

void resultantG()
{
	for (int i = 0; i < time; i++)
	{
		constantGravity += main_table[g][i] = sqrt(main_table[n_x][i] * main_table[n_x][i] + main_table[n_y][i] * main_table[n_y][i] + main_table[n_z][i] * main_table[n_z][i]);
	}
	constantGravity = constantGravity / time;
}

void countG() {
	for (int i = omittedSamples; i < time; i++)
	{
		constantGravity += main_table[g][i];
	}
	constantGravity = constantGravity / (time - omittedSamples);
}

void countZ() {
	for (int i = omittedSamples; i < time; i++)
	{
		Z += main_table[n_z][i];
	}
	Z = Z / (time - omittedSamples);	// average of component Z of gravity

	// double integral of acceleration (of component Z) gives a distance (height)
	double sum = 0;
	double sum_of_sum = 0;
	maxH = 0;
	for (int i = omittedSamples; i < time; i++) {
		sum = (sum + 0.04*(main_table[n_z][i] - Z));
		main_table[H][i] = sum_of_sum = sum_of_sum + 0.04*sum;
		if (maxH < -main_table[H][i])
			maxH = -main_table[H][i];
	}
}

void repaintWindow(HWND hWnd, HDC &hdc, PAINTSTRUCT &ps, RECT *drawArea)
{
	if (drawArea == NULL)
		InvalidateRect(hWnd, NULL, TRUE); // repaint all
	else
		InvalidateRect(hWnd, drawArea, TRUE); //repaint drawArea
	hdc = BeginPaint(hWnd, &ps);
	MyOnPaint(hdc);
	EndPaint(hWnd, &ps);
}

void inputData()
{
	std::fstream file;
	file.open("outputRobotForwardB01.log", std::ios::in);
	if (file.good())
	{
		for (time = 0; !file.eof(); time++)
		{
			std::string buffer = "cos";
			char* pEnd;
			do {
				getline(file, buffer, ' ');
			} while ((buffer == " " || buffer == "") && !file.eof());
			main_table[roll][time] = strtod(buffer.c_str(), &pEnd);

			do {
				getline(file, buffer, ' ');
			} while ((buffer == " " || buffer == "") && !file.eof());
			main_table[pitch][time] = strtod(buffer.c_str(), &pEnd);

			do {
				getline(file, buffer, ' ');
			} while ((buffer == " " || buffer == "") && !file.eof());
			main_table[yaw][time] = strtod(buffer.c_str(), &pEnd);

			do {
				getline(file, buffer, ' ');
			} while ((buffer == " " || buffer == "") && !file.eof());
			main_table[x][time] = strtod(buffer.c_str(), &pEnd);

			do {
				getline(file, buffer, ' ');
			} while ((buffer == " " || buffer == "") && !file.eof());
			main_table[y][time] = strtod(buffer.c_str(), &pEnd);

			do {
				getline(file, buffer, ' ');
			} while ((buffer == " " || buffer == "") && !file.eof());
			main_table[z][time] = strtod(buffer.c_str(), &pEnd);

			getline(file, buffer);
		}
		time--;				// we have to delete one because it takes also the last empty line

	}
	else {
		MessageBox(NULL, (LPCWSTR)L"Nie uda³o siê otowrzyæ pliku tekstowego", (LPCWSTR)L"B³¹d", MB_ICONINFORMATION | MB_OK);
		exit(0);
	}
	file.close();
}


int OnCreate(HWND window)
{
	inputData();
	RotationMatrix();
	resultantG();
	countZ();
	return 0;
}


// main function (exe hInstance)
int APIENTRY _tWinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPTSTR    lpCmdLine,
	int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	MSG msg;
	HACCEL hAccelTable;

	value = 0;

	GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR           gdiplusToken;
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_DRAW, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);



	// Perform application initialization:
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_DRAW));

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	GdiplusShutdown(gdiplusToken);

	return (int)msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage are only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_DRAW));
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCE(IDC_DRAW);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	HWND hWnd;


	hInst = hInstance; // Store instance handle (of exe) in our global variable

					   // main window
	hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPED | WS_BORDER,
		0, 10, 1500, 800, 0, NULL, NULL, hInstance, NULL);

	// create button and store the handle                                                       

	hwndButton = CreateWindow(TEXT("button"),                      // The class name required is button
		TEXT("Draw"),                  // the caption of the button
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,  // the styles
		1300, 0,                                  // the left and top co-ordinates
		80, 35,                              // width and height
		hWnd,                                 // parent window handle
		(HMENU)ID_BUTTON1,                   // the ID of your button
		hInstance,                            // the instance of your application
		NULL);                               // extra bits you dont really need

	hwndButton = CreateWindow(TEXT("button"),
		TEXT("DrawAll"),      
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 
		1400, 0,   
		80, 35,    
		hWnd,        
		(HMENU)ID_BUTTON2,  
		hInstance,              
		NULL);         
	hwndButton = CreateWindow(TEXT("button"),   
		TEXT("W lewo"), 
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		1300, 40,            
		80, 35,             
		hWnd,                       
		(HMENU)ID_BUTTONL,          
		hInstance,                  
		NULL);                         

	hwndButton = CreateWindow(TEXT("button"),    
		TEXT("W prawo"),              
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 
		1400, 40,            
		80, 35,          
		hWnd,               
		(HMENU)ID_BUTTONP,   
		hInstance,          
		NULL);         

	hwndButton = CreateWindow(TEXT("button"), 
		TEXT("Oœ G+"),   
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		1300, 80,                
		80, 35,         
		hWnd,          
		(HMENU)ID_BUTTONGP,   
		hInstance,   
		NULL);         

	hwndButton = CreateWindow(TEXT("button"), 
		TEXT("Oœ G-"),      
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		1400, 80,  
		80, 35,    
		hWnd,    
		(HMENU)ID_BUTTONGM, 
		hInstance,   
		NULL);       

	hwndButton = CreateWindow(TEXT("button"),  
		TEXT("Oœ H+"),  
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 
		1300, 120,    
		80, 35,                
		hWnd,                   
		(HMENU)ID_BUTTONHP,    
		hInstance,     
		NULL);                 

	hwndButton = CreateWindow(TEXT("button"),   
		TEXT("Oœ H-"),                
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		1400, 120,                      
		80, 35,                  
		hWnd,                          
		(HMENU)ID_BUTTONHM,          
		hInstance,                 
		NULL);                  

	hwndButton = CreateWindow(TEXT("button"),      
		TEXT("Oœ T+"),
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		1300, 160,           
		80, 35,          
		hWnd,               
		(HMENU)ID_BUTTONTP,    
		hInstance,          
		NULL);                     

	hwndButton = CreateWindow(TEXT("button"),  
		TEXT("Oœ T-"),          
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,  
		1400, 160,                 
		80, 35,                   
		hWnd,                        
		(HMENU)ID_BUTTONTM,             
		hInstance,                           
		NULL);                               

	hwndButton = CreateWindow(TEXT("button"), TEXT("G ON"),
		WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | WS_GROUP,
		1300, 200, 180, 30, hWnd, (HMENU)ID_RBUTTON1, GetModuleHandle(NULL), NULL);

	hwndButton = CreateWindow(TEXT("button"), TEXT("G OFF"),
		WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
		1300, 235, 180, 30, hWnd, (HMENU)ID_RBUTTON2, GetModuleHandle(NULL), NULL);
	SendMessage(hwndButton, BM_SETCHECK, BST_CHECKED, 0);							// by default it checks the button before this line

	hwndButton = CreateWindow(TEXT("button"), TEXT("Sta³a G ON"),
		WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | WS_GROUP,
		1300, 280, 180, 30, hWnd, (HMENU)ID_RBUTTON3, GetModuleHandle(NULL), NULL);

	hwndButton = CreateWindow(TEXT("button"), TEXT("Sta³a G OFF"),
		WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
		1300, 315, 180, 30, hWnd, (HMENU)ID_RBUTTON4, GetModuleHandle(NULL), NULL);
	SendMessage(hwndButton, BM_SETCHECK, BST_CHECKED, 0);

	hwndButton = CreateWindow(TEXT("button"), TEXT("Wysokoœæ ON"),
		WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | WS_GROUP,
		1300, 360, 180, 30, hWnd, (HMENU)ID_RBUTTON5, GetModuleHandle(NULL), NULL);

	hwndButton = CreateWindow(TEXT("button"), TEXT("Wysokoœæ OFF"),
		WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
		1300, 395, 180, 30, hWnd, (HMENU)ID_RBUTTON6, GetModuleHandle(NULL), NULL);
	SendMessage(hwndButton, BM_SETCHECK, BST_CHECKED, 0);

	hText = CreateWindowEx(WS_EX_CLIENTEDGE, (LPCWSTR)L"EDIT", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER, 1300, 455, 180, 30, hWnd, NULL, hInstance, NULL);
	SetWindowText(hText, (LPCWSTR)L"0");

	hwndButton = CreateWindow(TEXT("button"),                      // The class name required is button
		TEXT("ZatwierdŸ"),                  // the caption of the button
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,  // the styles
		1300, 490,                                  // the left and top co-ordinates
		80, 35,                              // width and height
		hWnd,                                 // parent window handle
		(HMENU)ID_BUTTON3,                   // the ID of your button
		hInstance,                            // the instance of your application
		NULL);

	OnCreate(hWnd);

	if (!hWnd)
	{
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}

void writeG(HDC hdc) {
	std::string variableToString = std::to_string(constantGravity);		// changing variable into string and then into LPCWSTR
	std::wstring temporary = std::wstring(variableToString.begin(), variableToString.end());
	LPCWSTR newString = temporary.c_str();
	TextOut(hdc, 1300, 570, (LPCWSTR)newString, 7);
	TextOut(hdc, 1300, 550, (LPCWSTR)L"Sk³adowa sta³a G:", 17);
	TextOut(hdc, 1360, 570, (LPCWSTR)L"G (9.81m/s^2)", 13);
}

void writeH(HDC hdc) {
	std::string variableToString = std::to_string(maxH*9.81);
	std::wstring temporary = std::wstring(variableToString.begin(), variableToString.end());
	LPCWSTR newString = temporary.c_str();
	TextOut(hdc, 1300, 620, (LPCWSTR)newString, 7);
	TextOut(hdc, 1300, 600, (LPCWSTR)L"Maksymalna wysokoœæ:", 20);
	TextOut(hdc, 1360, 620, (LPCWSTR)L"m", 1);
}

void writeT(HDC hdc) {
	double T = (time - omittedSamples) / 25.f;
	std::string variableToString = std::to_string(T);
	std::wstring temporary = std::wstring(variableToString.begin(), variableToString.end());
	LPCWSTR newString = temporary.c_str();
	TextOut(hdc, 1300, 670, (LPCWSTR)newString, 7);
	TextOut(hdc, 1300, 650, (LPCWSTR)L"Czas ruchu:", 11);
	TextOut(hdc, 1360, 670, (LPCWSTR)L"s", 1);
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window (low priority)
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_COMMAND:
		wmId = LOWORD(wParam);
		wmEvent = HIWORD(wParam);

		// MENU & BUTTON messages
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		case ID_BUTTON1:
			col++;
			if (col > 10)
				col = 0;
			repaintWindow(hWnd, hdc, ps, &drawAreaWsjo);
			break;
		case ID_BUTTON2:
			repaintWindow(hWnd, hdc, ps, NULL);
			break;
		case ID_RBUTTON1:
			diagramG = TRUE;
			repaintWindow(hWnd, hdc, ps, &drawAreaG);
			break;
		case ID_RBUTTON2:
			diagramG = FALSE;
			repaintWindow(hWnd, hdc, ps, &drawAreaG);
			break;
		case ID_RBUTTON3:
			constantG = TRUE;
			repaintWindow(hWnd, hdc, ps, &drawAreaG);
			break;
		case ID_RBUTTON4:
			constantG = FALSE;
			repaintWindow(hWnd, hdc, ps, &drawAreaG);
			break;
		case ID_RBUTTON5:
			diagramH = TRUE;
			repaintWindow(hWnd, hdc, ps, &drawAreaH);
			break;
		case ID_RBUTTON6:
			diagramH = FALSE;
			repaintWindow(hWnd, hdc, ps, &drawAreaH);
			break;
		case ID_BUTTON3:
			omittedSamples = GetText();
			translation = 0;
			axisT = 1;
			countG();
			countZ();
			repaintWindow(hWnd, hdc, ps, &drawAreaWsjo);
			repaintWindow(hWnd, hdc, ps, &drawAreaAverageG);
			break;
		case ID_BUTTONL:
			translation += 250 * axisT;
			if ((time - omittedSamples - translation) / axisT < window_width)
				translation = (time - omittedSamples) / axisT - window_width;
			repaintWindow(hWnd, hdc, ps, &drawAreaWsjo);
			break;
		case ID_BUTTONP:
			translation -= 250 * axisT;
			if (translation < 0)
				translation = 0;
			repaintWindow(hWnd, hdc, ps, &drawAreaWsjo);
			break;
		case ID_BUTTONGP:
			axisG += 5;
			repaintWindow(hWnd, hdc, ps, &drawAreaG);
			break;
		case ID_BUTTONGM:
			axisG -= 5;
			if (axisG < 0)
				axisG = 0;
			repaintWindow(hWnd, hdc, ps, &drawAreaG);
			break;
		case ID_BUTTONHP:
			axisH += 1;
			repaintWindow(hWnd, hdc, ps, &drawAreaH);
			break;
		case ID_BUTTONHM:
			axisH -= 1;
			if (axisH < 0)
				axisH = 0;
			repaintWindow(hWnd, hdc, ps, &drawAreaH);
			break;
		case ID_BUTTONTP:
			translation = 0;
			axisT += 0.1;
			if (axisT > 1)
				axisT = 1;
			repaintWindow(hWnd, hdc, ps, &drawAreaWsjo);
			break;
		case ID_BUTTONTM:
			translation = 0;
			axisT -= 0.1;
			if (axisT < 0.1)
				axisT = 0.1;
			repaintWindow(hWnd, hdc, ps, &drawAreaWsjo);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		TextOut(hdc, 1300, 435, (LPCWSTR)L"Liczba pominiêtych próbek:", 26);
		writeG(hdc);
		writeH(hdc);
		writeT(hdc);
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_TIMER:
		switch (wParam)
		{
		case TMR_1:
			//force window to repaint
			repaintWindow(hWnd, hdc, ps, &drawArea2);
			value++;
			break;
		}

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

int GetText() {
	int ignoreSamples;
	DWORD length = GetWindowTextLength(hText);
	LPWSTR buffer = (LPWSTR)GlobalAlloc(GPTR, length + 1);
	GetWindowText(hText, buffer, length + 1);
	ignoreSamples = _wtoi(buffer);
	GlobalFree(buffer);
	if (ignoreSamples > (time - omittedSamples) - window_width) {
		MessageBox(NULL, (LPCWSTR)L"Za du¿a wartoœæ", (LPCWSTR)L"B³¹d", MB_ICONINFORMATION | MB_OK);
		return 0;
	}
	if (ignoreSamples < 0) {
		MessageBox(NULL, (LPCWSTR)L"Liczba odbrzuconych próbek musi byæ liczb¹ dodatni¹", (LPCWSTR)L"B³¹d", MB_ICONINFORMATION | MB_OK);
		return 0;
	}
	return ignoreSamples;
}