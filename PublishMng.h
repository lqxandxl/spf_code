#ifndef SPFCODE_PUBLISHMNG_H
#define SPFCODE_PUBLISHMNG_H


#include <string>
#include <map>
#include <vector>
#include <set>
#include "json.h"
#include "jsonvalue.h"
#include "SerMessage.h"
#include "UtilService.h"
#include "ServiceTask.h"
#include "msgdef_rsc.h"


using namespace std;

class ServiceTask;
class PublishMng{
public:
    PublishMng(ServiceTask * proxy1);
    ~PublishMng();

    //处理msg业务
    void proc_msg_publish(TRscMsgHdr * rschdr, TRscMsgBody * rscbody); //简化为直接处理json publish 中的msg业务
    void proc_msg_notifyack(string msgid,string to); //需要知道两条信息 一个是对应的pub的消息id  还有一个是是谁发过来的ack

    //处理state业务
    void proc_state_pub(TRscMsgHdr * head ,TRscMsgBody * rscbody); //处理状态推送类消息 这样的消息 需要去查询订阅树
    string get_publish_body(string topic);

    //set<string> * get_msg_puback_set();
    void clean_stateset(); //清除stateset
    
private:
    map<string , PublishMsg *>  * msg_map;   //存放 msgid 和 msg消息的指针  当收到ack时 检查msg状态

    //state 业务
    set<string >  * stateset; //防止重复处理pub请求
    map<string,string> * topic_map; //按topic以及rscbody存储 便于发送


    UtilService * us;
    ServiceTask * proxy;

};


#endif //SPFCODE_PUBLISHMNG_H