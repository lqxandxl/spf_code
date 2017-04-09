//
// Created by dell on 2017/4/1.
//
#include "Proxy.h"

void Proxy :: procPubMsg(string s){

    publishmng->procPubMsg(s);

}


void Proxy:: procNTFAckMsg(string msgid,string to) { //需要知道两条信息 一个是对应的pub的消息id  还有一个是是谁发过来的ack
    publishmng->procNTFAckMsg(msgid,to);
}

Proxy :: Proxy(){
    publishmng=new PublishMng(this);
    //publishmng->setProxy(this);
    ntfmng=new NTFMng(this);


}
Proxy::~Proxy() {
    delete publishmng;
    delete ntfmng;
}

NTFMng*  Proxy ::getNTFMng() {
    return ntfmng;
}

