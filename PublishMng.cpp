//
// Created by dell on 2017/4/1.
//
#include "PublishMng.h"


PublishMng::PublishMng(ServiceTask * p) {
    proxy=p;
    us=new UtilService();
    msg_map = new map <string ,PublishMsg * > ();
    stateSet = new set<string > ();

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
    if (itum != msg_map->end()) { //find it 认为是重发 不处理
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
            else
                return;
        }
        else  //to 非法 不做处理
            return;
        map<string ,int > :: iterator itbegin=tmp1->userstate.begin();
        map<string ,int > :: iterator itend=tmp1->userstate.end();

        for(;itbegin!=itend;itbegin++){
            if(itbegin->second==0){
                return ;
            }
        }
        //走到这一步 证明都是1了 均已送达

        //send publish ack
         cout<<"send publish ack" <<endl;


        //删除publish消息
         delete tmp1;
         msg_map->erase(itm);

    }


}


/*
    state:{
       topic: "abc/def/ghi",
       content : "my first topic"
    }
 */
void PublishMng :: procPubState(TRscMsgHdr * rschdr ,TRscMsgBody * rscbody){
    //string from = rschdr->consumer;
    //string to = rschdr->producer;
    //cout<<"begin to proc pub state1" <<endl;

    string rid = rschdr->rid;
    string str = rscbody->rsc;
    //cout<<"str is " <<str<<endl;
    set <string >  :: iterator tmp1;
    tmp1=stateSet->find(rid);
    if(tmp1!=stateSet->end()){ //find it no proc

    }
    else{
       //cout<<"begin to proc pub state2" <<endl;
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
                    //cout<<"topic is" <<topic <<endl;
                }
                JSONObject::const_iterator itmcontent = msg.find(L"content");
                if (itmcontent != msg.end()) {
                    content = us->ws2s(itmcontent->second->AsString());
                    //cout<<"content is " <<content <<endl;
                }
            }
            //cout<<"topic is" <<topic <<endl;
            //cout<<"content is " <<content <<endl;

            set <string > clientSet;
            clientSet  =  proxy->getSubMng()->getClientForP(topic);
            if(!clientSet.empty()){
                set <string > :: iterator itbegin=clientSet.begin();
                set <string > :: iterator itend=clientSet.end();
                for(;itbegin!=itend;itbegin++){
                    cout << "send pub to " << *itbegin <<endl;
                    //生成新的publish消息
                    JSONObject newJsonObject;
                    JSONObject innerJsobj;
                    innerJsobj[L"topic"]=new (std::nothrow) JSONValue(us->s2ws(topic));
                    innerJsobj[L"content"]=new (std::nothrow) JSONValue(us->s2ws(content));
                    //JSONValue res=innerJsobj;
                    newJsonObject[L"state"]=new (std::nothrow) JSONValue(innerJsobj);
                    JSONValue res=newJsonObject;
                    //res.Stringify();
                    std::wstring resstr=res.Stringify().c_str();
                    string jsoncontent = us->ws2s(resstr); //body 内容

                }
            }
        }//root

    }
}