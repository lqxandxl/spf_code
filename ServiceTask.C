#include "ServiceTask.h"

ServiceTask::ServiceTask() : _addr_(0), TAbstractTask()
{
    publishmng=new PublishMng(this);
    //publishmng->setProxy(this);
    ntfmng=new NTFMng(this);
    subinfomng = new SubInfoMng(this);
    us=new UtilService();
    send_map=new map<string,set<string> * > ();
    count_move=0;

    //local
    local_map=new map<string ,TUniNetMsg*>();
    acc_map=new map<string, set<string> * >();

    //6-22
    cache_map=new map<string,string>();
}


ServiceTask::~ServiceTask()
{
    delete publishmng;
    delete ntfmng;
    delete subinfomng;
    delete us;
    //清除map
    map<string , set<string> *> :: iterator itbegin=send_map->begin();
    map<string , set<string> *> :: iterator itend=send_map->end();
    for(;itbegin!=itend;itbegin++){
        set<string>  * tmp=itbegin->second;
        delete tmp;
    }
    send_map->clear();
    delete send_map;


    //清map
    map<string , set<string> *> :: iterator itbegin1=acc_map->begin();
    map<string , set<string> *> :: iterator itend1=acc_map->end();
    for(;itbegin1!=itend1;itbegin1++){
        set<string>  * tmp=itbegin1->second;
        delete tmp;
    }
    acc_map->clear();
    delete acc_map;

    map<string , TUniNetMsg*> :: iterator itbegin2=local_map->begin();
    map<string , TUniNetMsg*> :: iterator itend2=local_map->end();
    for(;itbegin2!=itend2;itbegin2++){
        TUniNetMsg  * tmp=itbegin2->second;
        delete tmp;
    }
    local_map->clear();
    delete local_map;

    //6-22
    cache_map->clear();

}


//msg 业务接口
void ServiceTask ::proc_msg_publish(TRscMsgHdr *head, TRscMsgBody *body) {
    publishmng->proc_msg_publish(head,body);
}

void ServiceTask:: proc_msg_notifyack(TRscMsgHdr * head ,TRscMsgBody * body ) { //notify ack string 去处理
    ntfmng->proc_msg_notifyack(head,body);
}

// state 业务外部接口
void ServiceTask :: proc_state_pub(TRscMsgHdr * head , TRscMsgBody * body){
    publishmng->proc_state_pub(head,body);
}

void ServiceTask :: proc_state_sub(TRscMsgHdr * head, TRscMsgBody * body){
    subinfomng->proc_state_sub(head,body);
}


BOOL ServiceTask::reloadTaskEnv(CStr& cmd, TiXmlElement* & extend)
{
	return TRUE;
}

BOOL ServiceTask::onInit(TiXmlElement*	extend)
{
	TiXmlHandle handle(extend);
	TiXmlElement*	task=NULL;
	task=handle.FirstChild("task").Element();
	if(task)
	{
		TiXmlHandle psahandle(task);
		TiXmlElement*	target=NULL;
		target=psahandle.FirstChild("target").Element();

		if(target)
		{
			if(!target->Attribute("psaTask", &_addr_)) _addr_=4;
            if(!target->Attribute("wpTask", &wp_addr)) wp_addr=4;
            local_ip=target->Attribute("ip");
            if(!target->Attribute("local_port",&local_port)) local_port=8900;
        }

	}
    //设置定时器
    mMaintain=setTimer(10);

	return TRUE;
}

void ServiceTask::onTimeOut(TTimerKey timerKey, void* para)
{
    if(timerKey == mMaintain){
        //一定时间清除推送set
        PublishMng * p=getPubMng();
        p->clean_stateset();//定时清理一下
        SubInfoMng * s=getSubMng();
        s->clean_substateset();//定时清理，防止拒绝sub请求

        //6-22新增，定期查询userid归属，看是否发生变化，如果变化则数据迁移
        map<string,string>::iterator itbegin=cache_map->begin();
        map<string,string>::iterator itend=cache_map->end();
        for(;itbegin!=itend;++itbegin){
            get_satip(itbegin->first); //查询userid归属ip是否发生变化
        }

        mMaintain=setTimer(10);
    }

	return;
}

BOOL ServiceTask::preTaskStart()
{

    return TRUE;
}

