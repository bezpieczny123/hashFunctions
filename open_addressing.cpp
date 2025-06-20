#include <functional>
#include <cmath>
#include <cstddef>

// Created by mikol on 8.06.2025.
enum HashMethod {
    MODULO,
    MULTIPLICATION,
    ALGEBRAIC
};

template <typename K>
class HashTable {
private:
    int capacity;
    int size;
    K** table;
    K* dummy;
    HashMethod hashMethod;
    static const double A;

public:
    explicit HashTable(const HashMethod method = MODULO) {
        capacity = 100000;
        size = 0;
        hashMethod = method;
        table = new K * [capacity];

        for (int i = 0; i < capacity; i++) {
            table[i] = nullptr;
        }
        dummy = new K();
    }

    ~HashTable() {
        for (int i = 0; i < capacity; ++i) {
            if (table[i] && table[i] != dummy) {
                delete table[i];
            }
        }
        delete[] table;
        delete dummy;
    }

    int hashFunction(const K& item) {
        std::hash<K> hasher;
        const size_t hashValue = hasher(item);

        switch (hashMethod) {
        case MODULO:
            return moduloHash(hashValue);
        case MULTIPLICATION:
            return multiplicationHash(hashValue);
        case ALGEBRAIC:
            return algebraicHash(hashValue);
        default:
            return moduloHash(hashValue);
        }
    }

    int moduloHash(size_t rawHashValue) {
        return static_cast<int>(rawHashValue % capacity);
    }

    int multiplicationHash(size_t rawHashValue) {
        double temp = static_cast<double>(rawHashValue) * A;
        temp = temp - std::floor(temp);
        return static_cast<int>(capacity * temp);
    }

    int algebraicHash(size_t rawHashValue) {
        constexpr size_t a = 31;
        constexpr size_t b = 17;
        return static_cast<int>(((a * rawHashValue + b) % capacity + capacity) % capacity);
    }

    void insert(const K& item) {
        int hashIndex = hashFunction(item);
        int startIndex = hashIndex;

        while (table[hashIndex] != nullptr && table[hashIndex] != dummy) {
            if (*table[hashIndex] == item) {
                K* temp = new K(item);
                delete table[hashIndex];
                table[hashIndex] = temp;
                return;
            }
            ++hashIndex;
            hashIndex %= capacity;

            if (hashIndex == startIndex) return;
        }

        if (table[hashIndex] == nullptr || table[hashIndex] == dummy) {
            size++;
            table[hashIndex] = new K(item);
        }
    }

    bool deleteItem(const K& item) {
        int hashIndex = hashFunction(item);
        int startIndex = hashIndex;

        while (table[hashIndex] != nullptr) {
            if (table[hashIndex] != dummy && *table[hashIndex] == item) {
                K* temp = table[hashIndex];
                table[hashIndex] = dummy;
                delete temp;
                size--;
                return true;
            }
            ++hashIndex;
            hashIndex %= capacity;

            if (hashIndex == startIndex) {
                return false;
            }
        }
        return false;
    }
};

template <typename K>
const double HashTable<K>::A = 0.6180339887;
