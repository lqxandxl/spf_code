//
// Created by dell on 2017/4/6.
//

#include "SerMessage.h"
#include "NTFMng.h"

NTFMng ::NTFMng(ServiceTask * p) {
    proxy=p;
    us=new UtilService();
    ntfmap=new unordered_map <string , NotifyMsg * > ();
}

NTFMng :: ~NTFMng(){

    delete us;
    unordered_map<string , NotifyMsg *> :: iterator itbegin=ntfmap->begin();
    unordered_map<string , NotifyMsg *> :: iterator itend=ntfmap->end();
    for(;itbegin!=itend;itbegin++){
        NotifyMsg * tmp=itbegin->second;
        delete tmp;
    }
    ntfmap->clear();
    delete ntfmap;
}


void NTFMng :: procPubMsg(PublishMsg * publishmsg){
    //拿到内部结构对其进行处理 需要一个方法生成特定的json 然后调用proxytask发送消息
    int len=publishmsg->to.size();
    string newmsgid;
    for(int i=0;i<len;i++){
        newmsgid=publishmsg->msgid+"_"+(publishmsg->to)[i];
        unordered_map <string , NotifyMsg * > :: iterator it1;
        it1=ntfmap->find(newmsgid);
        if(it1!=ntfmap->end()){ //已经存在不处理
           break;
        }
        else{
            NotifyMsg * ntfmsg=new NotifyMsg();
            ntfmsg->msgid=newmsgid;
            ntfmsg->to=(publishmsg->to)[i];
            cout<<"ntf to is "<<ntfmsg->to<<endl;
            ntfmsg->type=publishmsg->type;
            ntfmsg->from=publishmsg->from;
            ntfmsg->content=publishmsg->content;
            ntfmsg->msgstate=0;
            ntfmsg->publishmsgid=publishmsg->msgid; //存储归属
            (*ntfmap)[ntfmsg->msgid] = ntfmsg; //存储ntfmsg消息
            string jsoncontent = us->ws2s(generateMsgJson(ntfmsg));
            std::cout<<"notify res is " << jsoncontent <<endl;
            //s赋予cstr  之后 proxy->sendmsg();

        }

    }



}

std::wstring NTFMng :: generateMsgJson(NotifyMsg * ntfmsg){
    JSONObject newJsonObject;
    JSONObject innerJsobj;
    //us->s2ws(ntfmsg->from);
    //innerJsobj[L"msgid"]=new (std::nothrow) JSONValue(us->s2ws(ntfmsg->msgid));
    //innerJsobj[L"from"]=new (std::nothrow) JSONValue(us->s2ws(ntfmsg->from));
    //innerJsobj[L"to"]=new (std::nothrow) JSONValue(us->s2ws(ntfmsg->to));
    innerJsobj[L"mtype"]=new (std::nothrow) JSONValue(us->s2ws(ntfmsg->type));
    innerJsobj[L"mcontent"]=new (std::nothrow) JSONValue(us->s2ws(ntfmsg->content));
    //JSONValue res=innerJsobj;
    newJsonObject[L"msg"]=new (std::nothrow) JSONValue(innerJsobj);
    JSONValue res=newJsonObject;
    //res.Stringify();
    std::wstring resstr=res.Stringify().c_str();
    //cout<<"all finish"<<endl;
    return resstr;
}

/*
{
  "msg":{
     "msgid":"99393939"     //里面已经包含了to字段 可以直接查找到notify消息
  }
}
*/


void NTFMng:: procNtfAckMsg(TRscMsgHdr * head ,TRscMsgBody * body){
    string notifyackmsgid = head->rid ;
    //提取出来之后要对map进行搜索
    unordered_map<string,NotifyMsg *> ::const_iterator itm;
    itm=ntfmap->find(notifyackmsgid);
    if(itm!=ntfmap->end()){ //find this notifymsg
       if(itm->second->msgstate==0){
           //需要让PUBMng模块进行处理 notifyack 的msg业务消息
           proxy->procNTFAckMsginP(itm->second->publishmsgid,itm->second->to);
           NotifyMsg * tmpnotify;
           tmpnotify=itm->second;
           delete tmpnotify;
           ntfmap->erase(itm);

       }//否则不进行处理
    }
    else{
        //忽略
    }
}



/*test for notifyack msg
void NTFMng:: procNtfAckMsg(string str){
    JSONValue * recjv=JSON::Parse(str.c_str());
    if(recjv==NULL||!recjv->IsObject()) return ;
    JSONObject root=recjv->AsObject();
    JSONObject::const_iterator it=root.find(L"msg");
    if(it!=root.end()){//have found
        if(it->second->IsObject()){ //it->second is JSONValue *
            JSONObject msg=it->second->AsObject();
            JSONObject::const_iterator itmsgid=msg.find(L"msgid");
            if(itmsgid!=msg.end()&&itmsgid->second->IsString()) {
                string notifyackmsgid = us->ws2s( itmsgid->second->AsString() );
                //提取出来之后要对map进行搜索
                unordered_map<string,NotifyMsg *> ::const_iterator itm;
                itm=ntfmap->find(notifyackmsgid);
                if(itm!=ntfmap->end()){ //find this notifymsg
                    if(itm->second->msgstate==0){
                        //需要让PUBMng模块进行处理 notifyack 的msg业务消息
                        proxy->procNTFAckMsg(itm->second->publishmsgid,itm->second->to);
                        NotifyMsg * tmpnotify;
                        tmpnotify=itm->second;
                        delete tmpnotify;
                        ntfmap->erase(itm);

                    }
                    //否则不进行处理
                }
                else{
                    //忽略
                }
            }


        }

    }//root


}

 */