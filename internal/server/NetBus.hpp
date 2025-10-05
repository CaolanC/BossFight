#pragma once

#include <crossguid/guid.hpp>
#include <concurrentqueue.h>
#include <server/NetMsg.hpp>

namespace server
{

class NetBus
{
public:
    NetBus();

    xg::Guid send_in(NetMsg msg);

    void try_get_in(NetMsgType type);

    moodycamel::ConcurrentQueue<NetMsg> out_q;
    moodycamel::ConcurrentQueue<NetMsg> in_q;
private:

};

}