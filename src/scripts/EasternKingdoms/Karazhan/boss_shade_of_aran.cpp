/* ScriptData
SDName: Boss_Shade_of_Aran
SD%Complete: 95
SDComment: Flame wreath missing cast animation, mods won't triggere.
SDCategory: Karazhan
EndScriptData */

#include "ScriptPCH.h"
#include "karazhan.h"
#include "GameObject.h"

#define SAY_AGGRO1                  -1532073
#define SAY_AGGRO2                  -1532074
#define SAY_AGGRO3                  -1532075
#define SAY_FLAMEWREATH1            -1532076
#define SAY_FLAMEWREATH2            -1532077
#define SAY_BLIZZARD1               -1532078
#define SAY_BLIZZARD2               -1532079
#define SAY_EXPLOSION1              -1532080
#define SAY_EXPLOSION2              -1532081
#define SAY_DRINK                   -1532082                //Low Mana / AoE Pyroblast
#define SAY_ELEMENTALS              -1532083
#define SAY_KILL1                   -1532084
#define SAY_KILL2                   -1532085
#define SAY_TIMEOVER                -1532086
#define SAY_DEATH                   -1532087
#define SAY_ATIESH                  -1532088                //Atiesh is equipped by a raid member

//Spells
#define SPELL_FROSTBOLT     29954
#define SPELL_FIREBALL      29953
#define SPELL_ARCMISSLE     33031
#define SPELL_CHAINSOFICE   29991
#define SPELL_DRAGONSBREATH 29964
#define SPELL_MASSSLOW      30035
#define SPELL_FLAME_WREATH  29946
#define SPELL_AOE_CS        29961
#define SPELL_PLAYERPULL    32265
#define SPELL_AEXPLOSION    29973
#define SPELL_MASS_POLY     29963
#define SPELL_BLINK_CENTER  29967
#define SPELL_ELEMENTALS    29962
#define SPELL_CONJURE       29975
#define SPELL_DRINK         30024
#define SPELL_POTION        32453
#define SPELL_AOE_PYROBLAST 29978
#define SPELL_IMMUNE        6724

//Creature Spells
#define SPELL_CIRCULAR_BLIZZARD     29969                //29952 is the REAL circular blizzard that leaves persistant blizzards that last for 10 seconds
#define SPELL_WATERBOLT             31012
#define SPELL_SHADOW_PYRO           29978

//Creatures
#define CREATURE_WATER_ELEMENTAL    17167
#define CREATURE_SHADOW_OF_ARAN     18254
#define CREATURE_ARAN_BLIZZARD      17161

enum SuperSpell
{
    SUPER_FLAME = 0,
    SUPER_BLIZZARD,
    SUPER_AE,
};

struct boss_aranAI : public ScriptedAI
{
    boss_aranAI(Creature *c) : ScriptedAI(c)
    {
        pInstance = c->GetInstanceData();
    }

    ScriptedInstance* pInstance;

    uint32 SecondarySpellTimer;
    uint32 NormalCastTimer;
    uint32 SuperCastTimer;
    uint32 BerserkTimer;
    uint32 CloseDoorTimer;                                  // Don't close the door right on aggro in case some people are still entering.

    uint8 LastSuperSpell;

    uint32 FlameWreathTimer;
    uint32 FlameWreathCheckTime;
    uint64 FlameWreathTarget[3];
    float FWTargPosX[3];
    float FWTargPosY[3];

    uint32 CurrentNormalSpell;
    uint32 ArcaneCooldown;
    uint32 FireCooldown;
    uint32 FrostCooldown;
        uint32 AETimer;

    uint32 DrinkInturruptTimer;

    bool ElementalsSpawned;
    bool Drinking;
    bool DrinkInturrupted;
        bool CastAE;

        float x_cord;
        float y_cord;
        float z_cord;

