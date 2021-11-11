#include "persys.h"

BOOL	InitTask(wchar_t* taskname, ITaskScheduler** pITS, ITask** pITask)
{
	HRESULT	hr;

	hr = CoInitialize(NULL);
	if (SUCCEEDED(hr))
	{
		hr = CoCreateInstance(&CLSID_CTaskScheduler, NULL,
			CLSCTX_INPROC_SERVER, &IID_ITaskScheduler, (void**)&(*pITS));
		if (SUCCEEDED(hr))
		{
			hr = (*pITS)->lpVtbl->NewWorkItem(*pITS, taskname,
				&CLSID_CTask, &IID_ITask, (IUnknown**)&(*pITask));
			if (SUCCEEDED(hr))
			{
				(*pITS)->lpVtbl->Release(*pITS);
				return TRUE;
			}
			else
				CoUninitialize();
		}
		else
			CoUninitialize();
	}
	return FALSE;
}

ITaskTrigger* ConfigTaskTrigger(ITask** pITask)
{
	TASK_TRIGGER pTrigger;
	ITaskTrigger* pITaskTrigger;
	WORD piNewTrigger;
	HRESULT	hr;

	ZeroMemory(&pTrigger, sizeof(TASK_TRIGGER));
	pTrigger.wBeginDay = 1;
	pTrigger.wBeginMonth = 1;
	pTrigger.wBeginYear = 1999;
	pTrigger.cbTriggerSize = sizeof(TASK_TRIGGER);
	pTrigger.TriggerType = TASK_EVENT_TRIGGER_AT_LOGON;
	hr = (*pITask)->lpVtbl->CreateTrigger(*pITask, &piNewTrigger, &pITaskTrigger);
	hr = pITaskTrigger->lpVtbl->SetTrigger(pITaskTrigger, &pTrigger);
	if (SUCCEEDED(hr))
		return pITaskTrigger;
	else
	{
		(*pITask)->lpVtbl->Release(*pITask);
		pITaskTrigger->lpVtbl->Release(pITaskTrigger);
		CoUninitialize();
		return NULL;
	}
}

BOOL	ConfigTaskAttr(wchar_t* filePath, ITask** pITask)
{
	HRESULT	hr;

	hr = (*pITask)->lpVtbl->SetApplicationName(*pITask, filePath);
	hr = (*pITask)->lpVtbl->SetFlags(*pITask, TASK_FLAG_RUN_ONLY_IF_LOGGED_ON);
	hr = (*pITask)->lpVtbl->SetAccountInformation(*pITask, L"audit_admin", NULL);
	if (FAILED(hr))
	{
		(*pITask)->lpVtbl->Release(*pITask);
		return FALSE;
	}
	return TRUE;
}

BOOL	SaveTask(ITask** pITask, ITaskTrigger** pITaskTrigger)
{
	IPersistFile* pIPersistFile;
	HRESULT	hr;

	hr = (*pITask)->lpVtbl->QueryInterface(*pITask, &IID_IPersistFile, (void**)&pIPersistFile);
	hr = pIPersistFile->lpVtbl->Save(pIPersistFile, NULL, TRUE);
	if (SUCCEEDED(hr))
	{
		pIPersistFile->lpVtbl->Release(pIPersistFile);
		(*pITask)->lpVtbl->Release(*pITask);
		(*pITaskTrigger)->lpVtbl->Release(*pITaskTrigger);
		pIPersistFile->lpVtbl->Release(pIPersistFile);
		CoUninitialize();
		return TRUE;
	}
	else
	{
		(*pITask)->lpVtbl->Release(*pITask);
		(*pITaskTrigger)->lpVtbl->Release(*pITaskTrigger);
		pIPersistFile->lpVtbl->Release(pIPersistFile);
		CoUninitialize();
		return FALSE;
	}
}

BOOL    CreateTask(wchar_t* filePath, wchar_t* taskname)
{
	ITaskTrigger* pITaskTrigger;
	ITaskScheduler* pITS;
	ITask* pITask;
	HRESULT	hr = S_OK;

	if (InitTask(taskname, &pITS, &pITask))
	{
		if ((pITaskTrigger = ConfigTaskTrigger(&pITask)) != NULL)
		{
			if (ConfigTaskAttr(filePath, &pITask))
			{
				if (SaveTask(&pITask, &pITaskTrigger))
				{
					wprintf(L"[i] Task '%ls' successfully created.\n", taskname);
					return TRUE;
				}
				else
				{
					printf("Error : saving the scheduled task.\n");
					return FALSE;
				}
			}
			else
			{
				fprintf(stderr, "Error : task attributes configuration failed.\n");
				pITS->lpVtbl->Release(pITS);
				pITaskTrigger->lpVtbl->Release(pITaskTrigger);
				CoUninitialize();
				return FALSE;
			}
		}
		else
		{
			fprintf(stderr, "Error : task trigger configuration failed.\n");
			return FALSE;
		}
	}
	else
	{
		fprintf(stderr, "Error : task initialization failed.\n");
		return FALSE;
	}
}

void	SchTask(S_args* params)
{
	printf("[*] Starting SchTask module...\n");
	if (CreateTask(params->filePath, L"Persys4") == FALSE)
		fwprintf(stderr, L"Error creating scheduled task\n");
}