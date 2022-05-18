#include <algorithm>
#include <chrono>
#include <deque>
#include <functional>
#include <iomanip>
#include <iostream>
#include <list>
#include <map>
#include <sstream>
#include <vector>

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

    explicit Date(const std::string& d) {
        date = d;
        dateToInt();
    }

    Date(const Date& d) {
        date = d.date, year = d.year, month = d.month, day = d.day,
        total = d.total;
    }

    Date(Date&& d) {
        date = std::move(d.date);
        year = d.year, month = d.month, day = d.day, total = d.total;
    }

    Date& operator=(Date d) {
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

    std::string getDate() const { return date; }
    int getTotal() const { return total; }

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
    Date& operator++() {
        goNextDay();
        return *this;
    }

    Date operator++(int) {
        Date res(*this);
        goNextDay();
        return res;
    }

    operator int() const { return total; }

    bool operator<(const Date& d) { return (total < d.total); }
    bool operator>(const Date& d) { return (total > d.total); }
    bool operator<=(const Date& d) { return (total <= d.total); }
    bool operator>=(const Date& d) { return (total >= d.total); }

    bool operator==(const Date& d) { return (total == d.total); }
    friend std::ostream& operator<<(std::ostream& os, const Date& dt);
};

std::ostream& operator<<(std::ostream& os, const Date& dt) {
    os << dt.getDate();
    return os;
}

class Transaction {
   public:
    Date date;
    double money;

    operator int() { return int(date); }
};

template <typename K, typename V>
class Btree;

template <typename K, typename V>
class TreeNode {
    std::pair<K, V>* m_data;
    int m_n, m_t;  // current number of keys, and the maximum number of keys
    TreeNode<K, V>** m_c;  // children
    bool m_leaf;           // is true if the node is a leaf

    TreeNode<K, V>(int t, bool leaf) : m_n(0), m_t(t), m_leaf(leaf) {
        m_data = new std::pair<K, V>[2 * t];
        m_c = new TreeNode<K, V>*[2 * t];
    }

    ~TreeNode<K, V>() {
        delete[] m_data;
        if (!m_leaf && m_n > 0)
            for (int i = 0; i <= m_n; ++i) delete m_c[i];
        delete[] m_c;
    }

    // Insert a new key in the subtree. Assumption: node must be non-full
    void insert(K key, V value) {
        int i = m_n - 1;  // last child

        if (m_leaf) {
            // move all greater keys one position right and find the location of
            // a new key
            while (i >= 0 && m_data[i].first > key) {
                m_data[i + 1] = m_data[i];
                i--;
            }

            m_data[i + 1] = {key, value};
            m_n++;
        } else {
            // Find the child which is going to have the new key
            while (i >= 0 && m_data[i].first > key) i--;

            // if the child is full
            if (m_c[i + 1]->m_n == 2 * m_t - 1) {
                splitChild(i + 1, m_c[i + 1]);

                if (m_data[i + 1].first < key) i++;
            }
            m_c[i + 1]->insert(key, value);
        }
    }

    // split the child 'child' of this node. Assumption: child must be full
    void splitChild(int i, TreeNode<K, V>* child) {
        TreeNode<K, V>* temp = new TreeNode<K, V>(child->m_t, child->m_leaf);
        temp->m_n = m_t - 1;

        for (int j = 0; j < m_t - 1; j++)
            temp->m_data[j] = child->m_data[j + m_t];

        if (!child->m_leaf) {
            for (int j = 0; j < m_t; j++) temp->m_c[j] = child->m_c[j + m_t];
        }

        child->m_n = m_t - 1;

        for (int j = m_n; j >= i + 1; --j) m_c[j + 1] = m_c[j];

        m_c[i + 1] = temp;

        for (int j = m_n - 1; j >= i; --j) m_data[j + 1] = m_data[j];

        m_data[i] = child->m_data[m_t - 1];

        m_n++;
    }

    void traverse() {
        for (int i = 0; i < m_n; i++) {
            if (!m_leaf) m_c[i]->traverse();
            std::cout << m_data[i].first << ' ' << m_data[i].second << ' ';
        }

        if (!m_leaf) m_c[m_n]->traverse();
        std::cout << '\n';
    }

    // recursive search O(logn)
    const TreeNode<K, V>* search(K key) const {
        int i = 0;
        while (i < m_n && m_data[i].first < key) i++;

        if (m_data[i].first == key) return this;

        if (m_leaf == true) return NULL;

        return m_c[i]->search(key);
    }

