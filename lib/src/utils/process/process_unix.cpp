#include "process_unix.h"
#include <fcntl.h>
#include <fmt/core.h>
#include <redasm/ct.h>
#include <sys/wait.h>
#include <unistd.h>

// https://stackoverflow.com/questions/10700982/redirecting-stdout-to-pipe-in-c

namespace redasm::process::impl {

namespace {

constexpr size_t BUFFER_SIZE = 4096;

}

Process::~Process() {
    m_pid = 0;
    if(m_asyncread.joinable()) m_asyncread.join();

    int res = ::close(m_stdoutpipe[0]);
    if(res != -1) res = ::close(m_stderrpipe[0]);
    if(res != -1) res = ::close(m_stdinpipe[1]);
    if(res == -1) ct_exceptf("Pipe close failed, error code: %d", errno);
}

void Process::start(std::string_view cmd,
                    std::initializer_list<std::string_view> args) {
    if(m_pid) return;

    int res = ::pipe(m_stdoutpipe.data());
    if(!res) res = ::pipe(m_stderrpipe.data());
    if(!res) res = ::pipe(m_stdinpipe.data());

    if(res == -1) ct_exceptf("Cannot create pipe, error code: %d", errno);

    m_pid = ::fork();

    switch(m_pid) {
        case -1:
            ct_exceptf("Cannot fork process, error code: %d", errno);
            break;

        case 0: { // Child - Reads from Pipe
            res = ::close(m_stdoutpipe[0]);
            if(res != -1) res = ::close(m_stderrpipe[0]);
            if(res != -1) res = ::close(m_stdinpipe[1]);
            if(res == -1)
                ct_exceptf("Pipe close failed (child), error code: %d", errno);

            res = ::dup2(m_stdoutpipe[1], STDOUT_FILENO);
            if(res != -1) res = ::dup2(m_stderrpipe[1], STDERR_FILENO);
            if(res != -1) res = ::dup2(m_stdinpipe[0], STDIN_FILENO);

            if(res == -1)
                ct_exceptf("Cannot duplicate pipe, error code: %d", errno);

            std::vector<char*> argv;
            argv.reserve(args.size() + 2);
            argv.push_back(const_cast<char*>(cmd.data()));

            for(const std::string_view& arg : args)
                argv.push_back(const_cast<char*>(arg.data()));

            argv.push_back(nullptr);

            // FIXME: res = ::execvp(cmd.data(), argv.data());
            // FIXME: if(res == -1)
            // FIXME: ct_exceptf("Cannot execute '%d', errocode: {}", errno);
            ct_unreachable;
        }

        default: { // Parent - Writes to Pipe
            res = ::close(m_stdoutpipe[1]);
            if(res != -1) res = ::close(m_stderrpipe[1]);
            if(res != -1) res = ::close(m_stdinpipe[0]);

            if(res == -1)
                ct_exceptf("Pipe close failed (parent), error code: %d", errno);

            // Set the read end of the pipe to non-blocking mode
            int flags = ::fcntl(m_stdoutpipe[0], F_GETFL, 0);
            res = ::fcntl(m_stdoutpipe[0], F_SETFL, flags | O_NONBLOCK);
            if(res == -1) {
                ct_exceptf("Cannot change stdout pipe flags, error code %d",
                           errno);
            }

            flags = ::fcntl(m_stderrpipe[0], F_GETFL, 0);
            ::fcntl(m_stderrpipe[0], F_SETFL, flags | O_NONBLOCK);
            if(res == -1) {
                ct_exceptf("Cannot change stderr pipe flags, error code: %d",
                           errno);
            }

            if(this->on_stdout || this->on_stderr)
                m_asyncread = std::thread{&Process::async_reader, this};
            break;
        }
    }
}

void Process::write(std::string_view arg) {
    if(!m_pid) ct_except("Process is not running");

    int res = ::write(m_stdinpipe[1], arg.data(), arg.size());
    if(res == -1) ct_exceptf("Write operation failed, error code: %d", errno);
}

int Process::wait() const {
    if(!m_pid) return EXIT_FAILURE;

    int status = -1;
    if(::waitpid(m_pid, &status, 0) == -1) return EXIT_FAILURE;
    if(WIFEXITED(status)) return WEXITSTATUS(status);
    ct_unreachable;
}

void Process::kill() {
    if(m_pid) ::kill(m_pid, SIGKILL);
    m_pid = 0;
    ::close(m_stdoutpipe[0]);
    ::close(m_stderrpipe[0]);
}

std::string Process::read_stdout() const {
    if(!m_pid) ct_except("Process is not running");
    return this->read_pipe(m_stdoutpipe[0]);
}

std::string Process::read_stderr() const {
    if(!m_pid) ct_except("Process is not running");
    return this->read_pipe(m_stderrpipe[0]);
}

std::string Process::read_pipe(int pipe) const {
    fd_set rfds;
    FD_ZERO(&rfds);
    FD_SET(pipe, &rfds);

    struct timeval tv;
    tv.tv_sec = 1;
    tv.tv_usec = 0;

    int ready = ::select(pipe + 1, &rfds, nullptr, nullptr, &tv);
    if(!ready) return {};

    if(ready > 0) {
        std::array<char, BUFFER_SIZE> buf;
        std::string res;
        ssize_t r;

        while((r = ::read(pipe, buf.data(), buf.size())) > 0)
            res.append(buf.data(), r);

        if(r == -1 && errno != EAGAIN)
            ct_exceptf("Read operation failed, error code: %d", errno);

        return res;
    }

    if(ready == -1) ct_exceptf("Pipe select failed, error code: %d", errno);
    ct_unreachable;
}

void Process::async_reader() const {
    while(m_pid) {
        if(this->on_stdout) {
            std::string s = this->read_stdout();
            if(!s.empty()) this->on_stdout(s);
        }

        if(this->on_stderr) {
            std::string s = this->read_stderr();
            if(!s.empty()) this->on_stderr(s);
        }
    }
}

} // namespace redasm::process::impl
