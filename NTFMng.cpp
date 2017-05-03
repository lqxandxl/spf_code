//
// Created by dell on 2017/4/6.
//

#include "SerMessage.h"
#include "NTFMng.h"

NTFMng ::NTFMng(ServiceTask * p) {
    proxy=p;
    us=new UtilService();
    ntfmap=new map <string , NotifyMsg * > ();
}

NTFMng :: ~NTFMng(){

    delete us;
    map<string , NotifyMsg *> :: iterator itbegin=ntfmap->begin();
    map<string , NotifyMsg *> :: iterator itend=ntfmap->end();
    for(;itbegin!=itend;itbegin++){
        NotifyMsg * tmp=itbegin->second;
        delete tmp;
    }
    ntfmap->clear();
    delete ntfmap;
}


void NTFMng ::proc_msg_publish(PublishMsg * publishmsg){
    //处理msg业务 publish消息 分成多个notify分发出去
    int len=publishmsg->to.size();
    string newmsgid;
    for(int i=0;i<len;i++){
        newmsgid=publishmsg->msgid+"_"+(publishmsg->to)[i]; //加上一个to作为新的msgid的标识
        map <string , NotifyMsg * > :: iterator it1;
        it1=ntfmap->find(newmsgid);
        if(it1!=ntfmap->end()){ //已经存在不处理
           break;
        }
        else{
            NotifyMsg * ntfmsg=new NotifyMsg();
            ntfmsg->msgid=newmsgid;
            ntfmsg->to=(publishmsg->to)[i];
            ntfmsg->from=publishmsg->from;
            ntfmsg->body=publishmsg->body;
            ntfmsg->msgstate=0;
            ntfmsg->publishmsgid=publishmsg->msgid; //存储归属
            (*ntfmap)[ntfmsg->msgid] = ntfmsg; //存储ntfmsg消息


            //发送消息
            //需要调用proxy的方法去将消息添加至map中，并且需要向移动性管理接口发送查询请求一次
           int res = proxy->send_map_add(ntfmsg->to,"msg","notify",ntfmsg->msgid);
           if(res==1){
              proxy->get_uaip(ntfmsg->to); //查询地址
           }
           else{ //防止内部失败也要查询

           }
        }
    }
}



/*
{
  "msg":{
     "msgid":"99393939"     //里面已经包含了to字段 可以直接查找到notify消息
  }
}
*/


void NTFMng:: proc_msg_notifyack(TRscMsgHdr * head ,TRscMsgBody * body){
    string notifyackmsgid = head->rid ;
    //提取出来之后要对map进行搜索
    map<string,NotifyMsg *> ::iterator itm;
    itm=ntfmap->find(notifyackmsgid);
    if(itm!=ntfmap->end()){ //find this notifymsg
       if(itm->second->msgstate==0){
           //需要让PUBMng模块进行处理notifyack的msg业务消息
           proxy->getPubMng()->proc_msg_notifyack(itm->second->publishmsgid,itm->second->to);
           //在publish中已经记录了已送达 此时这个notify在map中没有作用了 需要删除节省空间
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

NotifyMsg * NTFMng:: find_msg_notify(string msgid){
    map<string , NotifyMsg*> :: iterator it;
    it=ntfmap->find(msgid);
    if(it!=ntfmap->end()){
        return it->second;
    }
    else{
        return NULL;
    }
}

