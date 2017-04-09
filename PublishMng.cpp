//
// Created by dell on 2017/4/1.
//
#include "PublishMng.h"


PublishMng::PublishMng(Proxy * p) {
    proxy=p;
    us=new UtilService();
    msg_map = new unordered_map <string ,PublishMsg * > ();

}

PublishMng::~PublishMng(){

    delete us;
    delete msg_map;
}

/* 样例
{
  "msg":{
     "from":"18610191733",
     "to":["88725004772"],
     "mtype": "common",
     "mcontent": "give me the money"
     "mid":"1234999"
  }
}
*/


void PublishMng::procPubMsg(string str){ //body里的rsc内容
    //cout<<"begin str is" <<str<<endl;
    JSONArray toArray;  //存放to的信息
    int mark=0;//判断from合法性
    JSONValue * recjv=JSON::Parse(str.c_str());
    //cout<<"2"<<endl;
    if(recjv==NULL||!recjv->IsObject()) return ;
    //cout<<"1"<<endl;
    JSONObject root=recjv->AsObject();
    JSONObject::const_iterator it=root.find(L"msg");
    if(it!=root.end()){//have found
        PublishMsg * pubmsg=new PublishMsg();
        if(it->second->IsObject()){ //it->second is JSONValue *
            JSONObject msg=it->second->AsObject();
            JSONObject::const_iterator itfrom=msg.find(L"from");
            if(itfrom!=msg.end()&&itfrom->second->IsString()) {
                mark=1;
                pubmsg->from = us->ws2s( itfrom->second->AsString() );
                //cout<<pubmsg->from<<endl;
            }
            JSONObject::const_iterator itto=msg.find(L"to");
            if(itto!=msg.end()&&(itto->second)->IsArray()){
                toArray=itto->second->AsArray(); //实际上就是vector
                JSONArray:: iterator itb=toArray.begin();
                JSONArray:: iterator itend=toArray.end();
                for(;itb!=itend;++itb){
                    JSONValue * tmpjsv=*itb;
                    if(tmpjsv->IsString()){
                        pubmsg->to.push_back(us->ws2s(tmpjsv->AsString())); //所有to读到
                    }
                }
            }
            JSONObject::const_iterator itmtype=msg.find(L"mtype");
            if(itto!=msg.end()){
                pubmsg->type=us->ws2s(itmtype->second->AsString());
            }
            JSONObject::const_iterator itmcontent=msg.find(L"mcontent");
            if(itmcontent!=msg.end()){
                pubmsg->content=us->ws2s(itmcontent->second->AsString());
            }
            JSONObject::const_iterator itmsgid=msg.find(L"mid");
            //cout<<"mark is" <<mark<<endl;
            if(itmsgid!=msg.end()&&mark==1){
                string tmps=us->ws2s(itmsgid->second->AsString());
                pubmsg->msgid=pubmsg->from+"_"+tmps;  //生成msgid
                //cout<<"msgid" << pubmsg->msgid <<endl;
            }
            //cout<<"finish" <<endl;

        }

        unordered_map<string , PublishMsg *> :: iterator itum;
        itum=msg_map->find(pubmsg->msgid);
        if(itum!=msg_map->end()){ //find it 认为是重发 不处理
           //cout<<"find it " <<endl;
        }
        else{
           int len=(pubmsg->to).size();
           for(int i=0;i<len;i++) {
               pubmsg->userstate[pubmsg->to[i]]=0; //未发送状态
           }

            (*msg_map)[pubmsg->msgid]=pubmsg;

            //执行发送notify通知
           proxy->getNTFMng()->procPubMsg(pubmsg);
        }
    }//root
   // showMsgMap(); //看看效果
}

void PublishMng ::showMsgMap() {
    unordered_map<string , PublishMsg *> :: iterator itbegin=msg_map->begin();
    unordered_map<string , PublishMsg *> :: iterator itend=msg_map->end();
    for(;itbegin!=itend;itbegin++){
        cout<< "msgid is " << itbegin->first<<endl;
        PublishMsg * tmp=itbegin->second;
        cout << "from " << tmp->from<<endl;
        int len=tmp->to.size();
        for(int i=0;i<len;i++){
            cout<<"to " << tmp->to[i] <<endl;
        }
        cout << "type " << tmp->type<<endl;
        cout << "content " << tmp->content<<endl;
    }

}

void PublishMng :: procNTFAckMsg(string msgid,string to) { //需要知道两条信息 一个是对应的pub的消息id  还有一个是是谁发过来的ack
    unordered_map<string , PublishMsg *> :: iterator itm;
    itm=msg_map->find(msgid);
    if(itm!=msg_map->end()){  //have found pubmsg
        PublishMsg * tmp1 = itm->second;
        unordered_map<string ,int > :: iterator userstateit=tmp1->userstate.find(to);
        if(userstateit!=(tmp1->userstate.end())){
            if(userstateit->second==0){
                userstateit->second=1;  //改为已经送达
            }
            else
                return;
        }
        else  //to 非法 不做处理
            return;
        unordered_map<string ,int > :: iterator itbegin=tmp1->userstate.begin();
        unordered_map<string ,int > :: iterator itend=tmp1->userstate.end();

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