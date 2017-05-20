//
// Created by dell on 2017/3/24.
//





#include "SubInfoMng.h"



SubInfoMng::SubInfoMng(ServiceTask * p){
    root=new SerTreeNode();
    proxy=p;
    substateset=new set<string> ();
    us=new UtilService();

}

SubInfoMng::~SubInfoMng(){
    dfsTree(root);
    delete substateset;
    delete us;
}

void SubInfoMng::dfsTree(SerTreeNode * root){
    if(root==NULL) return;
    int len=(root->childList).size();
    for(int i=0;i<len;i++){
        dfsTree((root->childList)[i]);
    }
    delete root;
}



vector<SerTreeNode* > SubInfoMng:: searchNodeList(string topic){ //添加订阅  若无结点则创建结点 若存在结点则返回结点列表
    vector <string> * tmp1;
    vector <SerTreeNode * > res;
    tmp1=us->splitRuri(topic,'/'); //分解topic  /abc/def/ghi
    int len=tmp1->size();
    SerTreeNode * current=root;//定位到根节点
    for(int i=0;i<len;i++){
        SerTreeNode * tmptree1=NULL;
        tmptree1=current->searchChildren((*tmp1)[i]); //如果搜索到孩子  赋值到tmptree1上面
        if(tmptree1==NULL){  //没找到可以创建新结点
            tmptree1=new SerTreeNode();
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


void SubInfoMng:: add_clientid(vector<SerTreeNode *> & vecst , string clientid){//添加订阅者

    int len = vecst.size();
    for(int i=0;i<len;i++){
        vecst[i]->clientSet.insert(clientid);
    }

}


void SubInfoMng:: remove_clientid(vector<SerTreeNode *> & vecst , string clientid){//移除订阅者

    int len = vecst.size();
    for(int i=0;i<len;i++){
        vecst[i]->clientSet.erase(clientid);
    }

}



vector<SerTreeNode* > SubInfoMng :: PSearchNodeList(string topic){
    vector <string> * tmp1;
    vector <SerTreeNode * > res;
    tmp1=us->splitRuri(topic,'/'); //分解topic  /abc/def/ghi
    int len=tmp1->size();
    SerTreeNode * current=root;//定位到根节点
    for(int i=0;i<len;i++){
        SerTreeNode * tmptree1=NULL;
        tmptree1=current->searchChildren((*tmp1)[i]); //如果搜索到孩子  赋值到tmptree1上面
        if(tmptree1==NULL){  //没找到可以创建新结点
            return res;
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



set<string > SubInfoMng ::  get_clientid(vector<SerTreeNode* > & vec){
    set<string > set1;
    int len=vec.size();
    for(int i=0;i<len;i++){
         set<string > :: iterator itbegin;
         set<string > :: iterator itend;
         itbegin=vec[i]->clientSet.begin();
         itend=vec[i]->clientSet.end();
         for(;itbegin!=itend;itbegin++){
             set1.insert(*itbegin);
         }

    }
    return set1;


}

set<string >  SubInfoMng :: getClientForP(string topic) { //对外提供 将上面两个方法合为一种
     vector < SerTreeNode * > vec= PSearchNodeList ( topic );
     set<string > res = get_clientid(vec);
     if(vec.size()>0){
         vector<SerTreeNode *> vec2=getParents(vec[0]);
         set<string> res2=get_clientid(vec2);
         set<string>::iterator itbegin=res2.begin();
         set<string>::iterator itend=res2.end();
         for(;itbegin!=itend;itbegin++){
             res.insert(*itbegin); //把父亲结点的客户们加进来
         }
     }
     return res;
}

vector<SerTreeNode*> SubInfoMng ::getParents(SerTreeNode* current){
     vector<SerTreeNode*> parents;//仅仅添加路径上的所有父亲
     SerTreeNode* tmp1=NULL;
     if(current!=root){
         tmp1=current->parent;
         while(tmp1!=root&&tmp1!=NULL){
               parents.push_back(tmp1);
               tmp1=tmp1->parent;
         }
     }
    return parents;

}

/*
   sub消息

   consumer当作 clientid
   rid 依然作为判断是不是重复消息的依据
   isdelete用来 作为是否添加订阅 或者是退订

    state:{
       topic: "/abc/def/ghi",
       isdelete : "0"
    }
 */
void SubInfoMng::proc_state_sub(TRscMsgHdr * rschead , TRscMsgBody * rscbody){
      string clientid = rschead->consumer;
      string rid=rschead->rid;
      string originrid=rid;
      string str = rscbody->rsc;
      set <string >  :: iterator tmp1;
      rid=clientid+"_"+rid; //避免不同客户端冲突
      cout<<"rid is "<<rid<<endl;
      tmp1=substateset->find(rid);
      string topic;
      int isdelete=-1;
      if(tmp1!=substateset->end()){ //find it no proc

      }
      else{
      cout<<"proc"<<endl;
        substateset->insert(rid);
        //通过主题查找要发给谁
        JSONValue *recjv = JSON::Parse(str.c_str());
        if (recjv == NULL || !recjv->IsObject()) return;
        JSONObject root = recjv->AsObject();
        JSONObject::const_iterator it = root.find(L"state");
        if (it != root.end()) {//have found
            if (it->second->IsObject()) { //it->second is JSONValue *
                JSONObject msg = it->second->AsObject();
                JSONObject::const_iterator itmtype = msg.find(L"topic");
                if (itmtype != msg.end()) {
                    topic = us->ws2s(itmtype->second->AsString());
                }
                JSONObject::const_iterator itmcontent = msg.find(L"isdelete");
                if (itmcontent != msg.end()) {
                    isdelete = itmcontent->second->AsNumber();
                }
            }
            if (isdelete == 0) {
                vector<SerTreeNode *> vec = searchNodeList(topic); //创建订阅结点
                add_clientid(vec, clientid);//添加订阅者
                if(vec.size()>0) { //防止越界,给父亲结点也添加订阅者
                    vector<SerTreeNode *> vec2 = getParents(vec[0]);
                    add_clientid(vec2, clientid);//添加订阅者
                }
            }
            else { //执行删除订阅逻辑
                vector<SerTreeNode *> vec = searchNodeList(topic); //创建订阅结点
                remove_clientid(vec, clientid);//添加订阅者
            }

        }//root


    }



}