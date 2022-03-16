#ifndef NHC_DB_H_
#define NHC_DB_H_

#include <assert.h>
#include <vector>
#include <iostream>
#include <sstream>
#include <string>
#include <ctime>
#include <algorithm>
#include <cstdlib>
#include <sys/time.h> //测试定时操作
#include <signal.h>
#include <thread>
#include "leveldb/db.h"
#include "leveldb/filter_policy.h"
#include "leveldb/write_batch.h"
#include "kv_op.h"
#include "LRU_cache.h"
#include "Optimizer.h"

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

typedef struct NHC_DB
{
    KV_op * cache;
    KV_op * mainDB;
} nhc_db;


class NHC_KV {
public:
    NHC_DB * db;
    string cachePath;
    string dbPath;
    LRUcache * cacheManange;
    short int N = 999;//随机数产生到3位
public:
    NHC_KV(const string &path);
    NHC_DB *open_NHC(long long int cacheCap);
    void close_NHC_KV();
    ~NHC_KV();
    double getRand();
    bool nhc_get(const string &key, string &value);
    void nhc_set(const string &key,const string &value);
    void nhc_del(const string &key);
    void nhc_put(const string &key,const string &value);
};

#endif //NHC_DB_H