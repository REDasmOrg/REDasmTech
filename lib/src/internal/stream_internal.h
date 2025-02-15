#pragma once

#include <redasm/buffer.h>
#include <redasm/types.h>

struct RDStream {
    RDBuffer* buffer;
    usize position;
};
