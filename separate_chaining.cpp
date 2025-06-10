#include <vector>
#include <list>
#include <functional>
#include <cmath>
#include <cstddef>

// Created by mikol on 8.06.2025.
enum ChainHashMethod {
    CHAIN_MODULO,
    CHAIN_MULTIPLICATION,
    CHAIN_ALGEBRAIC
};

template <typename K>
class ChainHashTable {
private:
    int capacity;
    int size;
    std::vector<std::list<K>> table;
    ChainHashMethod hashMethod;
    static const double A;

public:
    explicit ChainHashTable(const ChainHashMethod method = CHAIN_MODULO) {
        capacity = 100000;
        size = 0;
        hashMethod = method;
        table.resize(capacity);
    }

    int hashFunction(const K& item) const {
        std::hash<K> hasher;
        const size_t hashValue = hasher(item);

        switch (hashMethod) {
        case CHAIN_MODULO:
            return moduloHash(hashValue);
        case CHAIN_MULTIPLICATION:
            return multiplicationHash(hashValue);
        case CHAIN_ALGEBRAIC:
            return algebraicHash(hashValue);
        default:
            return moduloHash(hashValue);
        }
    }

    int moduloHash(size_t rawHashValue) const {
        return static_cast<int>(rawHashValue % capacity);
    }

    int multiplicationHash(size_t rawHashValue) const {
        double temp = static_cast<double>(rawHashValue) * A;
        temp = temp - std::floor(temp);
        return static_cast<int>(capacity * temp);
    }

    int algebraicHash(size_t rawHashValue) const {
        constexpr size_t a = 31;
        constexpr size_t b = 17;
        return static_cast<int>(((a * rawHashValue + b) % capacity + capacity) % capacity);
    }

    void insert(const K& item) {
        int hashIndex = hashFunction(item);
        for (auto& elem : table[hashIndex]) {
            if (elem == item) {
                elem = item;
                return;
            }
        }
        table[hashIndex].push_back(item);
        size++;
    }

    bool deleteItem(const K& item) {
        int hashIndex = hashFunction(item);
        auto& chain = table[hashIndex];
        for (auto it = chain.begin(); it != chain.end(); ++it) {
            if (*it == item) {
                chain.erase(it);
                size--;
                return true;
            }
        }
        return false;
    }
};

template <typename K>
const double ChainHashTable<K>::A = 0.6180339887;
