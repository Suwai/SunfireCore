/*
 * BlizzLikeCore integrates as part of this file: CREDITS.md and LICENSE.md
 */

#ifndef _FOLLOWERREFERENCE_H
#define _FOLLOWERREFERENCE_H

#include "Utilities/LinkedReference/Reference.h"

class TargetedMovementGeneratorBase;
class Unit;

class FollowerReference : public Reference<Unit, TargetedMovementGeneratorBase>
{
    protected:
        void targetObjectBuildLink();
        void targetObjectDestroyLink();
        void sourceObjectDestroyLink();
};
#endif

