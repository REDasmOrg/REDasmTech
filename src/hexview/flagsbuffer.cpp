#include "flagsbuffer.h"

FlagsBuffer::FlagsBuffer(QObject* parent): QHexBuffer{parent} {
    rd_memoryinfo(&m_meminfo);
    m_size = rd_getmemory(&m_bytes);
}

qint64 FlagsBuffer::length() const { return m_size; }

bool FlagsBuffer::accept(qint64 idx) const {
    if(static_cast<quint64>(idx) < m_size) return rdbyte_hasbyte(m_bytes[idx]);
    return false;
}

void FlagsBuffer::insert(qint64 /*offset*/, const QByteArray& /*data*/) {}

void FlagsBuffer::remove(qint64 /*offset*/, int /*length*/) {}

QByteArray FlagsBuffer::read(qint64 offset, int length) {
    QByteArray data;

    for(qint64 i = 0; i < std::min<qint64>(length, m_size); i++) {
        u8 b;

        if(rdbyte_getbyte(m_bytes[offset + i], &b))
            data.push_back(b);
        else
            data.push_back(u8{0});
    }

    return data;
}

bool FlagsBuffer::read(QIODevice* /*iodevice*/) { return false; }

void FlagsBuffer::write(QIODevice* /*iodevice*/) {}

qint64 FlagsBuffer::indexOf(const QByteArray& /*ba*/, qint64 /*from*/) {
    return -1;
}

qint64 FlagsBuffer::lastIndexOf(const QByteArray& /*ba*/, qint64 /*from*/) {
    return -1;
}
