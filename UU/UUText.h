//
// UUText.h
//

#ifndef UU_TEXT_H
#define UU_TEXT_H

#include <concepts>
#include <cstring>
#include <format>
#include <memory>
#include <string>
#include <type_traits>

#include <UU/Assertions.h>
#include <UU/Compiler.h>
#include <UU/StaticByteBuffer.h>
#include <UU/Storage.h>
#include <UU/Stretch.h>
#include <UU/Types.h>

namespace UU {

namespace TextEncoding {

enum class Form {
    BASIC8 = 100,
    UTF8 = 1000,
    UTF16 = 1001,
    UTF32 = 1002,
};

template <Form FormV, typename CodePointT> requires IsCharType<CodePointT>
struct Attributes {
    static constexpr Size max_encoded_length = 0;
    static constexpr CodePointT sentinel_value = 0;
    static constexpr Size sentinel_bytes_length = 0;
    static constexpr StaticByteBuffer<sentinel_bytes_length> sentinel_bytes;
    static constexpr Size bom_bytes_length = 0;
    static constexpr StaticByteBuffer<bom_bytes_length> bom_bytes;
};

template <typename CodePointT>
struct Attributes<Form::UTF8, CodePointT> {
    static constexpr Size max_encoded_length = 4;
    static constexpr CodePointT sentinel_value = 0xFFFD;
    static constexpr Size sentinel_bytes_length = max_encoded_length;
    static constexpr StaticByteBuffer<sentinel_bytes_length> sentinel_bytes = { 
        (sentinel_value & 0xFF00) >> 8, (sentinel_value & 0xFF), 0, 0
    };
    static constexpr Size bom_bytes_length = 3;
    static constexpr StaticByteBuffer<bom_bytes_length> bom_bytes = { 0xEF, 0xBB, 0xBF };
};

template <Form FormV, typename CodePointT> requires IsCharType<CodePointT>
struct DecodeResult {
    using Attrs = Attributes<FormV, CodePointT>;
    UU_ALWAYS_INLINE constexpr bool is_ok() noexcept { 
        return code_point != Attrs::sentinel_value(); 
    }
    CodePointT code_point;
    Size advance;
};

template <Form FormV, typename CodePointT> requires IsCharType<CodePointT>
struct EncodeResult {
    using Attrs = Attributes<FormV, CodePointT>;
    using ByteBuffer = StaticByteBuffer<Attrs::max_encoded_length>;
    UU_ALWAYS_INLINE constexpr bool is_ok() noexcept { return length > 0; }
    ByteBuffer bytes = {};
    Size length = 0;
};

struct WellFormedResult {
    UU_ALWAYS_INLINE constexpr bool is_ok() noexcept { return count == bpos; }
    Size count;
    Size bpos;
};

template <Form F, typename CT, typename CP> 
requires IsCharType<CT> && IsCharType<CP>
struct Traits {
    static constexpr Form FormV = F;
    using CharT = CT;
    using CodePointT = CP;
    using CharTraits = std::char_traits<CharT>;
    using Attrs = Attributes<FormV, CodePointT>;
    using DecodeResult = DecodeResult<FormV, CodePointT>;
    using BOMT = StaticByteBuffer<Attrs::bom_bytes_length>;
    using EncodeResult = EncodeResult<FormV, CodePointT>;

    static constexpr CodePointT Sentinel = Attrs::sentinel_value;
    static constexpr BOMT BOM = Attrs::bom_bytes;
    static constexpr Size npos = SizeMax;

    static constexpr bool is_single(CodePointT c) noexcept { return true; }
    static constexpr bool not_single(CodePointT c) noexcept { return !(is_single(c)); }
    static constexpr bool is_lead(CodePointT c) noexcept { return false; }
    static constexpr bool not_lead(CodePointT c) noexcept { return !(is_lead(c)); }
    static constexpr bool is_trail(CodePointT c) noexcept { return false; }
    static constexpr bool not_trail(CodePointT c) noexcept { return !(is_trail(c)); }

    static constexpr DecodeResult decode(const CharT *ptr, Size len, Size bpos) noexcept { 
        return { Sentinel, 0 };
    }

    static constexpr DecodeResult check_decode(const CharT *ptr, Size len, Size bpos) noexcept {
        return decode(ptr, len, bpos);
    }

    static constexpr DecodeResult decode_bom(const CharT *ptr, Size len) noexcept {
        return { 0, 0 };
    }

