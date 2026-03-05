#pragma once

#include <crossguid/guid.hpp>
#include <PerfectServer.hpp>

namespace server

{

class Session
{
public:
    Session() : id(xg::newGuid()), ps(0) {

    };

    xg::Guid get_id() {
        return id;
    }

    void setPSPort(int p) {
        ps.setPort(p);
    }

    void startServer() { ps.start(); }
    void stopServer()  { ps.stop(); }

private:
    xg::Guid id;
    PerfectServer ps;

};

}