#include <iostream>
#include <random>
#include <string>
#include <vector>
#include <fstream>
#include <thread>
#include <chrono>
#include <condition_variable>

// ���������:
// https://en.cppreference.com/w/cpp/thread/condition_variable
// https://en.cppreference.com/w/cpp/thread/mutex
// https://en.cppreference.com/w/cpp/thread/lock_guard
// https://en.cppreference.com/w/cpp/thread/unique_lock
// https://en.cppreference.com/w/cpp/thread/sleep_for
// 

// ������-�����������, ������������ �������� ���
// ��������� ��������� �������� ������� � ��������
class wait_time_random {
public:
    wait_time_random()
        : generator_(static_cast<int>(random_())),
        bear_wait_random_(2, 5),
        bee_leave_wait_random_(2, 6),
        bee_should_leave_random_(0, 9) { }

	// ������ �������� ������� � ������, ����� ��� �������� �����.
    int bear_wait() {
        return bear_wait_random_(generator_);
    }
    // ����� �������� ���� � 1 �� 10 �������.
    int bee_decide_leave() {
        return bee_should_leave_random_(generator_);
    }
    // ������ �������, �� ���������� �������� ����� ��� � ����
	// ��� ������������ � ���� �� ����.
    int bee_cooldown_time() {
        return bee_leave_wait_random_(generator_);
    }
private:
    std::random_device random_{};
    std::mt19937 generator_;
    std::uniform_int_distribution<int> bear_wait_random_, bee_leave_wait_random_, bee_should_leave_random_;
} random;

// ������������ � ����������� ���������� ����.
const int MIN_BEE_COUNT = 4;
const int MAX_BEE_COUNT = 30;
// ����� ���������� ���������.
const int EXECUTION_DURATION = 30;
// ���������� ���� ���������� ���������� ���������.
bool execution_not_over = true;
// ���������� ��� � ���� �� ������ ������.
std::atomic_int32_t honey_count = 0;
// ���������� ���� � ���� �� ������ ������.
std::atomic_int32_t bee_count_in_hive = 0;
// �������, ������������ �������� ��� �������� ������� ���������� ���.
std::mutex bear_trap_mutex;
// �������, �������������� ��� ����������� ����������������� ������ ���������� ��� � ����.
std::mutex honey_mutex;
// ������� ������������������� ������ ������������� ���������� ���������� � �����.
std::mutex output_mutex;
// �������� ���������� ��� �������� ��������.
std::condition_variable bear_wait;

// ��������� ����� ������������������� ������ ������������� ���������� ����������.
// ����� ���������� ��������������� ��� fold-expression.
template<typename... Args>
void synchronized_console_print(Args&&... args) {
    std::lock_guard<std::mutex> console_out_lock(output_mutex);
    (std::cout << ... << args) << std::endl;
}
// ������ ���������� ����, �� ����������� ����������.
int parse_bee_count(const char* input_chars) noexcept {
    if (input_chars == nullptr) {
        return -1;
    }
    try {
        const int result = std::stoi(input_chars);
        return result >= MIN_BEE_COUNT && result <= MAX_BEE_COUNT ? result : -1;
    }
    catch (...) {
        return -1;
    }
}

