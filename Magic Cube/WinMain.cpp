#include <Windows.h>
#include "mApp.h"


int CALLBACK WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
	mApp mapp;
	mapp.Go();
	return 0;
}