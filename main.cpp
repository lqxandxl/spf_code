#include <iostream>
#include "SubInfoMng.h"
#include "Proxy.h"


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
    Proxy p;
    //string str="{\"msg\":{\"from\":\"18610191733\",\"to\":\"[\"88725004772\"]\",\"mtype\": \"common\",\"mcontent\": \"give me the money\", \"mid\":\"8282828\" } }";
    string str="{\"msg\":{\"from\":\"18610191733\",\"to\":[\"88725004772\"],\"mtype\": \"common\",\"mcontent\": \"give me the money\",\"mid\":\"3322\"} }";
    p.procPubMsg(str);
    string str1="{\"msg\":{\"from\":\"18610191733\",\"to\":[\"88725004772\",\"83838\"],\"mtype\": \"common\",\"mcontent\": \"give me the money\",\"mid\":\"33221\"} }";
    p.procPubMsg(str1);
   // PublishMng p1;
   // p1.showMsgMap();

}

void testpubandnotify(){
    Proxy p;
    //string str="{\"msg\":{\"from\":\"18610191733\",\"to\":\"[\"88725004772\"]\",\"mtype\": \"common\",\"mcontent\": \"give me the money\", \"mid\":\"8282828\" } }";
    string str="{\"msg\":{\"from\":\"18610191733\",\"to\":[\"88725004772\"],\"mtype\": \"common\",\"mcontent\": \"give me the money\",\"mid\":\"3322\"} }";
    p.procPubMsg(str);
}

void  testsendnotifyack(){
    Proxy p;
    //string str="{\"msg\":{\"from\":\"18610191733\",\"to\":\"[\"88725004772\"]\",\"mtype\": \"common\",\"mcontent\": \"give me the money\", \"mid\":\"8282828\" } }";
    string str="{\"msg\":{\"from\":\"18610191733\",\"to\":[\"88725004772\"],\"mtype\": \"common\",\"mcontent\": \"give me the money\",\"mid\":\"3322\"} }";
    //str的to字段可以尝试 多发 会产生多个notify
    p.procPubMsg(str);
    string strntfack="{\"msg\":{\"msgid\":\"18610191733_3322_88725004772\"} }";
    p.porcNTFAckMsginN(strntfack);
}
int main() {

    testsendnotifyack();

    return 0;
}