    static constexpr bool is_well_formed(const CharT *ptr, Size bpos) noexcept { return true; }

    static constexpr EncodeResult encode(CodePointT code_point) noexcept { 
        return EncodeResult(); 
    }
    
    static constexpr EncodeResult check_encode(CodePointT code_point) noexcept { 
        return EncodeResult(); 
    }
};

template <> struct Traits<Form::UTF8, Char8, Char32> {
    static constexpr Form FormV = Form::UTF8;
    using CharT = Char8;
    using CodePointT = Char32;
    using CharTraits = std::char_traits<CharT>;
    using Attrs = Attributes<FormV, CodePointT>;
    using DecodeResult = DecodeResult<FormV, CodePointT>;
    using BOMT = StaticByteBuffer<Attrs::bom_bytes_length>;
    using EncodeResult = EncodeResult<FormV, CodePointT>;

    static constexpr CodePointT Sentinel = Attrs::sentinel_value;
    static constexpr BOMT BOM = Attrs::bom_bytes;
    static constexpr Size npos = SizeMax;

    static constexpr bool is_single(CodePointT c) noexcept { return ((c & 0x80) == 0); }
    static constexpr bool not_single(CodePointT c) noexcept { return !(is_single(c)); }
    static constexpr bool is_lead(CodePointT c) noexcept { return (c - 0xc2 <= 0x32); }
    static constexpr bool not_lead(CodePointT c) noexcept { return !(is_lead(c)); }
    static constexpr bool is_trail(CodePointT c) noexcept { return (Int8(c) < -0x40); }
    static constexpr bool not_trail(CodePointT c) noexcept { return !(is_trail(c)); }

    UU_ALWAYS_INLINE
    static constexpr CodePointT decode_two_byte_code_point(CodePointT b1, CodePointT b2) noexcept {
        // Code point value: 00000yyy yyxxxxxx
        // First byte mask:  110yyyyy
        // Second byte mask: 10xxxxxx
        CodePointT s1 = ((b1 & 0b00011100) >> 2);
        CodePointT s2 = ((b1 & 0b00000011) << 6) | (b2 & 0b00111111);
        return (s1 << 8) | s2;
    }

    UU_ALWAYS_INLINE
    static constexpr CodePointT decode_three_byte_code_point(CodePointT b1, CodePointT b2, CodePointT b3) noexcept {
        // Code point value: zzzzyyyy yyxxxxxx
        // First byte mask:  1110zzzz
        // Second byte mask: 10yyyyyy
        // Third byte mask:  10xxxxxx
        CodePointT s1 = ((b1 & 0b00001111) << 4) | ((b2 & 0b00111100) >> 2);
        CodePointT s2 = ((b2 & 0b00000011) << 6) | ((b3 & 0b00111111));
        return (s1 << 8) | s2;
    }

    UU_ALWAYS_INLINE
    static constexpr CodePointT decode_four_byte_code_point(CodePointT b1, CodePointT b2, CodePointT b3, CodePointT b4) noexcept {
        // Code point value: 000uuuuu zzzzyyyy yyxxxxxx
        // First byte mask:  11110uuu
        // Second byte mask: 10uuzzzz
        // Third byte mask:  10yyyyyy
        // Fourth byte mask: 10xxxxxx
        CodePointT s1 = ((b1 & 0b00000111) << 2) | ((b2 & 0b00110000) >> 4);
        CodePointT s2 = ((b2 & 0b00001111) << 4) | ((b3 & 0b00111100) >> 2);
        CodePointT s3 = ((b3 & 0b00000011) << 6) | ((b4 & 0b00111111));
        return (s1 << 16) | (s2 << 8) | s3;
    }

    static constexpr DecodeResult decode_bom(const CharT *ptr, Size len) noexcept {
        if (UNLIKELY(len < Attrs::bom_bytes_length)) {
            return { 0, 0 };
        }
        const Byte *byte_ptr = reinterpret_cast<const Byte *>(ptr);
        BOMT maybe_bom(byte_ptr, Attrs::bom_bytes_length);
        if (maybe_bom == BOM) {
            return { 0, Attrs::bom_bytes_length };
        }
        else {
            return { 0, 0 };
        }
    }

    static constexpr DecodeResult decode(const CharT *ptr, Size len, Size bpos) noexcept { 
        Size bmax = len - bpos - 1;
        if (LIKELY(bmax < len)) {
            bmax = std::min(bmax + 1, 4UL);
        }
        else {
            return { Sentinel, 0 };
        }

        Byte first = ptr[bpos];
        
        // one-byte code point
        if (is_single(first)) {
            return { first, 1 };
        }
        ASSERT(is_lead(first));

        // peek ahead for a non-trail byte
        switch (bmax) {
            case 1: {
                ASSERT(false);
                return { Sentinel, 0 };
            }
            case 2: {
                Byte second = ptr[bpos + 1];
                ASSERT(is_trail(second));
                return { decode_two_byte_code_point(first, second), 2 };
            }
            case 3: {
                Byte second = ptr[bpos + 1];
                ASSERT(is_trail(second));
                Byte third = ptr[bpos + 2];
                if (not_trail(third)) {
                    return { decode_two_byte_code_point(first, second), 2 };
                }
                else {
                    return { decode_three_byte_code_point(first, second, third), 3 };
                }
            }
            default: {
                Byte second = ptr[bpos + 1];
                ASSERT(is_trail(second));
                Byte third = ptr[bpos + 2];
                if (not_trail(third)) {
                    return { decode_two_byte_code_point(first, second), 2 };
                }
                Byte fourth = ptr[bpos + 3];
                if (not_trail(fourth)) {
                    return { decode_three_byte_code_point(first, second, third), 3 };
                }
                else {
                    return { decode_four_byte_code_point(first, second, third, fourth), 4 };
                }
            }
        }
    }

    static constexpr DecodeResult check_decode(const CharT *ptr, Size len, Size bpos) noexcept { 
        using BStretch = Stretch<Byte>;

        Size idx = bpos;

        if (UNLIKELY(idx >= len)) {
            LOG(General, "check_decode [1]: bad length");
            ASSERT(false); 
            return { Sentinel, 0 };
        }
        CodePointT b1 = ptr[idx];
        
        // one-byte code point
        if (is_single(b1)) {
            LOG(General, "check_decode [2]: one byte code point");
            return { b1, 1 };
        }

        // two-byte code point
        idx++;
        if (UNLIKELY(idx == len)) {
            LOG(General, "check_decode [3]: bad length");
            ASSERT(false); 
            return { Sentinel, 0 };
        }
        CodePointT b2 = ptr[idx];

        // Code Points:  U+0080..U+07FF
        // First Byte:   C2..DF
        // Second Byte:  80..BF
        if (BStretch::contains(b1, 0xC2, 0xDF)) {
            if (LIKELY(BStretch::contains(b2, 0x80, 0xBF))) {
                LOG(General, "check_decode [4]: two byte code point");
                return { decode_two_byte_code_point(b1, b2), 2 };
            }
            else {
                LOG(General, "check_decode [5]: bad two byte code point");
                return { Sentinel, 0 };
            }
        }

        // three-byte code point
        idx++;
        if (UNLIKELY(idx == len)) {
            LOG(General, "check_decode [6]: bad length");
            ASSERT(false); 
            return { Sentinel, 0 };
        }
        CodePointT b3 = ptr[idx];

        // Code Points:  U+0800..U+0FFF
        // First Byte:   E0
        // Second Byte:  A0..BF
        // Third Byte:   80..BF
        // or
        // Code Points:  U+1000..U+CFFF
        // First Byte:   E1..EC
        // Second Byte:  80..BF
        // Third Byte:   80..BF
        // or
        // Code Points:  U+D000..U+D7FF
        // First Byte:   ED
        // Second Byte:  80..9F
        // Third Byte:   80..BF
        // or
        // Code Points:  U+E000..U+FFFF
        // First Byte:   EE..EF
        // Second Byte:  80..BF
        // Third Byte:   80..BF
        if (b1 == 0xE0) {
            if (LIKELY(BStretch::contains(b2, 0xA0, 0xBF) && BStretch::contains(b3, 0x80, 0xBF))) {
                LOG(General, "check_decode [7]: three byte code point");
                return { decode_three_byte_code_point(b1, b2, b3), 3 };
            }
            else {
                LOG(General, "check_decode [8]: bad three byte code point");
                return { Sentinel, 0 };
            }
        }
        if (BStretch::contains(b1, 0xE1, 0xEC)) {
            if (LIKELY(BStretch::contains(b2, 0x80, 0xBF) && BStretch::contains(b3, 0x80, 0xBF))) {
                LOG(General, "check_decode [9]: three byte code point");
                return { decode_three_byte_code_point(b1, b2, b3), 3 };
            }
            else {
                LOG(General, "check_decode [10]: bad three byte code point");
                return { Sentinel, 0 };
            }
        }
        if (b1 == 0xED) {
            if (LIKELY(BStretch::contains(b2, 0x80, 0x9F) && BStretch::contains(b3, 0x80, 0xBF))) {
                LOG(General, "check_decode [11]: three byte code point");
                return { decode_three_byte_code_point(b1, b2, b3), 3 };
            }
            else {
                LOG(General, "check_decode [12]: bad three byte code point");
                return { Sentinel, 0 };
            }
        }
        if (BStretch::contains(b1, 0xEE, 0xEF)) {
            if (LIKELY(BStretch::contains(b2, 0x80, 0xBF) && BStretch::contains(b3, 0x80, 0xBF))) {
                LOG(General, "check_decode [13]: three byte code point");
                return { decode_three_byte_code_point(b1, b2, b3), 3 };
            }
            else {
                LOG(General, "check_decode [14]: bad three byte code point");
                return { Sentinel, 0 };
            }
        }

        // four-byte code point
        idx++;
        if (UNLIKELY(idx == len)) {
            LOG(General, "check_decode [15]: bad length");
            ASSERT(false);
            return { Sentinel, 0 };
        }
        CodePointT b4 = ptr[idx];

        // Code Points:  U+10000..U+3FFFF
        // First Byte:   F0
        // Second Byte:  90..BF
        // Third Byte:   80..BF
        // Fourth Byte:  80..BF
        // or
        // Code Points:  U+40000..U+FFFFF
        // First Byte:   F1..F3
        // Second Byte:  80..BF
        // Third Byte:   80..BF
        // Fourth Byte:  80..BF
        // or
        // Code Points:  U+100000..U+10FFFF
        // First Byte:   F4
        // Second Byte:  80..8F
        // Third Byte:   80..BF
        // Fourth Byte:  80..BF
        if (b1 == 0xF0) {
            if (LIKELY(BStretch::contains(b2, 0x90, 0xBF) && 
                       BStretch::contains(b3, 0x80, 0xBF) && 
                       BStretch::contains(b4, 0x80, 0xBF))) {
                LOG(General, "check_decode [16]: four byte code point");
                return { decode_four_byte_code_point(b1, b2, b3, b4), 4 };
            }
            else {
                LOG(General, "check_decode [17]: bad three byte code point");
                return { Sentinel, 0 };
            }
        }
        if (BStretch::contains(b1, 0xF1, 0xF3)) {
            if (LIKELY(BStretch::contains(b2, 0x80, 0xBF) && 
                       BStretch::contains(b3, 0x80, 0xBF) && 
                       BStretch::contains(b4, 0x80, 0xBF))) {
                LOG(General, "check_decode [18]: four byte code point");
                return { decode_four_byte_code_point(b1, b2, b3, b4), 4 };
            }
            else {
                LOG(General, "check_decode [19]: bad three byte code point");
                return { Sentinel, 0 };
            }
        }
        if (b1 == 0xF4) {
            if (LIKELY(BStretch::contains(b2, 0x80, 0x8F) && 
                       BStretch::contains(b3, 0x80, 0xBF) && 
                       BStretch::contains(b4, 0x80, 0xBF))) {
                LOG(General, "check_decode [20]: four byte code point");
                return { decode_four_byte_code_point(b1, b2, b3, b4), 4 };
            }
            else {
                LOG(General, "check_decode [21]: bad three byte code point");
                return { Sentinel, 0 };
            }
        }

        LOG(General, "check_decode [22]: fail");
        return { Sentinel, 0 };
    }

    static WellFormedResult is_well_formed(const CharT *ptr, Size len, Size count = npos) noexcept {
        Size ecount = (count == npos) ? len : std::min(len, count);
        Size bpos = decode_bom(ptr, len).advance;
        while (bpos < ecount) {
            auto r = check_decode(ptr, len, bpos);
            if (r.code_point == Sentinel) {
                break;
            }
            bpos += r.advance;
        }
        return { ecount, bpos };
    }

    UU_ALWAYS_INLINE
    static constexpr bool is_one_byte_code_point(CodePointT code_point) noexcept { 
        // Code Points: U+0000..U+007F
        return code_point <= 0x7F; 
    }
    
    UU_ALWAYS_INLINE
    static constexpr bool is_two_byte_code_point(CodePointT code_point) noexcept {
        // Code Points: U+0080..U+07FF
        using CPStretch = Stretch<CodePointT>;
        return CPStretch::contains(code_point, 0x0080, 0x07FF); 
    }

    UU_ALWAYS_INLINE
    static constexpr bool is_three_byte_code_point(CodePointT code_point) noexcept {
        // Code Points: U+0800..U+0FFF
        //              U+1000..U+CFFF
        //              U+D000..U+D7FF
        //              U+E000..U+FFFF
        using CPStretch = Stretch<CodePointT>;
        return CPStretch::contains(code_point, 0x0800, 0xD7FF) || 
               CPStretch::contains(code_point, 0xE000, 0xFFFF); 
    }

    UU_ALWAYS_INLINE
    static constexpr bool is_four_byte_code_point(CodePointT code_point) noexcept {
        // Code Points: U+10000..U+3FFFF
        //              U+40000..U+FFFFF
        //              U+100000..U+10FFFF
        using CPStretch = Stretch<CodePointT>;
        return CPStretch::contains(code_point, 0x10000, 0x10FFFF); 
    }

    UU_ALWAYS_INLINE
    static constexpr EncodeResult encode_two_byte_code_point(CodePointT code_point) noexcept {
        // Code point value: 00000yyy yyxxxxxx
        // First byte mask:  110yyyyy
        // Second byte mask: 10xxxxxx
        EncodeResult result;
        result.bytes[0] = 0b11000000 | ((code_point & 0b0000011111000000) >> 6);
        result.bytes[1] = 0b10000000 |  (code_point & 0b0000000000111111);
        result.length = 2;
        return result;
    }

    UU_ALWAYS_INLINE
    static constexpr EncodeResult encode_three_byte_code_point(CodePointT code_point) noexcept {
        // Code point value: zzzzyyyy yyxxxxxx
        // First byte mask:  1110zzzz
        // Second byte mask: 10yyyyyy
        // Third byte mask:  10xxxxxx
        EncodeResult result;
        result.bytes[0] = 0b11100000 | ((code_point & 0b1111000000000000) >> 12);
        result.bytes[1] = 0b10000000 | ((code_point & 0b0000111111000000) >> 6);
        result.bytes[2] = 0b10000000 | ((code_point & 0b0000000000111111));
        result.length = 3;
        return result;
    }

    UU_ALWAYS_INLINE
    static constexpr EncodeResult encode_four_byte_code_point(CodePointT code_point) noexcept {
        // Code point value: 000uuuuu zzzzyyyy yyxxxxxx
        // First byte mask:  11110uuu
        // Second byte mask: 10uuzzzz
        // Third byte mask:  10yyyyyy
        // Fourth byte mask: 10xxxxxx
        EncodeResult result;
        result.bytes[0] = 0b11110000 | (code_point & 0b000111000000000000000000) >> 18;
        result.bytes[1] = 0b10000000 | (code_point & 0b000000111111000000000000) >> 12;
        result.bytes[2] = 0b10000000 | (code_point & 0b0000111111000000) >> 6;
        result.bytes[3] = 0b10000000 | (code_point & 0b0000000000111111);
        result.length = 4;
        return result;
    }

    static constexpr EncodeResult encode(CodePointT code_point) noexcept { 
        EncodeResult result;

        if (is_one_byte_code_point(code_point)) {
            result.bytes[0] = Byte(code_point);
            result.length = 1;
        }
        else if (is_two_byte_code_point(code_point)) {
            result = encode_two_byte_code_point(code_point);
        }
        else if (is_three_byte_code_point(code_point)) {
            result = encode_three_byte_code_point(code_point);
        }
        else if (is_four_byte_code_point(code_point)) {
            result = encode_four_byte_code_point(code_point);
        }
        else {
            result.bytes = Attrs::sentinel_bytes;
            result.length = Attrs::sentinel_bytes_length;
        }

        return result;
    }

    static constexpr EncodeResult check_encode(CodePointT code_point) noexcept { 
        return encode(code_point);
    }
};

using UTF8Traits = Traits<Form::UTF8, Char8, Char32>;

}  // namespace TextEncoding

#if 0

using StringStorage = Storage<48, Char32>;

enum class StringStoragePlan {
    OWNED,
    WRAPPED,
    FILE,
};

UU_ALWAYS_INLINE
static constexpr void void_copy(StringStorage *dst, const StringStorage *src) {}

UU_ALWAYS_INLINE
static constexpr void void_move(StringStorage *dst, StringStorage *src) {}

UU_ALWAYS_INLINE
static constexpr void void_ensure(StringStorage *s, Size new_size) {}

UU_ALWAYS_INLINE
static constexpr void void_shrink(StringStorage *s, Size new_size) {}

UU_ALWAYS_INLINE
static constexpr void void_drop(StringStorage *s) {}

struct StringStorageActions
{
    using Copy = void (*)(StringStorage *dst, const StringStorage *src);
    using Move = void (*)(StringStorage *dst, StringStorage *src);
    using Ensure = void (*)(StringStorage *s, Size new_size);
    using Shrink = void (*)(StringStorage *s, Size new_size);
    using Drop = void (*)(StringStorage *s);