    void Reset()
    {
		me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_TAUNT, true);
		me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_ATTACK_ME, true);

		DoCast(me, 31261);

        SecondarySpellTimer = 5000;
        NormalCastTimer = 0;
        SuperCastTimer = 35000;
        BerserkTimer = 720000;
        CloseDoorTimer = 15000;

        LastSuperSpell = rand()%3;

        FlameWreathTimer = 0;
        FlameWreathCheckTime = 0;

        CurrentNormalSpell = 0;
        ArcaneCooldown = 0;
        FireCooldown = 0;
        FrostCooldown = 0;

        DrinkInturruptTimer = 10000;

        ElementalsSpawned = false;
        Drinking = false;
        DrinkInturrupted = false;

                CastAE = false;
                AETimer = 2000;

        if(pInstance)
        {
            // Not in progress
            pInstance->SetData(TYPE_ARAN, NOT_STARTED);
            pInstance->HandleGameObject(pInstance->GetData64(DATA_GO_LIBRARY_DOOR), true);
        }
    }

    void KilledUnit(Unit * /*victim*/)
    {
        DoScriptText(RAND(SAY_KILL1,SAY_KILL2), me);
    }

    void JustDied(Unit * /*victim*/)
    {
        DoScriptText(SAY_DEATH, me);

        if (pInstance)
        {
            pInstance->SetData(TYPE_ARAN, DONE);
            pInstance->HandleGameObject(pInstance->GetData64(DATA_GO_LIBRARY_DOOR), true);
        }
    }

    void EnterCombat(Unit * /*who*/)
    {

		DoScriptText(RAND(SAY_AGGRO1,SAY_AGGRO2,SAY_AGGRO3), me);

        if (pInstance)
        {
            pInstance->SetData(TYPE_ARAN, IN_PROGRESS);
            pInstance->HandleGameObject(pInstance->GetData64(DATA_GO_LIBRARY_DOOR), false);
        }
    }

    void FlameWreathEffect()
    {
        std::vector<Unit*> targets;
        std::list<HostileReference *> t_list = me->getThreatManager().getThreatList();

        if (!t_list.size())
            return;

        //store the threat list in a different container
        for (std::list<HostileReference *>::const_iterator itr = t_list.begin(); itr!= t_list.end(); ++itr)
        {
            Unit *pTarget = Unit::GetUnit(*me, (*itr)->getUnitGuid());
            //only on alive players
            if (pTarget && pTarget->isAlive() && pTarget->GetTypeId() == TYPEID_PLAYER)
                targets.push_back(pTarget);
        }

        //cut down to size if we have more than 3 targets
        while (targets.size() > 3)
            targets.erase(targets.begin()+rand()%targets.size());

        uint32 i = 0;
        for (std::vector<Unit*>::const_iterator itr = targets.begin(); itr!= targets.end(); ++itr)
        {
            if (*itr)
            {
                FlameWreathTarget[i] = (*itr)->GetGUID();
                FWTargPosX[i] = (*itr)->GetPositionX();
                FWTargPosY[i] = (*itr)->GetPositionY();
                DoCast((*itr), SPELL_FLAME_WREATH, true);
                ++i;
            }
        }
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        if (CloseDoorTimer)
        {
            if (CloseDoorTimer <= diff)
            {
                if (pInstance)
                {
                    pInstance->HandleGameObject(pInstance->GetData64(DATA_GO_LIBRARY_DOOR), false);
                    CloseDoorTimer = 0;
                }
            } else CloseDoorTimer -= diff;
        }

        //Cooldowns for casts
        if (ArcaneCooldown)
        {
            if (ArcaneCooldown >= diff)
                ArcaneCooldown -= diff;
        else ArcaneCooldown = 0;
        }

        if (FireCooldown)
        {
            if (FireCooldown >= diff)
                FireCooldown -= diff;
        else FireCooldown = 0;
        }

        if (FrostCooldown)
        {
            if (FrostCooldown >= diff)
                FrostCooldown -= diff;
        else FrostCooldown = 0;
        }

        if (!Drinking && me->GetMaxPower(POWER_MANA) && (me->GetPower(POWER_MANA)*100 / me->GetMaxPower(POWER_MANA)) < 20)
        {
            Drinking = true;
            me->InterruptNonMeleeSpells(true);
			me->AttackStop();

            DoScriptText(SAY_DRINK, me);

            if (!DrinkInturrupted)
            {
				DoCast(me, SPELL_MASS_POLY, true);
                DoCast(me, SPELL_CONJURE, false);
                DoCast(me, SPELL_DRINK, false);
                me->SetStandState(UNIT_STAND_STATE_SIT);
                                                            //Sitting down
                me->SetUInt32Value(UNIT_FIELD_BYTES_1, 1);
                DrinkInturruptTimer = 10000;
            }
        }

        //Drink Inturrupt
        if (Drinking && DrinkInturrupted)
        {
            Drinking = false;
            me->RemoveAurasDueToSpell(SPELL_DRINK);
            me->SetStandState(UNIT_STAND_STATE_STAND);
            me->SetPower(POWER_MANA, me->GetMaxPower(POWER_MANA)-32000);
            DoCast(me, SPELL_POTION, false);
        }

        //Drink Inturrupt Timer
        if (Drinking && !DrinkInturrupted)
            if (DrinkInturruptTimer >= diff)
                DrinkInturruptTimer -= diff;
        else
        {
            me->SetStandState(UNIT_STAND_STATE_STAND);
            DoCast(me, SPELL_POTION, true);
            DoCast(me, SPELL_AOE_PYROBLAST, false);
            DrinkInturrupted = true;
            Drinking = false;
        }

        //Don't execute any more code if we are drinking
        if (Drinking)
            return;

        //Normal casts
        if (NormalCastTimer <= diff)
        {
            if (!me->IsNonMeleeSpellCasted(false))
            {
                Unit *pTarget = SelectTarget(SELECT_TARGET_RANDOM, 0, 100, true);
                if (!pTarget)
                    return;

                uint32 Spells[3];
                uint8 AvailableSpells = 0;

                //Check for what spells are not on cooldown
                if (!ArcaneCooldown)
                {
                    Spells[AvailableSpells] = SPELL_ARCMISSLE;
                    ++AvailableSpells;
                }
                if (!FireCooldown)
                {
                    Spells[AvailableSpells] = SPELL_FIREBALL;
                    ++AvailableSpells;
                }
                if (!FrostCooldown)
                {
                    Spells[AvailableSpells] = SPELL_FROSTBOLT;
                    ++AvailableSpells;
                }

                //If no available spells wait 1 second and try again
                if (AvailableSpells)
                {
                    CurrentNormalSpell = Spells[rand() % AvailableSpells];
                    DoCast(pTarget, CurrentNormalSpell);
                }
            }
            NormalCastTimer = 1000;
        } else NormalCastTimer -= diff;

        if (SecondarySpellTimer <= diff)
        {
            switch (rand()%3)
            {

                case 0:
                    DoCast(me, SPELL_AOE_CS);
                    break;
                case 1:
                    if (Unit* pTarget = SelectTarget(SELECT_TARGET_RANDOM, 0, 100, true))
                        DoCast(pTarget, SPELL_CHAINSOFICE);
                    break;
				case 2:
                    if (Unit* pTarget = SelectTarget(SELECT_TARGET_RANDOM, 0, 100, true))
                        DoCast(pTarget, SPELL_DRAGONSBREATH);
					break;

            }
            SecondarySpellTimer = urand(5000,15000);
        } else SecondarySpellTimer -= diff;

        if (SuperCastTimer <= diff)
        {
            uint8 Available[2];

            switch (LastSuperSpell)
            {
                case SUPER_AE:
                    Available[0] = SUPER_FLAME;
                    Available[1] = SUPER_BLIZZARD;
                    break;
                case SUPER_FLAME:
                    Available[0] = SUPER_AE;
                    Available[1] = SUPER_BLIZZARD;
                    break;
                case SUPER_BLIZZARD:
                    Available[0] = SUPER_FLAME;
                    Available[1] = SUPER_AE;
                    break;
            }

            LastSuperSpell = Available[urand(0,1)];

            Map::PlayerList const &PlayerList = pInstance->instance->GetPlayers();
                        WorldPacket data;
                        switch (LastSuperSpell)
            {
                case SUPER_AE:

                    if (rand()%2)
                        DoScriptText(SAY_EXPLOSION1, me);
                    else
                        DoScriptText(SAY_EXPLOSION2, me);

                                        me->CastSpell(me, SPELL_BLINK_CENTER, true);
                                        
                                        x_cord = -11165.07;
                                        y_cord = -1912.07;
                                        z_cord = 232;

                                        me->Relocate(x_cord,y_cord,z_cord,0);
                                        me->SendMessageToSet(&data, false);
                                        for(Map::PlayerList::const_iterator x = PlayerList.begin(); x != PlayerList.end(); ++x)
                                        {
                                        if (Player* i_pl = x->getSource())
                                                {
                                                        if (i_pl->isAlive() && i_pl->GetDistance2d(me->GetPositionX(),me->GetPositionY())<80)
                                                        {
                                                                i_pl->TeleportTo(me->GetMapId(),x_cord,y_cord,z_cord,i_pl->GetOrientation(),TELE_TO_NOT_LEAVE_COMBAT | TELE_TO_NOT_UNSUMMON_PET);
                                                        }
                                                }
                                        }

                    me->CastSpell(me, SPELL_PLAYERPULL, true);
                    me->CastSpell(me, SPELL_MASSSLOW, true);
                                        AETimer = 1500;
                                        CastAE = true;
                    break;

                case SUPER_FLAME:

					DoScriptText(RAND(SAY_FLAMEWREATH1,SAY_FLAMEWREATH2), me);

                    FlameWreathTimer = 20000;
                    FlameWreathCheckTime = 500;

                    FlameWreathTarget[0] = 0;
                    FlameWreathTarget[1] = 0;
                    FlameWreathTarget[2] = 0;

                    FlameWreathEffect();
                    break;

                case SUPER_BLIZZARD:

					DoScriptText(RAND(SAY_BLIZZARD1,SAY_BLIZZARD2), me);

					Creature* Spawn = NULL;
                    Spawn = DoSpawnCreature(CREATURE_ARAN_BLIZZARD, 0, 0, 0, 0, TEMPSUMMON_TIMED_DESPAWN, 30000);
                    if (Spawn)
                    {
					Spawn->setFaction(me->getFaction());
					Spawn->SetReactState(REACT_PASSIVE);
					Spawn->CastSpell(me, SPELL_CIRCULAR_BLIZZARD, true);
					Spawn->GetMotionMaster()->MoveRotate(30000, rand()%2 ? ROTATE_DIRECTION_LEFT : ROTATE_DIRECTION_RIGHT);
                    }
                    break;
            }

            SuperCastTimer = urand(35000,40000);
        } else SuperCastTimer -= diff;

        if (!ElementalsSpawned && me->GetHealth()*100 / me->GetMaxHealth() < 40)
        {
            ElementalsSpawned = true;

            for (uint32 i = 0; i < 4; ++i)
            {
                                float xm,ym = 0;
                                switch(i)
                                {
                                case 0:
                                        xm = -11191.08;
                                        ym = -1909.36;
                                        break;
                                case 1:
                                        xm = -11167.75;
                                        ym = -1938.19;
                                        break;
                                case 2:
                                        xm = -11138.91;
                                        ym = -1914.5;
                                        break;
                                case 3:
                                        xm = -11162.14;
                                        ym = -1886.025;
                                        break;
                                }
                                Creature* pUnit = me->SummonCreature(CREATURE_WATER_ELEMENTAL, xm, ym, me->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN, 90000);
                if (pUnit)
                {
                    pUnit->Attack(me->getVictim(), true);
                    pUnit->setFaction(me->getFaction());
                }
            }

            DoScriptText(SAY_ELEMENTALS, me);
        }

        if (BerserkTimer <= diff)
        {
            for (uint32 i = 0; i < 5; ++i)
            {
                Creature* pUnit = DoSpawnCreature(CREATURE_SHADOW_OF_ARAN, 0, 0, 0, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 5000);
                if (pUnit)
                {
                    pUnit->Attack(me->getVictim(), true);
                    pUnit->setFaction(me->getFaction());
                }
            }

            DoScriptText(SAY_TIMEOVER, me);

            BerserkTimer = 60000;
        } else BerserkTimer -= diff;

        //Flame Wreath check
        if (FlameWreathTimer)
        {
            if (FlameWreathTimer >= diff)
                FlameWreathTimer -= diff;
            else FlameWreathTimer = 0;

            if (FlameWreathCheckTime <= diff)
            {
                for (uint8 i = 0; i < 3; ++i)
                {
                    if (!FlameWreathTarget[i])
                        continue;

                    Unit* pUnit = Unit::GetUnit(*me, FlameWreathTarget[i]);
                    if (pUnit && !pUnit->IsWithinDist2d(FWTargPosX[i], FWTargPosY[i], 3))
                    {
                        pUnit->CastSpell(pUnit, 20476, true, 0, 0, me->GetGUID());
                        pUnit->CastSpell(pUnit, 11027, true);
                        FlameWreathTarget[i] = 0;
                    }
                }
                FlameWreathCheckTime = 500;
            } else FlameWreathCheckTime -= diff;
        }

        if (ArcaneCooldown && FireCooldown && FrostCooldown)
            me->InterruptNonMeleeSpells(false);
                if(CastAE)
                        if(AETimer < diff)
                        {
							me->CastSpell(me, SPELL_IMMUNE, false);
                                me->CastSpell(me, SPELL_AEXPLOSION, false);
								AETimer = 1500;
                                CastAE = false;
                        }else AETimer -= diff;
    }

    void DamageTaken(Unit* /*pAttacker*/, uint32 &damage)
    {
        if (!DrinkInturrupted && Drinking && damage)
            DrinkInturrupted = true;
    }

    void SpellHit(Unit* /*pAttacker*/, const SpellEntry* Spell)
    {
        //We only care about inturrupt effects and only if they are durring a spell currently being casted
        if ((Spell->Effect[0] != SPELL_EFFECT_INTERRUPT_CAST &&
            Spell->Effect[1] != SPELL_EFFECT_INTERRUPT_CAST &&
            Spell->Effect[2] != SPELL_EFFECT_INTERRUPT_CAST) || !me->IsNonMeleeSpellCasted(false))
            return;

        //Inturrupt effect
        me->InterruptNonMeleeSpells(false);

        //Normally we would set the cooldown equal to the spell duration
        //but we do not have access to the DurationStore

        switch (CurrentNormalSpell)
        {
            case SPELL_ARCMISSLE: ArcaneCooldown = 5000; break;
            case SPELL_FIREBALL: FireCooldown = 5000; break;
            case SPELL_FROSTBOLT: FrostCooldown = 5000; break;
        }
    }
};

