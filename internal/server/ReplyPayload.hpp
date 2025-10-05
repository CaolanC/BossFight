#pragma once

#include <server/NetMsg.hpp>

namespace server
{

class ReplyPayload
{
public:
    explicit ReplyPayload(NetMsgType type);

    NetMsgType type;
};

class JoinReplyPayload : public ReplyPayload
{
public:
    JoinReplyPayload() : ReplyPayload(JoinRequest) {

    }
};

}