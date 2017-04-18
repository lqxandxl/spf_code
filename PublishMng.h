//
// Created by dell on 2017/4/1.
//

#ifndef SPFCODE_PUBLISHMNG_H
#define SPFCODE_PUBLISHMNG_H

#include <string>
#include <unordered_map>
#include "JSON.h"
#include "JSONValue.h"
#include "SerMessage.h"
#include "UtilService.h"
#include "ServiceTask.h"
#include "msgdef_rsc.h"
#include "SerTreeNode.h"

using namespace std;

class ServiceTask;
class PublishMng{
public:
    PublishMng(ServiceTask * proxy1);
    void procPubMsg(TRscMsgHdr * rschdr, TRscMsgBody * rscbody); //简化为直接处理json publish 中的msg业务
    ~PublishMng();
    void procPubMsgTest(string s); //简化为直接处理json publish 中的msg业务   rsc body
    void procNTFAckMsg(string msgid,string to); //需要知道两条信息 一个是对应的pub的消息id  还有一个是是谁发过来的ack

    void procPubState(TRscMsgHdr * head ,TRscMsgBody * rscbody); //处理状态推送类消息 这样的消息 需要去查询订阅树
    //static int msgid =0; //msgid 自增 每来一个消息 加1
    //void deletePublishMsg(string msgid);  //从map中删除消息
    //~PublishMng(); //遍历map 先清除消息 再 clear map
    void showMsgMap(); //遍历显示一遍map

private:
    unordered_map<string , PublishMsg *>  * msg_map;   //存放 msgid 和 msg消息的指针  当收到ack时 检查msg状态
    // 当发送完notify 收到notifyack 则可以删除publsihMsg消息了
    set<string >  * stateSet;
    UtilService * us;
    ServiceTask * proxy;

};
#endif //SPFCODE_PUBLISHMNG_H
