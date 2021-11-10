#include "persys.h"

const modules mods[] = {
	{L"startupfolder", StartupFolder},
	{L"shortcutmodif", ShortcutModif},
	{L"regRunKeys", RegRunKeys},
	{L"logonscript", LogonScript},
	{L"screensaver", Screensaver},
	{L"psprofile", PsProfile},
	{L"comhijacking", ComHijacking},
	{L"schtask", SchTask},
	//{L"schtaskmodif", SchTaskmodif},
	{NULL, NULL}
};

void CheckModules(S_args* params)
{
	wchar_t* module = wcstok(params->modules, L",", 0);
	int			mod_found = 0;
	while (module != NULL)
	{
		for (int i = 0; mods[i].mName != NULL; i++)
		{
			if (wcsicmp(mods[i].mName, module) == 0)
			{
				mods[i].fctPtr(params);
				mod_found = 1;
			}
		}
		if (mod_found == 0)
			fwprintf(stderr, L"Error : module %ls not found.\n", module);
		mod_found = 1;
		module = wcstok(NULL, L",", 0);
	}
}

void Usage(void)
{
	printf("Usage : Persys.exe [-h] [-t [Persistence module (comma " \
		"separated)]] FILE (exe,dll,ps1,bat,vbs)\n\n" \
		"- Low privileges modules :\n\n" \
		"\t* startupfolder - The file is copied to the startup folder\n" \
		"\t* regrunkeys - Adds a new registry run key\n" \
		"\t* shortcutmodif - Modifies a specific shortcut (LNK file)\n" \
		"\t* screensaver - Adds a new SCRNSAVE.EXE registry value\n" \
		"\t* psprofile - Adds a new Powershell profile\n" \
		"\t* logonscript - Adds a new UserInitMprLogonScript registry value\n" \
		"\t* comhijacking - Hijacks a specific COM object\n\n" \
		"- Admin level modules :\n\n" \
		"\t* schtask - Creates a scheduled task in order to execute the file\n"\
		"\t* service - Creates or reconfigure a service in order to execute the file\n"\
		"\t* ...\n\n"
	);
}

void CheckOpts(int argc, wchar_t** argv)
{
	DWORD fileAttr;
	S_args* params;
	HANDLE hFile;

	if ((wcscmp(argv[1], L"-t") == 0) && argc == 4)
	{
		params = malloc(sizeof(S_args));
		if (params == NULL)
		{
			fprintf(stderr, "Error : memory allocation\n");
			exit(EXIT_FAILURE);
		}
		fileAttr = GetFileAttributesW(argv[3]);
		if ((fileAttr == INVALID_FILE_ATTRIBUTES) ||
			(fileAttr == FILE_ATTRIBUTE_DIRECTORY))
			fwprintf(stderr, L"Error : incorrect file (%ls).\n", argv[3]);
		else
		{
			params->modules = argv[2];
			hFile = CreateFileW(argv[3], GENERIC_READ, 0, NULL, OPEN_EXISTING,
				FILE_ATTRIBUTE_NORMAL, NULL);
			if (hFile != INVALID_HANDLE_VALUE)
			{
				wmemset(params->filePath, 0, MAX_PATH);
				GetFullPathNameW(argv[3], MAX_PATH, params->filePath, NULL);
				params->fileName = PathFindFileNameW(params->filePath);
				CloseHandle(hFile);
				CheckModules(params);
			}
			else
				fprintf(stderr, "Error : opening the file.\n");
		}
		free(params);
	}
	else
		Usage();
}

void	Banner(void)
{
	printf("Persys %s - Slim Meriah (@shab4z)\
		\nWindows Persistence Toolkit 2021\n\n", VERSION);
}

int	wmain(int argc, wchar_t** argv)
{
	Banner();
	if (argc > 1)
		CheckOpts(argc, argv);
	else
		Usage();
	return 0;
}