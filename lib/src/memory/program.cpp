#include "program.h"

namespace redasm {

Program::~Program() {
    for(const Segment& s : this->segments)
        delete[] s.name;
}

} // namespace redasm
