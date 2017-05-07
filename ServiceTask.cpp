//
// Created by dell on 2017/4/1.
//
#include "ServiceTask.h"

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


ServiceTask :: ServiceTask(){
    publishmng=new PublishMng(this);
    //publishmng->setProxy(this);
    ntfmng=new NTFMng(this);
    subinfomng = new SubInfoMng(this);
    us=new UtilService();
    send_map=new map<string,set<string> * > ();
    //local
    local_map=new map<string ,TUniNetMsg*>();
    acc_map=new map<string, set<string> * >();

}
ServiceTask::~ServiceTask() {
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
}


void ServiceTask ::procMsg(TRscMsgHdr *rschdr, TRscMsgBody * rscbody,int msgType) {

    TUniNetMsg * unimsg;

    if(rschdr==0) return; //非法rsc消息不进行处理
    int rscHdrCode=rschdr->code;
    string rscRuri=rschdr->ruri;
    vector<string> * topicVec=us->splitRuri(rscRuri,'/'); //ruri change to vector<string >  must free

    if(rscHdrCode==0x00000060) {//移动性管理接口的code
        if((*topicVec)[1]=="uaip"){
              proc_uaip(rschdr,rscbody);
        }
        else if((*topicVec)[1]=="satip"){

        }
        return;
    }
    if((*topicVec)[0] != "service" || topicVec->size()<2) return; //error message

    if(msgType==0){



    }
    else if(msgType==1){ //PUBLISH SUBSCRIBE NOTIFY
        switch(rscHdrCode){
            case PUBLISH :{ //PUBLISH 有不同的业务 需要 ruri 去判断 比如 消息业务 对话业务 状态推送等等
                if((*topicVec)[1]=="msg"){
                    proc_msg_publish(rschdr,rscbody);


                }
                else if((*topicVec)[1]=="dlg"){

                }
                else if ((*topicVec)[1] == "state") {
                    if (topicVec->size() < 3) return;
                    if ((*topicVec)[2] == "local") {
                        string from=rschdr->consumer;
                        string msgid=rschdr->rid;
                        //local消息一概不处理 放入队列
                        //收到sat内容后，再分类去相应模块处理 或转发
                        string tmp1 = "state";
                        string name = tmp1 + "_" + "publish" + "_" + from + "_" + msgid; //生成一个唯一标识
                        (*local_map)[name] = unimsg;

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
                        proc_state_pub(rschdr, rscbody);
                    }
                }



                break;
            }
            case SUBSCRIBE : {
                if ((*topicVec)[1] == "msg") {

                } else if ((*topicVec)[1] == "dlg") {

                } else if ((*topicVec)[1] == "state") {
                    if (topicVec->size() < 3) return;
                    if ((*topicVec)[2] == "local") {
                        string from=rschdr->consumer;
                        string msgid=rschdr->rid;
                        //local消息一概不处理 放入队列
                        //收到sat内容后，再分类去相应模块处理 或转发
                        string tmp1 = "state";
                        string name = tmp1 + "_" + "subscribe" + "_" + from + "_" + msgid; //生成一个唯一标识
                        (*local_map)[name] = unimsg;

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
                        int res = send_map_add(from,"state","subscribeack",msgid); //ack
                        if(res==1){
                            get_uaip(from); //查询地址
                        }
                        else{ //防止内部失败也要查询

                        }
                    } else if ((*topicVec)[2] == "remote") {
                        proc_state_sub(rschdr, rscbody);
                    }

                }
                break;
            }
            case NOTIFY :{
                break;
            }
        }


    }
    else if(msgType==2){ //PUBLISHACK SUBSCRIBEACK NOTIFYACK
        switch(rscHdrCode){
            case PUBLISHACK :{
                break;
            }
            case SUBSCRIBEACK :{
                break;
            }
            case NOTIFYACK :{
                if((*topicVec)[1]=="msg"){
                    proc_msg_notifyack(rschdr,rscbody);
                }
                else if((*topicVec)[1]=="dlg"){

                }
                else if((*topicVec)[1]=="state"){

                }
                break;
            }
        }


    }

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
           return 1;
        }
    }
    else{ //没找到添加元素
        (*send_map)[userid]=new set<string>();
        set<string> * tmpset=(*send_map)[userid];
        string newmsgid=servicename+"&"+msgtype+"&"+msgid;
        tmpset->insert(newmsgid);
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
 *    "uid":"username"
 *  }
 */
void ServiceTask :: get_uaip(string userid){
    JSONObject newJsonObject;
    newJsonObject[L"uid"]=new (std::nothrow) JSONValue(us->s2ws(userid));
    JSONValue res=newJsonObject;
    std::wstring resstr=res.Stringify().c_str();
    string body=us->ws2s(resstr); //得到body
    //ruri = /uaip
    //rid自己构造 不重复就行
    //code 填写get方式的code
    //taddr.log 填写王任务号
    //send msg
    /*
    TUniNetMsg* unimsg = new TUniNetMsg();
    TRscMsgHdr* rschdr=new TRscMsgHdr();
    TRscMsgBody* rscbody=new TRscMsgBody();

    CStr _message_(body.c_str());
    rscbody->rsc = _message_;

    rschdr->code = 0x00000060; //get

    CStr _ruri_("/uaip");
    rschdr->ruri = _ruri_;

    count_move++;
    int int_max=0x7fffffff;
    if(count_move == int_max)
        count_move=0;

    string tmps1=us->int_to_string(count_move);
    CStr _rid_(tmps1.c_str());
    rschdr->rid = _rid_;


    unimsg->tAddr.logAddr = _addr_; //wangpan id
    unimsg->msgName = RSC_MESSAGE;
    unimsg->dialogType = DIALOG_MESSAGE;
    unimsg->msgType = RSC_MESSAGE_TYPE;
    unimsg->setTransId();
    //unimsg->setCSeq(11);

    unimsg->setCtrlMsgHdr(rschdr);
    unimsg->setMsgBody(rscbody);
    sendMsg(unimsg);
    */
     //send msg

}

