#ifndef KV_OP_H_
#define KV_OP_H_

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

using std::string;
using std::vector;
using std::stringstream;
using leveldb::ReadOptions;
using leveldb::Options;
using leveldb::Status;
using leveldb::WriteBatch;
using leveldb::WriteOptions;
using leveldb::DB;

class KV_op {
public:
  DB * db; // leveldb指针
  string dir; //存储引擎存放目录
public:
  KV_op(const string &dirname);
  ~KV_op();
  bool leveldb_get(const string &key, string &value);
  //封装后的get，以下皆为封装后的操作
  void leveldb_set(const string &key, const string &value);
  void leveldb_del(const string &key);
  void destroy_leveldb();
  void open_leveldb();
};

#endif  //KV_OP_H_
