#include <chrono>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <unordered_map>

/*
 *   Amirlan Sharipov BS21-01
 *   https://codeforces.com/profile/RinRi
 *   I've copied most of the comments in this code from the assignment
 *   specification
 */

#define PRIMENUM1 1000007
#define PRIMENUM2 1003337
#define MAXN 50001
#define MINN 8

using std::chrono::duration;
using std::chrono::duration_cast;
using std::chrono::high_resolution_clock;
using std::chrono::milliseconds;

template <class T>
class ICircularBoundedQueue {
   public:
    virtual void offer(const T &value) = 0;  // insert an element to the rear of
                                             // the queue overwrite the oldest
                                             // elements when the queue is full

    virtual T &poll() = 0;  // remove an element from the front of the queue
    virtual T &peek() const = 0;  // look at the element at the front of the
                                  // queue (without removing it)
    virtual void flush() = 0;     // remove all elements from the queue
    virtual bool isEmpty() const = 0;  // is the queue empty?
    virtual bool isFull() const = 0;   // is the queue full?
    virtual int size() const = 0;      // number of elements
    virtual int capacity() const = 0;  // maximum capacity
};

template <class T>
class ArrayCircularBoundedQueue : ICircularBoundedQueue<T> {
   public:
    ArrayCircularBoundedQueue<T>(int maxSize) {
        mxsz = maxSize;
        data = new T *[mxsz]();
    }

    ~ArrayCircularBoundedQueue<T>() {
        for (int i = 0; i < mxsz; ++i) delete data[i];
        delete[] data;
    }

    // time complexity: O(1) in any case
    void offer(const T &value) {
        if (sz < mxsz) {
            curPush = (cur + sz) % mxsz;
            sz++;
        }
        data[curPush++] = new T(value);
        curPush %= mxsz;
    }

    // time complexity: O(1) in any case
    void offer(T *value) {
        if (sz < mxsz) {
            curPush = (cur + sz) % mxsz;
            sz++;
        }
        data[curPush++] = value;
        curPush %= mxsz;
    }

    // time complexity: O(1) in any case
    T &poll() try {
        if (sz == 0) throw "queue is empty!";

        // T &ans = *data[cur];
        delete data[cur];
        data[cur] = NULL;
        sz--;
        cur++;
        cur %= mxsz;
        // return ans;
    } catch (const char *msg) {
        std::cerr << msg << std::endl;
        throw;
    }

    // time complexity: O(1) in any case
    T *pollpoint() try {
        if (sz == 0) throw "queue is empty!";

        // T &ans = *data[cur];
        data[cur] = NULL;
        sz--;
        cur++;
        cur %= mxsz;
        // return ans;
    } catch (const char *msg) {
        std::cerr << msg << std::endl;
        throw;
    }

    // time complexity: O(1) in any case
    T &peek() const { return *data[cur]; }

    T *peekpoint() const { return data[cur]; }

    // time complexity: O(n) in any case
    void flush() {
        while (sz) poll();
    }

    // time complexity: O(1) in any case
    bool isEmpty() const { return !sz; }

    // time complexity: O(1) in any case
    bool isFull() const { return (sz == mxsz); }

    // time complexity: O(1) in any case
    int size() const { return sz; }

    // time complexity: O(1) in any case
    int capacity() const { return mxsz; }

    // time complexity: O(n) in any case
    void print() {
        for (int i = 0; i < sz; ++i) {
            // data[(cur + i) % mxsz].list();
            std::cout << *data[(cur + i) % mxsz] << '\n';
        }
    }

    void printset() {
        for (int i = 0; i < sz; ++i) {
            data[(cur + i) % mxsz]->list();
            std::cout << '\n';
        }
    }

   private:
    T **data;
    int sz = 0, mxsz = 0, cur = 0, curPush = 0;
};

template <class T>
class IBoundedStack {
   public:
    virtual void push(const T &value) = 0;  // push an element onto the stack
                                            // remove the oldest element
                                            // when if stack is full
    virtual T &pop() = 0;        // remove an element from the top of the stack
    virtual T &top() const = 0;  // look at the element at the top of the stack
                                 // (without removing it)
    virtual void flush() = 0;    // remove all elements from the stack
    virtual bool isEmpty() const = 0;  // is the stack empty?
    virtual bool isFull() const = 0;   // is the stack full?
    virtual int size() const = 0;      // number of elements
    virtual int capacity() const = 0;  // maximum capacity
};

