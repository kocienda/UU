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
