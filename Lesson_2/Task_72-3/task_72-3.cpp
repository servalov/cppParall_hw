// Задание 3. Защищённый обмен данными
#include<iostream>
#include<thread>
#include<mutex>
#include<chrono>

std::mutex m1, m2;

class Data
{
private:
    int value{};
    std::mutex& mtx;

public:
    Data(int a, std::mutex& m) :value{ a }, mtx{ m }
    {

    }
    int get_Value()
    {
        return value;
    }
    void set_Value(int new_value)
    {
        value = new_value;
    }
    std::mutex& get_mutex()
    {
        return mtx;
    }

};

void swap_1(Data& A1, Data& A2)
{

    std::cout << "Start swap_1  " << std::endl;
    std::lock(A1.get_mutex(), A2.get_mutex());
    std::lock_guard<std::mutex> la{ A1.get_mutex(), std::adopt_lock };
    std::lock_guard<std::mutex> lb{ A2.get_mutex(), std::adopt_lock };

    int temp{};
    temp = A1.get_Value();
    A1.set_Value(A2.get_Value());
    A2.set_Value(temp);

    std::cout << "\nValues after swap_1:" << std::endl;
    std::cout << "  Value of class D1 =" << A1.get_Value() << std::endl;
    std::cout << "  Value of class D2 =" << A2.get_Value() << std::endl;

    std::cout << "End swap_1\n" << std::endl;
}

void swap_2(Data& A1, Data& A2)
{
    std::cout << "Start swap_2  " << std::endl;
    std::unique_lock<std::mutex> la{ A1.get_mutex(), std::defer_lock };
    std::unique_lock<std::mutex> lb{ A2.get_mutex(), std::defer_lock };

    int temp{};
    temp = A1.get_Value();
    A1.set_Value(A2.get_Value());
    A2.set_Value(temp);

    std::lock(la, lb);

    std::cout << "\nValues after swap_2:" << std::endl;
    std::cout << "  Value of class D1 =" << A1.get_Value() << std::endl;
    std::cout << "  Value of class D2 =" << A2.get_Value() << std::endl;

    std::cout << "End swap_2\n" << std::endl;
}

void swap_3(Data& A1, Data& A2)
{
    std::cout << "Start swap_3  " << std::endl;
    std::scoped_lock guard(A1.get_mutex(), A2.get_mutex());

    int temp{};
    temp = A1.get_Value();
    A1.set_Value(A2.get_Value());
    A2.set_Value(temp);


    std::cout << "\nValues after swap_3:" << std::endl;
    std::cout << "  Value of class D1 =" << A1.get_Value() << std::endl;
    std::cout << "  Value of class D2 =" << A2.get_Value() << std::endl;

    std::cout << "End swap_3\n" << std::endl;
}

int main()
{
    Data D1(10, m1);
    Data D2(25, m2);

    std::cout << "Initial values:" << std::endl;
    std::cout << "  Value of class D1 =" << D1.get_Value() << std::endl;
    std::cout << "  Value of class D2 =" << D2.get_Value() << std::endl;

    std::thread t(swap_1, std::ref(D1), std::ref(D2));
    t.join();
    
    system("Pause");
    return 0;
}