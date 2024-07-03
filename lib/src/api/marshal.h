#pragma once

#include "../database/database.h"
#include "../disasm/function.h"
#include "../memory/byte.h"
#include <redasm/redasm.h>

namespace redasm {

namespace rdil {

struct ILExpression;
class ILExpressionPool;
class ILExpressionList;

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

inline rdil::ILExpressionPool* from_c(RDILExpressionPool* arg) {
    return reinterpret_cast<rdil::ILExpressionPool*>(arg);
}

inline const rdil::ILExpressionList* from_c(const RDILExpressionList* arg) {
    return reinterpret_cast<const rdil::ILExpressionList*>(arg);
}

inline rdil::ILExpressionList* from_c(RDILExpressionList* arg) {
    return reinterpret_cast<rdil::ILExpressionList*>(arg);
}

inline const rdil::ILExpression* from_c(const RDILExpression* arg) {
    return reinterpret_cast<const rdil::ILExpression*>(arg);
}

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

inline const RDGraph* to_c(const StyledGraph* arg) {
    return reinterpret_cast<const RDGraph*>(arg);
}

inline RDGraph* to_c(StyledGraph* arg) {
    return reinterpret_cast<RDGraph*>(arg);
}

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

inline RDFunction* to_c(Function* arg) {
    return reinterpret_cast<RDFunction*>(arg);
}

inline const RDFunction* to_c(const Function* arg) {
    return reinterpret_cast<const RDFunction*>(arg);
}

inline RDILExpressionPool* to_c(rdil::ILExpressionPool* arg) {
    return reinterpret_cast<RDILExpressionPool*>(arg);
}

inline RDILExpressionList* to_c(rdil::ILExpressionList* arg) {
    return reinterpret_cast<RDILExpressionList*>(arg);
}

inline const RDILExpression* to_c(const rdil::ILExpression* arg) {
    return reinterpret_cast<const RDILExpression*>(arg);
}

RDSegment to_c(const Segment& arg);
RDRef to_c(const AddressDetail::Ref& arg);
std::vector<RDRef> to_c(const std::vector<AddressDetail::Ref>& arg);
RDFunctionBasicBlock to_c(const Function::BasicBlock* bb);

} // namespace api

} // namespace redasm
