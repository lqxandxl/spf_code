//
// Created by dell on 2017/3/24.
//

#ifndef SPFCODE_TREENODE_H
#define SPFCODE_TREENODE_H
#include <string>
#include <vector>

using namespace std;


class TreeNode{
public:
    TreeNode * searchChildren(string singleTopic); //检索孩子结点是不是有要找的信息
    vector<TreeNode * >  getAllDes();//拿到所有子孙
    void addChild(TreeNode * child);  //添加孩子
    void setTopic(string s);
    void setParent(TreeNode * p);

    string singleTopic; //结点当前内容
    vector<TreeNode *> childList;  //孩子结点
    //vector<Subscription> clientList;  //订阅者列表 clientid
    TreeNode * parent; //父结点



};


#endif //SPFCODE_TREENODE_H
