#include <iostream>
#include "SubInfoMng.h"


using namespace std;

int main() {

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
    UtilService us;
    string str="{\"msg\":{\"from\":\"18610191733\",\"to\":\"88725004772\",\"mtype\": \"common\",\"mcontent\": \"give me the money\" } }";
    us.procMsgJson(str);
    return 0;
}