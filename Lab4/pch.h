#include <iostream>
#include "locale.h"
#include <stdio.h>
#include "windows.h"

void ShowError();
void Archiving(TCHAR* tFilePath, TCHAR* tArchivePath);
void Extracting(TCHAR* tArchivePath, TCHAR* tExtractArchivePath);
void CreateNew7ZipProcess(TCHAR* tCmdForNewProcess);