void ServiceTask::procMsg(std::auto_ptr<TUniNetMsg> msg)
{

	UniINFO("com a msg");
	TUniNetMsg* unimsg=msg.get();
	if(unimsg==NULL) return;

    TUniNetMsgName msgType=unimsg->msgName;
	TRscMsgHdr* rschdr = NULL;
    TRscMsgBody* rscbody =  NULL;
    rschdr=dynamic_cast<TRscMsgHdr*> (unimsg->getCtrlMsgHdr());
    rscbody=dynamic_cast<TRscMsgBody*> (unimsg->getMsgBody());

	if(rschdr==0) return; //非法rsc消息不进行处理
	UINT rscHdrCode=rschdr->code;
	CStr rscRuri=rschdr->ruri;
	string str_rscRuri=rscRuri.c_str();  //cstr change to string
	vector<string> * topicVec=us->splitRuri(str_rscRuri,'/'); //ruri change to vector<string >  must free
    UniINFO("com a msg ,code is %d",rscHdrCode);
    if(rscHdrCode==0x00000060) {//暂时认定60为移动性管理接口的code
        if(topicVec->size()<=1){
              UniINFO("recv a wp_msg,error len<=1");
              return;
        }
        if((*topicVec)[1]=="acsip"){
              UniINFO("recv a wp_msg acsip,begin to proc");
              proc_uaip(rschdr,rscbody);
        }
        else if((*topicVec)[1]=="satip"){
              UniINFO("recv a wp_msg satip,begin to proc");
              proc_satip(rschdr,rscbody);
        }
        return;
    }


    if((*topicVec)[0] != "service" || topicVec->size()<2) return; //error message

    switch(rscHdrCode){
			  case PUBLISH :{ //PUBLISH 有不同的业务 需要 ruri 去判断 比如 消息业务 对话业务 状态推送等等
                  if((*topicVec)[1]=="msg"){
                    UniINFO("begin to proc publish msg");
                    proc_msg_publish(rschdr,rscbody);
				  }
				  else if((*topicVec)[1]=="dlg"){

				  }
				  else if((*topicVec)[1]=="state"){
                    //std::cout << "come a publish state to process" << std::endl;
                    if (topicVec->size() < 3) return;
                    if ((*topicVec)[2] == "local") {
                        //std::cout << "come a publish state to process local" << std::endl;
                        string from=rschdr->producer.c_str();
                        string msgid=rschdr->rid.c_str();
                        //local消息一概不处理 放入队列
                        //收到sat内容后，再分类去相应模块处理 或转发
                        string tmp1 = "state";
                        string name = tmp1 + "_" + "publish" + "_" + from + "_" + msgid; //生成一个唯一标识
                        msg->tAddr.reset(); //新xframe taddr不能有内容 否则会按taddr转发
                        //防止消息丢失
                        (*local_map)[name] = msg.release();

                        //再用一个以userid为key的map<userid,set<name>> 加速后面的查找
                        map<string, set<string> *>::iterator itacc;
                        itacc = acc_map->find(from);
                        if (itacc != acc_map->end()) { //find it
                            set<string> *tmpaccset = itacc->second;
                            tmpaccset->insert(name);
                        } else {
                            (*acc_map)[from] = new set<string>();
                            map<string, set<string> *>::iterator tmpacc;
                            tmpacc = acc_map->find(from);
                            set<string> *tmpaccset = tmpacc->second;
                            tmpaccset->insert(name);
                        }

                        //查询归属骨干卫星
                        get_satip(from);
                        //回复用户收到了local请求
                        int res = send_map_add(from,"state","publishack",msgid); //ack
                        if(res==1){
                            get_uaip(from); //查询地址
                        }
                        else{ //防止内部失败也要查询

                        }
                    } else if ((*topicVec)[2] == "remote") {
                        UniINFO("recv a state pub ,remote and begin to proc");
                        proc_state_pub(rschdr, rscbody);
                    }
				  }
                  break;
              }//case publish
			  case SUBSCRIBE :{
                if((*topicVec)[1]=="msg"){

                }
                else if((*topicVec)[1]=="dlg"){

                }
                else if((*topicVec)[1]=="state"){
                    //std::cout << "come a subscribe state to process" << std::endl;
                    if (topicVec->size() < 3) return;
                    if ((*topicVec)[2] == "local") {
                        //std::cout << "come a subscribe state to process local" << std::endl;
                        string from=rschdr->producer.c_str();
                        string to=rschdr->consumer.c_str(); //定位sub到哪里
                        string msgid=rschdr->rid.c_str();
                        //local消息一概不处理 放入队列
                        //收到sat内容后，再分类去相应模块处理 或转发
                        string tmp1 = "state";
                        string name = tmp1 + "_" + "subscribe" + "_" + to + "_" + msgid; //生成一个唯一标识
                        msg->tAddr.reset(); //新xframe taddr不能有内容 否则会按taddr转发
                        //unimsg 修改为 msg.release 将控制权转交给map
                        (*local_map)[name] = msg.release();

                        //再用一个以userid为key的map<userid,set<name>> 加速后面的查找
                        map<string, set<string> *>::iterator itacc;
                        itacc = acc_map->find(to);
                        if (itacc != acc_map->end()) { //find it
                            //cout<<11111<<name<<endl;
                            set<string> *tmpaccset = itacc->second;
                            tmpaccset->insert(name);
                        } else {
                            //cout<<22222<<name<<endl;
                            (*acc_map)[to] = new set<string>();
                            map<string, set<string> *>::iterator tmpacc;
                            tmpacc = acc_map->find(to);
                            set<string> *tmpaccset = tmpacc->second;
                            tmpaccset->insert(name);
                        }
                        //查询归属骨干卫星
                        get_satip(to);
                        //回复用户收到了local请求
                        int res = send_map_add(from,"state","subscribeack",msgid); //ack
                        if(res==1){
                            get_uaip(from); //查询地址
                        }
                        else{ //防止内部失败也要查询

                        }
                    } else if ((*topicVec)[2] == "remote") {
                        UniINFO("recv a state sub ,remote and begin to proc");
                        proc_state_sub(rschdr, rscbody);
                    }
                }
                break;
			  }//case subsribe
			  case NOTIFY :{
                  break;
			  }
			  case PUBLISHACK :{
                  break;
			  }
			  case SUBSCRIBEACK :{
                  break;
			  }
			  case NOTIFYACK :{
				  if((*topicVec)[1]=="msg"){
                    //判断是否是本机的，不是则转发,不需要查询了，直接转发ip
                    string local_ip_tmp = rschdr->consumer.c_str(); //目的端ip客户端自己填写，终端发响应到接入卫星，接入卫星发给自己连接的骨干卫星，因此响应要进行一次判断
                    if(local_ip.c_str()==local_ip_tmp){
                      proc_msg_notifyack(rschdr,rscbody);
                    }
                    else{//消息转发
                         //通过tmps取出消息后 将ruri修改为/service/state/remote
                         //rid consumer都还在 不做区分 真正处理时再和一起
                         //send msg
                         //空就是不做修改
                         set_rsc_head(rschdr,local_ip_tmp,0x00006000,"","","","");
                         set_rsc_body(rscbody,"");
                         set_unimsg(unimsg,_addr_,1);//addr应该是psa的任务号
                         unimsg->setCtrlMsgHdr(rschdr);
                         unimsg->setMsgBody(rscbody);
                         sendMsg(unimsg);
                         //send msg
                     }
				  }

                  break;
			  }
		  }//switch
    delete topicVec; //free vec
	return;
}





