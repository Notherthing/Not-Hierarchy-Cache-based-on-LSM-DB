#ifndef LRU_CACHE_H_
#define LRU_CACHE_H_

#include <assert.h>
#include <vector>
#include <iostream>
#include <sstream>
#include <string>
#include <ctime>
#include <algorithm>
#include <cstdlib>
#include <unordered_map>
#include <mutex>
#include "leveldb/db.h"
#include "leveldb/filter_policy.h"
#include "leveldb/write_batch.h"
#include "kv_op.h"

using std::string;
using std::vector;
using std::stringstream;
using std::unordered_map;
using std::mutex;
using std::lock_guard;
using leveldb::ReadOptions;
using leveldb::Options;
using leveldb::Status;
using leveldb::WriteBatch;
using leveldb::WriteOptions;
using leveldb::DB;

typedef long long int lli;
struct DLinkedNode {
    string key;
    DLinkedNode* prev;
    DLinkedNode* next;
    DLinkedNode(): key(0), prev(nullptr), next(nullptr) {}
    DLinkedNode(string _key): key(_key), prev(nullptr), next(nullptr) {}
}; //定义了一个双向链表，用于实现LRU框架
//通过双向链表去维护更新删除

class LRUcache {
private:
    KV_op * cachePool;
    DLinkedNode* head;
    DLinkedNode* tail;
    mutex mtx;
public:
    lli size;
    lli capacity;
    unordered_map<string, DLinkedNode*> cache;
    //按一条记录64BYTE估算
    //30GB，约能存512*1024*1024条记录
    //建议开放capacity大小为2e
public:
    LRUcache(KV_op *_cache, lli _capacity);
    //初始化cache
    bool get(const string &key, string &value);
    //得到cache访问返回，返回值入value
    //true得到，false得不到
    void put(const string &key, const string &value);
    //为cahe放入一个KV记录
    void del(const string &key);
    //在cache里删除一个KV记录
    void addToHead(DLinkedNode* node);
    
    void removeNode(DLinkedNode* node);

    void moveToHead(DLinkedNode* node);

    DLinkedNode* removeTail();
};

#endif //LRU_CACHE_H_