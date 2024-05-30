#pragma once

#include "../memory/byte.h"
#include <redasm/redasm.h>

namespace redasm {

struct EmulateResult;
class AbstractStream;
class AbstractBuffer;
class Renderer;
class Context;
class Surface;

namespace api {

inline Byte from_c(RDByte arg) { return Byte{arg}; }

inline const EmulateResult* from_c(const RDEmulateResult* arg) {
    return reinterpret_cast<const EmulateResult*>(arg);
}

inline EmulateResult* from_c(RDEmulateResult* arg) {
    return reinterpret_cast<EmulateResult*>(arg);
}

inline Renderer* from_c(RDRenderer* arg) {
    return reinterpret_cast<Renderer*>(arg);
}

inline const AbstractStream* from_c(const RDStream* arg) {
    return reinterpret_cast<const AbstractStream*>(arg);
}

inline AbstractStream* from_c(RDStream* arg) {
    return reinterpret_cast<AbstractStream*>(arg);
}

inline const Surface* from_c(const RDSurface* arg) {
    return reinterpret_cast<const Surface*>(arg);
}

inline Surface* from_c(RDSurface* arg) {
    return reinterpret_cast<Surface*>(arg);
}

inline Context* from_c(RDContext* arg) {
    return reinterpret_cast<Context*>(arg);
}

inline const AbstractBuffer* from_c(const RDBuffer* arg) {
    return reinterpret_cast<const AbstractBuffer*>(arg);
}

inline AbstractBuffer* from_c(RDBuffer* arg) {
    return reinterpret_cast<AbstractBuffer*>(arg);
}

inline RDEmulateResult* from_c(EmulateResult* arg) {
    return reinterpret_cast<RDEmulateResult*>(arg);
}

inline RDByte to_c(Byte arg) { return arg.value; }

inline RDRenderer* to_c(Renderer* arg) {
    return reinterpret_cast<RDRenderer*>(arg);
}

inline RDStream* to_c(AbstractStream* arg) {
    return reinterpret_cast<RDStream*>(arg);
}

inline RDSurface* to_c(Surface* arg) {
    return reinterpret_cast<RDSurface*>(arg);
}

inline RDContext* to_c(Context* arg) {
    return reinterpret_cast<RDContext*>(arg);
}

inline RDBuffer* to_c(AbstractBuffer* arg) {
    return reinterpret_cast<RDBuffer*>(arg);
}

inline RDEmulateResult* to_c(EmulateResult* arg) {
    return reinterpret_cast<RDEmulateResult*>(arg);
}

} // namespace api

} // namespace redasm
