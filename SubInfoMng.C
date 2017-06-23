//
// Created by dell on 2017/3/24.
//





#include "SubInfoMng.h"



SubInfoMng::SubInfoMng(ServiceTask * p){
    //root=new SerTreeNode();
    root=NULL;
    proxy=p;
    substateset=new set<string> ();
    us=new UtilService();

}

SubInfoMng::~SubInfoMng(){
    map <string,SerTreeNode *> :: iterator itbegin=root_map.begin();
    map <string,SerTreeNode *> :: iterator itend=root_map.end();
    for(;itbegin!=itend;++itbegin){
        SerTreeNode *tmp = itbegin->second;
        root=tmp;
        dfsTree(root); //刪除整個樹結構
    }
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

//6-20新增形参 producer 
set<string >  SubInfoMng :: getClientForP(string topic,string producer) { //对外提供 将上面两个方法合为一种
     //added 6-20
     set<string> nullres;
     if(producer.size()<=0) return nullres;
      change_root(producer); //change root
           
     vector < SerTreeNode * > vec= searchNodeList ( topic ); //如果没有就添加主题，这样防止不能遍历父亲 edit 2017/6/2 去掉了P开头方法
     set<string > res = get_clientid(vec);
     //5-22 added
     if(vec.size()>0){
        vector<SerTreeNode *> vec2=getParents(vec[0]);
        set<string> res2=get_clientid(vec2);        
        set<string>::iterator itbegin=res2.begin();
        set<string>::iterator itend=res2.end();
        for(;itbegin!=itend;itbegin++){
             res.insert(*itbegin); //把父亲结点的客户们加进来
         }
     }
     //5-22 added
     return res;
}

/*
   sub消息

   producer当作 clientid
   consumer作为订阅了谁的状态
   rid 依然作为判断是不是重复消息的依据
   isdelete用来 作为是否添加订阅 或者是退订

    state:{
       topic: "/abc/def/ghi",
       isdelete : "0"
    }
 */
void SubInfoMng::proc_state_sub(TRscMsgHdr * rschead , TRscMsgBody * rscbody){
      string clientid = rschead->producer.c_str();
      string rid=rschead->rid.c_str();
      string toid=rschead->consumer.c_str();
      string originrid=rid;
      string str = rscbody->rsc.c_str();
      std :: set <string >  :: iterator tmp1;
      rid=clientid+"_"+rid; //避免不同客户端冲突
      //cout<<"proc state sub rid is "<<rid<<endl;
      UniINFO("beigin to proc state sub rid is %s",rid.c_str());
      tmp1=substateset->find(rid);
      string topic;
      int isdelete=-1;
      if(tmp1!=substateset->end()){ //find it no proc

      }
      else{  
        //added 6-20
        if(toid.size()<=0) return;
        change_root(toid); //change root
        
        //added 6-22
        add_cache_map(toid);

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
                //cout<<"proc sub state "<<topic<<" user is "<<clientid<<endl;
                UniINFO("beigin to proc state sub topic is %s",topic.c_str());
                UniINFO("beigin to proc state sub clientid is %s",clientid.c_str());
                vector<SerTreeNode *> vec = searchNodeList(topic); //创建订阅结点
                add_clientid(vec, clientid);//添加订阅者
                //added 5-22
                if(vec.size()>0) { //防止越界,给父亲结点也添加订阅者
                     vector<SerTreeNode *> vec2 = getParents(vec[0]);
                     add_clientid(vec2, clientid);//添加订阅者
                }
                //added 5-22               
            }
            else { //执行删除订阅逻辑
                vector<SerTreeNode *> vec = searchNodeList(topic); //创建订阅结点
                remove_clientid(vec, clientid);//添加订阅者
            }

        }//root


    }



}

//5-22 added
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

void SubInfoMng:: clean_substateset(){
    substateset->clear();
}


//這裡的參數指那一顆樹，sub則專門指consumer字段，pub則是producer字段
void SubInfoMng:: change_root(string consumerid){
     UniINFO("change root to consumerid is %s",consumerid.c_str());
     int res=root_map.count(consumerid);
     if(res==0){
        SerTreeNode * tmp=new SerTreeNode();
        root_map[consumerid]=tmp;
        root=tmp; //change root
     }
     else{
        root=root_map[consumerid]; 
     }

}

//6-22 缓存map中添加userid，数据迁移用
void SubInfoMng:: add_cache_map(string consumerid){
     map<string,string> * pcm=proxy->get_cache_map();
     string locip=proxy->get_local_ip();
     int res=pcm->count(consumerid);
     if(res==0){
        (*pcm)[consumerid]=locip;
     }
}

//6-22 
SerTreeNode * SubInfoMng:: get_root(string userid){
     int res=root_map.count(userid);
     if(res==0){
         return NULL;
     }
     else{
         return root_map[userid];
     }
}

//6-22
void SubInfoMng:: delete_tree(string userid){
    int res;
    res=root_map.count(userid);
    if(res==0){
        return;
    }
    else{
        root=root_map[userid];
        dfsTree(root); //刪除整個樹結構
    }
}