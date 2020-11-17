#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <random>

/*
���������:
https://en.cppreference.com/w/cpp/thread/condition_variable/notify_all
https://en.cppreference.com/w/cpp/thread/condition_variable/notify_one
https://en.cppreference.com/w/cpp/thread/condition_variable/wait
https://en.cppreference.com/w/cpp/thread/condition_variable
https://en.cppreference.com/w/cpp/thread/lock_guard
*/

// �������� ���������� ��� �������� �������� ���������� ����� �������� ������� � �����������
std::condition_variable cv;
// ��� ���� �������� ����������, ����� ����� � ������� ���������� ���������������
std::condition_variable cv2;

// ��������������� ��������
std::mutex cv_m;
std::mutex cv_m2;

bool she_chose_another = false;
bool he_answered = false;

int lucky_number = 0;

void loving_fan(int fan_number) {
	std::unique_lock<std::mutex> lk(cv_m);
	// ��� ���������, ������ ���� ����� ���������� ������ � �������, ������� ������� ������� (����� �����������)
	// ��� ���� ��� ��� ������� ����-�� �� ������������� ����� (����� �����������)
	cv.wait(lk, [&fan_number] { return lucky_number == fan_number || she_chose_another; });
	if (lucky_number == fan_number) {
		{
			// ���� ����� ������, �� ����������� ������, ������� ��� ����� � ������������� �������...
			// �����������, ��� ��������� ������������ ��������� ������.
			std::lock_guard<std::mutex> lock(cv_m2);
			std::cout << fan_number << ": I am the lucky one! She chose me!\n";
			he_answered = true;
		}
		cv2.notify_one();
	}
	else {
		std::cout << fan_number << ": Too bad... *sigh*\n";
	}
}

void server_tyan(int fan_count) {
	// ������� ���
	std::this_thread::sleep_for(std::chrono::seconds(1));
	{
		std::lock_guard<std::mutex> lk(cv_m);
		// ��������� �������
		std::cout << "An attractive girl appeared\n";

		std::random_device randomizer;
		std::mt19937 generator(randomizer());
		std::uniform_int_distribution<int> distributor(1, fan_count);
		// ��������� ������ ������������
		lucky_number = distributor(generator);
	}
	/*������� ��� ������, ����������� �� �������� ����������,
	� ������� ��������� �� ������� ���������, ����������� � �������� ���������
	���� �������� ����������, �� ����� �������� ����� � ��� ������
	*/
	cv.notify_all();
	{
		// ��� ��� ����, ����� ��������� ������ ������ ����������� ���������� � ������� 
		std::unique_lock<std::mutex> lk(cv_m2);
		cv2.wait(lk, [] { return he_answered; });
	}

	{
		// lock_guard ��������� ������� ��� �������� ������� ����
		// � ����������� ������� ��� ������ �� ���� ���������
		std::lock_guard<std::mutex> lk(cv_m);
		// ���� ��� ��������� �����������, ��� ��� ������� �� ��
		she_chose_another = true;
	}
	// ������� ���� ���������, ��� ������� �� �� ����
	// (�������� ���������� �� she_chose_another == true)
	cv.notify_all();
}


int main() {
	// ������ ���������� ��������� [0;1000]. ��������� ��������� ��� ��������� �����
	std::string fanCount;
	int N;
	std::cout << "Enter the number of fans [1; 1000]: ";
	std::cin >> fanCount;
	try {
		N = std::stoi(fanCount);
		if (N < 1 || N > 1000) {
			throw std::out_of_range("Fan amount out of range");
		}
	}
	catch (std::invalid_argument&) {
		std::cout << "Incorrect input: the string provided cannot be converted to a valid integer." << std::endl;
		return 0;
	}
	catch (std::out_of_range&) {
		std::cout << "The value cannot be less than 1 or bigger than 1000." << std::endl;
		return 0;
	}
	// ������ ��� ���������� ������� ���������.
	std::vector<std::thread> fans;

	for (int i = 0; i < N; i++)
	{
		// ��������� ������.
		fans.push_back(std::thread(loving_fan, i + 1));
	}
	// �������� ���������� "����������".
	std::thread girl(server_tyan, N);

	for (int i = 0; i < N; i++)
	{
		fans[i].join();
	}
	girl.join();
	
	system("pause");
	return 0;
}