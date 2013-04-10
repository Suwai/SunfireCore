/*
 * BlizzLikeCore integrates as part of this file: CREDITS.md and LICENSE.md
 */

#ifndef __WORLDRUNNABLE_H
#define __WORLDRUNNABLE_H

// Heartbeat thread for the World
class WorldRunnable : public ACE_Based::Runnable
{
    public:
        void run();
};
#endif

