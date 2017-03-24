//
// Created by dell on 2017/3/24.
//

#ifndef SPFCODE_SUBINFOMNG_H
#define SPFCODE_SUBINFOMNG_H

#include <string>
#include <vector>
#include "TreeNode.h"
#include "UtilService.h"


using namespace std;

class SubInfoMng{
public:
//	void addSubscription(Subscripton sub); //添加订阅
    vector<TreeNode* > searchNodeList(string topic); //添加订阅  若无结点则创建结点 若存在结点则返回结点列表
    SubInfoMng();
    ~SubInfoMng();
    UtilService util; //工具类对象
    TreeNode * root; //root结点
    void dfsFree(TreeNode * t); //释放treenode 之间new过 必须删除

};


#endif //SPFCODE_SUBINFOMNG_H
