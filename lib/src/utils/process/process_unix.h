#pragma once

#include <array>
#include <functional>
#include <initializer_list>
#include <string>
#include <string_view>
#include <sys/types.h>
#include <thread>

namespace redasm::process::impl {

class Process {
    using StdoutCallback = std::function<void(const std::string&)>;
    using StderrCallback = std::function<void(const std::string&)>;

public:
    using Pid = pid_t;

public:
    Process() = default;
    ~Process();
    [[nodiscard]] inline Pid pid() const { return m_pid; }
    [[nodiscard]] std::string read_stdout() const;
    [[nodiscard]] std::string read_stderr() const;
    void write(std::string_view arg);
    [[nodiscard]] int wait() const;
    void kill();

    void start(const std::string_view& cmd,
               std::initializer_list<std::string_view> args = {});

private:
    [[nodiscard]] std::string read_pipe(int pipe) const;
    void async_reader() const;

public:
    StdoutCallback on_stdout;
    StderrCallback on_stderr;

private:
    Pid m_pid{};
    std::thread m_asyncread;
    std::array<int, 2> m_stdinpipe;
    std::array<int, 2> m_stdoutpipe;
    std::array<int, 2> m_stderrpipe;
};

} // namespace redasm::process::impl
