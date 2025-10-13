// Занятие 3. Асинхронное программирование. Задание 2. For_each
#include <iostream>
#include <chrono>
#include <future>

template<typename Iterator,typename Func>
void parallel_for_each(Iterator first, Iterator last, Func f)
{
	const auto min_size = 10;
	auto size = distance(first, last);
	if (size <= min_size)
	{
		std::for_each(first, last, std::ref(f));
		return;
	}
	
	Iterator mid_p = first;
	advance(mid_p, size / 2);
	auto first_half = std::async(parallel_for_each <Iterator, Func>, first, mid_p, std::ref(f));
	//parallel_for_each(mid_p, last, std::ref(f));
	auto second_half = std::async(parallel_for_each <Iterator, Func>, mid_p, last, std::ref(f));
}

void change_element(int& n) 
{
	n += 1;
}
void print_element(int n)
{
	std::cout << n << "   ";
}

int main()
{	
	// Работа с обычным for_each
	auto start = std::chrono::high_resolution_clock::now();
	std::vector<int> v1(100, 5);
	std::for_each(v1.begin(), v1.end(), change_element);
	std::for_each(v1.begin(), v1.end(), print_element);
	auto end = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double, std::milli> time = end - start;
	std::cout << "\nfor_each time = " << time.count() << std::endl;
	
	std::cout << std::endl;
	
	// Работа с параллельным вариантом for_each
	auto start1 = std::chrono::high_resolution_clock::now();
	std::vector<int> v2(100, 5);
	parallel_for_each(v2.begin(), v2.end(), change_element);
    std::for_each(v2.begin(), v2.end(), print_element);
	auto end1 = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double, std::milli>time1 = end1 - start1;
	std::cout << "\nparallel_for_each time= " << time1.count() << std::endl;
	
	return EXIT_SUCCESS;
}