SubInfoMng * ServiceTask :: getSubMng(){
    return subinfomng;
}


NTFMng*  ServiceTask ::getNTFMng() {
    return ntfmng;
}


PublishMng * ServiceTask :: getPubMng(){
    return publishmng;
}


int ServiceTask :: send_map_add(string userid, string servicename, string msgtype,string msgid){
    map<string , set<string> *> :: iterator it1;
    if(userid.length()==0) return -1;
    UniINFO("send_map_add userid is %s",userid.c_str());
    it1=send_map->find(userid);
    if(it1!=send_map->end()){ //找到后对set进行操作
        set <string> * tmpset=it1->second;
        set<string > :: iterator it2;
        string newmsgid=servicename+"&"+msgtype+"&"+msgid;
        it2=tmpset->find(newmsgid);
        if(it2!=tmpset->end()){
           return -1;
        }
        else{
           tmpset->insert(newmsgid);
           UniINFO("send_map_add 1 newmsgid insert %s",newmsgid.c_str());
           return 1;
        }
    }
    else{ //没找到添加元素
        (*send_map)[userid]=new set<string>();
        set<string> * tmpset=(*send_map)[userid];
        string newmsgid=servicename+"&"+msgtype+"&"+msgid;
        tmpset->insert(newmsgid);
        UniINFO("send_map_add 2 newmsgid insert %s",newmsgid.c_str());
    }
    return 1;
}


int ServiceTask :: send_map_add(string userid, string servicename, string msgtype,string msgid,string topic){
    map<string , set<string> *> :: iterator it1;
    if(userid.length()==0) return -1;
    it1=send_map->find(userid);
    if(it1!=send_map->end()){ //找到后对set进行操作
        set <string> * tmpset=it1->second;
        set<string > :: iterator it2;
        string newmsgid=servicename+"&"+msgtype+"&"+msgid+"&"+topic;
        it2=tmpset->find(newmsgid);
        if(it2!=tmpset->end()){
            return -1;
        }
        else{
            tmpset->insert(newmsgid);
            return 1;
        }
    }
    else{ //没找到添加元素
        (*send_map)[userid]=new set<string>();
        set<string> * tmpset=(*send_map)[userid];
        string newmsgid=servicename+"&"+msgtype+"&"+msgid+"&"+topic;
        tmpset->insert(newmsgid);
    }
    return 1;
}


