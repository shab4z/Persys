#include "persys.h"

void	setRegVal(S_args* params, PHKEY hKey, const wchar_t* regVal)
{
	if (RegSetValueExW(*hKey, regVal, 0, REG_SZ, (LPBYTE)params->filePath,
		((lstrlenW(params->filePath) + 1) * sizeof(wchar_t))) == ERROR_SUCCESS)
	{
		printf("	[+] Registry value successfully created\n");
		wprintf(L"		[i] %ls REG_SZ '%ls'\n", regVal, params->filePath);
		RegCloseKey(*hKey);
	}
	else
		fprintf(stderr, "Error: creating registry value\n");
}

void		RegRunKeys(S_args* params)
{
	HKEY hKeyRun;
	HKEY hKeyRunOnce;

	printf("[*] Starting RegRunKeys module...\n");
	if (RegOpenKeyExW(HKEY_CURRENT_USER,
		L"Software\\Microsoft\\Windows\\CurrentVersion\\Run",
		0, KEY_SET_VALUE, &hKeyRun) == ERROR_SUCCESS)
		setRegVal(params, &hKeyRun, L"Persys");
	else
		fprintf(stderr, "Error : opening current user run key.\n");
	if (RegOpenKeyExW(HKEY_CURRENT_USER,
		L"Software\\Microsoft\\Windows\\CurrentVersion\\RunOnce",
		0, KEY_SET_VALUE, &hKeyRunOnce) == ERROR_SUCCESS)
		setRegVal(params, &hKeyRunOnce, L"Persys");
	else
		fprintf(stderr, "Error : opening current user run once key.\n");
}