//
// Created by dell on 2017/4/1.
//
#include "ServiceTask.h"


void ServiceTask ::procPUBMsginP(TRscMsgHdr *head, TRscMsgBody *body) {
    publishmng->procPubMsg(head,body);
}

void ServiceTask:: procNTFAckMsginP(string msgid,string to) { //需要知道两条信息 一个是对应的pub的消息id  还有一个是是谁发过来的ack
    publishmng->procNTFAckMsg(msgid,to);

}

void ServiceTask:: prccNTFAckMsginN(TRscMsgHdr * head ,TRscMsgBody * body ) { //notify ack string 去处理
    ntfmng->procNtfAckMsg(head,body);
}
ServiceTask :: ServiceTask(){
    publishmng=new PublishMng(this);
    //publishmng->setProxy(this);
    ntfmng=new NTFMng(this);
    subinfomng = new SubInfoMng(this);

}
ServiceTask::~ServiceTask() {
    delete publishmng;
    delete ntfmng;
    delete subinfomng;
}

NTFMng*  ServiceTask ::getNTFMng() {
    return ntfmng;
}

void ServiceTask ::procMsg(TRscMsgHdr *rschdr, TRscMsgBody * rscbody,int msgType) {


    if(rschdr==0) return; //非法rsc消息不进行处理
    int rscHdrCode=rschdr->code;
    string rscRuri=rschdr->ruri;
    string str_rscRuri=rscRuri.c_str();  //cstr change to string
    vector<string> * topicVec=us.splitTopic(str_rscRuri,'/'); //ruri change to vector<string >  must free
    if((*topicVec)[0] != "service" || topicVec->size()<2) return; //error message

    if(msgType==0){



    }
    else if(msgType==1){ //PUBLISH SUBSCRIBE NOTIFY
        switch(rscHdrCode){
            case PUBLISH :{ //PUBLISH 有不同的业务 需要 ruri 去判断 比如 消息业务 对话业务 状态推送等等
                if((*topicVec)[1]=="msg"){
                    cout<<"begin to proc publish" << endl;
                    procPUBMsginP(rschdr,rscbody);


                }
                else if((*topicVec)[1]=="dlg"){

                }
                else if((*topicVec)[1]=="state"){
                   publishmng->procPubState(rschdr,rscbody);
                }
                break;
            }
            case SUBSCRIBE :{
                if((*topicVec)[1]=="msg"){

                }
                else if((*topicVec)[1]=="dlg"){

                }
                else if((*topicVec)[1]=="state"){
                    subinfomng->procSubState(rschdr,rscbody);
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
                cout<<"begin to process notifyack" ;
                prccNTFAckMsginN(rschdr ,rscbody);
                break;
            }
        }


    }

    delete topicVec; //free vec




    return;


}

string ServiceTask ::getDest(string to) {
    if(to.length()>0)
    return "10.3.8.211:8080";
    else
    return "127.0.0.1";
}

void ServiceTask ::sendMsg(int code, string ruri, string from, string to, string rid, string destination,
                           string jsoncontent) {
    TRscMsgHdr * head=new TRscMsgHdr();
    TRscMsgBody * body = new TRscMsgBody();
    head->code=code;
    head->ruri=ruri;
    head->consumer=from;
    head->producer=to;
    head->rid=rid;
    body->rsc=jsoncontent;



}


SubInfoMng * ServiceTask :: getSubMng(){
    return subinfomng;
}