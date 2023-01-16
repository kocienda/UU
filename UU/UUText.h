//
// UUText.h
//

#ifndef UU_TEXT_H
#define UU_TEXT_H

#include "UU/Compiler.h"
#include <concepts>
#include <cstring>
#include <memory>
#include <string>
#include <type_traits>

#include <UU/Assertions.h>
#include <UU/IteratorWrapper.h>
#include <UU/Storage.h>
#include <UU/Types.h>

namespace UU {

template <typename T> requires IsCharType<T> class TextEncodingTraits : public std::char_traits<T> {
public:
    using CharT = T;
    using CodePointT = Char32;
    using Traits = std::char_traits<T>;

    static constexpr CodePointT Sentinel = 0xFFFD;
    static constexpr Size npos = SizeMax;
    
    template <bool B = true> 
    static constexpr bool equal(CharT a, CharT b) noexcept { return (Traits::eq(a, b)) == B; }

    template <bool B = true> 
    static constexpr bool less(CharT a, CharT b) noexcept { return (Traits::lt(a, b)) == B; }

    template <bool B = true> 
    static constexpr bool is_single(CharT c) noexcept { return true == B; }

    template <bool B = true> 
    static constexpr bool is_single(const CharT *ptr, Size bpos) noexcept { return is_single(ptr[bpos]) == B; }
    
    template <bool B = true> 
    static constexpr bool is_lead(CharT c) noexcept { return true == B; }

    template <bool B = true> 
    static constexpr bool is_lead(const CharT *ptr, Size bpos) noexcept { return is_lead(ptr[bpos]) == B; }
    
    template <bool B = true> 
    static constexpr bool is_trail(CharT c) noexcept { return true == B; }

    template <bool B = true> 
    static constexpr bool is_trail(const CharT *ptr, Size bpos) noexcept { return is_trail(ptr[bpos]) == B; }

    static constexpr CodePointT get(const CharT *ptr, Size len, Size bpos) noexcept { 
        return LIKELY(bpos < len) ? ptr[bpos] : Sentinel;
    }

    static constexpr Size blength(CodePointT c) noexcept {
        if (c <= 0xff) {
            return 1;
        }
        if (c <= 0xffff) {
            return 2;
        }
        if (c <= 0xffffff) {
            return 3;
        }
        return 4;
    }

    static constexpr Size clength(const CharT *ptr) noexcept { return length(ptr); }
    static constexpr Size max_clength() noexcept { return 1; }

    static constexpr Size cpos_start(const CharT *ptr, Size len, Size bpos) noexcept { 
        return bpos < len ? bpos : npos; 
    }



    static constexpr Size previous_cpos(const CharT *ptr, Size len, Size bpos) noexcept { 
        return bpos > 0 ? --bpos : npos; 
    }

    static constexpr Size next_cpos(const CharT *ptr, Size len, Size bpos) noexcept {
        return bpos + 1 < len ? bpos + 1 : npos; 
    }

    static constexpr std::pair<Size, bool> next_cpos_check(const CharT *ptr, Size len, Size bpos) noexcept {
        auto result = std::make_pair<Size, bool>(bpos + 1, true);
        if (result.first >= len) {
            result = { npos, false };
        }
        return result;
    }

    template <bool B = true> 
    static constexpr bool is_well_formed(const CharT *ptr, Size bpos) noexcept { return is_trail(ptr[bpos]) == B; }

    // case functions
};

class UTF8TextEncodingTraits {
public:
    using CharT = Char8;
    using CodePointT = Char32;
    using CharTraits = std::char_traits<Char8>;

    static constexpr CodePointT Sentinel = 0xFFFD;
    static constexpr Size npos = SizeMax;

private:
    UU_ALWAYS_INLINE
    static constexpr CodePointT decode_two_byte_code_point(CodePointT b1, CodePointT b2) {
        // Code point value: 00000yyy yyxxxxxx
        // First byte mask:  110yyyyy
        // Second byte mask: 10xxxxxx
        CodePointT s1 = ((b1 & 0b00011100) >> 2);
        CodePointT s2 = ((b1 & 0b00000011) << 6) | (b2 & 0b00111111);
        return (s1 << 8) | s2;
    }

