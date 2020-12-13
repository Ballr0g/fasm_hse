#include <iostream>
#include <random>
#include <string>
#include <vector>
#include <fstream>
#include <thread>
#include <chrono>
#include <condition_variable>

// Источники:
// https://en.cppreference.com/w/cpp/thread/condition_variable
// https://en.cppreference.com/w/cpp/thread/mutex
// https://en.cppreference.com/w/cpp/thread/lock_guard
// https://en.cppreference.com/w/cpp/thread/unique_lock
// https://en.cppreference.com/w/cpp/thread/sleep_for
// 

// Объект-рандомайзер, генерирующий значения для
// различных сценариев ожидания пчёлами и медведем
class wait_time_random {
public:
    wait_time_random()
        : generator_(static_cast<int>(random_())),
        bear_wait_random_(2, 5),
        bee_leave_wait_random_(2, 6),
        bee_should_leave_random_(0, 9) { }

	// Период ожидания медведя в случае, когда его покусали пчёлы.
    int bear_wait() {
        return bear_wait_random_(generator_);
    }
    // Пчёлы покидают улей в 1 из 10 случаев.
    int bee_decide_leave() {
        return bee_should_leave_random_(generator_);
    }
    // Период времени, на протяжение которого пчела ждёт в улье
	// или отправляется в путь за мёдом.
    int bee_cooldown_time() {
        return bee_leave_wait_random_(generator_);
    }
private:
    std::random_device random_{};
    std::mt19937 generator_;
    std::uniform_int_distribution<int> bear_wait_random_, bee_leave_wait_random_, bee_should_leave_random_;
} random;

// Максимальное и минимальное количество пчёл.
const int MIN_BEE_COUNT = 4;
const int MAX_BEE_COUNT = 30;
// Время выполнения программы.
const int EXECUTION_DURATION = 30;
// Глобальный флаг завершения выполнения программы.
bool execution_not_over = true;
// Количество мёда в улье на данный момент.
std::atomic_int32_t honey_count = 0;
// Количество пчёл в улье на данный момент.
std::atomic_int32_t bee_count_in_hive = 0;
// Мьютекс, используемый медведем для ожидания нужного количества мёда.
std::mutex bear_trap_mutex;
// Мьютекс, использующийся для организации последовательного вывода количества мёда в улье.
std::mutex honey_mutex;
// Мьютекс синхронизированного вывода произвольного количества аргументов в поток.
std::mutex output_mutex;
// Условная переменная для ожидания медведем.
std::condition_variable bear_wait;

// Шаблонный метод синхронизированного вывода произвольного количества аргументов.
// Пакет параметров распаковывается как fold-expression.
template<typename... Args>
void synchronized_console_print(Args&&... args) {
    std::lock_guard<std::mutex> console_out_lock(output_mutex);
    (std::cout << ... << args) << std::endl;
}
// Парсер количества пчёл, не выбрасывает исключений.
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

// Функция с рутинной работой среднестатистической пчелы
void trudovik_pro_bee(int bee_id) {
    while (execution_not_over) {
        int leave_time = random.bee_cooldown_time();
    	// Пчела решила улететь.
        if (random.bee_decide_leave() && bee_count_in_hive > 1) {
        	// Пчела улетает (её поток засыпает).
            --bee_count_in_hive;
            synchronized_console_print(std::string("Bee "), bee_id, std::string(" decided to leave for "),
                leave_time, std::string(" seconds."));
            std::this_thread::sleep_for(std::chrono::seconds(leave_time));
        	// Пчела возвращается с мёдом, увеличивает его количество и печатает сообщение об этом.
            ++bee_count_in_hive;
            {
                std::lock_guard<std::mutex> honey_lock(honey_mutex);
                if (honey_count < 30) {
                    synchronized_console_print(std::string("Bee "), bee_id, std::string(" is back with some honey! "),
                        ++honey_count, std::string(" honey in the hive now."));
                }
            	// Пчела не может принести ещё мёд, если его уже слишком много.
                else {
                    synchronized_console_print(std::string("Bee "), bee_id, std::string(" is back with some honey!\n"),
                        std::string("Looks like the hive is full and the bee had to eat what it brought. Brutal!"));
                }
                // Медведь оповещается, если мёда больше 14.
                if (honey_count >= 15) {
                    bear_wait.notify_one();
                }
            }
        }
    	// Пчела остаётся и засыпает на несколько секунд.
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
    	// При разблокировке в конце программы, медведь сразу завершает свои действия.
    	if (!execution_not_over) {
            return;
    	}
        synchronized_console_print(std::string("A wild bear appeared and it's ready for some honey!"));
    	// Если в улье меньше 3 пчёл, медведь похищает весь мёд.
        const int momentary_bee_count_in_hive = bee_count_in_hive;
        if (momentary_bee_count_in_hive < 3) {
            const int momentary_honey_count = honey_count;
            honey_count = 0;
            synchronized_console_print(std::string("There were only "), momentary_bee_count_in_hive,
                std::string(" bees in the hive...\nThe bear stole "), momentary_honey_count, " honey and ran away!");
        }
    	// Медведя жёстко покусали, он уходит залечивать свои раны на несколько секунд (и без мёда).
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
	// Парсинг введённых аргументов командной строки.
    const int bee_count = parse_bee_count(argv[1]);
	// При некорректном вводе программа сразу же завершается.
    if (bee_count == -1) {
        std::cout << "Error! be count could not be parsed or was out of range [4, 30]. terminating." << std::endl;
        std::cout << "Correct usage: Sagalov_Bees_and_Winnie <Bee count [4, 30]>." << std::endl;
        return -1;
    }
	// Создаём и размещаем в вектор потоки пчёл.
    std::vector<std::thread> bee_threads;
    bee_count_in_hive = bee_count;
    for (int i = 0; i < bee_count; ++i) {
        bee_threads.emplace_back(std::thread(trudovik_pro_bee, i + 1));
    }
	// Создаём и размещаем в вектор поток Винни Пуха.
    std::thread winnie_the_honey_burglar(winnie_pooh_ultimate_honey_burglar);
	// Приостанавливаем основной поток на 30 секунд для выполнения программы.
    std::this_thread::sleep_for(std::chrono::seconds(EXECUTION_DURATION));
	// Говорим,что выполнение закончено, синхронизируем все потоки.
    execution_not_over = false;
    bear_wait.notify_one();
    for (int i = 0; i < bee_count; i++) {
        bee_threads[i].join();
    }
    winnie_the_honey_burglar.join();
}