/*
 * BlizzLikeCore integrates as part of this file: CREDITS.md and LICENSE.md
 */

/* ScriptData
Name: Boss_instructormalicia
Complete(%): 100
Comment:
Category: Scholomance
EndScriptData */

#include "ScriptPCH.h"
#include "scholomance.h"

#define SPELL_CALLOFGRAVES         17831
#define SPELL_CORRUPTION           11672
#define SPELL_FLASHHEAL            10917
#define SPELL_RENEW                10929
#define SPELL_HEALINGTOUCH         9889

struct boss_instructormaliciaAI : public ScriptedAI
{
    boss_instructormaliciaAI(Creature* c) : ScriptedAI(c) {}

    uint32 CallOfGraves_Timer;
    uint32 Corruption_Timer;
    uint32 FlashHeal_Timer;
    uint32 Renew_Timer;
    uint32 HealingTouch_Timer;
    uint32 FlashCounter;
    uint32 TouchCounter;

    void Reset()
    {
        CallOfGraves_Timer = 4000;
        Corruption_Timer = 8000;
        FlashHeal_Timer = 38000;
        Renew_Timer = 32000;
        HealingTouch_Timer = 45000;
        FlashCounter = 0;
        TouchCounter = 0;
    }

    void JustDied(Unit* /*killer*/)
    {
        ScriptedInstance *pInstance = me->GetInstanceData();
        if (pInstance)
        {
            pInstance->SetData(DATA_INSTRUCTORMALICIA_DEATH, 0);

            if (pInstance->GetData(TYPE_GANDLING) == IN_PROGRESS)
                me->SummonCreature(1853, 180.73f, -9.43856f, 75.507f, 1.61399f, TEMPSUMMON_DEAD_DESPAWN, 0);
        }
    }

    void EnterCombat(Unit* /*who*/)
    {
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        //CallOfGraves_Timer
        if (CallOfGraves_Timer <= diff)
        {
            DoCast(me->getVictim(), SPELL_CALLOFGRAVES);
            CallOfGraves_Timer = 65000;
        } else CallOfGraves_Timer -= diff;

        //Corruption_Timer
        if (Corruption_Timer <= diff)
        {
            Unit* pTarget = NULL;
            pTarget = SelectUnit(SELECT_TARGET_RANDOM,0);
            if (pTarget) DoCast(pTarget, SPELL_CORRUPTION);

            Corruption_Timer = 24000;
        } else Corruption_Timer -= diff;

        //Renew_Timer
        if (Renew_Timer <= diff)
        {
            DoCast(me, SPELL_RENEW);
            Renew_Timer = 10000;
        } else Renew_Timer -= diff;

        //FlashHeal_Timer
        if (FlashHeal_Timer <= diff)
        {
            DoCast(me, SPELL_FLASHHEAL);

            //5 Flashheals will be casted
            if (FlashCounter < 2)
            {
                FlashHeal_Timer = 5000;
                ++FlashCounter;
            }
            else
            {
                FlashCounter=0;
                FlashHeal_Timer = 30000;
            }
        } else FlashHeal_Timer -= diff;

        //HealingTouch_Timer
        if (HealingTouch_Timer <= diff)
        {
            DoCast(me, SPELL_HEALINGTOUCH);

            //3 Healingtouchs will be casted
            if (HealingTouch_Timer < 2)
            {
                HealingTouch_Timer = 5500;
                ++TouchCounter;
            }
            else
            {
                TouchCounter=0;
                HealingTouch_Timer = 30000;
            }
        } else HealingTouch_Timer -= diff;

        DoMeleeAttackIfReady();
    }
};
CreatureAI* GetAI_boss_instructormalicia(Creature* pCreature)
{
    return new boss_instructormaliciaAI (pCreature);
}

void AddSC_boss_instructormalicia()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "boss_instructor_malicia";
    newscript->GetAI = &GetAI_boss_instructormalicia;
    newscript->RegisterSelf();
}

