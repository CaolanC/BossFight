#include <crossguid/guid.hpp>
#include <concurrentqueue.h>
#include <server/NetMsg.hpp>
#include <server/NetBus.hpp>

using namespace server;

namespace server
{
    NetBus::NetBus() {
    
    }

    xg::Guid NetBus::send_in(NetMsg msg) {
        in_q.enqueue(msg);

        return msg.id;
    }

    void NetBus::try_get_in(NetMsgType type) {
        // in_queue.try_dequeue();
    }
};