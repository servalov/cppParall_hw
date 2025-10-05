// Конкуренция, состояние гонки. Задание 2. Прогресс-бар
#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>
#include <cstdlib>
#include <random>
#include "windows.h"

HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
using namespace std::chrono_literals;

// устанавливает курсор в консоли на указанную позицию
void SetPosition(int x, int y)
{
	COORD point;
	point.X = x;
	point.Y = y;
	SetConsoleCursorPosition(hStdOut, point);
}

// отрисовка progress в консоли по координатам
void progress_bar(int x, int y, int width, double progress, char symbol)
{
	int fill_progress = static_cast<int>(width * progress);
	SetPosition(x, y);
	for (int i = 0; i < fill_progress; ++i)
	{
		std::cout << symbol;
	}
}

std::once_flag flag;

void print_once(int n)
{
	std::cout << "ind " << "     Progress_bar     " << "   Time_Progress_bar " << std::endl;

	for (int i{};i<n;++i)
	{
		std::cout << i + 1 << ": " << std::endl;
	}
}

thread_local int ind_progress;
thread_local int progress;
thread_local double time_progressbar;
std::mutex mtx;

void drawProgressBar(int ind_thread,int num_thread)
{
	std::call_once(flag, print_once,num_thread);
	

	int count{ 100 };
	int width{ 20 };
	double progress = 0;
	int x_pos{ 6 }, y_pos{ 6 };

	ind_progress = 0;

	std::mt19937 gen{ std::random_device()() };
	std::uniform_int_distribution<> distrib(0, 5);

	auto start_time = std::chrono::steady_clock::now();
	do
	{
		
		std::unique_lock<std::mutex> lk(mtx);
		ind_progress += 1 + distrib(gen);
		if (ind_progress > count)
		{
			ind_progress = count;
		}
		progress = static_cast<double>(ind_progress) / count;
		progress_bar(y_pos, ind_thread + 1, width, progress, 219);

		if (progress >= 1)
		{
			auto end_time = std::chrono::steady_clock::now();
			std::chrono::duration<double>time_prog = end_time - start_time;
			time_progressbar = time_prog.count();

			SetPosition(y_pos + width + 5, ind_thread + 1);
			std::cout << time_progressbar << "s ";
		}

		lk.unlock();
		std::this_thread::sleep_for(std::chrono::milliseconds(450));

	} while (progress < 1);
}

int main()
{
	SetConsoleCP(1251);
	int n{10};
	std::thread* t = new std::thread[n];
	for (int i{}; i < n; ++i)
	{
		 t[i]=std::thread(drawProgressBar, i, n);
	}

	for (int i{}; i < n; ++i)
	{
		t[i].join();
	}
	
	delete[] t;

	SetPosition(0, n+2);
	return EXIT_SUCCESS;
}