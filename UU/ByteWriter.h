//
//  ByteWriter.h
//

#ifndef UU_BYTE_WRITER_H
#define UU_BYTE_WRITER_H

#include <UU/Assertions.h>
#include <UU/Types.h>

#include <string>

namespace UU {

class ByteWriter
{
public:
    constexpr ByteWriter() {}
    virtual ~ByteWriter() {}

    virtual void write(const std::string &) = 0;
    virtual void write(const Byte *, SizeType) = 0;
    virtual void write(Byte) = 0;
    virtual SizeType size() const = 0;
};

}  // namespace UU

#endif  // UU_BYTE_WRITER_H