/*
 *  构造一个json
 *  {
 *    "userid":"username"
 *  }
 */
void ServiceTask :: get_uaip(string userid){
    JSONObject newJsonObject;
    newJsonObject[L"userid"]=new (std::nothrow) JSONValue(us->s2ws(userid));
    JSONValue res=newJsonObject;
    std::wstring resstr=res.Stringify().c_str();
    string body=us->ws2s(resstr); //得到body
    //ruri = /uaip
    //rid自己构造 不重复就行
    //code 填写get方式的code
    //taddr.log 填写王任务号
    //send msg
    TUniNetMsg* unimsg = new TUniNetMsg();
    TRscMsgHdr* rschdr=new TRscMsgHdr();
    TRscMsgBody* rscbody=new TRscMsgBody();

    count_move++;
    int int_max=0x7fffffff;
    if(count_move == int_max)
      count_move=0;
    string rid=us->int_to_string(count_move); //自动生成rid

    //get
    set_rsc_head(rschdr,"",0x00000050,"/get/acsip","","",rid);
    set_rsc_body(rscbody,body);
    set_unimsg(unimsg,wp_addr,0);//addr应该是移动管理的任务号

    unimsg->setCtrlMsgHdr(rschdr);
    unimsg->setMsgBody(rscbody);

    UniINFO("send to get uaip for %s",userid.c_str());
    UniINFO("wangpan task id is %d",wp_addr);
    sendMsg(unimsg);
    //send msg




}

/*
 *  构造一个json
 *  {
 *    "uid":"username"
 *  }
 */

void ServiceTask ::get_satip(string userid){
    JSONObject newJsonObject;
    newJsonObject[L"uid"]=new (std::nothrow) JSONValue(us->s2ws(userid));
    JSONValue res=newJsonObject;
    std::wstring resstr=res.Stringify().c_str();
    string body=us->ws2s(resstr); //得到body
    //send msg
    TUniNetMsg* unimsg = new TUniNetMsg();
    TRscMsgHdr* rschdr=new TRscMsgHdr();
    TRscMsgBody* rscbody=new TRscMsgBody();

    count_move++;
    int int_max=0x7fffffff;
    if(count_move == int_max)
      count_move=0;
    string rid=us->int_to_string(count_move); //自动生成rid

    //get
    set_rsc_head(rschdr,"",0x00000050,"/get/satip","","",rid);
    set_rsc_body(rscbody,body);
    set_unimsg(unimsg,wp_addr,0);//addr应该是移动管理的任务号

    unimsg->setCtrlMsgHdr(rschdr);
    unimsg->setMsgBody(rscbody);
    //std::cout << "get satip for" << userid << std::endl;
    UniINFO("send to get satip for %s",userid.c_str());
    UniINFO("wangpan task id is %d",wp_addr);
    sendMsg(unimsg);
    //send msg
}



