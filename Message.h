//
// Created by dell on 2017/3/24.
//

#ifndef SPFCODE_MESSAGE_H
#define SPFCODE_MESSAGE_H

#include <string>
#include <vector>
#include <iostream>
#include <unordered_map>
using namespace std;
/*
class Message{ //不管外界什么样 到内部都应该抽象到这个结构 proxy将外界消息抽象成内部消息 再转发至相应的模块
    string type;//消息类型
    string msgid; //唯一标识 缓存用
    string topic; //url pub A/B/msg/local
    string data; //消息内容
    //string time; //发送时间 暂不需要
    string sender; //发送者userid  必须有 直接拿来判断是不是归属于自己  本质上不是userid 而应该是 要更新谁的资源 就是谁
};
*/

class PublishMsg{  //PUBLISH来的msg业务
public:
    string msgid;
    string type;
    string content;
    string from;
    vector <string>  to;  //为了一对多发消息 msg 所有的要发送的用户信息
    unordered_map < string , int >  userstate;  // <userid,0> 代表还未送达  <userid,1> 代表已经送达  所有都为1 则可以删除该消息了

    ~PublishMsg();
};


#endif //SPFCODE_MESSAGE_H
