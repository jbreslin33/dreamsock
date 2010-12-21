#ifndef __TUTMAIN_H__
#define __TUTMAIN_H__

LRESULT CALLBACK ApplicationProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

extern char serverIP[32];

extern HINSTANCE hInst;
extern HWND hWnd_Application;
extern HWND hWnd_CreateAccountDialog;
extern HWND hWnd_LoginDialog;
extern HWND hWnd_LobbyDialog;

extern HWND hWnd_CreateGameDialog;
extern HWND hWnd_JoinGameDialog;

extern HWND hWnd_CreateViewPlayersDialog;

#endif