//针对body进行解析 并执行消息发送
void ServiceTask :: proc_uaip(TRscMsgHdr * head , TRscMsgBody * body){

    string content=body->rsc.c_str();
    string userid;
    string uaip;
    string status;
    JSONValue * recjv=JSON::Parse(content.c_str());
    UniINFO("begin to proc uaip msg content is  %s",content.c_str());
    if(recjv==NULL||!recjv->IsObject()) return ;
    JSONObject root=recjv->AsObject();
    JSONObject::const_iterator it=root.find(L"userid");
    if(it!=root.end()){//have found
        if(it->second->IsString()){
            wstring str = it->second->AsString();
            userid=us->ws2s(str);
        }
    }
    JSONObject::const_iterator it2=root.find(L"status");
    if(it2!=root.end()){//have found
        if(it2->second->IsString()){
            wstring str = it2->second->AsString();
            status=us->ws2s(str);
        }
    }
    JSONObject::const_iterator it3=root.find(L"uaip");
    if(it3!=root.end()){//have found
        if(it3->second->IsString()){
            wstring str = it3->second->AsString();
            uaip=us->ws2s(str);
        }
    }
    UniINFO("begin to proc uaip msg for %s",userid.c_str());
    if(status=="1"){ //合法状态 需要具体发出消息
        map<string , set<string> * > :: iterator it4;
        it4=send_map->find(userid);
        if(it4!=send_map->end()){ //find it
           UniINFO("proc uaip find userid is %s",userid.c_str());
           set<string > * tmpset=it4->second;
           set<string> :: iterator itbegin=tmpset->begin();
           set<string> :: iterator itend=tmpset->end();
           vector<string> * readytodel=new vector<string>();
            for(;itbegin!=itend;itbegin++){
                string tmps=*itbegin;
                UniINFO("proc uaip sendmap has tmpset to send %s",tmps.c_str());
                vector<string> * msgvec=us->splitTopic(tmps,'&');
                if(msgvec->size()<=2){
                    delete msgvec;
                    continue;
                }
                else{
                    string servicename=(*msgvec)[0];
                    string msgtype=(*msgvec)[1];
                    string msgid=(*msgvec)[2];
                    if(servicename=="msg"){
                         if(msgtype=="notify"){ //need to send notify
                             NotifyMsg * tmpmsg1=ntfmng->find_msg_notify(msgid);//find it
                             if(tmpmsg1!=NULL){
                               //send msg
                               TUniNetMsg* unimsg = new TUniNetMsg();
	                           TRscMsgHdr* rschdr=new TRscMsgHdr();
	                           TRscMsgBody* rscbody=new TRscMsgBody();
                               //get
                               //producer设置为本机ip，方便notifyack回响应
                               string local_ip_tmp=local_ip.c_str();
                               UniINFO("begin to send notify msg to %s",(tmpmsg1->to).c_str());
                               set_rsc_head(rschdr,uaip,0x00005000,"/service/msg",local_ip_tmp,tmpmsg1->to,tmpmsg1->from+"_"+tmpmsg1->msgid);
                               set_rsc_body(rscbody,tmpmsg1->body);
                               set_unimsg(unimsg,_addr_,1);//addr应该是psa的任务号

	                           unimsg->setCtrlMsgHdr(rschdr);
	                           unimsg->setMsgBody(rscbody);
	                           sendMsg(unimsg);
                               //send msg
                              }
                               //发送完从消息队列中移除消息
                               readytodel->push_back(tmps);
                         }
                         else if(msgtype=="publishack"){
                                   //send msg 需要msgid userid
                                   //send msg
                                   TUniNetMsg* unimsg = new TUniNetMsg();
	                               TRscMsgHdr* rschdr=new TRscMsgHdr();
	                               TRscMsgBody* rscbody=new TRscMsgBody();

                                     //msgid已经分解好了 直接回复即可
                                   set_rsc_head(rschdr,uaip,0x00002000,"/service/msg","",userid,msgid);
                                   set_rsc_body(rscbody,"this is publishack body");
                                   set_unimsg(unimsg,_addr_,1);//addr应该是psa的任务号

	                               unimsg->setCtrlMsgHdr(rschdr);
	                               unimsg->setMsgBody(rscbody);

	                               sendMsg(unimsg);

                                   //send msg
                                   //发送完从消息队列中移除消息
                                   readytodel->push_back(tmps);
                         }


                    }
                    else if(servicename=="state"){
                        if(msgtype=="notify"){//publish 消息要下发到订阅的用户
                             if(msgvec->size()<=3){
                                readytodel->push_back(tmps);
                                delete msgvec;
                                continue;
                             }
                             else{
                                string topic=(*msgvec)[3];
                                PublishMng * tmppub=getPubMng();
                                string body=tmppub->get_publish_body(topic); //取得内容
                                if(body.length()==0){
                                    readytodel->push_back(tmps);
                                    delete msgvec;
                                    continue;
                                }
                                else{
                                    //send msg
                                    TUniNetMsg* unimsg = new TUniNetMsg();
                                    TRscMsgHdr* rschdr=new TRscMsgHdr();
                                    TRscMsgBody* rscbody=new TRscMsgBody();

                                    //msgid这里为from+msgid
                                    set_rsc_head(rschdr,uaip,0x00005000,"/service/state","",userid,msgid);
                                    set_rsc_body(rscbody,body);
                                    set_unimsg(unimsg,_addr_,1);//addr应该是psa的任务号

                                    unimsg->setCtrlMsgHdr(rschdr);
                                    unimsg->setMsgBody(rscbody);
                                    sendMsg(unimsg);
                                    //send msg
                                    //发送完从消息队列中移除消息
                                    readytodel->push_back(tmps);
                                }


                             }
                        }
                        else if(msgtype=="publishack"){
                            //rid
                            //send msg
                            TUniNetMsg* unimsg = new TUniNetMsg();
                            TRscMsgHdr* rschdr=new TRscMsgHdr();
                            TRscMsgBody* rscbody=new TRscMsgBody();

                            //msgid已经分解好了 直接回复即可
                            set_rsc_head(rschdr,uaip,0x00002000,"/service/state","",userid,msgid);
                            set_rsc_body(rscbody,"this is publishack body");
                            set_unimsg(unimsg,_addr_,2);//addr应该是psa的任务号

                            unimsg->setCtrlMsgHdr(rschdr);
                            unimsg->setMsgBody(rscbody);
                            sendMsg(unimsg);

                            //send msg
                            //发送完从消息队列中移除消息
                            readytodel->push_back(tmps);

                        }
                        else if(msgtype=="subscribeack"){
                            //rid
                            TUniNetMsg* unimsg = new TUniNetMsg();
                            TRscMsgHdr* rschdr=new TRscMsgHdr();
                            TRscMsgBody* rscbody=new TRscMsgBody();

                            //msgid已经分解好了 直接回复即可
                            set_rsc_head(rschdr,uaip,0x00004000,"/service/state","",userid,msgid);
                            set_rsc_body(rscbody,"this is subscribeack body");
                            set_unimsg(unimsg,_addr_,2);//addr应该是psa的任务号

                            unimsg->setCtrlMsgHdr(rschdr);
                            unimsg->setMsgBody(rscbody);
                            sendMsg(unimsg);

                            //send msg
                            //发送完从消息队列中移除消息
                            readytodel->push_back(tmps);
                            //send msg

                        }
                    }//else if servicename == state

                }


                delete msgvec;
            }


            vector<string> :: iterator vecit1;
            for (vecit1=readytodel->begin(); vecit1!=readytodel->end(); ++vecit1){
                tmpset->erase(*vecit1);
            }
            delete readytodel;

        }
        else{

        }

    }




}


