// Многопоточность. Задание 1. Очередь клиентов
#include <iostream>
#include <thread>
#include <chrono>

using namespace std::chrono_literals;
int client_count{};

// поток для клиента
void client(int n)
{
	for (int i{}; i < n; ++i)
	{
		client_count += 1;
		std::cout << " Запрос клиента " << i + 1 << std::endl;
		std::this_thread::sleep_for(1s);
	}
}

// поток для операциониста
void operacianist()
{
	while (true)
	{
		std::this_thread::sleep_for(2s);
		client_count -= 1;
		std::cout << " Обработка операционистом запроса клиента. Число клиентов в оработке: " << client_count<< std::endl;
		if (client_count == 0)
		{
			std::cout << " Обслуживание клиентов завершено!!!" << std::endl;
			break;
		}
	}
}

int main()
{
	using namespace std::chrono_literals;
	setlocale(LC_ALL,"Russian");
	int n{};
	std::cout << std::thread::hardware_concurrency() << std::endl;
	std::cout << "Ввести количество запросов клиентов: ";
	std::cin >> n;
	
	std::thread t1(client, n);
	std::thread t2(operacianist);
	t1.join();
	t2.join();
	
	return EXIT_SUCCESS;
}