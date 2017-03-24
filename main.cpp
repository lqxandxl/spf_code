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
        cout<<(vec[i])->singleTopic<<endl;
    }

    return 0;
}