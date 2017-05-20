#include <iostream>

#include <sstream>
#include <algorithm>

#include "SubInfoMng.h"
#include "ServiceTask.h"


using namespace std;

/*
void testsubtree(){
    SubInfoMng sbm;
    sbm.searchNodeList("abc/def/cgh");
    sbm.searchNodeList("abc/def/dee");
    sbm.searchNodeList("abc/def/dee/dddds");
    vector<SerTreeNode *> vec=sbm.searchNodeList("abc/def");
    int len=vec.size();
    for(int i=0;i<len;i++){
        vector<SerTreeNode *> vec=sbm.searchNodeList("abc/def");
        cout<<(vec[i])->singleTopic<<endl;
    }
}
*/
/*
void testJsonmain(){
    UtilService us;
    wstring str=us.getMsgJsonStr();
    string sstr=us.ws2s(str);
    cout<<sstr<<endl;
    us.procMsgJson(sstr);
}
*/



void testPubLishMsg(){
    ServiceTask p;
    string str="{\"msg\":{\"from\":\"18610191733\",\"to\":[\"88725004772\"],\"mtype\": \"common\",\"mcontent\": \"give me the money\",\"mid\":\"3322\"} }";
    string str1="{\"msg\":{\"from\":\"18610191733\",\"to\":[\"88725004772\",\"83838\"],\"mtype\": \"common\",\"mcontent\": \"give me the money\",\"mid\":\"33221\"} }";
}

void testpubandnotify(){
    ServiceTask p;
    string str="{\"msg\":{\"from\":\"18610191733\",\"to\":[\"88725004772\"],\"mtype\": \"common\",\"mcontent\": \"give me the money\",\"mid\":\"3322\"} }";
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

    TRscMsgHdr * head3=new TRscMsgHdr();
    TRscMsgBody * body3 =new TRscMsgBody();
    head3->code=6;
    head3->rid="liqixin_234443332_houliangping";
    head3->ruri="service/msg";
    st->procMsg(head3,body3,2);


}

void testSub(){
    ServiceTask * st=new ServiceTask();
    TRscMsgHdr * head=new TRscMsgHdr();
    TRscMsgBody * body =new TRscMsgBody();
    head->code=3; //sub
    head->rid="23344442222";
    head->ruri="/service/state/remote";
    head->consumer="tianjun";
    body->rsc="{\"state\":{\"topic\": \"/abc/def/hg\",\"isdelete\": \"0\"} }";
    st->getSubMng()->proc_state_sub(head,body);

    TRscMsgHdr * head1=new TRscMsgHdr();
    TRscMsgBody * body1 =new TRscMsgBody();
    head1->code=3; //sub
    head1->rid="212222122";
    head1->ruri="/service/state/remote";
    head1->consumer="fengzihang";
    body1->rsc="{\"state\":{\"topic\": \"/abc/def/hg/ihh\",\"isdelete\": \"0\"} }";
    st->procMsg(head1,body1,1);

    TRscMsgHdr * head3=new TRscMsgHdr();
    TRscMsgBody * body3 =new TRscMsgBody();
    head3->code=3; //sub
    head3->rid="2122222122";
    head3->ruri="/service/state/remote";
    head3->consumer="liushangming";
    body3->rsc="{\"state\":{\"topic\": \"/abc\",\"isdelete\": \"0\"} }";
    st->procMsg(head3,body3,1);

    TRscMsgHdr * head2=new TRscMsgHdr();
    TRscMsgBody * body2 =new TRscMsgBody();
    head2->code=1; //pub
    head2->rid="200202023";
    head2->ruri="/service/state/remote";
    //head2->consumer="fengzihang";
    body2->rsc="{\"state\":{\"topic\": \"/abc/def/hg\",\"content\": \"give me the money\"} }";
    st->procMsg(head2,body2,1);



}


int main() {


    testSub();
    return 0;
}