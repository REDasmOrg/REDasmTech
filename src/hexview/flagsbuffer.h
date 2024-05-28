#pragma once

#include <qhexview/model/buffer/qhexbuffer.h>
#include <redasm/redasm.h>

class FlagsBuffer: public QHexBuffer {
    Q_OBJECT

public:
    explicit FlagsBuffer(QObject* parent = nullptr);
    [[nodiscard]] bool accept(qint64 idx) const override;

    [[nodiscard]] inline quint64 base_address() const {
        return m_meminfo.baseaddress;
    }

public:
    [[nodiscard]] qint64 length() const override;
    void insert(qint64 offset, const QByteArray& data) override;
    void remove(qint64 offset, int length) override;
    QByteArray read(qint64 offset, int length) override;
    bool read(QIODevice* iodevice) override;
    void write(QIODevice* iodevice) override;
    qint64 indexOf(const QByteArray& ba, qint64 from) override;
    qint64 lastIndexOf(const QByteArray& ba, qint64 from) override;

private:
    const RDByte* m_bytes;
    RDMemoryInfo m_meminfo;
    size_t m_size;
};
