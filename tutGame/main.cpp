/******************************************/
/* MMOG programmer's guide                */
/* Tutorial game client                   */
/* Programming:						      */
/* Teijo Hakala						      */
/******************************************/

#include "common.h"
#include "resource.h"
#include <math.h>

// Some global stuff
CLobby Lobby;
CSignin Signin;

char serverIP[32];

HINSTANCE hInst;
HWND hWnd_Application;
HWND hWnd_CreateAccountDialog;
HWND hWnd_LoginDialog;
HWND hWnd_LobbyDialog;

HWND hWnd_CreateGameDialog;
HWND hWnd_JoinGameDialog;

HWND hWnd_CreateViewPlayersDialog;

//-----------------------------------------------------------------------------
// Name: empty()
// Desc: 
//-----------------------------------------------------------------------------
float VectorLength(VECTOR2D *vec)
{
	return (float) sqrt(vec->x*vec->x + vec->y*vec->y);
}

//-----------------------------------------------------------------------------
// Name: empty()
// Desc: 
//-----------------------------------------------------------------------------
VECTOR2D VectorSubstract(VECTOR2D *vec1, VECTOR2D *vec2)
{
	VECTOR2D vec;

	vec.x = vec1->x - vec2->x;
	vec.y = vec1->y - vec2->y;

	return vec;
}

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

		case WM_KEYDOWN:
		{
			keys[wParam] = TRUE;
			break;
		}

		case WM_KEYUP:
		{
			keys[wParam] = FALSE;
			break;
		}

		case WM_SIZE:
		{
			GFX_Resize(LOWORD(lParam),HIWORD(lParam));
			break;
		}

		default:
			if(!Lobby.GetGameAmount())
			{
				EnableWindow(GetDlgItem(hWnd_LobbyDialog, IDC_JOINGAME), FALSE);
			}
			break;
	}
	
	// Pass All Unhandled Messages To DefWindowProc
	return DefWindowProc(hWnd,uMsg,wParam,lParam);
}

//-----------------------------------------------------------------------------
// Name: empty()
// Desc: 
//-----------------------------------------------------------------------------
LRESULT CALLBACK CreateViewPlayersDialogProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDC_STARTGAME:
			Lobby.GetLocalGame()->SendStartGame();
			Lobby.SendStartGame(Lobby.GetLocalGame()->GetGameIndex());

			DestroyWindow(hWnd_CreateViewPlayersDialog);
			break;

		case IDC_CANCELGAME:
			Lobby.SendRemoveGame(Lobby.GetLocalGame()->GetName());

			DestroyWindow(hWnd_CreateViewPlayersDialog);
			break;

		default:
			break;
		}

		break;
	}

	return 0;
}

//-----------------------------------------------------------------------------
// Name: empty()
// Desc: 
//-----------------------------------------------------------------------------
LRESULT CALLBACK CreateGameDialogProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	char gamename[32];

	switch (uMsg)
	{
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDC_DOCREATEGAME:
			GetWindowText(GetDlgItem(hWnd_CreateGameDialog, IDC_GAMENAME), gamename, 32);
			DestroyWindow(hWnd_CreateGameDialog);

			hWnd_CreateViewPlayersDialog = CreateDialog(hInst, MAKEINTRESOURCE(IDD_CREATEVIEWPLAYERS),
				hWnd_Application, (DLGPROC) CreateViewPlayersDialogProc);

			ShowWindow(hWnd_CreateViewPlayersDialog, SW_SHOW);

			Lobby.SendCreateGame(gamename);
			break;

		case IDC_CANCELCREATEGAME:
			DestroyWindow(hWnd_CreateGameDialog);
			break;

		default:
			if(SendMessage(GetDlgItem(hWnd_CreateGameDialog, IDC_GAMENAME),
				EM_GETMODIFY, 0, 0))
			{
				GetWindowText(GetDlgItem(hWnd_CreateGameDialog, IDC_GAMENAME),
					gamename, 32);

				if(strcmp(gamename, "") == 0)
				{
					EnableWindow(GetDlgItem(hWnd_CreateGameDialog, IDC_DOCREATEGAME), FALSE);
				}
				else
				{
					EnableWindow(GetDlgItem(hWnd_CreateGameDialog, IDC_DOCREATEGAME), TRUE);
				}
			}

			break;
		}

		break;
	}
	
	return 0;
}