// ������� � �������� ������� �������������������� �����
void trudovik_pro_bee(int bee_id) {
    while (execution_not_over) {
        int leave_time = random.bee_cooldown_time();
    	// ����� ������ �������.
        if (random.bee_decide_leave() && bee_count_in_hive > 1) {
        	// ����� ������� (� ����� ��������).
            --bee_count_in_hive;
            synchronized_console_print(std::string("Bee "), bee_id, std::string(" decided to leave for "),
                leave_time, std::string(" seconds."));
            std::this_thread::sleep_for(std::chrono::seconds(leave_time));
        	// ����� ������������ � ����, ����������� ��� ���������� � �������� ��������� �� ����.
            ++bee_count_in_hive;
            {
                std::lock_guard<std::mutex> honey_lock(honey_mutex);
                if (honey_count < 30) {
                    synchronized_console_print(std::string("Bee "), bee_id, std::string(" is back with some honey! "),
                        ++honey_count, std::string(" honey in the hive now."));
                }
            	// ����� �� ����� �������� ��� ��, ���� ��� ��� ������� �����.
                else {
                    synchronized_console_print(std::string("Bee "), bee_id, std::string(" is back with some honey!\n"),
                        std::string("Looks like the hive is full and the bee had to eat what it brought. Brutal!"));
                }
                // ������� �����������, ���� ��� ������ 14.
                if (honey_count >= 15) {
                    bear_wait.notify_one();
                }
            }
        }
    	// ����� ������� � �������� �� ��������� ������.
        else {
            synchronized_console_print(std::string("Bee "), bee_id, std::string(" decided to stay guard for "),
                leave_time, std::string(" seconds."));
            std::this_thread::sleep_for(std::chrono::seconds(leave_time));
        }
    }
}
void winnie_pooh_ultimate_honey_burglar() {
    while (execution_not_over) {
        std::unique_lock<std::mutex> bear_lock(bear_trap_mutex);
        bear_wait.wait(bear_lock, [] { return honey_count > 15 || !execution_not_over; });
    	// ��� ������������� � ����� ���������, ������� ����� ��������� ���� ��������.
    	if (!execution_not_over) {
            return;
    	}
        synchronized_console_print(std::string("A wild bear appeared and it's ready for some honey!"));
    	// ���� � ���� ������ 3 ����, ������� �������� ���� ��.
        const int momentary_bee_count_in_hive = bee_count_in_hive;
        if (momentary_bee_count_in_hive < 3) {
            const int momentary_honey_count = honey_count;
            honey_count = 0;
            synchronized_console_print(std::string("There were only "), momentary_bee_count_in_hive,
                std::string(" bees in the hive...\nThe bear stole "), momentary_honey_count, " honey and ran away!");
        }
    	// ������� ����� ��������, �� ������ ���������� ���� ���� �� ��������� ������ (� ��� ���).
        else {
            int bear_wait_time = random.bear_wait();
            synchronized_console_print(std::string("There were "), momentary_bee_count_in_hive,
                std::string(" bees in the hive.\nThey managed to repel Winnie's assault, making him heal for "),
                bear_wait_time, " seconds!");
            std::this_thread::sleep_for(std::chrono::seconds(bear_wait_time));
        }
    }
}

int main(int argc, char** argv) {
	// ������� �������� ���������� ��������� ������.
    const int bee_count = parse_bee_count(argv[1]);
	// ��� ������������ ����� ��������� ����� �� �����������.
    if (bee_count == -1) {
        std::cout << "Error! be count could not be parsed or was out of range [4, 30]. terminating." << std::endl;
        std::cout << "Correct usage: Sagalov_Bees_and_Winnie <Bee count [4, 30]>." << std::endl;
        return -1;
    }
	// ������ � ��������� � ������ ������ ����.
    std::vector<std::thread> bee_threads;
    bee_count_in_hive = bee_count;
    for (int i = 0; i < bee_count; ++i) {
        bee_threads.emplace_back(std::thread(trudovik_pro_bee, i + 1));
    }
	// ������ � ��������� � ������ ����� ����� ����.
    std::thread winnie_the_honey_burglar(winnie_pooh_ultimate_honey_burglar);
	// ���������������� �������� ����� �� 30 ������ ��� ���������� ���������.
    std::this_thread::sleep_for(std::chrono::seconds(EXECUTION_DURATION));
	// �������,��� ���������� ���������, �������������� ��� ������.
    execution_not_over = false;
    bear_wait.notify_one();
    for (int i = 0; i < bee_count; i++) {
        bee_threads[i].join();
    }
    winnie_the_honey_burglar.join();
}