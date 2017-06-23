#ifndef _SERVICE_TASK_HPP_
#define _SERVICE_TASK_HPP_

#include <iostream>

#include "msgdef_rsc.h"
#include "msgdef_com.h"
#include "msgdatadef.h"
#include "abstracttask.h"
//ServiceTask

//for use json show in NotifyTask.C in rtc line 530

#include "json.h"
#include "jsonvalue.h"
#include <string>
#include <map>
#include "SerMessage.h"
#include "PublishMng.h"
#include "NTFMng.h"
#include "msgdef_rsc.h"

//for state
#include "SubInfoMng.h"

//6-22 move
#include "SerTreeNode.h" 

using namespace std;

_CLASSDEF(ServiceTask);

typedef enum
      {
         PUBLISH=0x00001000,
		 PUBLISHACK=0x00002000,
		 SUBSCRIBE=0x00003000,
		 SUBSCRIBEACK=0x00004000,
		 NOTIFY=0x00005000,
		 NOTIFYACK=0x00006000
      } RSC_SERVICE_CODE;
    //RSC_SERVICE_CODE ser_Code; //switch(ser_Code)

class PublishMng;
class NTFMng;
class SubInfoMng;
class ServiceTask : public TAbstractTask
{
_COMPONETDEF(ServiceTask);

public:
	ServiceTask();
	virtual ~ServiceTask();

	BOOL onInit(TiXmlElement*	extend);
	virtual BOOL reloadTaskEnv(CStr& cmd, TiXmlElement* & extend);
    virtual BOOL preTaskStart();

	void procMsg(std::auto_ptr<TUniNetMsg> msg);
	void onTimeOut(TTimerKey timerKey, void* para);
	
	
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


    //发送消息写在函数中
    void set_rsc_head(TRscMsgHdr* rschdr,string hostip,int code,string ruri,string consumer,string producer,string rid);
    void set_rsc_body(TRscMsgBody* rscbody,string body);
    void set_unimsg(TUniNetMsg* unimsg,int addr, int type);


    //6-22 get_cache_map
    map<string,string> * get_cache_map();
    string get_local_ip();
    //6-22 数据迁移 将userid的数据迁移到satip上
    void move_to_newip(string userid,string satip);
    void dfs_deal(SerTreeNode * root,string userid,string satip,string topic);
    
private:
    PublishMng * publishmng;
    NTFMng * ntfmng;
    SubInfoMng * subinfomng;
    UtilService * us;

    //为了发送消息 需要将userid 以及 其对应的要发送的消息 存放为map
    map<string , set<string> * > * send_map;  //userid --->  <xxx_msgid> msgid 比如为 msg&notify&msgid

    //local 消息缓存队列
    map<string, TUniNetMsg* > * local_map;
    map<string , set<string> * > * acc_map; // key为userid  value为属于该userid的所有local消息
    
    //6-22
    //缓存存储userid，归属ip端口,只有在sub remote时才会向里面添加内容
    map<string,string> * cache_map;

    int _addr_; //psa的编号  taddr.logaddr
    int wp_addr;//wang pan id
    int  count_move; //计数
    CStr local_ip; //本地ip以及端口
    int  local_port;

    TTimerKey  mMaintain;//定时器

	
 };

#endif