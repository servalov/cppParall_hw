// Многопоточность. Задание 2. Параллельные вычисления
#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <random>
#include <algorithm>
#include <mutex>

std::once_flag flag;
void print_once()
{
	std::cout << "Количество аппаратных ядер: " << std::thread::hardware_concurrency() << std::endl;
}

// функция сложения двух векторов
void sum(std::vector<int>& a,std::vector<int>& b,std::vector<int>& c, int start,int end)
{
	std::call_once(flag, print_once);
	
	if (a.size() == b.size())
	{
		for (int i{start}; i < end; ++i)
		{
			c[i] = a[i] + b[i];
		}
	}
	else
	{
		std::cout << "Ошибка. Размерности двух векторов не совпадают!!!" << std::endl;
	}
}

int main()
{
	setlocale(LC_ALL,"Russian");
	int n{ 5 };        // количество разбиений на потоки 1,2,4,8,16
	int m{ 4 };        // количество массивов

	std::vector<std::vector<double>> res(n, std::vector<double>(m));   // матрица результатов
	
	// Создаем массивы максимального размера
	int k = pow(10, m) * 1000;
	std::vector<int>a(k);
	std::vector<int>b(k);
	std::vector<int>c(k);

	// Заполнение массивов случайными числами
	std::mt19937 gen{std::random_device()()}, gen2{ std::random_device()() };
	std::uniform_int_distribution<int> dis(0, 1000);
	auto rand_num([=]() mutable {return dis(gen); });
	generate(a.begin(), a.end(), rand_num);

	std::uniform_int_distribution<int> dis2(0, 1000);
	auto rand_num2([=]() mutable {return dis2(gen2); });
	generate(b.begin(), b.end(), rand_num2);
	
	std::vector<std::thread> threads;
	int num_thread{};
	int start{}, end{}, size{}, part_size{};

	// Цикл для работы с данными размерностью 1000, 10000, 100000, 1000000 элементов
	for (int h{}; h < m; ++h)
	{
		size= pow(10, h) * 1000;
		// Цикл по разбиениям на потоки
		for (int i{}; i < n; ++i)
		{
			num_thread = (i == 0) ? 1 : pow(2, i);
			part_size = size / num_thread;

			auto start_time = std::chrono::steady_clock::now();
			for (int j{}; j < num_thread; ++j)
			{
				start = j * part_size;
				end = (j == num_thread - 1) ? size : start + part_size;
				threads.push_back(std::thread(sum, std::ref(a), std::ref(b), std::ref(c), start, end));
			}

			// Ожидание завершения всех потоков
			for (std::thread& t : threads) 
			{
				t.join();
			}
			
			auto end_time = std::chrono::steady_clock::now();
			std::chrono::duration<double>time = end_time - start_time;
			
			// Запись результатов в матрицу
			res[i][h] = time.count();
			//std::cout << "Массив размерностью " << size << " элементов c разбиением на " << num_thread << " потоков отработал за " << res[i][h] <<"s"<<std::endl;

			// Освобождаем вектор потоков
			threads.clear();

			// Обнуляем результирующий массив
			fill(c.begin(), c.end(), 0);
		}
	}

	// Вывод результатов в консоль
	std::cout << "\nРезультаты вычислений суммы двух векторов для массивов из 1000, 10000, 100000, 1000000 элементов:" << std::endl;
	std::cout << "\t\t" << "1000" << "\t\t" << "10000" << "\t\t" << "100000" << "\t\t" << "1000000" << std::endl;
	for (int i{}; i < n; ++i)
	{
		num_thread = (i == 0) ? 1 : pow(2, i);
		std::cout << num_thread << " потоков:\t";
		for (int j{}; j < m; ++j)
		{
			std::cout << res[i][j] <<"s\t";
		}
		std::cout << std::endl;
	}

	// Нахождение количества потоков с самым бытрым результатом
	std::cout << "\nЧисло потоков с самым быстрым результатом:" << std::endl;
	
	int num_thread_fast{}, ind{};
	for (int i{}; i < m; ++i)
	{
		size = pow(10, i) * 1000;
		ind = 0;
		for (int j{1}; j < n; ++j)
		{
			if (res[j][i] < res[ind][i])
			{
				ind = j;
			}
		}
		num_thread_fast = (ind == 0) ? 1 : pow(2, ind);
		std::cout << "Для массива из " << size << " элементов - " << num_thread_fast << " потока (" << res[ind][i]<<"s)"<< std::endl;
	}

	return EXIT_SUCCESS;
}