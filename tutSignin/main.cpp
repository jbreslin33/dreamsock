/******************************************/
/* MMOG programmer's guide                */
/* Tutorial game client                   */
/* Programming:						      */
/* Teijo Hakala						      */
/******************************************/

#include "common.h"
#include "resource.h"

// Some global stuff
CSignin Signin;

char serverIP[32];

HINSTANCE hInst;
HWND hWnd_Application;
HWND hWnd_CreateAccountDialog;
HWND hWnd_LoginDialog;

//-----------------------------------------------------------------------------
// Name: empty()
// Desc: 
//-----------------------------------------------------------------------------
LRESULT CALLBACK ApplicationProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		case WM_CLOSE:
		{
			PostQuitMessage(0);
			break;
		}

		default:
			break;
	}
	
	// Pass All Unhandled Messages To DefWindowProc
	return DefWindowProc(hWnd,uMsg,wParam,lParam);
}

//-----------------------------------------------------------------------------
// Name: empty()
// Desc: 
//-----------------------------------------------------------------------------
LRESULT CALLBACK CreateAccountDialogProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	char nickname[30];
	char firstname[50];
	char surname[50];
	int age;
	char gender[10];
	char password[50];
	char password2[50];
	char email[150];

	int ret;

	switch (uMsg)
	{
		case WM_COMMAND:
		{
			switch(LOWORD(wParam))
			{
				case IDC_CREATEACCOUNT_CANCEL:
					DestroyWindow(hWnd_CreateAccountDialog);
					hWnd_CreateAccountDialog = NULL;
					break;

				case IDC_CREATEACCOUNT_CONTINUE:

					// -> First get the IP address of the server from the dialog
					GetDlgItemText(hWnd_CreateAccountDialog, IDC_CREATEACCOUNT_IPADDRESS, serverIP, 20);

					// -> Store the player data in local variables
					GetDlgItemText(hWnd_CreateAccountDialog, IDC_CREATEACCOUNT_NICKNAME, nickname, 30);
					GetDlgItemText(hWnd_CreateAccountDialog, IDC_CREATEACCOUNT_FIRSTNAME, firstname, 50);
					GetDlgItemText(hWnd_CreateAccountDialog, IDC_CREATEACCOUNT_SURNAME, surname, 50);
					age = GetDlgItemInt(hWnd_CreateAccountDialog, IDC_CREATEACCOUNT_AGE, NULL, FALSE);
					GetDlgItemText(hWnd_CreateAccountDialog, IDC_CREATEACCOUNT_GENDER, gender, 10);
					GetDlgItemText(hWnd_CreateAccountDialog, IDC_CREATEACCOUNT_PASSWORD1, password, 50);
					GetDlgItemText(hWnd_CreateAccountDialog, IDC_CREATEACCOUNT_PASSWORD2, password2, 50);
					GetDlgItemText(hWnd_CreateAccountDialog, IDC_CREATEACCOUNT_EMAIL, email, 150);
	
					// -> Check all the fields have been filled in
					if(!strcmp(nickname,"") || !strcmp(firstname,"") || !strcmp(surname,"") || !strcmp(gender,"") || !strcmp(password,"") || !strcmp(email,"") || age < 1)
					{
						MessageBox(hWnd_CreateAccountDialog, "Not all fields have been filled in!\n\nPlease check and try again...", "Information Error", MB_OK);
						break;
					}

					// -> Check the passwords match
					if(strcmp(password,password2))
					{
						MessageBox(hWnd_CreateAccountDialog,
							"The two passwords you entered do not match!\n\nPlease check and try again...", "Password Error", MB_OK);
						break;
					}

					ret = Signin.GetNetworkClient()->Initialise("", serverIP, 30002);

					if(ret == DREAMSOCK_CLIENT_ERROR)
					{
						char text[64];
						sprintf(text, "Could not open client socket");

						MessageBox(NULL, text, "Error", MB_OK);
					}

					Signin.Connect(nickname);

					DestroyWindow(hWnd_CreateAccountDialog);
					hWnd_CreateAccountDialog = NULL;

					Signin.SendSignIn(nickname, firstname, surname, age, gender, password, email);

					break;
				default:
					break;
			}
			return 0;
		}
		case WM_CLOSE:
			break;

		case WM_DESTROY:
			break;
	}
	
	return 0;
}

