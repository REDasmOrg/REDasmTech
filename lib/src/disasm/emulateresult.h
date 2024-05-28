#pragma once

#include <deque>
#include <redasm/types.h>

namespace redasm {

struct EmulateResult {
    // clang-format off
    enum {
        NONE = 0,
        INVALID,
        BRANCH, BRANCH_TRUE, BRANCH_FALSE, BRANCH_TABLE, BRANCH_INDIRECT, BRANCH_UNRESOLVED,
        CALL, CALL_TABLE, CALL_INDIRECT, CALL_UNRESOLVED,
        RETURN, SYSCALL, REF, REF_DATA, REF_STRING, REF_TYPE, REF_TYPENAME, TABLE,
    };
    // clang-format on

    struct Value {
        union {
            uintptr_t v1;
            RDAddress address;
            u64 syscall;
        };

        union {
            uintptr_t v2;
            usize size;
            const char* name;
        };
    };

    using Results = std::deque<std::pair<usize, Value>>;

    Results results;
    RDAddress address;
    usize size{0}, delayslot{0};
    bool canflow{true};
    bool invalid{false};

    explicit EmulateResult(RDAddress addr);
    void add_return();
    void add_branch_unresolved();
    void add_branch_indirect();
    void add_branch(RDAddress addr);
    void add_branch_true(RDAddress addr);
    void add_branch_false(RDAddress addr);
    void add_call(RDAddress addr);
    void add_call_indirect();
    void add_call_unresolved();
    void add_data(RDAddress addr);
    void add_string(RDAddress addr);
    void add_syscall(u64 n);
};

} // namespace redasm
