#pragma once

#include <NetMsg.hpp>

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

    class JoinAsGodReplyPayload : public ReplyPayload {
    public:
        JoinAsGodReplyPayload() : ReplyPayload(JoinAsGodRequest) {
        }
    };

    class CreateSessionReplyPayload : public ReplyPayload {
    public:
        CreateSessionReplyPayload() : ReplyPayload(CreateSessionRequest) {}
    };

}