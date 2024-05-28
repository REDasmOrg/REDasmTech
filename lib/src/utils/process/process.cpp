#include "process.h"

namespace redasm::process {

std::pair<int, std::string> run(std::string_view cmd,
                                std::initializer_list<std::string_view> args) {

    Process p;
    p.start(cmd, args);

    std::string out = p.read_stdout();
    int code = p.wait();

    if(!code)
        return {code, out};
    return {code, p.read_stderr()};
}

int exec(std::string_view cmd, std::initializer_list<std::string_view> args) {
    Process p;
    p.start(cmd, args);
    return p.wait();
}

} // namespace redasm::process
