#include <iostream>
#include <omp.h>
#include <random>
#include <string>

int main()
{
	std::cout << "Enter the number of fans [1; 100]: ";
	std::string user_input;
	std::cin >> user_input;
	int amount_of_fans;

	try {
		amount_of_fans = std::stoi(user_input);
		if (amount_of_fans < 1 || amount_of_fans > 100)
		{
			throw std::out_of_range("Fan count out of range");
		}
	}
	catch (const std::invalid_argument&) {
		std::cout << "\nIncorrect input format. Please enter a positive integer number in range [1; 100].\n";
		system("pause");
		return 1;
	}
	catch (const std::out_of_range&) {
		std::cout << "\nArgument out of range [1, 100]. Please try again.\n";
		system("pause");
		return 1;
	}

	omp_set_num_threads(amount_of_fans);

	int the_lucky_number;

#pragma omp parallel shared(the_lucky_number)
	{
		// Каждый из потоков (влюблённых поклонников) говорит о своём присутствии.
		// Critical говорит о том, что единовременно эту секцию может выполнить только один поток.
#pragma omp critical
		{
			std::cout << "Fan " << omp_get_thread_num() + 1
				<< ": Please accept my Valentine!\n";
		}
		// Ждём завершения вывода в консоль всех потоков
#pragma omp barrier

// Master - эту секцию может выполнить только master поток
#pragma omp master
		{
			std::random_device randomizer;
			std::mt19937 generator(randomizer());
			std::uniform_int_distribution<int> distributor(0, amount_of_fans - 1);
			the_lucky_number = distributor(generator);
			std::cout << "\nThe attractive girl has made her choice...\n";
		}
		// Все остальные ждут, пока мастер (привлекательная студентка) не сообщит о своём выборе
#pragma omp barrier

// Эта секция снова выполняется только одним потоком единовременно
#pragma omp critical
		{
			// Поклонник-счастливчик первым сообщает о совпадении номера
			if (omp_get_thread_num() == the_lucky_number) {
				std::cout << "Fan " << omp_get_thread_num() + 1 << ": I am the lucky one! She chose me!\n";
			}
		}
		// Ждём его.
#pragma omp barrier
// И эта тоже единовременно только одним.
#pragma omp critical
		{
			// Ну а теперь все остальные поклонники поочерёдно выскажут своё мнение
			if (omp_get_thread_num() != the_lucky_number) {
				std::cout << "Fan " << omp_get_thread_num() + 1 << ": Too bad... *sigh*\n";
			}
		}
	}
	system("pause");
}

