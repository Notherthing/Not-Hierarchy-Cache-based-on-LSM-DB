#include "kv_op.h"

using std::string;
using std::vector;
using std::stringstream;
using leveldb::ReadOptions;
using leveldb::Options;
using leveldb::Status;
using leveldb::WriteBatch;
using leveldb::WriteOptions;
using leveldb::DB;


KV_op::KV_op(const string &dirname) : dir(dirname){};

KV_op::~KV_op(){
    delete db;
}

bool KV_op::leveldb_get(const string &key, string &value){
    ReadOptions ropt;
    Status s = db->Get(ropt, key, &value);
    assert(s.ok());
    if (s.IsNotFound()) {
        return false;
    } 
    else {
        return true;
    }
}

void KV_op::leveldb_set(const string &key, const string &value){
    WriteBatch wb;
    Status s;
    WriteOptions wopt;
    wb.Put(key, value);
    s = db->Write(wopt, &wb);
    assert(s.ok());
}

void KV_op::leveldb_del(const string &key){
    WriteOptions wopt;
    Status s;
    s = db->Delete(wopt, key);
    assert(s.ok());
}

void KV_op::destroy_leveldb(){
    Options options;
    leveldb::DestroyDB(dir, options);
}

void KV_op::open_leveldb(){
    Options options;
    options.create_if_missing = true;
    options.filter_policy = leveldb::NewBloomFilterPolicy(10);
    options.write_buffer_size = 1u << 21; //2mb
    destroy_leveldb();
    DB::Open(options, dir, &db);
}

