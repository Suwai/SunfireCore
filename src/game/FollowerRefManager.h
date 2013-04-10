/*
 * BlizzLikeCore integrates as part of this file: CREDITS.md and LICENSE.md
 */

#ifndef _FOLLOWERREFMANAGER
#define _FOLLOWERREFMANAGER

#include "Utilities/LinkedReference/RefManager.h"

class Unit;
class TargetedMovementGeneratorBase;

class FollowerRefManager : public RefManager<Unit, TargetedMovementGeneratorBase>
{

};
#endif

