//
// Created by dell on 2017/3/24.
//



#include "TreeNode.h"

TreeNode * TreeNode::searchChildren(string singleTopic){
    int len=childList.size();
    for(int i=0;i<len;i++){
        if(childList[i]->singleTopic==singleTopic){
            return childList[i];
        }
    }
    return NULL;

}

void TreeNode :: addChild(TreeNode * child){  //增加孩子
    if(child)
        childList.push_back(child);
}


vector <TreeNode * >    TreeNode :: getAllDes(){  //拿到所有子孙以及自己
    int len=childList.size();
    vector <TreeNode * > res;
    res.push_back((TreeNode *&&) this);
    int childlen;
    if(len>0){
        for(int i=0;i<len;i++){
            vector<TreeNode * >   vec=childList[i]->getAllDes();
            childlen=vec.size();
            for(int j=0;j<childlen;j++){
                res.push_back(vec[j]);
            }

        }
    }
    return res;
}

void TreeNode :: setTopic(string s){
    singleTopic=s;
}

void TreeNode :: setParent(TreeNode * p){
    parent=p;
}


