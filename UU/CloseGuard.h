//
// CloseGuard.h
//

#ifndef UU_CLOSE_GUARD_H
#define UU_CLOSE_GUARD_H

namespace UU {

template <class Resource>
class CloseGuard
{
public:
    CloseGuard(Resource &rsrc) : m_rsrc(rsrc) {}
    ~CloseGuard() { m_rsrc.close(); }
private:
    CloseGuard(const CloseGuard &) = delete;
    CloseGuard &operator=(const CloseGuard &) = delete;

    Resource &m_rsrc;
};

}  // namespace UU

#endif  // UU_CLOSE_GUARD_H
