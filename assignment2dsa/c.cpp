#include <algorithm>
#include <chrono>
#include <deque>
#include <functional>
#include <iomanip>
#include <iostream>
#include <list>
#include <map>
#include <sstream>
#include <unordered_map>
#include <vector>

#define MaxN 2147483647
#define MinN -1

template <typename K, typename V>
class Obj {
   public:
    K k;
    V v;

    Obj() {}
    Obj(K key, V value) : k(key), v(value) {}

    bool operator<(const Obj &o) const {
        if (k == o.k) return (v < o.v);
        return (k < o.k);
    }

    bool operator>(const Obj &o) const {
        if (k == o.k) return (v > o.v);
        return (k > o.k);
    }

    Obj<K, V> operator=(const Obj<K, V> &o) {
        k = o.k;
        v = o.v;
        return *this;
    }

    bool operator==(const Obj<K, V> &o) {
        if (k == o.k && v == o.v)
            return true;
        return false;
    }
};

template <typename K, typename V>
class IPriorityQueue {
    virtual void insert(const Obj<K, V> &item) = 0;
    virtual Obj<K, V> findMin() = 0;
    virtual Obj<K, V> extractMin() = 0;
    virtual void decreaseKey(Obj<K, V> &item, K newKey) = 0;
    virtual void remove(Obj<K, V> &item) = 0;
    virtual void unite(Obj<K, V> &anotherQueue) = 0;
};

template <typename K, typename V>
class PriorityQueue : IPriorityQueue<K, V> {
    Obj<K, V> *m_data;
    int sz, mxsz;
    std::unordered_map<V, int> map;

   public:
    PriorityQueue<K, V>(int capacity) : sz(0), mxsz(capacity) {
        m_data = new Obj<K, V>[capacity]();
    }
    // get indexes
    int parent(int i) { return (i - 1) / 2; }
    int left(int i) { return (2 * i + 1); }
    int right(int i) { return (2 * i + 2); }

    int findId(const Obj<K, V> &item) {
        // I used unordered_map to find the index of the item by value
        return map[item.v];
    }

    Obj<K, V> findMin() { return m_data[0]; }

    // insert an object. O(logn)
    void insert(const Obj<K, V> &item) {
        if (sz == mxsz) {
            throw "Couldn't insert!";
        }

        sz++;
        int i = sz - 1, j = sz - 1;
        m_data[i] = item;

        while (i != 0 && m_data[parent(i)] > m_data[i]) {
            swap(m_data[i], m_data[parent(i)]);
            j = i;
            i = parent(i);
            if (i == 0)
                break;
        }
        map[item.v] = j;
    }

    // decrease the key of an object. O(logn)
    void decreaseKey(Obj<K, V> &item, K newKey) {
        int i = findId(item);
        m_data[i].k = newKey;
        while (i != 0 && m_data[parent(i)] > m_data[i]) {
            swap(m_data[i], m_data[parent(i)]);
            i = parent(i);
        }
    }

    // return minimum and pop it from the queue. O(logn)
    Obj<K, V> extractMin() {
        if (sz <= 0) return Obj<K, V>(MinN, std::string());
        if (sz == 1) {
            sz--;
            return m_data[0];
        }

        Obj<K, V> root = m_data[0];
        m_data[0] = m_data[sz - 1];
        sz--;
        MinHeapify(0);

        return root;
    }

    // delete the object from the queue. O(logn)
    void remove(Obj<K, V> &item) {
        decreaseKey(item, K());
        extractMin();
    }

    // stabilize the heap recursively. O(logn)
    void MinHeapify(int i) {
        int l = left(i);
        int r = right(i);
        int mn = i;
        if (l < sz && m_data[l] < m_data[i]) mn = l;
        if (r < sz && m_data[r] < m_data[mn]) mn = r;
        if (mn != i) {
            swap(m_data[i], m_data[mn]);
            MinHeapify(mn);
        }
    }

    void unite(Obj<K, V> &anotherQueue) {}
};

int main() {
    int n;
    std::cin >> n;
    PriorityQueue<int, std::string> q(1000000);
    while (n--) {
        std::string query, branch;
        std::cin >> query;
        if (query == "ADD") {
            int value;
            std::cin >> branch >> value;
            q.insert(Obj<int, std::string>(value, branch));
        } else {
            std::cout << q.extractMin().v << '\n';
        }
    }
}
