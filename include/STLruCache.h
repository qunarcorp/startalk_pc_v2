//
// Created by may on 2019-03-19.
//

//
// Created by may on 2019-03-19.
//

#ifndef STLRUCACHE_H
#define STLRUCACHE_H


#include <iostream>
#include <map>

#define CACHE_DEBUG 0


using namespace std;

template<typename K, typename V>
class Node {
public:
    K key;
    V value;
    Node *prev, *next;

    Node(K k, V v) : key(k), value(v), prev(nullptr), next(nullptr) {}
};

template<typename K, typename V>
class LRUNode : public Node<K, V> {
public:
    int accessTimes;

    LRUNode(K k, V v) : Node<K, V>(k, v), accessTimes(0) {};

};

template<typename K, typename V>
class DoublyLinkedList {
    LRUNode<K, V> *front, *rear;
    int itemCount;
    int capacity;

    bool isEmpty() {
        return rear == nullptr;
    }

public:
    explicit DoublyLinkedList(int capacity) : capacity(capacity), front(nullptr), rear(nullptr), itemCount(0) {}

    int size() {
        return itemCount;
    }

    LRUNode<K, V> *add_page_to_head(K &key, V &value) {
        auto *page = new LRUNode<K, V>(key, value);
        if (!front && !rear) {
            front = rear = page;
        } else {
            page->next = front;
            front->prev = page;
            front = page;
        }
        itemCount++;
        return page;
    }

    void move_page_to_head(LRUNode<K, V> *page) {
        if (page == front) {
            return;
        }
        page->accessTimes++;
        if (page == rear) {
            rear = (LRUNode<K, V> *) rear->prev;
            rear->next = nullptr;
        } else {
            page->prev->next = (LRUNode<K, V> *) page->next;
            page->next->prev = (LRUNode<K, V> *) page->prev;
        }

        page->next = front;
        page->prev = nullptr;
        front->prev = page;
        front = page;
    }

    void remove_rear_page(std::vector<K> &removedPage) {
        if (isEmpty()) {
            return;
        }
        itemCount--;
        if (front == rear) {
            delete rear;
            front = rear = nullptr;
        } else {
            int rear_access_times = rear->accessTimes;

            int maxItem = static_cast<int>(capacity * 0.15);

            maxItem = maxItem <= 0 ? 1 : maxItem;

            for (int i = 0; i < maxItem; ++i) {
                if (rear->accessTimes > rear_access_times)
                    continue;
                LRUNode<K, V> *temp = rear;
                rear = (LRUNode<K, V> *) rear->prev;
                rear->next = nullptr;
                removedPage.push_back(temp->key);
                delete temp;
            }

//            LRUNode<K, V> *temp = rear;
//
//            rear = (LRUNode<K, V> *) rear->prev;
//            rear->next = nullptr;
//            delete temp;
        }
    }

    LRUNode<K, V> *getHead() {
        return front;
    }

    LRUNode<K, V> *getNext(LRUNode<K, V> *item) {
        return (LRUNode<K, V> *) item->next;
    }

    LRUNode<K, V> *get_rear_page() {
        return rear;
    }

};

template<typename K, typename V>
class LRUCache {
    mutable std::mutex m_mutex;
    int capacity, size;
    DoublyLinkedList<K, V> *pageList;
    map<K, LRUNode<K, V> *> pageMap;

public:
    explicit LRUCache(int capacity) : capacity(capacity), size(0) {
        this->capacity = capacity;
        size = 0;
        pageList = new DoublyLinkedList<K, V>(capacity);
        pageMap = map<K, LRUNode<K, V> *>();
    }

    V *get(K key) {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (pageMap.find(key) == pageMap.end()) {
            return nullptr;
        }
        V val = pageMap[key]->value;

        // move the page to front
        pageList->move_page_to_head(pageMap[key]);
        return &val;
    }

    void put(K &key, V &value) {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (pageMap.find(key) != pageMap.end()) {
            // if key already present, update value and move page to head
            pageMap[key]->value = value;
            pageList->move_page_to_head(pageMap[key]);
            return;
        }

        if (size == capacity) {
            auto thePage = pageList->get_rear_page();
            K k = thePage->key;
            std::vector<K> vectors;
            pageList->remove_rear_page(vectors);

            if (vectors.size() > 0) {
                auto item = vectors.begin();
                while (item != vectors.end()) {
                    pageMap.erase(*item);
                    size--;
                    item++;
                }
            }
        }

        auto *page = pageList->add_page_to_head(key, value);
        size++;
        pageMap[key] = page;
    }

    ~LRUCache() {
        std::lock_guard<std::mutex> lock(m_mutex);
        auto i1 = pageMap.begin();
        for (; i1 != pageMap.end(); i1++) {
            delete i1->second;
        }
        delete pageList;
    }

#if CACHE_DEBUG

    void print_out_items() {
//        auto i1 = pageMap.begin();
//        for (; i1 != pageMap.end(); i1++) {
//            std::cout << "mapping: key is" << i1->first << " value is " << pageMap[i1->first]->value;
//        }

        auto page = pageList->getHead();

        while (page != nullptr) {
            std::cout << "page: key is " << page->key << " access times are " << page->accessTimes << endl;
            page = pageList->getNext(page);
        }
    }

#endif
};

#endif //STLRUCACHE_H
