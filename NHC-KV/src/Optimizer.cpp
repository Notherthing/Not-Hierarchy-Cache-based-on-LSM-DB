#include "Optimizer.h"

void Opt::init(){
    signal(SIGALRM,change_state);
    tick.it_value.tv_sec=0;
    tick.it_value.tv_usec=10;
    //进程10微妙拉起定时任务
    tick.it_interval.tv_sec=0;
    tick.it_interval.tv_usec=10e4; 
    //百分之一秒定时
    last_hit_rate = -1;
    //初始化命中率
    setitimer(ITIMER_REAL,&tick,NULL);
}

void Opt::change_state(int signo) {
    //cache未装载满，状态0
    if (watcher.cache_deal->size != watcher.cache_deal->capacity) {
        watcher.state = (unsigned)0;
        return;
    }
    if (watcher.state == (unsigned)0) {
        if (watcher.cache_deal->size != watcher.cache_deal->capacity) {
            return;
        }
        watcher.state = (unsigned)1;
        return;
    }
    //装载满后，等待cache命中率稳定，状态1
    if (watcher.state == (unsigned)1) {
        watcher.data_admit = true;
        watcher.load_admit = 1;
        double tmp_hit_rate = watcher.hit_rate();
        if (abs(tmp_hit_rate - last_hit_rate) < accuracy) {
            watcher.state = (unsigned)2;
            watcher.data_admit = false;
            last_hit_rate = tmp_hit_rate;
            return;
            //准备进入状态2，将da置为false，不进行cache替换
        }
        else {
            watcher.reset();
            last_hit_rate = tmp_hit_rate;
            last_la = watcher.load_admit; //获得开始la
            return;
            //维持状态1,等待命中率稳定
        }
    }
    //进入状态2，着手测量la-step与la+step
    //状态2预备测量la-step性能,获得la性能
    if (watcher.state == (unsigned)2) {
        la_perfer = watcher.perfermance(); 
        //获得f(ratio)
        last_la = watcher.load_admit;
        //记录当前la
        la_min_step = last_la - watcher.step > 0 ? last_la - watcher.step : 0;
        //获得la - step
        la_add_step = last_la + watcher.step > 1 ? 1 : last_la + watcher.step;
        //获得la + step
        if (la_min_step == 0) { //la - step = 0，直接准备开测la+step
            la_min_per = -1;//显然不能全部找容量层,直接将其表现挂为-1
            watcher.state = (unsigned)4;
            return;
        }
        else { //进入3测试la-step
            watcher.state = (unsigned)3;
            watcher.load_admit = la_min_step;//更新la
            watcher.reset(); //重置
            return;
        }
    }
    if (watcher.state == (unsigned)3) {
        la_min_per = watcher.perfermance();
        //获得了f(ratio - step)
        watcher.state = (unsigned)4;
        watcher.load_admit = la_add_step;
        watcher.reset();
        return;
        //准备进入状态4.预备测试la+step
    }
    if (watcher.state == (unsigned)4) {
        la_add_per = watcher.perfermance();
        //获得了f(ratio + step)
        //开始进行判断，重新更新状态
        double max_f = max(la_perfer,la_add_per);
        max_f = max(max_f,la_min_per);
        if (max_f == la_min_per) {
            watcher.load_admit = la_min_step;
        }
        else {
            if (max_f == la_add_per) {
                watcher.load_admit = la_add_step;
            }
            else {
                watcher.load_admit = last_la;
                if (watcher.load_admit == 1) {
                    watcher.state = (unsigned)1;
                    watcher.reset();
                    return;
                }
            }
        }
    }
    //判断工作负载（命中率是否仍然稳定）
    double tmp_hit_rate = watcher.hit_rate();
    if (abs(tmp_hit_rate - last_hit_rate) < accuracy) {
        watcher.state = (unsigned)1;
        watcher.reset();
        return;
    }
    watcher.state = (unsigned)2;
    watcher.reset();
    return;
}