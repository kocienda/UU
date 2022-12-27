//
// AcquireReleaseGuard.h
//

#ifndef UU_ACQUIRE_RELEASE_GUARD_H
#define UU_ACQUIRE_RELEASE_GUARD_H

namespace UU {

template <class Resource>
class AcquireReleaseGuard
{
public:
    AcquireReleaseGuard(Resource &rsrc) : m_rsrc(rsrc) { m_rsrc.acquire(); }
    ~AcquireReleaseGuard() { m_rsrc.release(); }
private:
    Resource &m_rsrc;
};

}  // namespace UU

#endif  // UU_ACQUIRE_RELEASE_GUARD_H