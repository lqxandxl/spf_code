//
// Created by dell on 2017/3/24.
//

#include <iostream>
#include "UtilService.h"
using namespace std;

vector <string> * UtilService:: splitTopic(const string s,char f){
    vector <string> * vec = new vector<string>();
    int last=0;
    int index=s.find_first_of(f,last);
    while(index!=-1){
        vec->push_back(s.substr(last,index-last));
        last=index+1;
        index=s.find_first_of(f,last);
    }
    //还剩下最后一个分割的字符串
    vec->push_back(s.substr(last));
    //cout<<"split finish"<<endl;
    return vec;
}

vector<string> * UtilService::splitRuri(const string s,char f){ //将 /A/B/msg/local这样的url 划分为数组形式存储    
    vector <string> * vec = new vector<string>();
    int last=1;
    int index=s.find_first_of(f,last);
    while(index!=-1){
        vec->push_back(s.substr(last,index-last));
        last=index+1;
        index=s.find_first_of(f,last);
    }
    //还剩下最后一个分割的字符串
    vec->push_back(s.substr(last));
    //cout<<"split finish"<<endl;
    return vec;

}

std::string UtilService::ws2s(const std::wstring& ws)
{
    std::string curLocale = setlocale(LC_ALL, NULL);        // curLocale = "C";
    setlocale(LC_ALL, "chs");
    const wchar_t* _Source = ws.c_str();
    size_t _Dsize = 2 * ws.size() + 1;
    char *_Dest = new char[_Dsize];
    memset(_Dest,0,_Dsize);
    wcstombs(_Dest,_Source,_Dsize);
    std::string result = _Dest;
    delete []_Dest;
    setlocale(LC_ALL, curLocale.c_str());
    return result;
}

std::wstring UtilService::s2ws(const std::string& s)
{
    setlocale(LC_ALL, "chs");
    const char* _Source = s.c_str();
    size_t _Dsize = s.size() + 1;
    wchar_t *_Dest = new wchar_t[_Dsize];
    wmemset(_Dest, 0, _Dsize);
    mbstowcs(_Dest,_Source,_Dsize);
    std::wstring result = _Dest;
    delete []_Dest;
    setlocale(LC_ALL, "C");
    return result;
}


string UtilService :: int_to_string(int n){
    string res;
    stringstream ss;
    ss<<n;
    ss>>res;
    return res;
}