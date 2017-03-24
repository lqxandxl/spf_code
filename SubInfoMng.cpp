//
// Created by dell on 2017/3/24.
//





#include "SubInfoMng.h"



SubInfoMng::SubInfoMng(){
    root=new TreeNode();
}

SubInfoMng::~SubInfoMng(){
    dfsFree(root);
}

void SubInfoMng::dfsFree(TreeNode * root){
    if(root==NULL) return;
    int len=(root->childList).size();
    for(int i=0;i<len;i++){
        dfsFree((root->childList)[i]);
    }
    delete root;
}



vector<TreeNode* > SubInfoMng:: searchNodeList(string topic){ //添加订阅  若无结点则创建结点 若存在结点则返回结点列表
    vector <string> * tmp1;
    vector <TreeNode * > res;
    tmp1=util.splitTopic(topic); //分解topic  abc/def/ghi
    int len=tmp1->size();
    //cout<<"len is"<<len<<endl;
    TreeNode * current=root;//定位到根节点
    for(int i=0;i<len;i++){
        //cout<<"i si"<<i<<endl;
        TreeNode * tmptree1=NULL;
        tmptree1=current->searchChildren((*tmp1)[i]); //如果搜索到孩子  赋值到tmptree1上面
        if(tmptree1==NULL){  //没找到可以创建新结点
            tmptree1=new TreeNode();
            tmptree1->setTopic((*tmp1)[i]);
            tmptree1->setParent(current);
            current->addChild(tmptree1); //添加孩子
            current=tmptree1;//下一次从孩子这里迭代
        }
        else{   //找到了则继续匹配下一项
            current=tmptree1;

        }

        if(i==len-1){   //最后一个结点  不管是新添加的还是已经有的 都要返回其以及其所有子孙
            res=current->getAllDes();
        }

    }

    delete tmp1; //删除topic分割产生的内存
    return res;


}