struct water_elementalAI : public ScriptedAI
{
    water_elementalAI(Creature *c) : ScriptedAI(c) {}

    uint32 CastTimer;

    void Reset()
    {
        CastTimer = 2000 + (rand()%3000);
    }

    void EnterCombat(Unit* /*who*/) {}

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        if (CastTimer <= diff)
        {
            DoCast(me->getVictim(), SPELL_WATERBOLT);
            CastTimer = 2000 + (rand()%3000);
        }else CastTimer -= diff;
    }
};
struct npc_shade_of_aran_blizzardAI : public ScriptedAI
{
	npc_shade_of_aran_blizzardAI(Creature *c) : ScriptedAI(c) {}


    void Reset()
	{       
		me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
	}
    void MoveInLineOfSight(Unit* /*pWho*/)
	{ }
    void AttackStart(Unit* /*pWho*/)
	{ }
    void UpdateAI(const uint32 /*uiDiff*/)
	{ }
};

CreatureAI* GetAI_npc_shade_of_aran_blizzard(Creature* pCreature)
{
    return new npc_shade_of_aran_blizzardAI(pCreature);
}


CreatureAI* GetAI_boss_aran(Creature* pCreature)
{
    return new boss_aranAI (pCreature);
}

CreatureAI* GetAI_water_elemental(Creature* pCreature)
{
    return new water_elementalAI (pCreature);
}

void AddSC_boss_shade_of_aran()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "boss_shade_of_aran";
    newscript->GetAI = &GetAI_boss_aran;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_aran_elemental";
    newscript->GetAI = &GetAI_water_elemental;
    newscript->RegisterSelf();

	newscript = new Script;
    newscript->Name = "npc_shade_of_aran_blizzard";
    newscript->GetAI = &GetAI_npc_shade_of_aran_blizzard;
    newscript->RegisterSelf();
}

