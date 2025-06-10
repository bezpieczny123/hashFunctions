#include <chrono>
#include <iostream>
#include <fstream>
#include <vector>
#include <random>
#include "open_addressing.cpp"
#include "separate_chaining.cpp"

std::ofstream file;

std::chrono::steady_clock::time_point begin, end;

std::mt19937 data_gen(42);
std::mt19937 test_gen(123);
std::uniform_int_distribution<> dist(0, 500000);

void startTimer() {
    begin = std::chrono::steady_clock::now();
}

void stopTimer() {
    end = std::chrono::steady_clock::now();
}

#define MEASURE_BLOCK(block) \
    startTimer(); \
    block; \
    stopTimer();

template <template <typename> class HashType, typename HashEnum>
void measure_hash(const std::string& hashName, const std::vector<int>& tableSizes, HashEnum method)
{
    file.open(hashName + ".csv");
    file << hashName << "\n";
    file << "fill_percentage,insert_time_ns,delete_time_ns\n";

    const int capacity = 100000;
    const int measurement_ops = 100;

    for (int current_size : tableSizes) {
        if (current_size == 0) continue;

        HashType<int> table(method);

        for (int i = 0; i < current_size; ++i) {
            table.insert(dist(data_gen));
        }

        uint64_t sumAdd = 0;
        MEASURE_BLOCK({
            for (int i = 0; i < measurement_ops; ++i) {
                int key_to_insert = dist(test_gen);
                table.insert(key_to_insert);
            }
        });
        sumAdd = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count();

        uint64_t sumDel = 0;
        MEASURE_BLOCK({
            for (int i = 0; i < measurement_ops; ++i) {
                int key_to_delete = dist(test_gen) + 1000000;
                table.deleteItem(key_to_delete);
            }
        });
        sumDel = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count();

        double fill_percentage = static_cast<double>(current_size) * 100.0 / capacity;
        long long avg_add_ns = sumAdd / measurement_ops;
        long long avg_del_ns = sumDel / measurement_ops;

        file << fill_percentage << "," << avg_add_ns << "," << avg_del_ns << std::endl;
        std::cout << "Fill: " << fill_percentage << "%, Avg Insert: " << avg_add_ns << " ns, Avg Delete (Miss): " << avg_del_ns << " ns" << std::endl;
    }
    file.close();
}

int main() {
    const int n = 100000;
    std::vector<int> tableFullLevels;

    for (int i = 5; i <= 95; i += 5) {
        tableFullLevels.push_back(n * i / 100);
    }

    // Open Addressing
    measure_hash<HashTable>("OpenAddressingModulo", tableFullLevels, MODULO);
    measure_hash<HashTable>("OpenAddressingMultiplication", tableFullLevels, MULTIPLICATION);
    measure_hash<HashTable>("OpenAddressingAlgebraic", tableFullLevels, ALGEBRAIC);

    // Separate Chaining
    measure_hash<ChainHashTable>("ChainingModulo", tableFullLevels, CHAIN_MODULO);
    measure_hash<ChainHashTable>("ChainingMultiplication", tableFullLevels, CHAIN_MULTIPLICATION);
    measure_hash<ChainHashTable>("ChainingAlgebraic", tableFullLevels, CHAIN_ALGEBRAIC);

    return EXIT_SUCCESS;
}