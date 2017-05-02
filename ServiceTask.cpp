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
void ServiceTask :: procPUBStateinP(TRscMsgHdr * head , TRscMsgBody * body){
    publishmng->procPubState(head,body);
}

void ServiceTask :: procSUBStateinS(TRscMsgHdr * head, TRscMsgBody * body){
    subinfomng->procSubState(head,body);
}


ServiceTask :: ServiceTask(){
    publishmng=new PublishMng(this);
    //publishmng->setProxy(this);
    ntfmng=new NTFMng(this);
    subinfomng = new SubInfoMng(this);
    us=new UtilService();
    send_map=new map<string,set<string> * > ();

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
}


void ServiceTask ::procMsg(TRscMsgHdr *rschdr, TRscMsgBody * rscbody,int msgType) {


    if(rschdr==0) return; //非法rsc消息不进行处理
    int rscHdrCode=rschdr->code;
    string rscRuri=rschdr->ruri;
    vector<string> * topicVec=us->splitTopic(rscRuri,'/'); //ruri change to vector<string >  must free

    if(rscHdrCode==200) {//暂时认定200为移动性管理接口的code
        if((*topicVec)[1]=="uaip"){

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
                else if((*topicVec)[1]=="state"){
                   procPUBStateinP(rschdr,rscbody);
                }
                break;
            }
            case SUBSCRIBE :{
                if((*topicVec)[1]=="msg"){

                }
                else if((*topicVec)[1]=="dlg"){

                }
                else if((*topicVec)[1]=="state"){
                    procSUBStateinS(rschdr,rscbody);
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
            for(;itbegin!=itend;itbegin++){
                string tmps=*itbegin;
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
                             //send msg



                         }
                    }
                }


                delete msgvec;
            }

        }
        else{

        }

    }




}