#include "emulateresult.h"

namespace redasm {

EmulateResult::EmulateResult(RDAddress addr): address{addr} {}

void EmulateResult::add_return() {
    this->canflow = false;
    this->results.emplace_back(EmulateResult::RETURN, Value{});
}

void EmulateResult::add_branch_unresolved() {
    this->canflow = false;
    this->results.emplace_back(EmulateResult::BRANCH_UNRESOLVED, Value{});
}

void EmulateResult::add_branch_indirect() {
    this->canflow = false;
    this->results.emplace_back(EmulateResult::BRANCH_INDIRECT, Value{});
}

void EmulateResult::add_branch(RDAddress addr) {
    this->canflow = false;
    this->results.emplace_back(EmulateResult::BRANCH, Value{{addr}, {}});
}

void EmulateResult::add_branch_true(RDAddress addr) {
    this->canflow = false;
    this->results.emplace_back(EmulateResult::BRANCH_TRUE, Value{{addr}, {}});
}

void EmulateResult::add_branch_false(RDAddress addr) {
    this->canflow = false;
    this->results.emplace_back(EmulateResult::BRANCH_FALSE, Value{{addr}, {}});
}

void EmulateResult::add_call(RDAddress addr) {
    this->results.emplace_back(EmulateResult::CALL, Value{{addr}, {}});
}

void EmulateResult::add_call_indirect() {
    this->results.emplace_back(EmulateResult::CALL_INDIRECT, Value{});
}

void EmulateResult::add_call_unresolved() {
    this->results.emplace_back(EmulateResult::CALL_UNRESOLVED, Value{});
}

void EmulateResult::add_data(RDAddress addr) {
    this->results.emplace_back(EmulateResult::REF_DATA, Value{{addr}, {}});
}

void EmulateResult::add_string(RDAddress addr) {
    this->results.emplace_back(EmulateResult::REF_STRING, Value{{addr}, {}});
}

void EmulateResult::add_syscall(u64 n) {
    this->results.emplace_back(EmulateResult::SYSCALL, Value{{n}, {}});
}

} // namespace redasm
