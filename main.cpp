#include <iostream>
#include "SubInfoMng.h"
#include "ServiceTask.h"


using namespace std;

void testsubtree(){
    SubInfoMng sbm;
    sbm.searchNodeList("abc/def/cgh");
    sbm.searchNodeList("abc/def/dee");
    sbm.searchNodeList("abc/def/dee/dddds");
    vector<TreeNode *> vec=sbm.searchNodeList("abc/def");
    int len=vec.size();
    for(int i=0;i<len;i++){
        vector<TreeNode *> vec=sbm.searchNodeList("abc/def");
        cout<<(vec[i])->singleTopic<<endl;
    }
}

void testJsonmain(){
    UtilService us;
    //string str="{\"msg\":{\"from\":\"18610191733\",\"to\":\"88725004772\",\"mtype\": \"common\",\"mcontent\": \"give me the money\" } }";
    wstring str=us.getMsgJsonStr();
    //wcout<<str<<endl;
    string sstr=us.ws2s(str);
    cout<<sstr<<endl;
    us.procMsgJson(sstr);
}


void testPubLishMsg(){
    ServiceTask p;
    //string str="{\"msg\":{\"from\":\"18610191733\",\"to\":\"[\"88725004772\"]\",\"mtype\": \"common\",\"mcontent\": \"give me the money\", \"mid\":\"8282828\" } }";
    string str="{\"msg\":{\"from\":\"18610191733\",\"to\":[\"88725004772\"],\"mtype\": \"common\",\"mcontent\": \"give me the money\",\"mid\":\"3322\"} }";
   // p.procPubMsg(str);
    string str1="{\"msg\":{\"from\":\"18610191733\",\"to\":[\"88725004772\",\"83838\"],\"mtype\": \"common\",\"mcontent\": \"give me the money\",\"mid\":\"33221\"} }";
    //p.procPubMsg(str1);
   // PublishMng p1;
   // p1.showMsgMap();

}

void testpubandnotify(){
    ServiceTask p;
    //string str="{\"msg\":{\"from\":\"18610191733\",\"to\":\"[\"88725004772\"]\",\"mtype\": \"common\",\"mcontent\": \"give me the money\", \"mid\":\"8282828\" } }";
    string str="{\"msg\":{\"from\":\"18610191733\",\"to\":[\"88725004772\"],\"mtype\": \"common\",\"mcontent\": \"give me the money\",\"mid\":\"3322\"} }";
    //p.procPubMsg(str);
}

void testPublishMsg1(){
    TRscMsgHdr * head=new TRscMsgHdr();
    TRscMsgBody * body =new TRscMsgBody();
    head->code=1;
    head->consumer="liqixin";
    head->producer="fengzihang&zhouzheng&houliangping";
    head->rid="234443332";
    head->ruri="service/msg";
    body->rsc="{\"msg\":{\"mtype\": \"common\",\"mcontent\": \"give me the money\"} }";
    ServiceTask * st=new ServiceTask();
    st->procMsg(head,body,1);

    //for notifyack
    TRscMsgHdr * head1=new TRscMsgHdr();
    TRscMsgBody * body1 =new TRscMsgBody();
    head1->code=6;
    head1->rid="liqixin_234443332_fengzihang";
    head1->ruri="service/msg";
    st->procMsg(head1,body1,2);

    TRscMsgHdr * head2=new TRscMsgHdr();
    TRscMsgBody * body2 =new TRscMsgBody();
    head2->code=6;
    head2->rid="liqixin_234443332_zhouzheng";
    head2->ruri="service/msg";
    st->procMsg(head2,body2,2);
/*
    TRscMsgHdr * head3=new TRscMsgHdr();
    TRscMsgBody * body3 =new TRscMsgBody();
    head3->code=6;
    head3->rid="liqixin_234443332_houliangping";
    head3->ruri="service/msg";
    st->procMsg(head3,body3,2);
*/

}
int main() {

    testPublishMsg1();

    return 0;
}