    constexpr StringStorageActions() noexcept {}

    constexpr StringStorageActions(Copy c, Move m, Ensure e, Shrink s, Drop d) noexcept : 
        copy(c), move(m), ensure(e), shrink(s), drop(d) {}

    Copy copy = void_copy;
    Move move = void_move;
    Ensure ensure = void_ensure;
    Shrink shrink = void_shrink;
    Drop drop = void_drop;
};

template <StringStoragePlan P> concept IsOwnedStringStoragePlan = (P == StringStoragePlan::OWNED);
template <StringStoragePlan P> concept IsWrappedStringStoragePlan = (P == StringStoragePlan::WRAPPED);
template <StringStoragePlan P> concept IsFileStringStoragePlan = (P == StringStoragePlan::FILE);

template <StringStoragePlan P = StringStoragePlan::OWNED>
struct StringStorageTraits
{
    template <StringStoragePlan X = P> requires IsOwnedStringStoragePlan<X>
    static void init(StringStorage *s, const Char8 *ptr, Size length) {
        actions.ensure(s, length);
        memcpy(s->data(), ptr, length);
        s->set_length(length);
    }

private:
    template <StringStoragePlan X = P> requires IsOwnedStringStoragePlan<X>
    UU_ALWAYS_INLINE
    static void copy(StringStorage *dst, const StringStorage *src) {
    }

