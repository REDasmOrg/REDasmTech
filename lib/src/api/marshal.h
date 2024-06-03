#pragma once

#include "../memory/byte.h"
#include <redasm/redasm.h>

namespace redasm {

class AbstractStream;
class AbstractBuffer;
class Renderer;
class Emulator;
class Context;
class Surface;
struct Segment;

namespace api {

inline Byte from_c(RDByte arg) { return Byte{arg}; }

inline Emulator* from_c(RDEmulator* arg) {
    return reinterpret_cast<Emulator*>(arg);
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

inline RDByte to_c(Byte arg) { return arg.value; }

inline RDEmulator* to_c(Emulator* arg) {
    return reinterpret_cast<RDEmulator*>(arg);
}

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

RDSegment to_c(const Segment& arg);

} // namespace api

} // namespace redasm
