#ifndef SPFCODE_MESSAGE_H
#define SPFCODE_MESSAGE_H

#include <string>
#include <vector>
#include <iostream>
#include <map>

using namespace std;

class PublishMsg{  //PUBLISH来的msg业务
public:
    string msgid;
    string from;
    vector <string>  to;  //为了一对多发消息 msg 所有的要发送的用户信息
    map < string , int >  userstate;  // <userid,0> 代表还未送达  <userid,1> 代表已经送达  所有都为1 则可以删除该消息了
    string body; //msg 业务转发即可 没必要解析内容
    ~PublishMsg();
};


class NotifyMsg{
public:
    string msgid; // publishmsgid_toid
    string from;
    string to;
    int msgstate; //消息是否发送成功 0为没收到ack 1为收到ack
    string publishmsgid;//属于哪个publish生成的notify
    string body; //body存放内容
};


#endif //SPFCODE_MESSAGE_H