template <class T>
class QueuedBoundedStack : IBoundedStack<T> {
   public:
    QueuedBoundedStack<T>(int size) {
        q1 = new ArrayCircularBoundedQueue<T>(size);
        q2 = new ArrayCircularBoundedQueue<T>(size);
        mxsz = size;
    }

    ~QueuedBoundedStack<T>() {
        delete q1;
        delete q2;
    }

    // time complexity: O(n*m), Theta(n*m). The algorithm's complexity depends
    // on the size of q1 as well as the cost of operation of copying. In this
    // case(problem C) it is O(n*m), that is why I decided to use pointers
    // instead. See function below this one. Worst case O(n), best case O(1),
    // avg O(n)
    void push(const T &value) {
        q2->offer(value);

        while (!q1->isEmpty()) {
            if (sz == mxsz && q1->size() == 1) {
                q1->poll();
                break;
            }
            T &temp = q1->peek();
            q2->offer(temp);
            q1->poll();
        }

        if (sz < mxsz) sz++;

        ArrayCircularBoundedQueue<T> *temp = q1;
        q1 = q2;
        q2 = temp;
    }

    // time complexity: O(n)
    void push(T *value) {
        q2->offer(value);

        while (!q1->isEmpty()) {
            if (sz == mxsz && q1->size() == 1) {
                q1->poll();
                break;
            }
            T *temp = q1->peekpoint();
            q2->offer(temp);
            q1->pollpoint();
        }

        if (sz < mxsz) sz++;

        ArrayCircularBoundedQueue<T> *temp = q1;
        q1 = q2;
        q2 = temp;
    }

    // time complexity: O(1) in any case (theta(1))
    T &pop() try {
        if (q1->isEmpty()) throw "q1 is empty!";

        // T &ans = q1->peek();
        q1->poll();
        sz--;
        // return ans;
    } catch (const char *msg) {
        std::cerr << msg << std::endl;
        throw;
    }

    // time complexity: O(1) in any case
    T &top() const try {
        if (q1->isEmpty()) throw "q1 is empty!";
        return q1->peek();
    } catch (const char *msg) {
        std::cerr << msg << std::endl;
        throw;
    }

    // time complexity: O(1) in any case
    void flush() {
        sz = 0;
        q1->flush();
        q2->flush();
    }

    // time complexity: O(1) in any case
    bool isEmpty() const { return !sz; }

    // time complexity: O(1) in any case
    bool isFull() const { return (sz == mxsz); }

    // time complexity: O(1) in any case
    int size() const { return sz; }

    // time complexity: O(1) in any case
    int capacity() const { return mxsz; }

    void print() { q1->print(); }
    void printset() { q1->printset(); }

   private:
    ArrayCircularBoundedQueue<T> *q1, *q2;
    int sz = 0, mxsz = 0, cur = 0, curPush = 0;
};

template <class T>
class ISet {
    virtual void add(const T &item) = 0;  // add an item to the set
    virtual void remove(T &item) = 0;     // remove an item from the set
    virtual bool contains(
        T &item) const = 0;            // check if an item belongs to the set
    virtual int size() const = 0;      // the number of elements in the set
    virtual bool isEmpty() const = 0;  // check if the set is empty
};

template <class T>
class DoubleHashSet : ISet<T> {
   public:
    // TODO: Write comments about every method

    enum State : char { EMPTY = 0, DELETED = 1, TAKEN = 2 };

    DoubleHashSet<T>() {
        sz = 0;
        mxsz = 0;
        data = 0;
        state = 0;
    }

    DoubleHashSet<T>(int maxSize) {
        mxsz = maxSize;
        sz = 0;
        data = new T *[mxsz]();
        state = new State[mxsz]();
    }

    // O(n) because of copying
    DoubleHashSet<T>(const DoubleHashSet<T> &st) {
        sz = st.size();
        mxsz = st.getMaxSize();
        data = new T *[mxsz]();
        state = new State[mxsz]();
        T **tempdata = st.getData();
        State *tempstate = st.getState();
        for (int i = 0; i < mxsz; ++i) {
            if (tempstate[i] == TAKEN) data[i] = new T(*tempdata[i]);
            state[i] = tempstate[i];
        }
    }

    // O(n*m) deleting
    ~DoubleHashSet<T>() {
        for (int i = 0; i < mxsz; ++i)
            if (state[i] == TAKEN) delete data[i];
        delete[] data;
        delete[] state;
    }

