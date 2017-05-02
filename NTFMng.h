//
// Created by dell on 2017/4/6.
//

#ifndef SPFCODE_NTFMNG_H
#define SPFCODE_NTFMNG_H

#include "PublishMng.h"
#include "ServiceTask.h"
#include "msgdef_rsc.h"

class ServiceTask;//前置声明
class NTFMng{
public:
    NTFMng(ServiceTask * proxy1);
    ~NTFMng();

    //处理msg业务
    void proc_msg_publish(PublishMsg * publishmsg); //处理publish的msg业务消息
    void proc_msg_notifyack(TRscMsgHdr * head , TRscMsgBody * body);//专门处理msg类的NOTIFYACK消息
    NotifyMsg * find_msg_notify(string msgid);


private:
    ServiceTask * proxy;
    map <string , NotifyMsg * > * ntfmap; //管理离线消息的结构
    UtilService * us;
};
#endif //SPFCODE_NTFMNG_H
