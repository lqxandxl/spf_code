//
// Created by dell on 2017/3/24.
//

#include <iostream>
#include "UtilService.h"
using namespace std;

vector <string> * UtilService:: splitTopic(const string s){
    vector <string> * vec = new vector<string>();
    int last=0;
    int index=s.find_first_of("/",last);
    while(index!=-1){
        vec->push_back(s.substr(last,index-last));
        last=index+1;
        index=s.find_first_of("/",last);
    }
    //还剩下最后一个分割的字符串
    vec->push_back(s.substr(last));
    //cout<<"split finish"<<endl;
    return vec;
}

void UtilService:: showVec(vector<string> * vec){
    vector <string> & v=*vec;
    int len=v.size();
    for(int i=0;i<len;i++){
        cout<< v[i] << endl;
    }



}
/* 样例
{
  "msg":{
     "from":"18610191733",
     "to":"88725004772",
     "mtype": "common",
     "mcontent": "give me the money"
  }
}
*/
void UtilService :: procMsgJson(string str){
    JSONValue * recjv=JSON::Parse(str.c_str());
    if(recjv==NULL||!recjv->IsObject()) return ;
    JSONObject root=recjv->AsObject();
    JSONObject::const_iterator it=root.find(L"msg");
    if(it!=root.end()){//have found
        if(it->second->IsObject()){ //it->second is JSONValue *
            JSONObject msg=it->second->AsObject();
            JSONObject::const_iterator itfrom=msg.find(L"from");
            if(itfrom!=msg.end()) {
                wstring strfrom = itfrom->second->AsString();
                //操作赋值给内部结构
                wcout<<strfrom<<endl;
               // string sfrom=ws2s(strfrom);
               // cout<<sfrom<<endl;
               // wstring test=s2ws(sfrom);
                //wcout<<test<<endl;
            }
            //msg=it->second->AsObject();
            JSONObject::const_iterator itto=msg.find(L"to");
            if(itto!=msg.end()){
                wstring strto=itto->second->AsString();
                wcout<<strto<<endl;
            }
            //msg=it->second->AsObject();
            JSONObject::const_iterator itmtype=msg.find(L"mtype");
            if(itto!=msg.end()){
                wstring strmtype=itmtype->second->AsString();
                wcout<<strmtype<<endl;
            }
            //msg=it->second->AsObject();
            JSONObject::const_iterator itmcontent=msg.find(L"mcontent");
            if(itmcontent!=msg.end()){
                wstring strmcontent=itmcontent->second->AsString();
                wcout<<strmcontent<<endl;
            }

        }
    }


    return;
}


/* 样例
{
  "msg":{
     "from":"18610191733",
     "to":"88725004772",
     "mtype": "common",
     "mcontent": "give me the money"
  }
}
*/
std::wstring UtilService ::getMsgJsonStr() {
    //cout<<"begin to process"<<endl;
    //JSONValue res;
    JSONObject newJsonObject;
    JSONObject innerJsobj;
    innerJsobj[L"from"]=new (std::nothrow) JSONValue(L"1877777777");
    innerJsobj[L"to"]=new (std::nothrow) JSONValue(L"18722222777");
    innerJsobj[L"mtype"]=new (std::nothrow) JSONValue(L"coooommm");
    innerJsobj[L"mcontent"]=new (std::nothrow) JSONValue(L"don not give money");
    //JSONValue res=innerJsobj;
    newJsonObject[L"msg"]=new (std::nothrow) JSONValue(innerJsobj);
    JSONValue res=newJsonObject;
    //res.Stringify();
    std::wstring resstr=res.Stringify().c_str();
    //cout<<"all finish"<<endl;
    return resstr;
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
