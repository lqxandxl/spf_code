//
// Created by dell on 2017/4/1.
//


#ifndef SPFCODE_PROXY_H
#define SPFCODE_PROXY_H

#include <string>
//#include <map>
#include <unordered_map>
#include "Message.h"
#include "PublishMng.h"

using namespace std;



class Proxy{ //等效为task就好了
public:
    void procPubMsg(string s); //简化为直接处理json publish 中的msg业务

private:
    PublishMng publishmng;

};
#endif //SPFCODE_PROXY_H