//-----------------------------------------------------------------------------
// Name: empty()
// Desc: 
//-----------------------------------------------------------------------------
LRESULT CALLBACK JoinGameDialogProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDC_JOINCANCEL:
			Lobby.GetLocalGame()->Disconnect();

			DestroyWindow(hWnd_JoinGameDialog);
			break;

		default:
			break;
		}

		break;
	}
	
	return 0;
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
	char nickname[30];
	char password[50];
	int ret;

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
					// -> First get the IP address of the server from the dialog
					GetDlgItemText(hWnd_LoginDialog, IDC_LOGIN_IPADDRESS, serverIP, 16);

					// -> Store the player data in local variables
					GetDlgItemText(hWnd_LoginDialog, IDC_LOGIN_NICKNAME, nickname, 30);
					GetDlgItemText(hWnd_LoginDialog, IDC_LOGIN_PASSWORD, password, 50);

					// -> Hide the Login Window
					ShowWindow(hWnd_LoginDialog, SW_HIDE);

					ret = Lobby.GetNetworkClient()->Initialise("", serverIP, 30003);

					if(ret == DREAMSOCK_CLIENT_ERROR)
					{
						char text[64];
						sprintf(text, "Could not open client socket");

						MessageBox(NULL, text, "Error", MB_OK);
					}

					Lobby.Connect(nickname, password);

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
LRESULT CALLBACK LobbyDialogProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	char chatMessage[256];
	char temp[256];
	int selectedGame;

	int i;
	CArmyWar *list;

	switch (uMsg)
	{
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDC_LOGOUT:
			Lobby.Disconnect();

			// Hide the Lobby
			ShowWindow(hWnd_LobbyDialog, SW_HIDE);
			// Re-Create the LoginDialog
			hWnd_LoginDialog = CreateDialog(hInst,MAKEINTRESOURCE(IDD_LOGINDIALOG),hWnd_Application,(DLGPROC)LoginDialogProc);
			break;

		case IDC_SENDCHATMESSAGE:
			GetWindowText(GetDlgItem(hWnd_LobbyDialog, IDC_CHATMESSAGE), temp, 255);

			sprintf(chatMessage, "%s: ", Lobby.GetLocalClient()->nickname);
			strcat(chatMessage, temp);

			Lobby.SendChat(chatMessage);
			
			SetWindowText(GetDlgItem(hWnd_LobbyDialog, IDC_CHATMESSAGE), "");
			break;

		case IDC_CREATEGAME:
			hWnd_CreateGameDialog = CreateDialog(hInst, MAKEINTRESOURCE(IDD_CREATEGAME),
			hWnd_Application, (DLGPROC) CreateGameDialogProc);

			ShowWindow(hWnd_CreateGameDialog, SW_SHOW);
			break;

		case IDC_JOINGAME:
			selectedGame = SendMessage(GetDlgItem(hWnd_LobbyDialog, IDC_GAMELIST), LB_GETCURSEL, 0, 0);

			list = Lobby.GetGameList();

			for(i = 0; i < selectedGame; i++)
			{
				list = list->next;
			}

			if(list->GetInProgress())
				break;

			LogString("Joining game %d", selectedGame);

			list->StartConnection(selectedGame);
			Lobby.SetLocalGame(list);

			hWnd_JoinGameDialog = CreateDialog(hInst, MAKEINTRESOURCE(IDD_JOINVIEWPLAYERS),
				hWnd_Application, (DLGPROC) JoinGameDialogProc);

			ShowWindow(hWnd_JoinGameDialog, SW_SHOW);

			break;

		default:
			int count = SendMessage(GetDlgItem(hWnd_LobbyDialog, IDC_GAMELIST),
				LB_GETSELCOUNT, 0, 0);

			if(count)
			{
				int sel = SendMessage(GetDlgItem(hWnd_LobbyDialog, IDC_GAMELIST),
					LB_GETCURSEL, 0, 0);

				list = Lobby.GetGameList();

				for(i = 0; i < sel; i++)
				{
					list = list->next;
				}

				if( (sel > -1) && (!list->GetInProgress()) )
				{
					EnableWindow(GetDlgItem(hWnd_LobbyDialog, IDC_JOINGAME), TRUE);
				}
			}
			else
			{
				EnableWindow(GetDlgItem(hWnd_LobbyDialog, IDC_JOINGAME), FALSE);
			}

			break;
		}

		break;

	case WM_CLOSE:
		Lobby.Disconnect();
		PostQuitMessage(0);
		break;
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

	// Create the Lobby
	hWnd_LobbyDialog = CreateDialog(hInst, MAKEINTRESOURCE(IDD_LOBBYDIALOG),
		hWnd_Application, (DLGPROC)LobbyDialogProc);

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
					Lobby.Disconnect();
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

			// Run lobby and sign-in network
			Lobby.RunNetwork(time);
			Signin.RunNetwork(time);

			// If we have a local game, run the frames for it
			if(Lobby.GetLocalGame() != NULL)
			{
				Lobby.GetLocalGame()->RunNetwork(time);
				Lobby.GetLocalGame()->CheckKeys();
				Lobby.GetLocalGame()->Frame();
			}

			oldTime = newTime;
		}
	}
	catch(...)
	{
		if(Lobby.GetLocalGame() != NULL)
			Lobby.GetLocalGame()->Shutdown();

		Lobby.Disconnect();
		Signin.Disconnect();

		LogString("Unknown Exception caught in main loop");

		MessageBox(NULL, "Unknown Exception caught in main loop", "Error", MB_OK | MB_TASKMODAL);

		return -1;
	}

	return msg.wParam;
}
