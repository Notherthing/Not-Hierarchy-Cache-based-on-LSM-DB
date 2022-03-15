#include "NHC_DB.h"

NHC_KV::NHC_KV(const string &path) {
    stringstream ss;
    ss<<path;
    ss>>cachePath;
    ss>>dbPath;
}


NHC_DB * NHC_KV::open_NHC() {
    NHC_DB * nhc_db = new NHC_DB;
    nhc_db->cache = new KV_op(cachePath);
    nhc_db->mainDB = new KV_op(dbPath);
    nhc_db->cache->open_leveldb();
    nhc_db->mainDB->open_leveldb();
    cacheManange = new LRUcache(nhc_db->cache,2e12);
    return nhc_db;
}