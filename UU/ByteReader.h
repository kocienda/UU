//
//  ByteReader.h
//

#ifndef UU_BYTE_READER_H
#define UU_BYTE_READER_H

#include <UU/Assertions.h>
#include <UU/Types.h>

namespace UU {

class ByteReader
{
public:
    constexpr ByteReader() {}
    virtual ~ByteReader() {}

    virtual Byte *bytes() const = 0;
    virtual SizeType size() const = 0;
};

}  // namespace UU

#endif  // UU_BYTE_READER_H
