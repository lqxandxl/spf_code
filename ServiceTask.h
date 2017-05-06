//
// Created by dell on 2017/4/1.
//


#ifndef SPFCODE_PROXY_H
#define SPFCODE_PROXY_H

#include <string>
#include <map>
#include "SerMessage.h"
#include "PublishMng.h"
#include "NTFMng.h"
#include "msgdef_rsc.h"
#include "SubInfoMng.h"

using namespace std;

typedef enum
{
    PUBLISH=1,
    PUBLISHACK=2,
    SUBSCRIBE=3,
    SUBSCRIBEACK=4,
    NOTIFY=5,
    NOTIFYACK=6
} RSC_SERVICE_CODE;
//RSC_SERVICE_CODE ser_Code; //switch(ser_Code)


class PublishMng;
class NTFMng;
class SubInfoMng;
class ServiceTask{ //等效为task就好了
public:
    ServiceTask();
    ~ServiceTask();

    void procMsg(TRscMsgHdr * rschdr, TRscMsgBody * rscbody,int msgType);

    //msg 业务
    void proc_msg_notifyack(TRscMsgHdr * head ,TRscMsgBody * body); //notify ack string 去处理
    void proc_msg_publish(TRscMsgHdr * head , TRscMsgBody * body);

    //state业务
    void proc_state_pub(TRscMsgHdr * head , TRscMsgBody * body);
    void proc_state_sub(TRscMsgHdr * head, TRscMsgBody * body);


    //处理移动性管理部分
    void proc_uaip(TRscMsgHdr * head , TRscMsgBody * body);
    void proc_satip(TRscMsgHdr * head , TRscMsgBody * body);

    //调用内部模块
    SubInfoMng * getSubMng();
    NTFMng *  getNTFMng();
    PublishMng * getPubMng();

    int send_map_add(string userid, string servicename, string msgtype,string msgid);
    int send_map_add(string userid, string servicename, string msgtype,string msgid,string topic);//为state publish下发做准备
    void get_uaip(string userid);
    void get_satip(string userid); //查询归属骨干卫星

private:
    PublishMng * publishmng;
    NTFMng * ntfmng;
    SubInfoMng * subinfomng;
    UtilService * us;
    int _addr_; //psa的编号  taddr.logaddr

    //为了发送消息 需要将userid 以及 其对应的要发送的消息 存放为map
    map<string , set<string> * > * send_map;  //userid --->  <xxx_msgid> msgid 比如为 msg&notify&msgid

    //local 消息缓存队列
    map<string, TUniNetMsg* > * local_map;
    map<string , set<string> * > * acc_map; // key为userid  value为属于该userid的所有local消息

    string local_ip; //本地ip以及端口
    int  local_port;

};
#endif //SPFCODE_PROXY_H
