// draw.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "Resource.h"
#include <vector>
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

const int MAX_TABLICA = 3000;
double tablica[11][MAX_TABLICA];
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
double pi = 3.14159265358979323846;
bool wykresG = FALSE;
bool stalaG = FALSE;
bool wykresH = FALSE;
int ilosc = 0;
int czas;
int przesuniecie;
double G;
double Z;
double maxH = 0;

INT value;

// buttons
HWND hwndButton;

HWND hText;

// sent data
int col = 0;
std::vector<Point> data;
RECT drawArea1 = { 0, 0, 150, 200 };
RECT drawArea2 = { 50, 400, 650, 422 };
RECT drawAreaG = { 0, 0, 1300, 200 };
RECT drawAreaH = { 0, 200, 1300, 800 };
RECT drawAreaWsjo = { 0, 0, 1300, 800 };
RECT drawAreaSredniaG = { 1300, 550, 1600, 800 };

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	Buttons(HWND, UINT, WPARAM, LPARAM);

int GetText();
void piszG(HDC hdc);
void piszH(HDC hdc);
void liczZ();
void piszT(HDC hdc);

void MyOnPaint(HDC hdc)
{
	Graphics graphics(hdc);
	Pen pen(Color(255, 0, 0, 0));
	Pen pen1(Color(255, 255, 0, 0));
	Pen pen2(Color(255, 255, 128, 192));
	Pen pen3(Color(255, 0, 230, 60));

	TextOut(hdc, 1300, 435, (LPCWSTR)L"Liczba pominiêtych próbek:", 26);
	piszG(hdc);
	piszH(hdc);
	piszT(hdc);

	graphics.DrawLine(&pen, 0, 199, 1300, 199);		//Wartoœæ odniesienia (0) dla G
	graphics.DrawLine(&pen, 0, 600, 1300, 600);

	if (stalaG) {
		graphics.DrawLine(&pen2, 0, 200 - G * 10, 1300, 200 - G * 10);
	}

	if (wykresG) {
		for (int i = ilosc; i < (1300 + ilosc); i++)
			graphics.DrawLine(&pen1, i - 1 - ilosc, 200 - tablica[g][i - 1 + przesuniecie] * 100, i - ilosc, 200 - tablica[g][i + przesuniecie] * 100);
	}

	if (wykresH) {
		for (int i = ilosc; i < (1300 + ilosc); i++)
			graphics.DrawLine(&pen3, i - 1 - ilosc, 600 - tablica[H][i - 1 + przesuniecie] * -100, i - ilosc, 600 - tablica[H][i + przesuniecie] * -100);
	}
	/*
	double suma = 0;
	double suma_sum = 0;
	for (int i = ilosc; i < (1300 + ilosc); i++) {
	graphics.DrawLine(&pen3, i - 1, 700 - suma * -1, i, 700 - (suma - Z + tablica[n_z][i + przesuniecie] * -1));
	suma = (suma - Z + tablica[n_z][i + przesuniecie]);
	graphics.DrawLine(&pen3, i - 1, 600 - (suma_sum)*0.01, i, 600 - (suma_sum + suma)*0.01);
	suma_sum = suma_sum + suma;
	}
	*/
}

