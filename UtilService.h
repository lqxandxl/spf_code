//
// Created by dell on 2017/3/24.
//


#ifndef SPFCODE_UTILSERVICE_H
#define SPFCODE_UTILSERVICE_H

#include <string>
#include <vector>
#include "json.h"
#include "jsonvalue.h"
#include <sstream>



using namespace std;

class UtilService{  //收到内部消息后所采取的一系列行为  共性的行为
public:
    vector<string> * splitTopic(const string s,char f); //将 A/B/msg/local这样的url 划分为数组形式存储
    vector<string> * splitRuri(const string s,char f); //将 /A/B/msg/local这样的url 划分为数组形式存储    
    std::string ws2s(const std::wstring& ws);
    std::wstring s2ws(const std::string& s);
    string int_to_string(int n);
};


#endif //SPFCODE_UTILSERVICE_H
