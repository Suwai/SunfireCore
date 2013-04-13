/*
 * BlizzLikeCore integrates as part of this file: CREDITS.md and LICENSE.md
 */

/* ScriptData
Name: Boss_Blackheart_the_Inciter
Complete(%): 75
Comment: Incite Chaos not functional since core lacks Mind Control support
Category: Auchindoun, Shadow Labyrinth
EndScriptData */

#include "ScriptPCH.h"
#include "shadow_labyrinth.h"

#define SPELL_INCITE_CHAOS    33676
#define SPELL_INCITE_CHAOS_B  33684                         //debuff applied to each member of party
#define SPELL_CHARGE          33709
#define SPELL_WAR_STOMP       33707

#define SAY_INTRO1          -1555008
#define SAY_INTRO2          -1555009
#define SAY_INTRO3          -1555010
#define SAY_AGGRO1          -1555011
#define SAY_AGGRO2          -1555012
#define SAY_AGGRO3          -1555013
#define SAY_SLAY1           -1555014
#define SAY_SLAY2           -1555015
#define SAY_HELP            -1555016
#define SAY_DEATH           -1555017

#define SAY2_INTRO1         -1555018
#define SAY2_INTRO2         -1555019
#define SAY2_INTRO3         -1555020
#define SAY2_AGGRO1         -1555021
#define SAY2_AGGRO2         -1555022
#define SAY2_AGGRO3         -1555023
#define SAY2_SLAY1          -1555024
#define SAY2_SLAY2          -1555025
#define SAY2_HELP           -1555026
#define SAY2_DEATH          -1555027

struct boss_blackheart_the_inciterAI : public ScriptedAI
{
    boss_blackheart_the_inciterAI(Creature* c) : ScriptedAI(c)
    {
        pInstance = c->GetInstanceData();
    }

    ScriptedInstance *pInstance;

    uint32 m_uiInciteChaosTimer;
    uint32 m_uiInciteChaosWaitTimer;
    uint32 m_uiChargeTimer;
    uint32 m_uiKnockbackTimer;


    void Reset() override
    {
        m_uiInciteChaosWaitTimer = 0;
        m_uiInciteChaosTimer = 15000;
        m_uiChargeTimer      = urand(30000, 37000);
        m_uiKnockbackTimer   = urand(10000, 14000);
    }

    void KilledUnit(Unit* /*pVictim*/)
    {
        DoScriptText(urand(0, 1) ? SAY_SLAY1 : SAY_SLAY2, me);
    }

    void JustDied(Unit* /*pKiller*/)
    {
        DoScriptText(SAY_DEATH, me);

        if (pInstance)
            pInstance->SetData(DATA_BLACKHEARTTHEINCITEREVENT, DONE);
    }

    void EnterCombat(Unit* /*pWho*/)
    {
        switch (urand(0, 2))
        {
            case 0: DoScriptText(SAY_AGGRO1, me); break;
            case 1: DoScriptText(SAY_AGGRO2, me); break;
            case 2: DoScriptText(SAY_AGGRO3, me); break;
        }

        if (pInstance)
            pInstance->SetData(DATA_BLACKHEARTTHEINCITEREVENT, IN_PROGRESS);
    }

    void JustReachedHome() override
    {
        if (pInstance)
            pInstance->SetData(DATA_BLACKHEARTTHEINCITEREVENT, FAIL);
    }

    void EnterEvadeMode()
    {
        // if we are waiting for Incite chaos to expire don't evade
        if (m_uiInciteChaosWaitTimer)
            return;

        ScriptedAI::EnterEvadeMode();
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (m_uiInciteChaosWaitTimer)
        {
            if (m_uiInciteChaosWaitTimer <= uiDiff)
            {
                // Restart attack on all targets
				std::list<HostileReference *> t_list = me->getThreatManager().getThreatList();
                for (std::list<HostileReference *>::iterator itr = t_list.begin(); itr != t_list.end(); ++itr)
                {
                    Unit* pTarget = Unit::GetUnit(*me, (*itr)->getUnitGuid());
                        AttackStart(pTarget);
                }

                me->HandleEmoteCommand(EMOTE_STATE_NONE);
                m_uiInciteChaosWaitTimer = 0;
            }
            else
                m_uiInciteChaosWaitTimer -= uiDiff;
        }

        // Return since we have no pTarget
        if (!UpdateVictim())
            return;

        if (m_uiInciteChaosTimer < uiDiff)
		{
			DoCast(me, SPELL_INCITE_CHAOS);
				DoResetThreat();
                me->HandleEmoteCommand(EMOTE_STATE_LAUGH);
                m_uiInciteChaosTimer = 55000;
                m_uiInciteChaosWaitTimer = 0;
                return;
            }
        else
            m_uiInciteChaosTimer -= uiDiff;

        // Charge Timer
        if (m_uiChargeTimer < uiDiff)
        {
            if (Unit* pTarget = SelectUnit(SELECT_TARGET_RANDOM, 0))
            {
                DoCast(pTarget, SPELL_CHARGE);
                    m_uiChargeTimer = urand(30000, 43000);
            }
        }
        else
            m_uiChargeTimer -= uiDiff;

        // Knockback Timer
        if (m_uiKnockbackTimer < uiDiff)
        {
            DoCast(me, SPELL_WAR_STOMP);
                m_uiKnockbackTimer = urand(15000, 30000);
        }
        else
            m_uiKnockbackTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_blackheart_the_inciter(Creature* pCreature)
{
    return new boss_blackheart_the_inciterAI(pCreature);
}

void AddSC_boss_blackheart_the_inciter()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_blackheart_the_inciter";
    pNewScript->GetAI = &GetAI_boss_blackheart_the_inciter;
    pNewScript->RegisterSelf();
}

