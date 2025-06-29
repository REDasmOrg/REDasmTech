#pragma once

#include "../disasm/function.h"
#include <redasm/redasm.h>
#include <tl/optional.hpp>

namespace redasm {

namespace rdil {

class ILExprPool;
class ILExprList;

} // namespace rdil

namespace signature {

struct SignatureManager;

}

class Renderer;
class Emulator;
class Surface;
class Context;
class StyledGraph;
struct Function;

namespace api {

static inline Context* from_c(RDContext* arg) {
    return reinterpret_cast<Context*>(arg);
}

static inline signature::SignatureManager* from_c(RDSignatureManager* arg) {
    return reinterpret_cast<signature::SignatureManager*>(arg);
}

static inline const StyledGraph* from_c(const RDGraph* arg) {
    return reinterpret_cast<const StyledGraph*>(arg);
}

static inline StyledGraph* from_c(RDGraph* arg) {
    return reinterpret_cast<StyledGraph*>(arg);
}

static inline const Function* from_c(const RDFunction* arg) {
    return reinterpret_cast<const Function*>(arg);
}

static inline Function* from_c(RDFunction* arg) {
    return reinterpret_cast<Function*>(arg);
}

static inline rdil::ILExprPool* from_c(RDILPool* arg) {
    return reinterpret_cast<rdil::ILExprPool*>(arg);
}

static inline const rdil::ILExprList* from_c(const RDILList* arg) {
    return reinterpret_cast<const rdil::ILExprList*>(arg);
}

static inline rdil::ILExprList* from_c(RDILList* arg) {
    return reinterpret_cast<rdil::ILExprList*>(arg);
}

static inline Renderer* from_c(RDRenderer* arg) {
    return reinterpret_cast<Renderer*>(arg);
}

static inline Emulator* from_c(RDEmulator* arg) {
    return reinterpret_cast<Emulator*>(arg);
}

static inline const Emulator* from_c(const RDEmulator* arg) {
    return reinterpret_cast<const Emulator*>(arg);
}

static inline const Surface* from_c(const RDSurface* arg) {
    return reinterpret_cast<const Surface*>(arg);
}

static inline Surface* from_c(RDSurface* arg) {
    return reinterpret_cast<Surface*>(arg);
}

static inline RDContext* to_c(Context* arg) {
    return reinterpret_cast<RDContext*>(arg);
}

static inline RDSignatureManager* to_c(signature::SignatureManager* arg) {
    return reinterpret_cast<RDSignatureManager*>(arg);
}

static inline const RDGraph* to_c(const StyledGraph* arg) {
    return reinterpret_cast<const RDGraph*>(arg);
}

static inline RDGraph* to_c(StyledGraph* arg) {
    return reinterpret_cast<RDGraph*>(arg);
}

static inline RDRenderer* to_c(Renderer* arg) {
    return reinterpret_cast<RDRenderer*>(arg);
}

static inline RDEmulator* to_c(Emulator* arg) {
    return reinterpret_cast<RDEmulator*>(arg);
}

static inline const RDEmulator* to_c(const Emulator* arg) {
    return reinterpret_cast<const RDEmulator*>(arg);
}

static inline RDSurface* to_c(Surface* arg) {
    return reinterpret_cast<RDSurface*>(arg);
}

static inline RDFunction* to_c(Function* arg) {
    return reinterpret_cast<RDFunction*>(arg);
}

static inline const RDFunction* to_c(const Function* arg) {
    return reinterpret_cast<const RDFunction*>(arg);
}

static inline RDILPool* to_c(rdil::ILExprPool* arg) {
    return reinterpret_cast<RDILPool*>(arg);
}

static inline RDILList* to_c(rdil::ILExprList* arg) {
    return reinterpret_cast<RDILList*>(arg);
}

static inline RDRegValue to_c(const tl::optional<u64>& arg) {
    if(!arg) return RDRegValue_none();
    return RDRegValue_some(arg.value());
}

} // namespace api

} // namespace redasm
