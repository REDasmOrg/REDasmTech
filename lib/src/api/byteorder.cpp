#include "../memory/byteorder.h"
#include <redasm/byteorder.h>
#include <spdlog/spdlog.h>

u16 rd_fromle16(u16 hostval) {
    spdlog::trace("rd_fromle16({:x}", hostval);
    return redasm::byteorder::from_littleendian(hostval);
}

u32 rd_fromle32(u32 hostval) {
    spdlog::trace("rd_fromle32({:x})", hostval);
    return redasm::byteorder::from_littleendian(hostval);
}

u64 rd_fromle64(u64 hostval) {
    spdlog::trace("rd_fromle64({:x})", hostval);
    return redasm::byteorder::from_littleendian(hostval);
}

u16 rd_frombe16(u16 hostval) {
    spdlog::trace("rd_frombe16({:x})", hostval);
    return redasm::byteorder::from_bigendian(hostval);
}

u32 rd_frombe32(u32 hostval) {
    spdlog::trace("rd_frombe32({:x})", hostval);
    return redasm::byteorder::from_bigendian(hostval);
}

u64 rd_frombe64(u64 hostval) {
    spdlog::trace("rd_frombe64({:x})", hostval);
    return redasm::byteorder::from_bigendian(hostval);
}

u16 rd_tole16(u16 hostval) {
    spdlog::trace("rd_tole16({:x})", hostval);
    return redasm::byteorder::to_littleendian(hostval);
}

u32 rd_tole32(u32 hostval) {
    spdlog::trace("rd_tole32({:x})", hostval);
    return redasm::byteorder::to_littleendian(hostval);
}

u64 rd_tole64(u64 hostval) {
    spdlog::trace("rd_tole64({:x})", hostval);
    return redasm::byteorder::to_littleendian(hostval);
}

u16 rd_tobe16(u16 hostval) {
    spdlog::trace("rd_tobe16({:x})", hostval);
    return redasm::byteorder::to_bigendian(hostval);
}

u32 rd_tobe32(u32 hostval) {
    spdlog::trace("rd_tobe32({:x})", hostval);
    return redasm::byteorder::to_bigendian(hostval);
}

u64 rd_tobe64(u64 hostval) {
    spdlog::trace("rd_tobe64({:x})", hostval);
    return redasm::byteorder::to_bigendian(hostval);
}
