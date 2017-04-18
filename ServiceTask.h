//
// Created by dell on 2017/4/1.
//


#ifndef SPFCODE_PROXY_H
#define SPFCODE_PROXY_H

#include <string>
//#include <map>
#include <unordered_map>
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
    void procMsg(TRscMsgHdr * rschdr, TRscMsgBody * rscbody,int msgType);


    void procNTFAckMsginP(string msgid,string to); //需要知道两条信息 一个是对应的pub的消息id  还有一个是是谁发过来的ack
    void prccNTFAckMsginN(TRscMsgHdr * head ,TRscMsgBody * body); //notify ack string 去处理
    void procPUBMsginP(TRscMsgHdr * head , TRscMsgBody * body);
    SubInfoMng * getSubMng();
    ServiceTask();
    ~ServiceTask();
    NTFMng *  getNTFMng();
    string getDest(string to);//向移动性管理模块查询地址
    void sendMsg(int code,string ruri,string from,string to,string rid,string destination,string jsoncontent);
private:
    PublishMng * publishmng;
    NTFMng * ntfmng;
    SubInfoMng * subinfomng;
    UtilService us;


};
#endif //SPFCODE_PROXY_H