    template <StringStoragePlan X = P> requires IsOwnedStringStoragePlan<X>
    UU_ALWAYS_INLINE
    static void move(StringStorage *dst, StringStorage *src) {
    }

    template <StringStoragePlan X = P> requires IsOwnedStringStoragePlan<X>
    UU_ALWAYS_INLINE
    static void ensure(StringStorage *s, Size new_size) {
    }

    template <StringStoragePlan X = P> requires IsOwnedStringStoragePlan<X>
    UU_ALWAYS_INLINE
    static void shrink(StringStorage *s, Size new_size) {
    }

    template <StringStoragePlan X = P> requires IsOwnedStringStoragePlan<X>
    UU_ALWAYS_INLINE
    static void drop(StringStorage *s) {
    }

    StringStorageTraits(const StringStorageTraits &) = default;
    StringStorageTraits(StringStorageTraits &&) = default;
    StringStorageTraits &operator=(const StringStorageTraits &) = default;
    StringStorageTraits &operator=(StringStorageTraits &&) = default;

public:
    static constexpr StringStorageActions actions = 
        StringStorageActions(copy<P>, move<P>, ensure<P>, shrink<P>, drop<P>);

};

#if 0
template <typename CharT, typename TraitsT = ProtoTraits<CharT>>
class ProtoString {
private:
    static constexpr StringStorageActions _VoidStringStorageActions = StringStorageActions();
    static constexpr const StringStorageActions * const VoidStringStorageActions = &_VoidStringStorageActions;

public:
    constexpr ProtoString()  noexcept : actions(VoidStringStorageActions) {}

    template <StringStoragePlan P = StringStoragePlan::OWNED>
    ProtoString(const Char8 *ptr, Size length) : actions(&StringStorageTraits<P>::actions) {
        StringStorageTraits<P>::init(&storage, ptr, length);
    }

    template <StringStoragePlan P = StringStoragePlan::OWNED>
    ProtoString(const Char8 *ptr) : actions(&StringStorageTraits<P>::actions) {
        StringStorageTraits<P>::init(&storage, ptr, std::char_traits<Char8>::length(ptr));
    }

    constexpr Size length() const { return storage.length(); }

private:
    const StringStorageActions *actions;
    StringStorage storage;
};

using Text8 = ProtoString<Char8>;
#endif

#endif

}  // namespace UU

#endif  // UU_TEXT_H