    // range query
    std::list<V> search(const K& from, const K& to) const {
        std::list<V> res;
        if (m_leaf) {
            for (int i = 0; i < m_n; ++i) {
                if (m_data[i].first >= from && m_data[i].first <= to)
                    res.push_back(m_data[i].second);
            }
        } else {
            if (m_data[0].first >= from) {
                res.splice(res.end(), m_c[0]->search(from, to));
                if (m_data[0].first >= from && m_data[0].first <= to)
                    res.push_back(m_data[0].second);
            }
            K prev = m_data[0].first;
            for (int i = 1; i < m_n; ++i) {
                if (prev > to) break;
                if (m_data[i].first >= from) {
                    res.splice(res.end(), m_c[i]->search(from, to));
                }
                if (m_data[i].first >= from && m_data[i].first <= to)
                    res.push_back(m_data[i].second);
                prev = m_data[i].first;
            }
            if (prev <= to) {
                res.splice(res.end(), m_c[m_n]->search(from, to));
            }
        }
        return res;
    }

    friend class Btree<K, V>;
};

template <typename K, typename V>
class IRangeMap {
    virtual int size() const = 0;
    virtual bool isEmpty() const = 0;
    virtual void add(const K& key, const V& value) = 0;
    virtual bool contains(const K& key) const = 0;
    virtual V& lookup(const K& key) const = 0;
    virtual std::list<V> lookupRange(const K& from, const K& to) const = 0;
};

template <typename K, typename V>
class Btree : IRangeMap<K, V> {
    TreeNode<K, V>* m_root;
    int m_sz, m_t;

   public:
    explicit Btree(int t) : m_root(nullptr), m_sz(0), m_t(t) {}
    ~Btree<K, V>() { delete m_root; }

    // search a key in the tree. Returns a pointer to a TreeNode which has the
    // key.
    const TreeNode<K, V>* search(const K& key) const {
        return (m_root == NULL) ? NULL : m_root->search(key);
    }

    // Return the value of the key. Assumption: key exists in the tree
    V& lookup(const K& key) const {
        const auto it = search(key);
        if (it == NULL) throw "key doesn't exist!";
        for (int i = 0; i < m_t * 2 - 1; ++i)
            if (it->m_data[i].first == key) return it->m_data[i].second;
        throw "key doesn't exist!";
    }

    bool contains(const K& key) const { return (search(key) != NULL); }

    std::list<V> lookupRange(const K& from, const K& to) const {
        if (m_root == NULL) return std::list<V>();
        return m_root->search(from, to);
    }

    void traverse() {
        if (m_root) m_root->traverse();
    }

    void add(const K& key, const V& value) {
        if (key == Date("2021-11-11")) {
            Date d = Date("2021-11-11");
        }
        if (m_root == NULL) {
            m_root = new TreeNode<K, V>(m_t, true);
            m_root->m_data[0] = {key, value};
            m_root->m_n = 1;
        } else {
            // if root is full
            if (m_root->m_n == m_t * 2 - 1) {
                TreeNode<K, V>* temp = new TreeNode<K, V>(m_t, false);

                // rearrange root and its child
                temp->m_c[0] = m_root;

                // split the old root
                temp->splitChild(0, m_root);

                int i = 0;
                if (temp->m_data[0].first < key) i++;
                temp->m_c[i]->insert(key, value);

                m_root = temp;
            } else
                m_root->insert(key, value);
        }
        m_sz++;
    }

    int size() const { return m_sz; }

    bool isEmpty() const { return (m_sz == 0); }
};

int main() {
#ifdef DEBUG
    freopen("test", "r", stdin);
#endif
    int n;
    std::cin >> n;
    Btree<Date, double> map(2);
    //std::map<Date, double> mp;
    for (int i = 0; i < n; ++i) {
        std::string datestr, type;
        double money;
        std::cin >> datestr;
        if (datestr == "REPORT") {
            std::string from, to;
            std::cin >> type >> from >> type >> to;
            std::list<double> l = map.lookupRange(Date(from), Date(to));
            double ans = 0, ans2 = 0;
            for (const auto& it : l) ans += it;

            /* 
            auto itfrom = mp.lower_bound(Date(from)),
                 itto = mp.upper_bound(Date(to));
            for (auto it = itfrom; it != itto; ++it) {
                ans2 += it->second;
                std::cout << it->first << ' ' << it->second << '\n';
            }
            */

            std::cout << ans << '\n';
        } else {
            std::cin >> type >> money;
            if (type == "WITHDRAW") money *= -1;

            Date d(datestr);
            //mp[d] += money;
            if (map.contains(d)) {
                map.lookup(d) += money;
            } else {
                map.add(Date(datestr), money);
            }
        }
    }

#ifdef DEBUG
#endif
}