void ServiceTask :: set_rsc_head(TRscMsgHdr* rschdr ,
    string hostip,int code,string ruri,string producer,string consumer,string rid){

    if(hostip.length()!=0){
       CStr _destination_(hostip.c_str());
       TURI _host_info_(_destination_);
       rschdr->host = _host_info_;
    }

    rschdr->code = code;

    if(ruri.length()!=0){
       CStr _ruri_(ruri.c_str());
       rschdr->ruri = _ruri_;
    }

    if(producer.length()!=0){//from
       CStr _producer_(producer.c_str());
       rschdr->producer = _producer_;
    }

    if(consumer.length()!=0){//to //wangpan get it to send to client
       CStr _consumer_(consumer.c_str());
       rschdr->consumer = _consumer_;
    }

    if(rid.length()!=0){
       CStr _rid_(rid.c_str());
       rschdr->rid = _rid_;
    }
}


void ServiceTask :: set_rsc_body(TRscMsgBody* rscbody,string body){
    if(body.length()!=0){
       CStr _message_(body.c_str());
       rscbody->rsc = _message_;
    }
}

void ServiceTask :: set_unimsg(TUniNetMsg* unimsg,
    int addr,int type){
    unimsg->tAddr.logAddr = addr;

    if(type==0){
     unimsg->msgName = RSC_MESSAGE;
    }
    else if(type==1){
     unimsg->msgName = RSC_REQUEST;
    }
    else if(type==2){
     unimsg->msgName = RSC_RESPONSE;
    }

    unimsg->dialogType = DIALOG_MESSAGE;
    unimsg->msgType = RSC_MESSAGE_TYPE;
    unimsg->setTransId();
    //防止rsc拒绝，必须设置seq
    count_move++;
    int int_max=0x7fffffff;
    if(count_move == int_max)
      count_move=0;
    //string rid=us->int_to_string(count_move); //自动生成id
    unimsg->setCSeq(count_move);

}