void ServiceTask ::get_satip(string userid){
    JSONObject newJsonObject;
    newJsonObject[L"uid"]=new (std::nothrow) JSONValue(us->s2ws(userid));
    JSONValue res=newJsonObject;
    std::wstring resstr=res.Stringify().c_str();
    string body=us->ws2s(resstr); //得到body
    //send msg
}


//针对body进行解析 并执行消息发送
void ServiceTask :: proc_uaip(TRscMsgHdr * head , TRscMsgBody * body){
    string content=body->rsc;
    string userid;
    string uaip;
    string status;
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
    JSONObject::const_iterator it3=root.find(L"uaip");
    if(it3!=root.end()){//have found
        if(it3->second->IsString()){
            wstring str = it3->second->AsString();
            uaip=us->ws2s(str);
        }
    }

    if(status=="1"){ //合法状态 需要具体发出消息
        map<string , set<string> * > :: iterator it4;
        it4=send_map->find(userid);
        if(it4!=send_map->end()){ //find it
           set<string > * tmpset=it4->second;
           set<string> :: iterator itbegin=tmpset->begin();
           set<string> :: iterator itend=tmpset->end();
           vector<string> * readytodel=new vector<string>();
            for(;itbegin!=itend;itbegin++){
                string tmps=*itbegin;
                vector<string> * msgvec=us->splitTopic(tmps,'&');
                if(msgvec->size()<=2){
                    readytodel->push_back(tmps);
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
                             //send msg

                             //发送完从消息队列中移除消息
                             readytodel->push_back(tmps);
                         }
                         else if(msgtype=="publishack"){
                             //send msg 需要msgid userid
                             //send msg
                             readytodel->push_back(tmps);//将要删除的值存入vector 后面集体删除
                         }
                    }
                    else if(servicename=="state"){
                        if(msgtype=="publish"){//publish 消息要下发到订阅的用户
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

                                    //send msg
                                }


                             }
                        }
                        else if(msgtype=="publishack"){
                            //rid
                            //send msg

                        }
                        else if(msgtype=="subscribeack"){
                            //rid
                            //send msg

                        }
                    }
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

void ServiceTask :: proc_satip(TRscMsgHdr * head , TRscMsgBody * body){ //处理骨干卫星归属结果

    //map string
    //string name="state"+"_"+"subscirbe"+"_"+rschdr->consumer+"_"+rschdr->rid; //生成一个唯一标识
    string content=body->rsc;
    string userid;
    string satip;
    string status;
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

    if(status=="1"&&satip.length()>0){ //用户合法 且 有结果

        map<string , set<string> * > :: iterator it4;
        it4=acc_map->find(userid);
        if(it4!=send_map->end()){ //find it
            set<string > * tmpset=it4->second;
            set<string> :: iterator itbegin=tmpset->begin();
            set<string> :: iterator itend=tmpset->end();
            vector<string> * readytodel=new vector<string>();
            for(;itbegin!=itend;itbegin++){
                string tmps=*itbegin;  //tmps 在这里是一长串msgid
                vector<string> * msgvec=us->splitTopic(tmps,'_');
                if(msgvec->size()<=3){
                    readytodel->push_back(tmps);
                    delete msgvec;
                    continue;
                }
                else{
                    string servicename=(*msgvec)[0];
                    string msgtype=(*msgvec)[1];
                    string fromid=(*msgvec)[2];
                    string rid=(*msgvec)[3];
                    if(servicename=="state"){
                        if(msgtype=="subscribe"){ //need to send notify
                            vector<string > * ipvec=us->splitTopic(satip,':');
                            if(local_ip==(*ipvec)[0]){ //本机
                                //Tunitmsg转换
                                //proc_state_sub(head,body); 直接处理消息
                            }
                            //通过tmps取出消息后 将ruri修改为/service/state/remote
                            //rid consumer都还在 不做区分 真正处理时再和一起
                            //send msg

                            //发送完从消息队列中移除消息
                            readytodel->push_back(tmps);
                            delete ipvec;
                        }
                        else if(msgtype=="publish"){
                            //通过tmps取出消息后 将ruri修改为/service/state/remote
                            vector<string > * ipvec=us->splitTopic(satip,':');
                            if(local_ip==(*ipvec)[0]){ //本机
                                //proc_state_pub(head,body); 直接处理消息
                            }
                            //send msg 需要msgid userid

                            //send msg
                            readytodel->push_back(tmps);//将要删除的值存入vector 后面集体删除
                            delete ipvec;
                        }
                    }
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