void MacierzRotacji() {
	for (int i = 0; i < czas; i++) {
		double r_yaw = tablica[yaw][i] * pi / 180;
		double r_pitch = tablica[pitch][i] * pi / 180;
		double r_roll = tablica[roll][i] * pi / 180;
		double aMatrix[3][3] = { { cos(-r_yaw),-sin(-r_yaw),0 },{ sin(-r_yaw),cos(-r_yaw),0 },{ 0,0,1 } };
		double bMatrix[3][3] = { { cos(-r_pitch),0,sin(-r_pitch) },{ 0,1,0 },{ -sin(-r_pitch),0,cos(-r_pitch) } };
		double cMatrix[3][3] = { { 1,0,0 },{ 0,cos(-r_roll),-sin(-r_roll) },{ 0,sin(-r_roll),cos(-r_roll) } };
		double dMatrix[3][3] = { { tablica[x][i] },{ tablica[y][i] },{ tablica[z][i] } };
		double product[3][3] = { { 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 } };
		double product2[3][3] = { { 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 } };
		double product3[3][3] = { { 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 } };

		for (int row = 0; row < 3; row++) {
			for (int col = 0; col < 3; col++) {
				for (int inner = 0; inner < 3; inner++) {
					product[row][col] = product[row][col] + aMatrix[row][inner] * bMatrix[inner][col];
				}
				std::cout << product[row][col] << "  ";
			}
			std::cout << "\n";
		}
		for (int row = 0; row < 3; row++) {
			for (int col = 0; col < 3; col++) {
				for (int inner = 0; inner < 3; inner++) {
					product2[row][col] = product2[row][col] + product[row][inner] * cMatrix[inner][col];
				}
				std::cout << product2[row][col] << "  ";
			}
			std::cout << "\n";
		}
		for (int row = 0; row < 3; row++) {
			for (int col = 0; col < 1; col++) {
				for (int inner = 0; inner < 3; inner++) {
					product3[row][col] = product3[row][col] + product2[row][inner] * dMatrix[inner][col];
				}
				std::cout << product3[row][col] << "  ";
			}
			std::cout << "\n";
		}
		tablica[n_x][i] = -product3[0][0];
		tablica[n_y][i] = -product3[1][0];
		tablica[n_z][i] = -product3[2][0];
	}
}

void wypadkowaG()
{
	for (int i = 0; i < czas; i++)
	{
		G += tablica[g][i] = sqrt(tablica[n_x][i] * tablica[n_x][i] + tablica[n_y][i] * tablica[n_y][i] + tablica[n_z][i] * tablica[n_z][i]);
	}
	G = G / czas;
}

void liczG() {
	for (int i = ilosc; i < czas; i++)
	{
		G += tablica[g][i];
	}
	G = G / (czas - ilosc);
}

