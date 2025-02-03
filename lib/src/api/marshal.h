#pragma once

#include "../disasm/function.h"
#include <redasm/redasm.h>

namespace redasm {

namespace rdil {

class ILExprPool;
class ILExprList;

} // namespace rdil

class Renderer;
class Emulator;
class Context;
class Surface;
class StyledGraph;
struct Function;

namespace api {

inline const StyledGraph* from_c(const RDGraph* arg) {
    return reinterpret_cast<const StyledGraph*>(arg);
}

inline StyledGraph* from_c(RDGraph* arg) {
    return reinterpret_cast<StyledGraph*>(arg);
}

inline const Function* from_c(const RDFunction* arg) {
    return reinterpret_cast<const Function*>(arg);
}

inline Function* from_c(RDFunction* arg) {
    return reinterpret_cast<Function*>(arg);
}

inline rdil::ILExprPool* from_c(RDILPool* arg) {
    return reinterpret_cast<rdil::ILExprPool*>(arg);
}

inline const rdil::ILExprList* from_c(const RDILList* arg) {
    return reinterpret_cast<const rdil::ILExprList*>(arg);
}

inline rdil::ILExprList* from_c(RDILList* arg) {
    return reinterpret_cast<rdil::ILExprList*>(arg);
}

inline Renderer* from_c(RDRenderer* arg) {
    return reinterpret_cast<Renderer*>(arg);
}

inline Emulator* from_c(RDEmulator* arg) {
    return reinterpret_cast<Emulator*>(arg);
}

inline const Emulator* from_c(const RDEmulator* arg) {
    return reinterpret_cast<const Emulator*>(arg);
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

inline const RDGraph* to_c(const StyledGraph* arg) {
    return reinterpret_cast<const RDGraph*>(arg);
}

inline RDGraph* to_c(StyledGraph* arg) {
    return reinterpret_cast<RDGraph*>(arg);
}

inline RDRenderer* to_c(Renderer* arg) {
    return reinterpret_cast<RDRenderer*>(arg);
}

inline RDEmulator* to_c(Emulator* arg) {
    return reinterpret_cast<RDEmulator*>(arg);
}

inline RDSurface* to_c(Surface* arg) {
    return reinterpret_cast<RDSurface*>(arg);
}

inline RDContext* to_c(Context* arg) {
    return reinterpret_cast<RDContext*>(arg);
}

inline RDFunction* to_c(Function* arg) {
    return reinterpret_cast<RDFunction*>(arg);
}

inline const RDFunction* to_c(const Function* arg) {
    return reinterpret_cast<const RDFunction*>(arg);
}

inline RDILPool* to_c(rdil::ILExprPool* arg) {
    return reinterpret_cast<RDILPool*>(arg);
}

inline RDILList* to_c(rdil::ILExprList* arg) {
    return reinterpret_cast<RDILList*>(arg);
}

RDFunctionBasicBlock to_c(const Function::BasicBlock* bb);

} // namespace api

} // namespace redasm
