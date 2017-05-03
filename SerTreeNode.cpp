//
// Created by dell on 2017/3/24.
//



#include "SerTreeNode.h"

SerTreeNode * SerTreeNode::searchChildren(string singleTopic){
    int len=childList.size();
    for(int i=0;i<len;i++){
        if(childList[i]->singleTopic==singleTopic){
            return childList[i];
        }
    }
    return NULL;

}

void SerTreeNode :: addChild(SerTreeNode * child){  //增加孩子
    if(child)
        childList.push_back(child);
}


vector <SerTreeNode * >    SerTreeNode :: getAllDes(){  //拿到所有子孙以及自己
    int len=childList.size();
    vector <SerTreeNode * > res;
    // res.push_back((SerTreeNode *&&) this); 原语句在c++11编译可以
    res.push_back(this);
    int childlen;
    if(len>0){
        for(int i=0;i<len;i++){
            vector<SerTreeNode * >   vec=childList[i]->getAllDes();
            childlen=vec.size();
            for(int j=0;j<childlen;j++){
                res.push_back(vec[j]);
            }

        }
    }
    return res;
}

void SerTreeNode :: setTopic(string s){
    singleTopic=s;
}

void SerTreeNode :: setParent(SerTreeNode * p){
    parent=p;
}


