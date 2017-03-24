//
// Created by dell on 2017/3/24.
//


#ifndef SPFCODE_UTILSERVICE_H
#define SPFCODE_UTILSERVICE_H

#include <string>
#include <vector>

using namespace std;

class UtilService{  //收到内部消息后所采取的一系列行为  共性的行为
public:
    vector<string> * splitTopic(const string s); //将 A/B/msg/local这样的url 划分为数组形式存储
    void showVec(vector<string> * vec); //遍历显示vec
    //assembleTopic();
    //changeTopic();// 将 A/B/..../local 修改为 B/A/.../remote
    // procMsg(Message msg);// 主逻辑 处理消息流程

};


#endif //SPFCODE_UTILSERVICE_H
