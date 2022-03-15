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
public:
    NHC_KV(const string &path);
    NHC_DB *open_NHC();
};

#endif //NHC_DB_H