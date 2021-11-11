#include "persys.h"

void		StartupFolder(S_args* params)
{
	wchar_t* ppszPath;
	BOOL	bFailIfExists = 0;
	wchar_t* dstFile;

	printf("[*] Starting StartupFolder module...\n");
	if (SUCCEEDED(SHGetKnownFolderPath(&FOLDERID_Startup, 0, 0, &ppszPath)))
	{
		if (GetFileAttributesW(ppszPath) == INVALID_FILE_ATTRIBUTES)
			fprintf(stderr, "Error : invalid startup folder\n");
		else
		{
			dstFile = malloc(sizeof(wchar_t*) * (wcslen(ppszPath) +
				wcslen(params->fileName) + 2));
			if (dstFile != NULL)
			{
				wmemset(dstFile, 0, (wcslen(ppszPath) +
					wcslen(params->fileName) + 2));
				wcscpy(dstFile, ppszPath);
				wcscat(dstFile, L"\\");
				wcscat(dstFile, params->fileName);
				if (CopyFileW(params->filePath, dstFile, bFailIfExists) != 0)
				{
					wprintf(L"	[+] %ls file successfully copied to startup folder.\n",
						params->fileName);
					wprintf(L"		[i] %ls\n", dstFile);
				}
				else
					fprintf(stderr, "Error : File copy to startup folder.\n");
				free(dstFile);
			}
			else
				fprintf(stderr, "Error : memory allocation.\n");
		}
	}
	else
		fprintf(stderr, "Error : startup folder not found.\n");
}