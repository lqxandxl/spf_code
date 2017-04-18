//
// Created by dell on 2017/3/24.
//

#ifndef SPFCODE_SUBINFOMNG_H
#define SPFCODE_SUBINFOMNG_H

#include <string>
#include <vector>
#include "SerTreeNode.h"
#include "UtilService.h"
#include <set>
#include "ServiceTask.h"
#include "msgdef_rsc.h"

using namespace std;

class ServiceTask;
class SubInfoMng{
public:
//	void addSubscription(Subscripton sub); //添加订阅
    vector<SerTreeNode* > searchNodeList(string topic); //添加订阅  若无结点则创建结点 若存在结点则返回结点列表
    void add_clientid(vector<SerTreeNode *> & vecst,string clientid);//添加订阅者
    vector<SerTreeNode* > PSearchNodeList(string topic); //只是单纯的搜索，没有就返回null，针对publish
    set<string >  get_clientid(vector<SerTreeNode* > & vec); //从结点中拿到clientid 方便推送
    set<string >  getClientForP(string topic); //对外提供 将上面两个方法合为一种
    void procSubState(TRscMsgHdr * rschead , TRscMsgBody * rscbody); //处理subscribe消息
    SubInfoMng(ServiceTask * p);
    ~SubInfoMng();
    UtilService util; //工具类对象
    SerTreeNode * root; //root结点
    void dfsFree(SerTreeNode * t); //释放treenode 之间new过 必须删除

private:
    ServiceTask * proxy;
};


#endif //SPFCODE_SUBINFOMNG_H
