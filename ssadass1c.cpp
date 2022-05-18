#include <fstream>
#include <iostream>

template <class T>
class Node {
   public:
    Node<T>(T item) {
        value = item;
        next = NULL;
    }

    T value;
    Node<T>* next;
};

template <class T>
class List {
    Node<T>* head;
    int sz;

   public:
    List<T>() {
        head = NULL;
        sz = 0;
    }

    // Create a list from nodes. O(n)
    List<T>(Node<T>* node) {
        head = node;
        sz = 1;
        while (node->next != NULL) sz++;
    }

    int size() const { return sz; }

    Node<T>* begin() const { return head; }

    // Return the n'th Node address O(n)
    Node<T>* seek(int pos) const {
        Node<T>* cur = head;
        for (int i = 0; i < pos; ++i) {
            cur = cur->next;
        }
        return cur;
    }

    // Remove the next node O(1)
    void remove(Node<T>* node) {
        Node<T>* nextNode = node->next;
        node->next = nextNode->next;
        delete nextNode;
        sz--;
    }

    // remove the n'th node O(n)
    void remove(int nodeNumber) {
        if (sz == 0) std::abort();

        Node<T>* cur = head;

        if (nodeNumber == 0) {
            head = head->next;
            delete cur;
            sz--;
            return;
        }

        for (int i = 0; i < nodeNumber - 1; ++i) {
            cur = cur->next;
        }

        remove(cur);
    }

    // insert a Node. O(n)
    void insert(Node<T>* node, int pos) {
        Node<T>* cur = head;

        if (pos > sz) std::abort();

        if (pos == 0) {
            node->next = head;
            head = node;
            sz++;
            return;
        }

        for (int i = 0; i < pos - 1; ++i) {
            cur = cur->next;
        }

        node->next = cur->next;
        cur->next = node;
        sz++;
    }

    // insert a Node. O(n)
    void insert(T item, int pos) {
        Node<T>* node = new Node<T>(item);
        insert(node, pos);
    }

    // insert a Node. O(n)
    void push_back(Node<T>* node) { insert(node, sz); }

    // insert an item. O(n)
    void push_back(T item) { insert(item, sz); }

    // Swap every two adjacent Nodes
    void swapAdjacent() {
        Node<T>*cur = head, *curNext = head->next, *curPrev = NULL;

        for (int i = 0; i < (sz % 2 ? sz - 1 : sz); i += 2) {
            if (curPrev == NULL)
                head = curNext;
            else
                curPrev->next = curNext;

            cur->next = curNext->next;
            curNext->next = cur;

            curPrev = cur;
            if (cur->next) {
                cur = cur->next;
                curNext = cur->next;
            }
        }
    }

    // convert the list to string. O(n)
    std::string toString() {
        std::string ans = "";
        Node<T>* cur = head;

        while (cur) {
            ans += std::to_string(cur->value) + " ";
            cur = cur->next;
        }

        return ans;
    }
};

int main() {
    std::ifstream in;
    in.open("input.txt");

    std::ofstream out;
    out.open("output.txt");

    List<int> l;

    int a;
    while (in >> a) {
        l.push_back(a);
    }

    l.swapAdjacent();

    out << l.toString() << '\n';
}
