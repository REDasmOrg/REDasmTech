#pragma once

#include "abstractstream.h"

namespace redasm {

class MemoryStream: public AbstractStream {
public:
    [[nodiscard]] tl::optional<u8> at(usize absidx) const override;
    [[nodiscard]] usize size() const override;

protected:
    [[nodiscard]] const AbstractBuffer& buffer() const override;
    [[nodiscard]] usize index_base() const override;
};

} // namespace redasm
