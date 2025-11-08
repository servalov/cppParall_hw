// Курсовой проект "Потокобезопасная очередь"
#include <iostream>
#include <vector>
#include <mutex>
#include <queue>
#include <thread>
#include <condition_variable>
#include <future>
#include <chrono>

using type_task= std::packaged_task<void()>;

// Шаблонный класс для потокобезопасной очереди
template <class T>
class Safe_queue
{
	private:
		std::queue<T> task_queue;
		std::mutex mtx;
		std::condition_variable notice;
		bool stop_request{ false };

	public:
		Safe_queue()
		{ }

		// Метод push для добавлении задачи в очередь
		void push(T func)
		{
			std::unique_lock<std::mutex> lk(mtx);
			task_queue.emplace(std::move(func));
			std::cout << "В очередь добавлена новая задача!!!" << std::endl;
			lk.unlock();
			notice.notify_one();
		}

		// Метод pop для извлечения задачи из очереди
		bool pop(T& task)
		{
			std::unique_lock<std::mutex> lk(mtx);
			notice.wait(lk, [this]() {return !task_queue.empty()|| stop_request; });
			
			if (task_queue.empty()&& stop_request)
			{
				std::cout << "Процесс завершен. Очередь пуста. ";
				return false;
			}
			
			task = std::move(task_queue.front());
			task_queue.pop();
			lk.unlock();
			return true;
		}
		
		// Остановка потоков
		void stop_threads()
		{
			std::unique_lock<std::mutex> lk(mtx);
			stop_request = true;
			notice.notify_all();
		}

};

// Класс для реализации пула потоков
class Thread_pool
{
	private:
		std::vector<std::thread> threads_vector;
		Safe_queue<type_task> tasks_queue;
		size_t num_threads{};

	public:
		Thread_pool(size_t numThreads): num_threads(numThreads)
		{
			std::cout << "Вызов конструктора класса thread_pool. Создание потоков" << std::endl;
			std::cout << "Число рабочих потоков: " << num_threads << std::endl;
			threads_vector.reserve(num_threads);
			for (size_t i{}; i < num_threads; ++i)
			{
				threads_vector.emplace_back(std::thread(&Thread_pool::work,this));
			}
		}
		
		~Thread_pool()
		{
			std::cout << "\nВызов деструктора класса thread_pool: "<<std::endl;
			tasks_queue.stop_threads();
			for (size_t i{}; i < threads_vector.size(); ++i)
			{
				threads_vector[i].join();
			}
			
		}

		// Метод для выбора и исполнения задачи из очереди
		void work()
		{
			while (true)
			{
				type_task task;
				if (!tasks_queue.pop(task))
				{
					std::cout <<"Поток "<< std::this_thread::get_id() <<". "<<std::endl;
					break;
				}
				task();
			}
		}
		
		// Метод помещает в очередь задачу (вызов метода push класса Safe_queue)  
		auto submit(type_task task)
		{
			std::future<void> result=task.get_future();
			tasks_queue.push(std::move(task));
			return result;
		}

};

// Тестовая функция
void TestFunc1() 
{
	std::cout <<"Задача пользователя Алексея выполняется потоком "<< std::this_thread::get_id() << std::endl;
}

// Тестовая функция
void TestFunc2()
{
	std::cout << "Задача пользователя Михаила выполняется потоком " << std::this_thread::get_id() << std::endl;
}

int main()
{
	setlocale(LC_ALL,"Russian");

	std::cout << "Курсовой проект. Потокобезопасная очередь." << std::endl;
	std::cout << "------------------------------------------------" << std::endl;
	size_t numTread{ std::thread::hardware_concurrency()-6};
	Thread_pool tpool{ numTread };

	int n{};
	std::cout << "Введите количество задач (задачи от двух пользователей Михаила и Алексея): ";
	std::cin >> n;
	std::cout<< std::endl;
	
	std::vector<std::future<void>> results;
	results.reserve(n);
	for (int i{}; i < n; ++i)
	{
		std::this_thread::sleep_for(std::chrono::seconds(1));
		type_task task1(TestFunc1);
		type_task task2(TestFunc2);
		type_task task = rand() % 2 == 0 ? std::move(task1) : std::move(task2);
		results.emplace_back(tpool.submit(std::move(task)));
	}
	for (auto& result : results) 
	{
		result.get();  // Ждём завершения выполнения каждой задачи
	}
	return EXIT_SUCCESS;
}