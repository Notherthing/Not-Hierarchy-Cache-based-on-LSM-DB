#ifndef OPTIMIZER_H_
#define OPTIMIZER_H_

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
#include "LRU_cache.h"

using std::string;
using std::vector;
using std::stringstream;
using std::unordered_map;
using std::mutex;
using std::lock_guard;
using std::max;
using leveldb::ReadOptions;
using leveldb::Options;
using leveldb::Status;
using leveldb::WriteBatch;
using leveldb::WriteOptions;
using leveldb::DB;

typedef long long int lli;
struct Optimizer{
    lli vis = 0; //单位时间内请求量
    lli get_num = 0; //单位时间内请求返回成功量
    lli cache_vis = 0; //访问缓存次数
    lli cache_hit = 0; //缓存命中次数
    bool data_admit = true; //是否将数据分配至缓存
    double load_admit = 1.0; //由该值决定，命中时，多少访问缓存，多少访问容量层
    double step = 0.05; //la修改步长
    unsigned state = 0; //状态
    // 0：cache未满
    // 1：等待cache命中率稳定
    // 2：寻求更优表现,开始着手测量
    LRUcache * cache_deal; //用于查看cache是否装满的指针
    double hit_rate() {
        return cache_hit/cache_vis;
    } //返回命中率
    double perfermance() {
        return get_num/vis;
    } //返回完成请求的比例
    void reset() {
        vis = 0;
        get_num = 0;
        cache_vis = 0;
        cache_hit = 0;
    }
    //重新计数
};

class Opt {
public:
    static Optimizer watcher;
    static struct itimerval tick;
    static double last_hit_rate; //也可做start_hit_rate
    static const double accuracy = 0.02; //误差精度
    static double last_la; //当前la
    static double la_add_step; //la+step
    static double la_min_step; //la-step
    static double la_perfer; //la的表现
    static double la_min_per;//la-step的表现
    static double la_add_per;//la+steo的表现
public:
    static void init();
    //初始化tick
    static void change_state();
};
#endif //OPTIMIZER_H_