#pragma once
#include<iomanip>
#include <iostream>
#include <Windows.h>
using namespace std;

void TimeSpend(double start) {
	double end;
	int total_time;
	end = clock();
	total_time = (int)(end - start);
	int hour = total_time / (CLOCKS_PER_SEC * 60 * 60);
	int min = (total_time % (CLOCKS_PER_SEC * 60 * 60)) / (CLOCKS_PER_SEC * 60);
	double sec = (total_time % (CLOCKS_PER_SEC * 60)) / (double)CLOCKS_PER_SEC;
	cout << hour << ":" << min << ":" << fixed << setprecision(3) << sec << "\n\n";
}