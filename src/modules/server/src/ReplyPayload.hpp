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
        CreateSessionReplyPayload(xg::Guid session_id, int ws_port, bool ok)
        : ReplyPayload(CreateSessionRequest),
          session_id(session_id),
          ws_port(ws_port),
          ok(ok) {}

        xg::Guid session_id;
        int ws_port;
        bool ok;
    };

}