void liczZ() {
	for (int i = ilosc; i < czas; i++)
	{
		Z += tablica[n_z][i];
	}
	Z = Z / (czas - ilosc);

	double suma = 0;
	double suma_sum = 0;
	maxH = 0;
	for (int i = ilosc; i < czas; i++) {
		suma = (suma + 0.04*(tablica[n_z][i] - Z));
		tablica[H][i] = suma_sum = 0.04(suma_sum + suma);
		if (maxH < -tablica[H][i])
			maxH = -tablica[H][i];
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
	std::fstream plik;
	plik.open("outputRobotForwardB01.log", std::ios::in);
	if (plik.good())
	{
		for (czas = 0; !plik.eof(); czas++)
		{
			std::string bufor = "cos";
			char* pEnd;
			do {
				getline(plik, bufor, ' ');
			} while ((bufor == " " || bufor == "") && !plik.eof());
			tablica[roll][czas] = strtod(bufor.c_str(), &pEnd);

			do {
				getline(plik, bufor, ' ');
			} while ((bufor == " " || bufor == "") && !plik.eof());
			tablica[pitch][czas] = strtod(bufor.c_str(), &pEnd);

			do {
				getline(plik, bufor, ' ');
			} while ((bufor == " " || bufor == "") && !plik.eof());
			tablica[yaw][czas] = strtod(bufor.c_str(), &pEnd);

			do {
				getline(plik, bufor, ' ');
			} while ((bufor == " " || bufor == "") && !plik.eof());
			tablica[x][czas] = strtod(bufor.c_str(), &pEnd);

			do {
				getline(plik, bufor, ' ');
			} while ((bufor == " " || bufor == "") && !plik.eof());
			tablica[y][czas] = strtod(bufor.c_str(), &pEnd);

			do {
				getline(plik, bufor, ' ');
			} while ((bufor == " " || bufor == "") && !plik.eof());
			tablica[z][czas] = strtod(bufor.c_str(), &pEnd);

			getline(plik, bufor);
		}
		czas--;				//usuwamy jeden bo wczytuje kolejny ju¿ pusty wiersz

	}
	else {
		MessageBox(NULL, (LPCWSTR)L"Nie uda³o siê otowrzyæ pliku tekstowego", (LPCWSTR)L"B³¹d", MB_ICONINFORMATION | MB_OK);
		exit(0);
	}
	plik.close();

	/*
	data.push_back(Point(0, 0));
	for (int i = 1; i < 100; i++) {
	data.push_back(Point(2 * i + 1, 200 * rand() / RAND_MAX));
	}
	*/
}


int OnCreate(HWND window)
{
	inputData();
	MacierzRotacji();
	wypadkowaG();
	liczZ();
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

	// TODO: Place code here.
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
		80, 50,                              // width and height
		hWnd,                                 // parent window handle
		(HMENU)ID_BUTTON1,                   // the ID of your button
		hInstance,                            // the instance of your application
		NULL);                               // extra bits you dont really need

	hwndButton = CreateWindow(TEXT("button"),                      // The class name required is button
		TEXT("DrawAll"),                  // the caption of the button
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,  // the styles
		1400, 0,                                  // the left and top co-ordinates
		80, 50,                              // width and height
		hWnd,                                 // parent window handle
		(HMENU)ID_BUTTON2,                   // the ID of your button
		hInstance,                            // the instance of your application
		NULL);                               // extra bits you dont really need

	hwndButton = CreateWindow(TEXT("button"),                      // The class name required is button
		TEXT("W lewo"),                  // the caption of the button
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,  // the styles
		1300, 80,                                  // the left and top co-ordinates
		80, 50,                              // width and height
		hWnd,                                 // parent window handle
		(HMENU)ID_BUTTONL,                   // the ID of your button
		hInstance,                            // the instance of your application
		NULL);                               // extra bits you dont really need

	hwndButton = CreateWindow(TEXT("button"),                      // The class name required is button
		TEXT("W prawo"),                  // the caption of the button
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,  // the styles
		1400, 80,                                  // the left and top co-ordinates
		80, 50,                              // width and height
		hWnd,                                 // parent window handle
		(HMENU)ID_BUTTONP,                   // the ID of your button
		hInstance,                            // the instance of your application
		NULL);                               // extra bits you dont really need

											 // create button and store the handle                                                       
											 /*
											 hwndButton = CreateWindow(TEXT("button"), TEXT("Timer ON"),
											 WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
											 300, 155, 100, 30, hWnd, (HMENU)ID_RBUTTON1, GetModuleHandle(NULL), NULL);

											 hwndButton = CreateWindow(TEXT("button"), TEXT("Timer OFF"),
											 WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
											 300, 200, 100, 30, hWnd, (HMENU)ID_RBUTTON2, GetModuleHandle(NULL), NULL);
											 */

	hwndButton = CreateWindow(TEXT("button"), TEXT("G ON"),
		WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | WS_GROUP,
		1300, 155, 180, 30, hWnd, (HMENU)ID_RBUTTON1, GetModuleHandle(NULL), NULL);

	hwndButton = CreateWindow(TEXT("button"), TEXT("G OFF"),
		WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
		1300, 200, 180, 30, hWnd, (HMENU)ID_RBUTTON2, GetModuleHandle(NULL), NULL);
	SendMessage(hwndButton, BM_SETCHECK, BST_CHECKED, 0);							//Domyœlnie zaznaczony przycisk wy¿ej

	hwndButton = CreateWindow(TEXT("button"), TEXT("Sta³a G ON"),
		WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | WS_GROUP,
		1300, 255, 180, 30, hWnd, (HMENU)ID_RBUTTON3, GetModuleHandle(NULL), NULL);

	hwndButton = CreateWindow(TEXT("button"), TEXT("Sta³a G OFF"),
		WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
		1300, 300, 180, 30, hWnd, (HMENU)ID_RBUTTON4, GetModuleHandle(NULL), NULL);
	SendMessage(hwndButton, BM_SETCHECK, BST_CHECKED, 0);

	hwndButton = CreateWindow(TEXT("button"), TEXT("Wysokoœæ ON"),
		WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | WS_GROUP,
		1300, 355, 180, 30, hWnd, (HMENU)ID_RBUTTON5, GetModuleHandle(NULL), NULL);

	hwndButton = CreateWindow(TEXT("button"), TEXT("Wysokoœæ OFF"),
		WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
		1300, 400, 180, 30, hWnd, (HMENU)ID_RBUTTON6, GetModuleHandle(NULL), NULL);
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

void piszG(HDC hdc) {
	std::string varAsString = std::to_string(G);
	std::wstring stemp = std::wstring(varAsString.begin(), varAsString.end());
	LPCWSTR sw = stemp.c_str();
	TextOut(hdc, 1300, 570, (LPCWSTR)sw, 7);
	TextOut(hdc, 1300, 550, (LPCWSTR)L"Sk³adowa sta³a G:", 17);
	TextOut(hdc, 1360, 570, (LPCWSTR)L"G (9.81m/s^2)", 13);
}

void piszH(HDC hdc) {
	std::string varAsString = std::to_string(maxH*9.81);
	std::wstring stemp = std::wstring(varAsString.begin(), varAsString.end());
	LPCWSTR sw = stemp.c_str();
	TextOut(hdc, 1300, 620, (LPCWSTR)sw, 7);
	TextOut(hdc, 1300, 600, (LPCWSTR)L"Maksymalna wysokoœæ:", 20);
	TextOut(hdc, 1360, 620, (LPCWSTR)L"m", 1);
}

void piszT(HDC hdc) {
	double T = (czas - ilosc) / 25.f;
	std::string varAsString = std::to_string(T);
	std::wstring stemp = std::wstring(varAsString.begin(), varAsString.end());
	LPCWSTR sw = stemp.c_str();
	TextOut(hdc, 1300, 670, (LPCWSTR)sw, 7);
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
			repaintWindow(hWnd, hdc, ps, &drawArea1);
			break;
		case ID_BUTTON2:
			repaintWindow(hWnd, hdc, ps, NULL);
			break;
		case ID_RBUTTON1:
			wykresG = TRUE;
			repaintWindow(hWnd, hdc, ps, &drawAreaG);
			break;
		case ID_RBUTTON2:
			wykresG = FALSE;
			repaintWindow(hWnd, hdc, ps, &drawAreaG);
			break;
		case ID_RBUTTON3:
			stalaG = TRUE;
			repaintWindow(hWnd, hdc, ps, &drawAreaG);
			break;
		case ID_RBUTTON4:
			stalaG = FALSE;
			repaintWindow(hWnd, hdc, ps, &drawAreaG);
			break;
		case ID_RBUTTON5:
			wykresH = TRUE;
			repaintWindow(hWnd, hdc, ps, &drawAreaH);
			break;
		case ID_RBUTTON6:
			wykresH = FALSE;
			repaintWindow(hWnd, hdc, ps, &drawAreaH);
			break;
		case ID_BUTTON3:
			ilosc = GetText();
			przesuniecie = 0;
			liczG();
			liczZ();
			repaintWindow(hWnd, hdc, ps, &drawAreaWsjo);
			repaintWindow(hWnd, hdc, ps, &drawAreaSredniaG);
			break;
		case ID_BUTTONL:
			przesuniecie += 250;
			if (przesuniecie < 0)
				przesuniecie = 0;
			if (przesuniecie + ilosc + 1300 > czas)
				przesuniecie = czas - 1300 - ilosc;
			repaintWindow(hWnd, hdc, ps, &drawAreaWsjo);
			break;
		case ID_BUTTONP:
			przesuniecie -= 250;
			if (przesuniecie < 0)
				przesuniecie = 0;
			if (przesuniecie + ilosc + 1300 > czas)
				przesuniecie = czas - 1300 - ilosc;
			repaintWindow(hWnd, hdc, ps, &drawAreaWsjo);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code here (not depend on timer, buttons)
		TextOut(hdc, 1300, 435, (LPCWSTR)L"Liczba pominiêtych próbek:", 26);
		piszG(hdc);
		piszH(hdc);
		piszT(hdc);
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
	int odrzucone;
	DWORD dlugosc = GetWindowTextLength(hText);
	LPWSTR Bufor = (LPWSTR)GlobalAlloc(GPTR, dlugosc + 1);
	GetWindowText(hText, Bufor, dlugosc + 1);
	odrzucone = _wtoi(Bufor);
	GlobalFree(Bufor);
	if (odrzucone > czas - 1300) {
		MessageBox(NULL, (LPCWSTR)L"Za du¿a wartoœæ", (LPCWSTR)L"B³¹d", MB_ICONINFORMATION | MB_OK);
		return 0;
	}
	if (odrzucone < 0) {
		MessageBox(NULL, (LPCWSTR)L"Liczba odbrzucinych próbek musi byæ liczb¹ dodatni¹", (LPCWSTR)L"B³¹d", MB_ICONINFORMATION | MB_OK);
		return 0;
	}
	return odrzucone;
}