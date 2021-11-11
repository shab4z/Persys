#include "persys.h"

void	SvcCreate(S_args* params)
{
    HMODULE module;
	SC_HANDLE schSCManager;
	SC_HANDLE schService;
    wchar_t svcName[] = L"PersysSvc";
    wchar_t szPath[MAX_PATH];

	printf("[*] Starting Service module...\n");
	
    module = LoadLibraryW(params->filePath);
    if (module != NULL)
    {
        if (!GetModuleFileNameW(module, szPath, MAX_PATH))
        {
            fprintf(stderr, "[*] Cannot install service (%d)\n",
                GetLastError());
        }
        else
        {
            schSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
            if (NULL == schSCManager)
                fprintf(stderr, "OpenSCManager failed (%d)\n", GetLastError());
            else
            {
                schService = CreateServiceW(schSCManager, svcName, svcName,
                    SERVICE_ALL_ACCESS, SERVICE_WIN32_OWN_PROCESS,
                    SERVICE_AUTO_START, SERVICE_ERROR_IGNORE,
                    szPath, NULL, NULL, NULL, NULL, NULL);
                if (schService == NULL)
                {
                    printf("CreateService failed (%d)\n", GetLastError());
                    CloseServiceHandle(schSCManager);
                    return;
                }
                else
                    printf("[i] Service installed successfully.\n");
                CloseServiceHandle(schService);
                CloseServiceHandle(schSCManager);
            }
        }
    }
}