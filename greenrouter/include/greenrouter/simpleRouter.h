#ifndef __simpleRouter_h__
#define __simpleRouter_h__

#include "genericRouter.h"
#include "protocol/SimpleBus/simpleBusProtocol.h"
#include "scheduler/fixedPriorityScheduler.h"

namespace gs {
namespace gp {

template <unsigned int BUSWIDTH>
class SimpleRouter : public gs::gp::GenericRouter<BUSWIDTH>
{
public:
    SimpleRouter(const char* name)
        : GenericRouter<BUSWIDTH>(name)
    {
        gs::gp::SimpleBusProtocol<BUSWIDTH> *protocol;
        gs::gp::fixedPriorityScheduler *scheduler;

        protocol = new gs::gp::SimpleBusProtocol<BUSWIDTH>(sc_gen_unique_name("protocol"), 10);
        scheduler = new gs::gp::fixedPriorityScheduler(sc_gen_unique_name("scheduler"));

        protocol->router_port(*this);
        protocol->scheduler_port(*scheduler);
        this->protocol_port(*protocol);
    }
};

}
}

#endif