//-----------------------------------------------------------------------------
// Name: empty()
// Desc: 
//-----------------------------------------------------------------------------
LRESULT CALLBACK LoginDialogProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		case WM_COMMAND:
		{
			switch(LOWORD(wParam))
			{
				case IDC_LOGIN_QUIT:
					PostQuitMessage(0);
					break;

				case IDC_LOGIN_CREATEACCOUNT:
					if(!hWnd_CreateAccountDialog)
					{
						hWnd_CreateAccountDialog = CreateDialog(hInst,
							MAKEINTRESOURCE(IDD_CREATEACCOUNT),
							hWnd_Application, (DLGPROC) CreateAccountDialogProc);
					}
					break;

				case IDC_DOLOGIN:
					break;

				default:
					break;
			}
			return 0;
		}
		case WM_CLOSE:
		{
			PostQuitMessage(0);
			break;
		}
	}
	
	return 0;
}

//-----------------------------------------------------------------------------
// Name: empty()
// Desc: 
//-----------------------------------------------------------------------------
int PASCAL WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
				   TCHAR *pCmdLine, int nCmdShow)
{
	int time, oldTime, newTime;

	WNDCLASSEX wcl;

	// Create our Main Window
	wcl.cbSize = sizeof(WNDCLASSEX);

	wcl.hInstance		= hInstance;
	wcl.lpszClassName	= "ArmyWar";
	wcl.lpfnWndProc		= ApplicationProc;
	wcl.style			= 0;

	wcl.hIcon			= LoadIcon(NULL, IDI_APPLICATION);
	wcl.hIconSm			= LoadIcon(NULL, IDI_WINLOGO);
	wcl.hCursor			= LoadCursor(NULL, IDC_ARROW);

	wcl.lpszMenuName	= NULL;
	wcl.cbClsExtra		= 0;
	wcl.cbWndExtra		= 0;

	wcl.hbrBackground = (HBRUSH) GetStockObject(LTGRAY_BRUSH);

	if(!RegisterClassEx(&wcl)) return 0;

	hWnd_Application = CreateWindow(
		"ArmyWar",
		"ARMY WAR Online 2.0",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		640,
		480,
		HWND_DESKTOP,
		NULL,
		hInstance,
		NULL
		);

	// Initialize the Network Library
	if(dreamSock_Initialize() != 0)
	{
		MessageBox(NULL, "Error initialising Communication Library!",
			"Fatal Error", MB_OK);

		return 1;
	}

	ShowWindow(hWnd_Application, nCmdShow);
	UpdateWindow(hWnd_Application);

	// Set Global Instance Variable
	hInst = hInstance;

	// Display the LoginDialog
	hWnd_LoginDialog = CreateDialog(hInst, MAKEINTRESOURCE(IDD_LOGINDIALOG),
		hWnd_Application, (DLGPROC)LoginDialogProc);

	MSG msg;
	BOOL bMsg = FALSE;

	PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE);

	oldTime = dreamSock_GetCurrentSystemTime();

	bool done = false;

	try
	{
		while(!done)
		{
			while(PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
			{
				if(!GetMessage(&msg, NULL, 0, 0))
				{
					Signin.Disconnect();
					done = true;
				}

				TranslateMessage(&msg);
   				DispatchMessage(&msg);
			}

			do
			{
				newTime = dreamSock_GetCurrentSystemTime();
				time = newTime - oldTime;
			} while (time < 1);

			// Run sign-in network
			Signin.RunNetwork(time);

			oldTime = newTime;
		}
	}
	catch(...)
	{
		Signin.Disconnect();

		LogString("Unknown Exception caught in main loop");

		MessageBox(NULL, "Unknown Exception caught in main loop", "Error", MB_OK | MB_TASKMODAL);

		return -1;
	}

	return msg.wParam;
}
