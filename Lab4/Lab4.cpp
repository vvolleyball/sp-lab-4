#include "pch.h"

using namespace std;

int main()
{
    setlocale(LC_ALL, "RUS");
    CHAR cKey;
    for (;;)
    {
        cout << "\n\t [MENU]" << endl;
        cout << " 1 - заархивировать" << endl;
        cout << " 2 - извлечь из архива" << endl;
        cout << " 3 - закончить работу" << endl;
        cout << "Выберите операцию: ";
        cin >> cKey;
		switch (cKey)
		{
			case '1': // archive files
			{
				TCHAR tPathToFiles[MAX_PATH] = { '\0' };
				TCHAR tPathToNewArchive[MAX_PATH] = { '\0' };

				cout << "\nВведите путь к файлам для архивации: ";
				scanf_s("%s", tPathToFiles, MAX_PATH);
				cout << "Введите путь для нового архива: ";
				scanf_s("%s", tPathToNewArchive, MAX_PATH);

				Archiving(tPathToFiles, tPathToNewArchive);
			} break;

			case '2': // extract files
			{
				TCHAR tPathToArchive[MAX_PATH] = { '\0' };
				TCHAR tPathToExtractArchive[MAX_PATH] = { '\0' };

				cout << "\nВведите путь к архиву с файлами: ";
				scanf_s("%s", tPathToArchive, MAX_PATH);
				cout << "Введите путь для извлечения файлов: ";
				scanf_s("%s", tPathToExtractArchive, MAX_PATH);

				Extracting(tPathToArchive, tPathToExtractArchive);
			} break;

			case '3': // exit
			{ return 0; } break;

			default: {
				cout << "\nВы ввели неверный ключ!";
			} continue;
		}
    }
}

void Archiving(TCHAR* tFilePath, TCHAR* tArchivePath)
{
	TCHAR tCmdForNewProcess[MAX_PATH] = { '\0' };
	// create a require command(to archive files) for new process
	printf("Command: ");
	snprintf(tCmdForNewProcess, MAX_PATH, "%s%s %s", TEXT("7z.exe a "), tArchivePath, tFilePath);
	cout << tCmdForNewProcess;
	CreateNew7ZipProcess(tCmdForNewProcess);
}

void Extracting(TCHAR* tArchivePath, TCHAR* tExtractArchivePath)
{
	TCHAR tCmdForNewProcess[MAX_PATH] = { '\0' };
	// create a require command(to extract files) for new process
	printf("Command: ");
	snprintf(tCmdForNewProcess, MAX_PATH, "%s%s -o\"%s\"", TEXT("7z.exe x "), tArchivePath, tExtractArchivePath);
	cout << tCmdForNewProcess;
	CreateNew7ZipProcess(tCmdForNewProcess);
}

void CreateNew7ZipProcess(TCHAR* tCmdForNewProcess)
{
	// server and client part of pipe
	HANDLE hServer = { NULL };
	HANDLE hClient = { NULL };

	// set default setting for SECURITY_ATTRIBUTES and enable inherit 
	SECURITY_ATTRIBUTES sa;
	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.bInheritHandle = TRUE;
	sa.lpSecurityDescriptor = NULL;

	// create pipe
	if ((!CreatePipe(&hClient, &hServer, &sa, NULL)) ||
		(hServer == INVALID_HANDLE_VALUE) || (hClient == INVALID_HANDLE_VALUE))
	{
		CloseHandle(hClient);
		CloseHandle(hServer);
		ShowError();
		return;
	}

	// set default settings for STARTUPINFO
	STARTUPINFO si = { NULL };
	si.cb = sizeof(si);
	si.dwFlags |= STARTF_USESTDHANDLES;
	// redirect stderr and stdout to pipe (hServer)
	si.hStdError = hServer;
	si.hStdOutput = hServer;

	PROCESS_INFORMATION pi = { NULL };

	// create process
	if (!CreateProcessA(NULL, tCmdForNewProcess, NULL, NULL, TRUE,
		ABOVE_NORMAL_PRIORITY_CLASS, NULL, NULL, &si, &pi))
	{
		CloseHandle(hClient);
		CloseHandle(hServer);
		ShowError();
		return;
	}

	// wait until the process complete
	WaitForSingleObject(pi.hProcess, INFINITE);
	// and terminate process
	TerminateProcess(pi.hProcess, 0);

	// close server part of pipe
	CloseHandle(hServer);
	// close child process handle
	CloseHandle(pi.hThread);
	CloseHandle(pi.hProcess);

	DWORD dwReadedBytes = 0;
	TCHAR tBuff[1024] = { '\0' };
	BOOLEAN bAlrightFlag = TRUE;

	// read all information from pipe
	while (ReadFile(hClient, tBuff, sizeof(tBuff) - 1, &dwReadedBytes, NULL) || (dwReadedBytes != 0))
	{
		printf("%s", tBuff);
		if (strstr(tBuff, "Everything is Ok") == NULL) bAlrightFlag = FALSE;
		memset(tBuff, '\0', sizeof(tBuff));
	}

	// error handling from pipe
	cout << "Выполнение родительского процесса: ";
	if (!bAlrightFlag)
		cout << "ОШИБКА" << endl;
	else
		cout << "УСПЕШНО" << endl;

	// close client part of pipe
	CloseHandle(hClient);
}

void ShowError()
{
	LPVOID lpMsgBuf;
	DWORD dwLastError = GetLastError();
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, dwLastError,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	printf("\n%s\n", lpMsgBuf);
	LocalFree(lpMsgBuf);
	return;
}