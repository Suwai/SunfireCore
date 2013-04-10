/*
 * BlizzLikeCore integrates as part of this file: CREDITS.md and LICENSE.md
 */

#ifndef _BLIZZLIKE_RARUNNABLE_H_
#define _BLIZZLIKE_RARUNNABLE_H_

#include "Common.h"

#include <ace/Reactor.h>

class RARunnable : public ACE_Based::Runnable
{
public:
    RARunnable();
    virtual ~RARunnable();
    void run();

private:
    ACE_Reactor* m_Reactor;

};

#endif /* _BLIZZLIKE_RARUNNABLE_H_ */
/// @}
