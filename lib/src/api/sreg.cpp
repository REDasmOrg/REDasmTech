#include "../context.h"
#include "../state.h"
#include <redasm/sreg.h>
#include <spdlog/spdlog.h>

u64 rd_getsreg_ex(RDAddress address, int sreg, u64 d) {
    spdlog::trace("rd_getsreg_ex({:x}, {}, {:x})", address, sreg, d);

    if(redasm::state::context) {
        auto v = redasm::state::context->get_sreg(address, sreg);
        if(v) return *v;
    }

    return d;
}

RDRegValue rd_getsreg(RDAddress address, int sreg) {
    spdlog::trace("rd_getsreg({:x}, {})", address, sreg);

    if(redasm::state::context) {
        if(auto v = redasm::state::context->get_sreg(address, sreg); v)
            return RDRegValue_some(*v);
    }

    return RDRegValue_none();
}

void rd_addsreg_range(RDAddress start, RDAddress end, int sreg, u64 val) {
    spdlog::trace("rd_addsreg_range({:x}, {:x}, {}, {:x})", start, end, sreg,
                  val);
    if(redasm::state::context)
        redasm::state::context->program.add_sreg_range(start, end, sreg, val);
}

void rd_setsreg(RDAddress address, int sreg, u64 val) {
    spdlog::trace("rd_setsreg({:x}, {}, {:x})", address, sreg, val);
    if(redasm::state::context)
        redasm::state::context->set_sreg(address, sreg, RDRegValue_some(val));
}

void rd_setsreg_from(RDAddress address, int sreg, u64 val, RDAddress fromaddr) {
    spdlog::trace("rd_setsreg_from({:x}, {}, {:x})", address, sreg, val,
                  fromaddr);
    if(redasm::state::context) {
        redasm::state::context->set_sreg(address, sreg, RDRegValue_some(val),
                                         fromaddr);
    }
}
