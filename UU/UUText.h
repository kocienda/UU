//
// UUText.h
//

#ifndef UU_TEXT_H
#define UU_TEXT_H

#include <concepts>
#include <memory>
#include <string>
#include <type_traits>

#include <UU/Assertions.h>
#include <UU/Types.h>

namespace UU {

template <Size S> requires IsNonZeroSize<S> && IsDivisibleByChar32Size<S>
class BasicTextStorage 
{
public:
    static constexpr Size InlineCapacity = S;

    static constexpr UInt32 UsingAllocatedBuffer = 0x01;

    constexpr BasicTextStorage() : m_ptr(reinterpret_cast<Byte *>(m_buf)) {}

    constexpr Byte *data() const { return m_ptr; }
    constexpr Size length() const { return m_length; }
    constexpr void set_length(Size length) { m_length = length; }
    constexpr Size capacity() const { return m_capacity; }
    constexpr UInt32 flags() const { return m_flags; }

    constexpr void set_flags(UInt32 flags) { m_flags = flags; }

    template <bool B = true> constexpr bool is_using_allocated_buffer() const { 
        return (m_flags & UsingAllocatedBuffer) == B; 
    }

    template <bool B = true> constexpr void set_using_allocated_buffer() {
        if (B) {
            m_flags |= UsingAllocatedBuffer;
        }
        else {
            m_flags &= ~UsingAllocatedBuffer;
        }
    }

    template <bool B = true> constexpr bool is_using_inline_buffer() const { 
        return !is_using_allocated_buffer() == B; 
    }

    template <bool B = true> constexpr void set_using_inline_buffer() {
        set_using_allocated_buffer<!B>();
    }

private:
    BasicTextStorage(const BasicTextStorage &) = delete;
    BasicTextStorage(BasicTextStorage &&) = delete;
    BasicTextStorage &operator=(const BasicTextStorage &) = delete;
    BasicTextStorage &operator=(BasicTextStorage &&) = delete;

    using Buffer = std::aligned_storage_t<InlineCapacity, std::alignment_of_v<Char32>>;

    Buffer m_buf[InlineCapacity];
    Byte *m_ptr = nullptr;
    Size m_length = 0;
    Size m_capacity = InlineCapacity;
    UInt32 m_flags = 0;
};

using TextStorage = BasicTextStorage<48>;

enum class TextStoragePlan {
    OWNED,
    WRAPPED,
    FILE,
};

UU_ALWAYS_INLINE
static constexpr void void_copy(TextStorage *dst, const TextStorage *src) {}

UU_ALWAYS_INLINE
static constexpr void void_move(TextStorage *dst, TextStorage *src) {}

UU_ALWAYS_INLINE
static constexpr void void_ensure(TextStorage *s, Size new_size) {}

UU_ALWAYS_INLINE
static constexpr void void_shrink(TextStorage *s, Size new_size) {}

UU_ALWAYS_INLINE
static constexpr void void_drop(TextStorage *s) {}

struct TextStorageActions
{
    using Copy = void (*)(TextStorage *dst, const TextStorage *src);
    using Move = void (*)(TextStorage *dst, TextStorage *src);
    using Ensure = void (*)(TextStorage *s, Size new_size);
    using Shrink = void (*)(TextStorage *s, Size new_size);
    using Drop = void (*)(TextStorage *s);

    constexpr TextStorageActions() noexcept {}

    constexpr TextStorageActions(Copy c, Move m, Ensure e, Shrink s, Drop d) noexcept : 
        copy(c), move(m), ensure(e), shrink(s), drop(d) {}

    Copy copy = void_copy;
    Move move = void_move;
    Ensure ensure = void_ensure;
    Shrink shrink = void_shrink;
    Drop drop = void_drop;
};

template <TextStoragePlan P> concept IsOwnedTextStoragePlan = (P == TextStoragePlan::OWNED);
template <TextStoragePlan P> concept IsWrappedTextStoragePlan = (P == TextStoragePlan::WRAPPED);
template <TextStoragePlan P> concept IsFileTextStoragePlan = (P == TextStoragePlan::FILE);

template <TextStoragePlan P = TextStoragePlan::OWNED>
struct TextStorageTraits
{
    template <TextStoragePlan X = P> requires IsOwnedTextStoragePlan<X>
    static void init(TextStorage *s, const Char8 *ptr, Size length) {
        actions.ensure(s, length);
        memcpy(s->data(), ptr, length);
        s->set_length(length);
    }

private:
    template <TextStoragePlan X = P> requires IsOwnedTextStoragePlan<X>
    UU_ALWAYS_INLINE
    static void copy(TextStorage *dst, const TextStorage *src) {
    }

    template <TextStoragePlan X = P> requires IsOwnedTextStoragePlan<X>
    UU_ALWAYS_INLINE
    static void move(TextStorage *dst, TextStorage *src) {
    }

    template <TextStoragePlan X = P> requires IsOwnedTextStoragePlan<X>
    UU_ALWAYS_INLINE
    static void enlarge(TextStorage *s, Size new_size) {
    }

    template <TextStoragePlan X = P> requires IsOwnedTextStoragePlan<X>
    UU_ALWAYS_INLINE
    static void shrink(TextStorage *s, Size new_size) {
    }

    template <TextStoragePlan X = P> requires IsOwnedTextStoragePlan<X>
    UU_ALWAYS_INLINE
    static void drop(TextStorage *s) {
    }

    TextStorageTraits(const TextStorageTraits &) = default;
    TextStorageTraits(TextStorageTraits &&) = default;
    TextStorageTraits &operator=(const TextStorageTraits &) = default;
    TextStorageTraits &operator=(TextStorageTraits &&) = default;

public:
    static constexpr TextStorageActions actions = 
        TextStorageActions(copy<P>, move<P>, enlarge<P>, shrink<P>, drop<P>);

};

template <typename CharT, typename TraitsT = std::char_traits<CharT>>
class BasicText {
private:
    static constexpr TextStorageActions _VoidTextStorageActions = TextStorageActions();
    static constexpr const TextStorageActions * const VoidTextStorageActions = &_VoidTextStorageActions;

public:
    constexpr BasicText()  noexcept : actions(VoidTextStorageActions) {}

    template <TextStoragePlan P = TextStoragePlan::OWNED>
    BasicText(const Char8 *ptr, Size length) : actions(&TextStorageTraits<P>::actions) {
        TextStorageTraits<P>::init(&storage, ptr, length);
    }

    template <TextStoragePlan P = TextStoragePlan::OWNED>
    BasicText(const Char8 *ptr) : actions(&TextStorageTraits<P>::actions) {
        TextStorageTraits<P>::init(&storage, ptr, std::char_traits<Char8>::length(ptr));
    }

    constexpr Size length() const { return storage.length(); }

private:
    const TextStorageActions *actions;
    TextStorage storage;
};

using Text8 = BasicText<Char8>;

}  // namespace UU

#endif  // UU_TEXT_H