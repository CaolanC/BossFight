#pragma once

#include <crossguid/guid.hpp>

namespace server
{

typedef enum {
    JoinRequest,
    JoinReply,
    CreateSessionRequest,
    CreateSessionReply,
    DefaultReply
} NetMsgType;

struct NetMsg
{
    xg::Guid id;
    NetMsgType type;
    xg::Guid req_id;
};

};