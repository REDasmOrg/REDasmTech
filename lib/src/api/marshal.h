#pragma once

#include "../database/database.h"
#include "../disasm/function.h"
#include "../memory/byte.h"
#include "../typing/typing.h"
#include <redasm/redasm.h>

namespace redasm {

namespace rdil {

struct ILExpr;
class ILExprPool;
class ILExprList;

} // namespace rdil

class AbstractStream;
class AbstractBuffer;
class Renderer;
class Emulator;
class Context;
class Surface;
class StyledGraph;
struct Function;
struct Segment;

namespace api {

inline Byte from_c(RDByte arg) { return Byte{arg}; }

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

inline const rdil::ILExpr* from_c(const RDILExpr* arg) {
    return reinterpret_cast<const rdil::ILExpr*>(arg);
}

inline Renderer* from_c(RDRenderer* arg) {
    return reinterpret_cast<Renderer*>(arg);
}

inline Emulator* from_c(RDEmulator* arg) {
    return reinterpret_cast<Emulator*>(arg);
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

inline const typing::Value* from_c(const RDValue* arg) {
    return reinterpret_cast<const typing::Value*>(arg);
}

inline typing::Value* from_c(RDValue* arg) {
    return reinterpret_cast<typing::Value*>(arg);
}

inline RDByte to_c(Byte arg) { return arg.value; }

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

inline const RDILExpr* to_c(const rdil::ILExpr* arg) {
    return reinterpret_cast<const RDILExpr*>(arg);
}

inline const RDValue* to_c(const typing::Value* v) {
    return reinterpret_cast<const RDValue*>(v);
}

inline RDValue* to_c(typing::Value* v) { return reinterpret_cast<RDValue*>(v); }

RDSegment to_c(const Segment& arg);
RDRef to_c(const Database::Ref& arg);
std::vector<RDRef> to_c(const std::vector<Database::Ref>& arg);
RDFunctionBasicBlock to_c(const Function::BasicBlock* bb);

} // namespace api

} // namespace redasm
