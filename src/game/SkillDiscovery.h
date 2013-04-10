/*
 * BlizzLikeCore integrates as part of this file: CREDITS.md and LICENSE.md
 */

#ifndef BLIZZLIKE_SKILLDISCOVERY_H
#define BLIZZLIKE_SKILLDISCOVERY_H

#include "Common.h"

class Player;

void LoadSkillDiscoveryTable();
uint32 GetSkillDiscoverySpell(uint32 skillId, uint32 spellId, Player* player);
#endif

