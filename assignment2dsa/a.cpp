#include <algorithm>
#include <chrono>
#include <deque>
#include <functional>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <vector>

#define MAXNUM 4000001

class Date {
    std::string date;
    int year, month, day, total;
    void dateToInt() {
        year = stoi(date.substr(0, 4));
        month = stoi(date.substr(5, 2));
        day = stoi(date.substr(8, 2));
        total = year * 400 + month * 31 + day;
    }

   public:
    Date() {
        date = "1970-01-01";
        dateToInt();
    }

    explicit Date(const std::string &d) {
        date = d;
        dateToInt();
    }

    Date(const Date &d) {
        date = d.date, year = d.year, month = d.month, day = d.day,
        total = d.total;
    }

    Date(Date &&d) {
        date = std::move(d.date);
        year = d.year, month = d.month, day = d.day, total = d.total;
    }

    Date &operator=(Date d) {
        date = d.date, year = d.year, month = d.month, day = d.day,
        total = d.total;
        return *this;
    }

    bool isLeap(int year) {
        if (year % 400 == 0 || (year % 4 == 0 && year % 100 != 0)) return true;
        return false;
    }

    int maxDay(int month, int year) {
        switch (month) {
            case 4:
            case 6:
            case 9:
            case 11:
                return 30;
                break;
            case 2:
                return (isLeap(year) ? 29 : 28);
                break;
            default:
                return 31;
        }
    }

    std::string getDate() { return date; }
    int getTotal() { return total; }

    void goNextDay() {
        day++;
        if (day > maxDay(month, year)) day = 1, month++;
        if (month > 12) month = 1, year++;
        total = year * 400 + month * 31 + day;

        std::stringstream yearss, monthss, dayss;
        yearss << std::setfill('0') << std::setw(4) << year;
        monthss << std::setfill('0') << std::setw(2) << month;
        dayss << std::setfill('0') << std::setw(2) << day;
        date = yearss.str() + "-" + monthss.str() + "-" + dayss.str();
    }

    // prefix ++ operator for getting the next day
    Date &operator++() {
        goNextDay();
        return *this;
    }

    Date operator++(int) {
        Date res(*this);
        goNextDay();
        return res;
    }

    operator int() const { return total; }
};

class Transaction {
   public:
    Date date;
    double money;

    operator int() { return int(date); }
};

// countingsort. Linear complexity. Works only for whole numbers, but can be
// modified to work with negative integers as well. It is perfect for the task,
// since it allows to sort the numbers and to not corrupt the initial order
template <typename T>
void countingsort(T arr[], size_t n) {
    int c[MAXNUM];
    T temp[n + 1];
    std::fill(c, c + MAXNUM, 0);
    for (size_t i = 0; i < n; ++i) c[(arr[i])]++;
    for (size_t i = 1; i < MAXNUM; ++i) c[i] += c[i - 1];
    for (size_t i = n; i-- > 0;) {
        temp[c[int(arr[i])]] = arr[i];
        c[int(arr[i])]--;
    }
    for (size_t i = 0; i < n; ++i) {
        arr[i] = temp[i + 1];
    }
}

// merge sort. time complexity O(n*logn) in the worst case
template <typename T>
void mergesortlr(T arr[], size_t l, size_t r) {
    if (r - l < 2 || r < l) return;

    size_t m = (l + r) / 2;
    mergesortlr<T>(arr, l, m);
    mergesortlr<T>(arr, m, r);

    size_t i = 0, j = 0, k = 0;
    T temp[r - l];
    while (i < m - l && j < r - m) {
        if (arr[l + i] < arr[m + j])
            temp[k++] = arr[l + i++];
        else
            temp[k++] = arr[m + j++];
    }

    while (i < m - l) temp[k++] = arr[l + i++];

    while (j < r - m) temp[k++] = arr[m + j++];

    for (size_t i = l; i < r; ++i) arr[i] = temp[i - l];
}

template <typename T>
void mergesort(T arr[], size_t sz) {
    mergesortlr<T>(arr, 0, sz);
}

// test a sorting function
template <typename T>
void testsort(std::function<void(T arr[], size_t sz)> test, size_t sz) {
    srand(time(NULL));
    int a[sz], b[sz];

    for (size_t i = 0; i < sz; ++i) a[i] = b[i] = rand() % 30000;

    using std::chrono::duration;
    using std::chrono::duration_cast;
    using std::chrono::high_resolution_clock;
    using std::chrono::milliseconds;

    // executing test sort
    auto t1 = high_resolution_clock::now();

    test(a, sz);

    auto t2 = high_resolution_clock::now();
    auto ms_int = duration_cast<milliseconds>(t2 - t1);
    std::cout << "Test sort time: " << ms_int.count() << "ms\n";

    // executing std::sort
    t1 = high_resolution_clock::now();

    std::sort(b, b + sz);

    t2 = high_resolution_clock::now();
    ms_int = duration_cast<milliseconds>(t2 - t1);
    std::cout << "std::sort time: " << ms_int.count() << "ms\n";

    // Checking correctness of the test sort
    bool correct = 1;
    for (size_t i = 0; i < sz; ++i) {
        if (a[i] != b[i]) {
            correct = 0;
            break;
        }
    }

    if (correct)
        std::cout << "Test sort works properly...\n";
    else
        std::cout << "Test sort failed...\n";
}

int main() {
#ifdef DEBUG
    std::cout << "Testing countingsort:\n";
    testsort<int>(&mergesort<int>, 10000);
    std::cout << "\nTesting countingsort:\n";
    testsort<int>(&countingsort<int>, 10000);

        Date feb("2020-02-01"), mar("2020-03-10");
    while (feb != mar) {
        std::cout << feb.getDate() << '\n';
        feb++;
    }
#endif

    size_t n, d;
    std::cin >> n >> d;
    Transaction transactions[n];
    for (size_t i = 0; i < n; ++i) {
        std::string datestr;
        char c;
        double money;
        std::cin >> datestr >> c >> money;
        transactions[i].date = Date(datestr);
        transactions[i].money = money;
    }

    countingsort(transactions, n);

#ifdef DEBUG
    for (size_t i = 0; i < n; ++i)
        std::cout << transactions[i].date.getDate() << ' '
                  << transactions[i].money << '\n';

#endif

    int ans = 0;
    // median
    double mid = 0;
    // using deque as a queue to iterate through it efficiently
    std::deque<double> q;
    Date today(transactions[0].date);

    for (size_t i = 0; i < n; ++today) {
        double sum = 0;
        while (transactions[i].date.getTotal() <= today.getTotal()) {
            sum += transactions[i++].money;
            if (sum >= mid * 2.0 && q.size() == d) ans++;
            if (i >= n)
                break;
        }

        q.push_back(sum);
        if (q.size() > d) q.pop_front();

        std::vector<double> v;
        for (const auto &it : q) v.push_back(it);

        if (v.size() == d) {
            mergesort(&v[0], d);
            if (d % 2 == 0)
                mid = (v[(d - 1) / 2] + v[d / 2]) / 2.0;
            else
                mid = v[d / 2];
        }
    }

    std::cout << ans << '\n';

    return 0;
}