    // O(n*m) n -> size, m -> size of data
    void list() {
        std::ios_base::sync_with_stdio(0);
        std::cin.tie(0);
        std::cout.tie(0);

        for (int i = 0; i < mxsz; ++i) {
            if (state[i] == TAKEN) std::cout << *data[i] << ' ';
        }
    }

    // Compression function. O(1)
    int hash(int h1, int h2, int64_t i) const {
        // size_t h1 = hasher(item);
        // h1 %= PRIMENUM1;
        // size_t h2 = hasher(item);
        // h2 %= PRIMENUM2;
        return ((int64_t)h1 + i * (int64_t)h2) % (int64_t)mxsz;
        // return ((int64_t)hash1(item, PRIMENUM1) + i * (int64_t)hash2(item,
        // PRIMENUM2)) % mxsz; return (h1 + i * (int64_t)hash2(item, PRIMENUM2))
        // % mxsz;
    }

    // Hash function n1. O(n)
    int hash1(const std::string &item, int prime) const {
        int ans = 0, sz = item.size();
        int64_t cur = 1;
        for (int i = 0; i < item.size(); ++i) {
            ans += (cur * (int64_t)item[i]) % prime;
            ans %= prime;
            cur *= 127;
            cur %= prime;
        }
        return ans;
    }

    // Hash function n2. O(n)
    int hash2(const std::string &item, int prime) const {
        int ans = 0, sz = item.size();
        int64_t cur = 1;
        for (int i = 0; i < item.size(); ++i) {
            ans += (cur * (int64_t)item[sz - i - 1]) % prime;
            ans %= prime;
            cur *= 127;
            cur %= prime;
        }
        return ans;
    }

    // Add an item to the set O(n^2) in worst case (all colisions). Copying
    // takes time as well. Avg and best case: O(n)
    void add(const T &item) {
        int h1 = hash1(item, PRIMENUM1), h2 = hash2(item, PRIMENUM2);
        while (1) {
            for (int i = 0; i < mxsz; ++i) {
                int pos = hash(h1, h2, i);
                if (state[pos] == EMPTY || state[pos] == DELETED) {
                    data[pos] = new T(item);
                    state[pos] = TAKEN;
                    sz++;
                    return;
                } else if (state[pos] == TAKEN) {
                    if (*data[pos] == item) return;
                }
            }

            int tempmxsz = mxsz;
            T **tempdata = data;
            State *tempstate = state;
            int newsz = std::max(2 * tempmxsz, 8);

            data = new T *[newsz]();
            state = new State[newsz]();
            mxsz = newsz;
            sz = 0;

            for (int i = 0; i < tempmxsz; ++i) {
                if (tempstate[i] == TAKEN) {
                    add(*tempdata[i]);
                }
            }

            for (int i = 0; i < tempmxsz; ++i)
                if (state[i] == TAKEN) delete tempdata[i];
            delete[] tempdata;
            delete[] tempstate;
        }
    }

    // Remove an item. worst case: O(n*m), avg and best: O(m), where m is size
    // of data.
    void remove(T &item) try {
        int h1 = hash1(item, PRIMENUM1), h2 = hash2(item, PRIMENUM2);
        for (int i = 0; i < mxsz; ++i) {
            int pos = hash(h1, h2, i);

            if (state[pos] == EMPTY) break;

            if (state[pos] == TAKEN) {
                if (*data[pos] == item) {
                    delete data[pos];
                    data[pos] = NULL;
                    state[pos] = DELETED;
                    sz--;
                    return;
                }
            }
        }

        throw "specified item doesn't exist!";
    } catch (const char *msg) {
        std::cerr << msg << std::endl;
        throw;
    }

    // worst case: O(n)
    bool contains(T &item) const {
        int h1 = hash1(item, PRIMENUM1), h2 = hash2(item, PRIMENUM2);
        for (int i = 0; i < mxsz; ++i) {
            int pos = hash(h1, h2, i);

            if (state[pos] == EMPTY) break;

            if (state[pos] == TAKEN)
                if (*data[pos] == item) return true;
        }
        return false;
    }

    int size() const { return sz; }

    bool isEmpty() const { return (sz == 0); }

    T **getData() const { return data; }

    State *getState() const { return state; }

    int getMaxSize() const { return mxsz; }