    UU_ALWAYS_INLINE
    static constexpr CodePointT decode_three_byte_code_point(CodePointT b1, CodePointT b2, CodePointT b3) {
        // Code point value: zzzzyyyy yyxxxxxx
        // First byte mask:  1110zzzz
        // Second byte mask: 10yyyyyy
        // Third byte mask:  10xxxxxx
        CodePointT s1 = ((b1 & 0b00001111) << 4) | ((b2 & 0b00111100) >> 2);
        CodePointT s2 = ((b2 & 0b00000011) << 6) | ((b3 & 0b00111111));
        return (s1 << 8) | s2;
    }

    UU_ALWAYS_INLINE
    static constexpr CodePointT decode_four_byte_code_point(CodePointT b1, CodePointT b2, CodePointT b3, CodePointT b4) {
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

    template <bool B = true> 
    static constexpr bool is_in_range(CodePointT c, CodePointT lo, CodePointT hi) noexcept {
        return ((c >= lo) && (c <= hi)) == B;
    }

public:
    static constexpr bool is_single(Byte b) noexcept { return ((b & 0x80) == 0); }
    static constexpr bool not_single(Byte b) noexcept { return !(is_single(b)); }
    static constexpr bool is_lead(Byte b) noexcept { return (b - 0xc2 <= 0x32); }
    static constexpr bool not_lead(Byte b) noexcept { return !(is_lead(b)); }
    static constexpr bool is_trail(Byte b) noexcept { return (Int8(b) < -0x40); }
    static constexpr bool not_trail(Byte b) noexcept { return !(is_trail(b)); }

    static constexpr CodePointT decode(const CharT *ptr, Size len, Size bpos) noexcept { 
        Size bmax = len - bpos - 1;
        if (LIKELY(bmax < len)) {
            bmax = std::min(bmax + 1, 4UL);
        }
        else {
            return Sentinel;
        }

        Byte first = ptr[bpos];
        
        // one-byte code point
        if (is_single(first)) {
            return first;
        }
        ASSERT(is_lead(first));

        // peek ahead for a non-trail byte
        switch (bmax) {
            case 1: {
                ASSERT(false);
                return Sentinel;
            }
            case 2: {
                Byte second = ptr[bpos + 1];
                ASSERT(is_trail(second));
                return decode_two_byte_code_point(first, second);
            }
            case 3: {
                Byte second = ptr[bpos + 1];
                ASSERT(is_trail(second));
                Byte third = ptr[bpos + 2];
                if (not_trail(third)) {
                    return decode_two_byte_code_point(first, second);
                }
                else {
                    return decode_three_byte_code_point(first, second, third);
                }
            }
            default: {
                Byte second = ptr[bpos + 1];
                ASSERT(is_trail(second));
                Byte third = ptr[bpos + 2];
                if (not_trail(third)) {
                    return decode_two_byte_code_point(first, second);
                }
                Byte fourth = ptr[bpos + 3];
                if (not_trail(fourth)) {
                    return decode_three_byte_code_point(first, second, third);
                }
                else {
                    return decode_four_byte_code_point(first, second, third, fourth);
                }
            }
        }
    }

    static constexpr CodePointT decode_check(const CharT *ptr, Size len, Size bpos) noexcept { 
        Size idx = bpos;

        if (UNLIKELY(idx >= len)) {
            ASSERT(false); 
            return Sentinel;
        }
        CodePointT b1 = ptr[idx];
        
        // one-byte code point
        if (is_single(b1)) {
            return b1;
        }

        // two-byte code point
        idx++;
        if (UNLIKELY(idx == len)) {
            ASSERT(false); 
            return Sentinel;
        }
        CodePointT b2 = ptr[idx];

        // Code Points:  U+0080..U+07FF
        // First Byte:   C2..DF
        // Second Byte:  80..BF
        if (is_in_range(b1, 0xC2, 0xDF)) {
            if (LIKELY(is_in_range(b2, 0x80, 0xBF))) {
                return decode_two_byte_code_point(b1, b2);
            }
        }

        // three-byte code point
        idx++;
        if (UNLIKELY(idx == len)) {
            ASSERT(false); 
            return Sentinel;
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
            if (LIKELY(is_in_range(b2, 0xA0, 0xBF) && is_in_range(b3, 0x80, 0xBF))) {
                return decode_three_byte_code_point(b1, b2, b3);
            }
        }
        if (is_in_range(b1, 0xE1, 0xEC)) {
            if (LIKELY(is_in_range(b2, 0xA0, 0xBF) && is_in_range(b3, 0x80, 0xBF))) {
                return decode_three_byte_code_point(b1, b2, b3);
            }
        }
        if (b1 == 0xED) {
            if (LIKELY(is_in_range(b2, 0x80, 0x9F) && is_in_range(b3, 0x80, 0xBF))) {
                return decode_three_byte_code_point(b1, b2, b3);
            }
        }
        if (is_in_range(b1, 0xEE, 0xEF)) {
            if (LIKELY(is_in_range(b2, 0x80, 0xBF) && is_in_range(b3, 0x80, 0xBF))) {
                return decode_three_byte_code_point(b1, b2, b3);
            }
        }

        // four-byte code point
        idx++;
        if (UNLIKELY(idx == len)) {
            ASSERT(false);
            return Sentinel;
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
            if (LIKELY(is_in_range(b2, 0x90, 0xBF) && is_in_range(b3, 0x80, 0xBF) && is_in_range(b4, 0x80, 0xBF))) {
                return decode_four_byte_code_point(b1, b2, b3, b4);;
            }
        }
        if (is_in_range(b1, 0xF1, 0xF3)) {
            if (LIKELY(is_in_range(b2, 0x80, 0xBF) && is_in_range(b3, 0x80, 0xBF) && is_in_range(b4, 0x80, 0xBF))) {
                return decode_four_byte_code_point(b1, b2, b3, b4);;
            }
        }
        if (b1 == 0xF4) {
            if (LIKELY(is_in_range(b2, 0x80, 0x8F) && is_in_range(b3, 0x80, 0xBF) && is_in_range(b4, 0x80, 0xBF))) {
                return decode_four_byte_code_point(b1, b2, b3, b4);;
            }
        }

        return Sentinel;
    }

    static constexpr Size length_in_bytes(CodePointT c) noexcept {
        if (c <= 0x7f) {
            return 1;
        }
        if (c <= 0x7ff) {
            return 2;
        }
        if (c <= 0xd7ff || c <= 0xdfff || c > 0x10ffff) {
            return 3;
        }
        return 4;
    }

    static constexpr Size length_in_code_points(const CharT *ptr) noexcept { return CharTraits::length(ptr); }
    static constexpr Size max_code_point_length_in_bytes() noexcept { return 4; }

    static constexpr Size previous_cpos(const CharT *ptr, Size len, Size bpos) noexcept {
        return 0;
    }

    static constexpr Size next_cpos(const CharT *ptr, Size bpos) noexcept {
        return ptr[bpos + 1] == CharT(0) ? npos : bpos + 1; 
    }

    static constexpr std::pair<Size, bool> next_cpos_check(const CharT *ptr, Size len, Size bpos) noexcept {
        auto result = std::make_pair<Size, bool>(bpos + 1, true);
        if (bpos + 1 >= len) {
            result = { npos, false };
        }
        else {

        }
        return result;
    }

    // case functions
};



using UTF32TextEncodingTraits = TextEncodingTraits<Char32>;

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

template <typename CharT, typename TraitsT = TextEncodingTraits<CharT>>
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

}  // namespace UU

#endif  // UU_TEXT_H