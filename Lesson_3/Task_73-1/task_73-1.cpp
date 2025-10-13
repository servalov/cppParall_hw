//Занятие 3. «Асинхронное программирование». Задание 1.Сортировка выбором
#include <iostream>
#include <vector>
#include <random>
#include <future>
#include <thread>
#include <algorithm>

// поиск минимального элемента в векторе
void min_vecror(std::vector<int>& arr, int ind_begin, std::promise<int> prm)
{
	int min_ind = ind_begin;
	for (int i= ind_begin+1;i < arr.size(); ++i)
	{
		if (arr[i] < arr[min_ind])
		{
			min_ind = i;
		}
	}
	prm.set_value(min_ind);
}

void print(int value)
{
	std::cout << value << " ";
}

int main()
{
	setlocale(LC_ALL,"Russian");
	int n{ 10 };
	std::vector<int> arr(n);

	std::random_device rd;
	std::mt19937 gen{rd()};
	std::uniform_int_distribution<int> dis(0, 1000);
	auto rand_num([=]() mutable {return dis(gen); });
	generate(arr.begin(), arr.end(), rand_num);

	std::cout << "Исходный массив:" << std::endl;
	std::for_each(arr.begin(), arr.end(), print);

	for (int i{}; i < arr.size()-1; ++i)
	{
		std::promise<int> Promise;
		std::future<int> Future = Promise.get_future();
		auto res = std::async(min_vecror, std::ref(arr), i, move(Promise));

		Future.wait();
		std::swap(arr[i], arr[Future.get()]);
	}

	std::cout << "\nОтсортированный массив:" << std::endl;
	std::for_each(arr.begin(), arr.end(), print);
	
	return EXIT_SUCCESS;
}