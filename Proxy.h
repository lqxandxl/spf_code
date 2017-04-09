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
#include "NTFMng.h"

using namespace std;


class PublishMng;
class NTFMng;
class Proxy{ //等效为task就好了
public:
    void procPubMsg(string s); //简化为直接处理json publish 中的msg业务
    void procNTFAckMsg(string msgid,string to); //需要知道两条信息 一个是对应的pub的消息id  还有一个是是谁发过来的ack
    void porcNTFAckMsginN(string s); //notify ack string 去处理
    Proxy();
    ~Proxy();
    NTFMng *  getNTFMng();
private:
    PublishMng * publishmng;
    NTFMng * ntfmng;


};
#endif //SPFCODE_PROXY_H
