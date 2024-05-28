#pragma once

#include "abstractstream.h"

namespace redasm {

class Stream: public AbstractStream {
public:
    [[nodiscard]] tl::optional<u8> at(usize idx) const override;
    [[nodiscard]] usize size() const override;

protected:
    [[nodiscard]] const AbstractBuffer& buffer() const override;
};

} // namespace redasm