void ServiceTask :: proc_satip(TRscMsgHdr * head , TRscMsgBody * body){ //处理骨干卫星归属结果

    //map string
    //string name="state"+"_"+"subscirbe"+"_"+rschdr->consumer+"_"+rschdr->rid; //生成一个唯一标识
    string content=body->rsc.c_str();
    string userid;
    string satip;
    string status;
    UniINFO("begin to proc satip msg the content is %s",content.c_str());
    JSONValue * recjv=JSON::Parse(content.c_str());
    if(recjv==NULL||!recjv->IsObject()) return ;
    JSONObject root=recjv->AsObject();
    JSONObject::const_iterator it=root.find(L"uid");
    if(it!=root.end()){//have found
        if(it->second->IsString()){
            wstring str = it->second->AsString();
            userid=us->ws2s(str);
        }
    }
    JSONObject::const_iterator it2=root.find(L"status");
    if(it2!=root.end()){//have found
        if(it2->second->IsString()){
            wstring str = it2->second->AsString();
            status=us->ws2s(str);
        }
    }
    JSONObject::const_iterator it3=root.find(L"satip");
    if(it3!=root.end()){//have found
        if(it3->second->IsString()){
            wstring str = it3->second->AsString();
            satip=us->ws2s(str);
        }
    }
    UniINFO("begin to proc satip msg for %s",userid.c_str());
    if(status=="1"&&satip.length()>0){ //用户合法 且 有结果
        //6-22 新增处理 查看cache_map中是否有变化
        if(cache_map->count(userid)!=0){ //不为0证明本机存放了该id的订阅树
              if((*cache_map)[userid]!=satip){ //ip地址发生变化，需要数据迁移
                move_to_newip(userid,satip);//将userid的数据迁移到新的ip上
                return;
              }

        }
        //6-22 end
        map<string , set<string> * > :: iterator it4;
        it4=acc_map->find(userid);
        if(it4!=acc_map->end()){ //find it
            //cout<<"proc satip find user in accmap "<<userid<<endl;
            UniINFO("proc satip find userid in acc_map %s",userid.c_str());
            set<string > * tmpset=it4->second;
            set<string> :: iterator itbegin=tmpset->begin();
            set<string> :: iterator itend=tmpset->end();
            vector<string> * readytodel=new vector<string>();
            for(;itbegin!=itend;itbegin++){
                string tmps=*itbegin;  //tmps 在这里是一长串msgid
                //cout<<"proc satip find unimsg"<<tmps<<endl;
                UniINFO("proc satip find unimsg`s id is %s",tmps.c_str());
                vector<string> * msgvec=us->splitTopic(tmps,'_');
                map<string,TUniNetMsg*> :: iterator itmsg;
                itmsg = local_map->find(tmps);
                if(msgvec->size()<=3||itmsg==local_map->end()){ //或者没找到该有的消息则跳过
                    readytodel->push_back(tmps);
                    delete msgvec;
                    continue;
                }
                else{
                    //cout<<"legal proc satip"<<endl;
                    UniINFO("proc satip find real unimsg in local_map msgid is %s",tmps.c_str());
                    string servicename=(*msgvec)[0];
                    string msgtype=(*msgvec)[1];
                    string fromid=(*msgvec)[2];
                    string rid=(*msgvec)[3];

                    TUniNetMsg * unimsg=itmsg->second;
                    if(unimsg==NULL){
                        cout<<"msg come unimsg is null" <<endl;
                    }
                    TRscMsgHdr* rschdr = NULL;
                    TRscMsgBody* rscbody =  NULL;
                    rschdr=dynamic_cast<TRscMsgHdr*> (unimsg->getCtrlMsgHdr());
                    rscbody=dynamic_cast<TRscMsgBody*> (unimsg->getMsgBody());
                    if(rschdr==NULL){
                        cout<<"rschdr is null" <<endl;
                    }
                    if(servicename=="state"){
                        vector<string > * ipvec=us->splitTopic(satip,':');
                        if(msgtype=="subscribe"){
                            string tmplocal_port=us->int_to_string(local_port);
                            UniINFO("proc sub local port is %s",tmplocal_port.c_str());
                            if(ipvec->size()>1&&local_ip.c_str()==(*ipvec)[0]&&tmplocal_port.c_str()==(*ipvec)[1]){ //本机
                                UniINFO("begin to proc local state sub because ip is same");
                                proc_state_sub(rschdr,rscbody); //直接处理消息
                                //删掉local map中该条消息 释放空间 只处理不send
                                delete unimsg;

                            }
                            else{//消息转发
                            //通过tmps取出消息后 将ruri修改为/service/state/remote
                            //rid consumer都还在 不做区分 真正处理时再和一起
                            //send msg
                            //空就是不做修改
                            UniINFO("begin to resend local state sub and change to remote");
                            set_rsc_head(rschdr,satip,0x00003000,"/service/state/remote","","","");
                            set_rsc_body(rscbody,"");
                            set_unimsg(unimsg,_addr_,1);//addr应该是psa的任务号
                            unimsg->setCtrlMsgHdr(rschdr);
                            unimsg->setMsgBody(rscbody);
                            sendMsg(unimsg);
                            //send msg
                            }

                        }
                        else if(msgtype=="publish"){
                            //通过tmps取出消息后 将ruri修改为/service/state/remote
                            string tmplocal_port=us->int_to_string(local_port);
                            UniINFO("proc pub local port is %s",tmplocal_port.c_str());
                            if(ipvec->size()>1&&local_ip.c_str()==(*ipvec)[0]&&tmplocal_port.c_str()==(*ipvec)[1]){ //本机
                                UniINFO("begin to proc local state pub because ip is same");
                                proc_state_pub(rschdr,rscbody); //直接处理消息
                                //删掉local map中该条消息
                                delete unimsg;
                            }
                            else{
                            //send msg 需要msgid userid
                            UniINFO("begin to resend local state pub and change to remote");
                            set_rsc_head(rschdr,satip,0x00001000,"/service/state/remote","","","");
                            set_rsc_body(rscbody,"");
                            set_unimsg(unimsg,_addr_,1);//addr应该是psa的任务号
                            unimsg->setCtrlMsgHdr(rschdr);
                            unimsg->setMsgBody(rscbody);
                            sendMsg(unimsg);
                            //send msg
                            }

                        }
                        //发送完从消息队列中移除消息
                        readytodel->push_back(tmps);
                        delete ipvec;
                    }
                    //local_map中清理掉指针
                    local_map->erase(tmps);

                }

                delete msgvec;
            }//循环遍历属于该user的消息 待处理或转发

            vector<string> :: iterator vecit1;
            for (vecit1=readytodel->begin(); vecit1!=readytodel->end(); ++vecit1){
                tmpset->erase(*vecit1);
            }
            delete readytodel;

        }
        else{

        }

    }



}

