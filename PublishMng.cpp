//
// Created by dell on 2017/4/1.
//
#include "PublishMng.h"


PublishMng::PublishMng(ServiceTask * p) {
    proxy=p;
    us=new UtilService();
    msg_map = new map <string ,PublishMsg * > ();
    stateSet = new set<string > ();
    topic_map =new map<string,string>();
}

PublishMng::~PublishMng(){

    delete us;
    map<string , PublishMsg *> :: iterator itbegin=msg_map->begin();
    map<string , PublishMsg *> :: iterator itend=msg_map->end();
    for(;itbegin!=itend;itbegin++){
        PublishMsg * tmp=itbegin->second;
        delete tmp;
    }
    msg_map->clear();
    delete msg_map;
    delete stateSet;
    delete topic_map;


}






//将一些数据放在外面不放在json中 简化处理 加快速度
void PublishMng::proc_msg_publish(TRscMsgHdr * rschdr , TRscMsgBody * rscbody) { //处理rsc消息  属于 publish 中的 msg业务
    string from = rschdr->consumer;
    string to = rschdr->producer;
    string rid = rschdr->rid;
    string str = rscbody->rsc;
    string msgid;
    if (from.length() > 0 && rid.length() > 0) {
        msgid = from + "_" + rid;  //from + rid 来区分 避免不同的终端所带来的冲突
    }
    map<string, PublishMsg *>::iterator itum;
    itum = msg_map->find(msgid);
    if (itum != msg_map->end()) { //find it 认为是重发
        //notifyack 没有收全 就会导致重发 所以需要检查一下notify发出了哪些 之后重新发送notify
        vector<string> *vecto = us->splitTopic(to, '&'); //remember to delete
        PublishMsg * tmppub=itum->second;
        map < string , int > :: iterator itbegin=tmppub->userstate.begin();
        map < string , int > :: iterator itend=tmppub->userstate.end();
        for(;itbegin!=itend;itbegin++){
            int tmp1=itbegin->second;
            if(tmp1==0){ //未送达 重新发
                string newmsgid=tmppub->msgid+"_"+itbegin->first; //加上一个to作为新的msgid的标识
                int res = proxy->send_map_add(itbegin->first,"msg","notify",newmsgid);
                if(res==1){
                    proxy->get_uaip(itbegin->first); //查询地址
                }
                else{ //防止内部失败也要查询

                }
            }
        }
        delete vecto;
        return;
    } else {
        PublishMsg *pubmsg = new PublishMsg();
        vector<string> *vecto = us->splitTopic(to, '&'); //remember to delete
        pubmsg->from = from;
        int vecto_len = vecto->size();
        for (int i = 0; i < vecto_len; i++) {
            pubmsg->to.push_back((*vecto)[i]); //存储to字段的用户
        }
        pubmsg->msgid = msgid;
        int len = (pubmsg->to).size();
        for (int i = 0; i < len; i++) {
            pubmsg->userstate[pubmsg->to[i]] = 0; //未发送状态
        }

        pubmsg->body=str;

        (*msg_map)[pubmsg->msgid] = pubmsg;

        //执行发送notify通知
        proxy->getNTFMng()->proc_msg_publish(pubmsg);

        delete vecto;

    }
}





void PublishMng :: proc_msg_notifyack(string msgid,string to) { //需要知道两条信息 一个是对应的pub的消息id  还有一个是是谁发过来的ack
    map<string , PublishMsg *> :: iterator itm;
    itm=msg_map->find(msgid);
    if(itm!=msg_map->end()){  //have found pubmsg
        PublishMsg * tmp1 = itm->second;
        map<string ,int > :: iterator userstateit=tmp1->userstate.find(to);
        if(userstateit!=(tmp1->userstate.end())){
            if(userstateit->second==0){
                userstateit->second=1;  //改为已经送达
            }
        }
        map<string ,int > :: iterator itbegin=tmp1->userstate.begin();
        map<string ,int > :: iterator itend=tmp1->userstate.end();

        for(;itbegin!=itend;itbegin++){
            if(itbegin->second==0){
                return ;
            }
        }
        //走到这一步 证明都是1了 均已送达

        //send publish ack
        //tmp1 msgid 为 from_rid  在这里拆开 后面就不用拆了 减少发送时间
        vector<string> * msgvec2=us->splitTopic(tmp1->msgid,'_');
        string puback_msgid = (*msgvec2)[1];
        int res = proxy->send_map_add(tmp1->from,"msg","publishack",puback_msgid);
        delete msgvec2;
        if(res==1){
            proxy->get_uaip(tmp1->from); //查询地址
        }
        else{ //防止内部失败也要查询

        }


        //删除publish消息
        delete tmp1;
        msg_map->erase(itm);

    }


}


/*
    state:{
       topic: "/abc/def/ghi",
       content : "my first topic"
    }
 */
void PublishMng :: proc_state_pub(TRscMsgHdr * rschdr , TRscMsgBody * rscbody){

    string rid  = rschdr->rid;
    string str  = rscbody->rsc;
    string from = rschdr->consumer;
    string originalid=rid;

    set <string >  :: iterator tmp1;
    rid=from+"_"+rid;
    tmp1=stateSet->find(rid);
    if(tmp1!=stateSet->end()){ //find it no proc

    }
    else{

       stateSet->insert(rid);
       string topic;
       string content;
       //通过主题查找要发给谁
        JSONValue *recjv = JSON::Parse(str.c_str());
        if (recjv == NULL || !recjv->IsObject()) return;
        JSONObject root = recjv->AsObject();
        JSONObject::const_iterator it = root.find(L"state");
        if (it != root.end()) {//have found
            if (it->second->IsObject()) { //it->second is JSONValue *
                JSONObject msg = it->second->AsObject();
                JSONObject::const_iterator itmtype = msg.find(L"topic");
                if (itmtype != msg.end()) {
                    topic = us->ws2s(itmtype->second->AsString());
                }
                JSONObject::const_iterator itmcontent = msg.find(L"content");
                if (itmcontent != msg.end()) {
                    content = us->ws2s(itmcontent->second->AsString());
                }
            }


            set <string > clientSet;
            clientSet  =  proxy->getSubMng()->getClientForP(topic);
            if(!clientSet.empty()){
                set <string > :: iterator itbegin=clientSet.begin();
                set <string > :: iterator itend=clientSet.end();
                for(;itbegin!=itend;itbegin++){
                   //将publish消息下发，区别在于需要储存消息内容
                   //一个topic对应的内容应该是一样的 所以需要按topic为key去存储内容 推送的时候以topic为核心取数据
                   //发送队列 msgid 为 from+rid 后面多一个 & topic 使得分解时 可以知道topic 从而知道内容
                    cout<<"need to send publish state to "<<*itbegin<<endl;
                    (*topic_map)[topic]=str;
                    string to=*itbegin; //取出目标 userid
                    int res = proxy->send_map_add(to,"state","publish",rid,topic); //rid 包含from local已经处理过了
                    if(res==1){
                        proxy->get_uaip(to); //查询地址
                    }
                    else{ //防止内部失败也要查询

                    }

                }
            }
        }//root

    }
}

string PublishMng ::get_publish_body(string topic){
    map<string ,string> :: iterator  it;
    it=topic_map->find(topic);
    if(it!=topic_map->end()){
        return it->second;
    }
    else return "";
}
