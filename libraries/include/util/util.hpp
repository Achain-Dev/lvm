/*
author: saiy
date: 2017.10.17
utility file
*/
#ifndef _UTIL_H_
#define _UTIL_H_

#include <stdlib.h>
#include <time.h>


class MsgIdMaker {
  public:
    MsgIdMaker::MsgIdMaker() = delete;
    
    static uint32_t get_msg_id() {
        uint32_t id = 0;
        srand((unsigned)time(NULL));
        id = rand();
        return id;
    }
};
#endif