//6-22 added
map<string,string> * ServiceTask:: get_cache_map(){
   return cache_map;  
}

string ServiceTask:: get_local_ip(){
    string tmp=local_ip.c_str();
    string tmplocal_port=us->int_to_string(local_port);
    tmp=tmp+":"+tmplocal_port; //合成ip:port
    return tmp;
}

/*
   sub消息
   rid 依然作为判断是不是重复消息的依据
   isdelete用来 作为是否添加订阅 或者是退订

    state:{
       topic: "/abc/def/ghi",
       isdelete : "0"
    }
 */
//主题是按照/abc/def/ghi 分解为abc，def，ghi来做的，结点中并没有存储/
void ServiceTask::dfs_deal(SerTreeNode * root,string userid,string satip,string topic){
    if(root==NULL) return;
    int len=(root->childList).size();
    SubInfoMng * sbm=getSubMng();
    string tmps="/";
    if(len==0){ //没孩子，叶子结点或者是根节点
        if(root==sbm->get_root(userid)){ //根节点没孩子证明没有数据，不做处理
            return;
        }
        else{ //叶子结点
            set<string> cs=root->clientSet;
            set<string> :: iterator itbegin=cs.begin();
            set<string> :: iterator itend=cs.end();
            for(;itbegin!=itend;++itbegin){
                string clientid=*itbegin;
                //重新构造sub 发送
                //send msg
                TUniNetMsg* unimsg = new TUniNetMsg();
                TRscMsgHdr* rschdr=new TRscMsgHdr();
                TRscMsgBody* rscbody=new TRscMsgBody();

                count_move++;
                int int_max=0x7fffffff;
                if(count_move == int_max)
                count_move=0;
                string rid=us->int_to_string(count_move); //自动生成rid

                //sub
                set_rsc_head(rschdr,satip,0x00003000,"/service/state/remote",clientid,userid,rid);
                //构造json消息
                JSONObject newJsonObject;
                JSONObject innerJsobj;
                innerJsobj[L"topic"]=new (std::nothrow) JSONValue(us->s2ws(topic));
                innerJsobj[L"isdelete"]=new (std::nothrow) JSONValue(us->s2ws("0"));
                newJsonObject[L"state"]=new (std::nothrow) JSONValue(innerJsobj);
                JSONValue res=newJsonObject;
                std::wstring resstr=res.Stringify().c_str();
                string sbody=us->ws2s(resstr);
                set_rsc_body(rscbody,sbody);
                set_unimsg(unimsg,_addr_,0);//addr应该是psa的任务号
                //构造json消息 end
                unimsg->setCtrlMsgHdr(rschdr);
                unimsg->setMsgBody(rscbody);
                sendMsg(unimsg);
                //send msg
            }
        }
    }
    for(int i=0;i<len;i++){
        if(root==sbm->get_root(userid)){
         dfs_deal((root->childList)[i],userid,satip,"");
        }
        else{ //非根节点，可以添加topic了
         string singletopic=root->singleTopic;
         string ress;
         if(topic.size()==0){
           ress=tmps+singletopic; // /abc
         }
         else{
           ress=topic+tmps+singletopic; // /abc+/+ghi = /abc/ghi
         }
         dfs_deal((root->childList)[i],userid,satip,ress);
        }
    }   
}

void ServiceTask:: move_to_newip(string userid,string satip){
    //找到订阅树结构
    SubInfoMng * sbm=getSubMng();
    SerTreeNode * tmproot=sbm->get_root(userid);
    if(tmproot!=NULL){
        //向satip发出新的sub，重新订阅
        dfs_deal(tmproot,userid,satip,"");
        //删除订阅树
        sbm->delete_tree(userid);
        //清除cache_map中userid的信息
        cache_map->erase(userid);

    }

}