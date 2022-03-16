#include "LRU_cache.h"

using std::string;
using std::vector;
using std::stringstream;
using std::unordered_map;
using leveldb::ReadOptions;
using leveldb::Options;
using leveldb::Status;
using leveldb::WriteBatch;
using leveldb::WriteOptions;
using leveldb::DB;

static string tmp; //用于做临时变量，减小开销

LRUcache::LRUcache(KV_op *_cache, lli _capacity):cachePool(_cache),capacity(_capacity),size(0) {
    head = new DLinkedNode();
    tail = new DLinkedNode();
    head->next = tail;
    tail->prev = head;
    // 使用伪头部和伪尾部节点
};

bool LRUcache::get(const string &key, string &value) {
    if (!cachePool->leveldb_get(key, value)) {
        return false;
    }
    lock_guard<mutex> guard(mtx);
    moveToHead(cache[key]);
    return true;
}

void LRUcache::put(const string &key, const string &value) {
    if (!cachePool->leveldb_get(key, tmp)) {
        // 添加进哈希表
        lock_guard<mutex> guard(mtx);
        cache[key] = new DLinkedNode(key);
        // 添加至双向链表的头部
        addToHead(cache[key]);
        ++size;
        if (size > capacity) {
            // 如果超出容量，删除双向链表的尾部节点
            // 删除哈希表中对应的项
            cache.erase(removeTail()->key);
            // 防止内存泄漏
            --size;
        }
    }
    else {
        cachePool->leveldb_set(key, value);
        lock_guard<mutex> guard(mtx);
        moveToHead(cache[key]);
    }
}

void LRUcache::del(const string &key) {
    cachePool->leveldb_del(key);
    lock_guard<mutex> guard(mtx);
    removeNode(cache[key]);
    delete cache[key];
    cache.erase(key);
    size--;
}

void LRUcache::addToHead(DLinkedNode* node) {
    node->prev = head;
    node->next = head->next;
    head->next->prev = node;
    head->next = node;
}

void LRUcache::removeNode(DLinkedNode* node) {
    node->prev->next = node->next;
    node->next->prev = node->prev;
}

void LRUcache::moveToHead(DLinkedNode* node) {
    removeNode(node);
    addToHead(node);
}

DLinkedNode* LRUcache::removeTail() {
    DLinkedNode* node = tail->prev;
    removeNode(node);
    return node;
}

