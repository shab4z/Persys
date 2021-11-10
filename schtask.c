#include "persys.h"

BOOL    CreateTask(S_args* params, wchar_t* taskname)
{
	TASK_TRIGGER pTrigger;
	ITaskTrigger* pITaskTrigger;
	IPersistFile* pIPersistFile;
	ITaskScheduler* pITS;
	ITask* pITask;
	HRESULT	hr = S_OK;
	WORD piNewTrigger;

	hr = CoInitialize(NULL);
	if (SUCCEEDED(hr))
	{
		hr = CoCreateInstance(&CLSID_CTaskScheduler, NULL,
			CLSCTX_INPROC_SERVER, &IID_ITaskScheduler, (void**)&pITS);
		if (FAILED(hr))
		{
			fprintf(stderr, "Error : CoCreateInstance function.\n");
			CoUninitialize();
			return FALSE;
		}
		else
		{
			hr = pITS->lpVtbl->NewWorkItem(pITS, taskname, &CLSID_CTask,
				&IID_ITask, (IUnknown**)&pITask);
			pITS->lpVtbl->Release(pITS);
			if (FAILED(hr))
			{
				wprintf(L"Failed calling ITaskScheduler::Activate: ");
				wprintf(L"error = 0x%x\n", hr);
				CoUninitialize();
				return FALSE;
			}
			ZeroMemory(&pTrigger, sizeof(TASK_TRIGGER));
			pTrigger.wBeginDay = 1;
			pTrigger.wBeginMonth = 1;
			pTrigger.wBeginYear = 1999;
			pTrigger.cbTriggerSize = sizeof(TASK_TRIGGER);
			pTrigger.TriggerType = TASK_EVENT_TRIGGER_AT_LOGON;
			pTrigger.Type.Daily.DaysInterval = 1;
			hr = pITask->lpVtbl->CreateTrigger(pITask, &piNewTrigger, &pITaskTrigger);
			hr = pITaskTrigger->lpVtbl->SetTrigger(pITaskTrigger, &pTrigger);
			if (FAILED(hr))
			{
				fprintf(stderr, "Error : SetTrigger function.\n");
				pITask->lpVtbl->Release(pITask);
				pITaskTrigger->lpVtbl->Release(pITaskTrigger);
				CoUninitialize();
				return FALSE;
			}
			else
			{
				hr = pITask->lpVtbl->QueryInterface(pITask, &IID_IPersistFile, (void**)&pIPersistFile);
				hr = pIPersistFile->lpVtbl->Save(pIPersistFile, NULL, TRUE);
				hr = pITask->lpVtbl->SetApplicationName(pITask, params->filePath);
				if (FAILED(hr))
				{
					fprintf(stderr, "Error : SetApplicationName function.\n");
					pITask->lpVtbl->Release(pITask);
					pITaskTrigger->lpVtbl->Release(pITaskTrigger);
					pIPersistFile->lpVtbl->Release(pIPersistFile);
					CoUninitialize();
					return FALSE;
				}
				else
				{
					pITask->lpVtbl->Release(pITask);
					pITaskTrigger->lpVtbl->Release(pITaskTrigger);
					pIPersistFile->lpVtbl->Release(pIPersistFile);
					CoUninitialize();
					wprintf(L"[i] Task '%ls' successfully created.\n", taskname);
				}
			}
		}
	}
	return TRUE;
}

void	SchTask(S_args* params)
{
	printf("[*] Starting SchTask module...\n");
	if (CreateTask(params, L"TESTSLIM2") == FALSE)
	{
		fwprintf(stderr, L"Error creating scheduled task\n");
	}
}