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
