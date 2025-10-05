#pragma once

#include <crossguid/guid.hpp>

namespace server

{

class Session
{
public:
    Session() : id(xg::newGuid()) {

    };

    xg::Guid get_id() {
        return id;
    }

private:
    xg::Guid id;

};

}