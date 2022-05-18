#include "NHC_DB.h"

NHC_KV::NHC_KV(const string &path) {
    stringstream ss;
    ss<<path;
    ss>>cachePath;
    ss>>dbPath;
    srand(time(NULL));
}


NHC_DB * NHC_KV::open_NHC(long long int cacheCap) {
    NHC_DB * nhc_db = new NHC_DB;
    nhc_db->cache = new KV_op(cachePath);
    nhc_db->mainDB = new KV_op(dbPath);
    nhc_db->cache->open_leveldb();
    nhc_db->mainDB->open_leveldb();
    cacheManange = new LRUcache(nhc_db->cache,cacheCap);
    Opt::watcher.cache_deal = cacheManange;
    //可以存放2e12条记录，1024*4
    Opt::init();//开优化控制器
    return nhc_db;
}

void NHC_KV::close_NHC_KV(){
    delete db->cache;
    delete db->mainDB;
    delete cacheManange;
}

NHC_KV::~NHC_KV(){
    delete db;
}

double NHC_KV::getRand() {
    return rand() % (N + 1) / (double) (N +1);
}

bool NHC_KV::nhc_get(const string &key, string &value){
    bool result;
    Opt::watcher.vis++;
    Opt::watcher.cache_vis++;
    if (cacheManange->cache.find(key)!=cacheManange->cache.end()) {
        Opt::watcher.cache_hit++;
        if (getRand() < Opt::watcher.load_admit) {
        //从缓存中找
            if (cacheManange->get(key, value)) {
                Opt::watcher.get_num++;
                return true;
            }
        }
        else {
            result = db->mainDB->leveldb_get(key, value);
            if (Opt::watcher.data_admit) { //如果da为true，更新缓存
                cacheManange->put(key,value);
            }
            Opt::watcher.get_num++;
            return result;
        }
    }
    //缓存中没有,直接去数据找
    result = db->mainDB->leveldb_get(key, value);
    if (Opt::watcher.data_admit) { //如果da为true，更新缓存
        cacheManange->put(key,value);
    }
    Opt::watcher.get_num++;
    return result;
}

void NHC_KV::nhc_set(const string &key,const string &value){
    //先更新数据库,再尝试删除缓存
    //修改了方案，先删除缓存，再更新数据库，由于leveldb在绝大多数情况下，写比读快
    if (cacheManange->cache.find(key)==cacheManange->cache.end()) {
        db->mainDB->leveldb_set(key, value);
        return;
    }
    cacheManange->del(key);
    db->mainDB->leveldb_set(key, value);
    return;
}

void NHC_KV::nhc_del(const string &key) {
    db->mainDB->leveldb_del(key);
    //先删除数据库，再尝试删除缓存
    if (cacheManange->cache.find(key)==cacheManange->cache.end()) {
        return;
    }
    cacheManange->del(key);
    return;
}

void NHC_KV::nhc_put(const string &key,const string &value) {
    db->mainDB->leveldb_set(key, value);
    return;
}
