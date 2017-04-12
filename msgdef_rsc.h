//
// Created by dell on 2017/4/12.
//

#ifndef SPFCODE_MSGDEF_RSC_H
#define SPFCODE_MSGDEF_RSC_H

#include <string>
using namespace std;

class TRscMsgHdr
{
public:
    int           code;
    string           ruri;
    string           consumer;
    string           producer;
    string          host;
    string          rid;

};


class TRscMsgBody
{
public:
    string           rsc;

};



#endif //SPFCODE_MSGDEF_RSC_H
