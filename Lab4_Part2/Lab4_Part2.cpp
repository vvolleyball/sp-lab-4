#include "pch.h"

using namespace std;

int gdwTlsIndex; // index for Tls memeory
CRITICAL_SECTION gCriticalSection; // critical section
HANDLE* threadHandles; // handles of creat threads

int main()
{
    setlocale(LC_ALL, "RUS");
    int countOfThreads = 0;
    cout << "Введите количество потоков: ";
	cin >> countOfThreads;
	if (countOfThreads > 0)
	{
		// create array for containing array for each new thread
		int** arrOfArraysForNewThread;
		arrOfArraysForNewThread = (int**)malloc(sizeof(int**) * countOfThreads);

		for (int i = 0; i < countOfThreads; i++)
		{
			arrOfArraysForNewThread[i] = (int*)malloc(sizeof(int) * ARRAY_SIZE);
			GenereteArray(arrOfArraysForNewThread[i]);
		}

		InitializeCriticalSection(&gCriticalSection);
		// alloc index in tls memory
		gdwTlsIndex = TlsAlloc();
		// alloc memory for new threads handles 
		threadHandles = (HANDLE*)malloc(sizeof(HANDLE) * countOfThreads);
		// alloc memory for ID of new threads
		DWORD* dwThreadsID = (DWORD*)malloc(sizeof(DWORD) * countOfThreads);

		// create new threads
		for (int i = 0; i < countOfThreads; i++)
			threadHandles[i] = CreateThread(NULL, NULL, StartNewThread, 
				arrOfArraysForNewThread[i], NULL, &dwThreadsID[i]);

		// wait until all threads complete
		WaitForMultipleObjects(countOfThreads, threadHandles, TRUE, INFINITE);
	}
}

DWORD WINAPI StartNewThread(LPVOID param)
{
	// only 1 thread in one moment can execute this code
	EnterCriticalSection(&gCriticalSection);
	// create memory for max divisors
	int maxDivisors[ARRAY_SIZE] = { 1 };
	// save pointer to array with max divisors in tls memory
	TlsSetValue(gdwTlsIndex, maxDivisors);

	// output all info about thread
	cout << "\n-------------------- ID: " << GetCurrentThreadId() << " --------------------" << endl;
	// print source array
	cout << "Исходный массив:     ";
	PrintArray((int*)param, ARRAY_SIZE);

	// calculate largest divisor for each value in array
	for (int i = 0; i < ARRAY_SIZE; i++)
		maxDivisors[i] = CalculateLargestDivisor(((int*)param)[i]);
	// print array of largest divisors from TLS memory
	cout << "Наибольший делитель: ";
	PrintArray((int*)TlsGetValue(gdwTlsIndex), ARRAY_SIZE);

	// print sum of all array values
	cout << "Сумма всех элементов: " << CalculateSumOfArrayValues((int*)param) << endl;
	cout << "---------------------------------------------------" << endl;

	// allow other threads to execute this code
	LeaveCriticalSection(&gCriticalSection);
	return 0;
}

// print array
void PrintArray(int* arr, int length)
{
	for (int i = 0; i < length; i++)
		cout << " " << arr[i] << " ";
	cout << endl;
}

// generate array
void GenereteArray(int* arr)
{
	int randRange = ARRAY_VALUES_MAX - ARRAY_VALUES_MIN;
	for (int i = 0; i < ARRAY_SIZE; i++)
		arr[i] = (rand() % randRange) + ARRAY_VALUES_MIN;
}

// calculate largest divisor
int CalculateLargestDivisor(int num)
{
	int halfNum = ceil(num / 2), res = 1;
	for (int i = 2; i <= halfNum; i++)
	{
		if (num % i == 0)
			res = i;
	}
	return res;
}

// calculate sum of array values
int CalculateSumOfArrayValues(int* arr)
{
	int sum = arr[0];
	for (int i = 1; i < ARRAY_SIZE; i++)
		sum += arr[i];
	return sum;
}