    // worst, avg, best case: O(n*m)
    void operator=(const DoubleHashSet<T> &temp) {
        for (int i = 0; i < mxsz; ++i)
            if (state[i] == TAKEN) delete data[i];
        delete[] data;
        delete[] state;

        sz = temp.size();
        mxsz = temp.getMaxSize();

        data = new T *[mxsz]();
        state = new State[mxsz]();

        T **tempdata = temp.getData();
        State *tempstate = temp.getState();
        for (int i = 0; i < mxsz; ++i) {
            if (tempstate[i] == TAKEN) data[i] = new T(*tempdata[i]);
            state[i] = tempstate[i];
        }
    }

   private:
    int mxsz = 0, sz = 0;
    T **data;
    State *state;
};

void solveBoundedCommandQueue() {
    int n, k;
    std::cin >> n >> k;
    std::cin.ignore();
    std::string s;
    ArrayCircularBoundedQueue<std::string> q(k);
    for (int i = 0; i < n; ++i) {
        getline(std::cin, s);
        if (q.size() == k) q.poll();
        q.offer(s);
    }

    q.print();
}

void solveCMDWithoutRollbacks() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(0);
    std::cout.tie(0);
    int n;
    std::cin >> n;
    std::cin.ignore();

    DoubleHashSet<std::string> files;

    for (int i = 0; i < n; ++i) {
        std::string cmd, cmdName, fileName;
        getline(std::cin, cmd);

        std::stringstream s(cmd);
        s >> cmdName;
        if (cmdName == "LIST") {
            // TODO improve LIST command
            files.list();
            std::cout << '\n';
        } else if (cmdName == "NEW") {
            s >> fileName;
            bool exists = files.contains(fileName);
            if (exists) {
                std::cout << "ERROR: cannot execute " << cmd << '\n';
            } else {
                files.add(fileName);
            }
        } else {
            s >> fileName;
            bool exists = files.contains(fileName);
            if (exists) {
                files.remove(fileName);
            } else {
                std::cout << "ERROR: cannot execute " << cmd << '\n';
            }
        }
    }
}

void solveCMDWithRollbacks() {
    std::ios_base::sync_with_stdio(0);
    std::cin.tie(0);
    std::cout.tie(0);
    int n, k;
    std::cin >> n >> k;
    std::cin.ignore();

    QueuedBoundedStack<DoubleHashSet<std::string>> states(k);
    DoubleHashSet<std::string> dummy;
    states.push(dummy);

    for (int i = 0; i < n; ++i) {
        std::string cmd, cmdName, fileName;
        getline(std::cin, cmd);

        std::stringstream s(cmd);
        s >> cmdName;

        if (cmdName == "LIST") {
            // TODO improve LIST command
            states.top().list();
            std::cout << '\n';
        } else if (cmdName == "NEW") {
            // auto t1 = high_resolution_clock::now();

            DoubleHashSet<std::string> *files =
                new DoubleHashSet<std::string>(states.top());
            s >> fileName;
            bool exists = files->contains(fileName);
            std::string similar = (fileName.back() == '/'
                                       ? fileName.substr(0, fileName.size() - 1)
                                       : fileName + "/");
            bool existsSimilar = files->contains(similar);

            if (exists || existsSimilar) {
                std::cout << "ERROR: cannot execute " << cmd << '\n';
                delete files;
            } else {
                files->add(fileName);
                states.push(files);
                // auto t2 = high_resolution_clock::now();
                // auto ms_int = duration_cast<milliseconds>(t2 - t1);
                // std::cout << ms_int.count() << "ms\n";

                continue;
            }

        } else if (cmdName == "REMOVE") {
            DoubleHashSet<std::string> *files =
                new DoubleHashSet<std::string>(states.top());
            s >> fileName;
            bool exists = files->contains(fileName);

            if (exists) {
                files->remove(fileName);
                states.push(files);
                continue;
            } else {
                std::cout << "ERROR: cannot execute " << cmd << '\n';
                delete files;
            }
        } else if (cmdName == "UNDO") {
            int times = 1;
            if (cmd != "UNDO") s >> times;
            if (states.size() > times) {
                while (times--) {
                    states.pop();
                }
            } else {
                std::cout << "ERROR: cannot execute " << cmd << '\n';
            }

        } else {
            std::cout << "ERROR: cannot execute " << cmd << '\n';
        }
    }
}

int main() {
    // solveBoundedCommandQueue();
    // solveCMDWithoutRollbacks();
    solveCMDWithRollbacks();
}

