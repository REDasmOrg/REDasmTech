#include "utils.h"
#include <climits>

namespace utils {

QString to_hex(size_t n) {
    const int N_CHARS = (utils::bits / CHAR_BIT) * 2;
    return QStringLiteral("%1").arg(n, N_CHARS, 16, QLatin1Char('0')).toUpper();
}

} // namespace utils
