#include <iostream>
#include <stdio.h>
#include "locale.h"
#include "windows.h"

#define ARRAY_SIZE 7 // array size
#define ARRAY_VALUES_MIN 10 // rand array values from 10
#define ARRAY_VALUES_MAX 100 // rand array values from 100

DWORD WINAPI StartNewThread(LPVOID param); // start new thread
void GenereteArray(int* arr); // generate array
int CalculateLargestDivisor(int num); // calculate largest divisor
int CalculateSumOfArrayValues(int* arr); // calculate sum of array values
void PrintArray(int* arr, int length); // output all array