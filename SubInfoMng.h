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


    SubInfoMng(ServiceTask * p);
    ~SubInfoMng();

    //for subscribe 产生订阅
    void proc_state_sub(TRscMsgHdr * rschead , TRscMsgBody * rscbody); //处理subscribe消息
    vector<SerTreeNode* > searchNodeList(string topic); //添加订阅  若无结点则创建结点 若存在结点则返回结点列表
    void add_clientid(vector<SerTreeNode *> & vecst,string clientid);//添加订阅者
    void remove_clientid(vector<SerTreeNode *> & vecst,string clientid); //移除订阅者
    vector<SerTreeNode*> getParents(SerTreeNode* current); //拿到其父亲们的结点，不包括root added 5-22
    
    //for publish 搜索 客户结点
    set<string >  getClientForP(string topic,string producer); //对外提供 将下面两个方法合为一种
    vector<SerTreeNode* > PSearchNodeList(string topic); //只是单纯的搜索，没有就返回null，针对publish
    set<string >  get_clientid(vector<SerTreeNode* > & vec); //从结点中拿到clientid 方便推送

    void dfsTree(SerTreeNode * t); //释放treenode 之间new过 必须删除
    void clean_substateset();
    //added 6-20
    void change_root(string consumerid);//订阅的是谁的树

    //added 6-22
    void add_cache_map(string consumerid);
    SerTreeNode * get_root(string userid);
    void delete_tree(string userid);
 
private:
    ServiceTask * proxy;
    set <string > * substateset; //存放subscribe state消息 避免重复消息的处理
    UtilService * us; //工具类对象
    SerTreeNode * root; //root结点
    //added 6-20
    map <string,SerTreeNode *> root_map;
};


#endif //SPFCODE_SUBINFOMNG_H