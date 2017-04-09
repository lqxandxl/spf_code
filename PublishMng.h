//
// Created by dell on 2017/4/1.
//

#ifndef SPFCODE_PUBLISHMNG_H
#define SPFCODE_PUBLISHMNG_H

#include <string>
#include <unordered_map>
#include "JSON.h"
#include "JSONValue.h"
#include "Message.h"
#include "UtilService.h"
#include "Proxy.h"

using namespace std;

class Proxy;
class PublishMng{
public:
    PublishMng(Proxy * proxy1);
    ~PublishMng();
    void procPubMsg(string s); //简化为直接处理json publish 中的msg业务   rsc body
    void procNTFAckMsg(string msgid,string to); //需要知道两条信息 一个是对应的pub的消息id  还有一个是是谁发过来的ack
    //static int msgid =0; //msgid 自增 每来一个消息 加1
    //void deletePublishMsg(string msgid);  //从map中删除消息
    //~PublishMng(); //遍历map 先清除消息 再 clear map
    void showMsgMap(); //遍历显示一遍map

private:
    unordered_map<string , PublishMsg *>  * msg_map;   //存放 msgid 和 msg消息的指针  当收到ack时 检查msg状态
    // 当发送完notify 收到notifyack 则可以删除publsihMsg消息了
    UtilService * us;
    Proxy * proxy;

};
#endif //SPFCODE_PUBLISHMNG_H
