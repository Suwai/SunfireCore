#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "mechanar.h"
#include "Player.h"

#define SPELL_HEADCRACK 35161
#define SPELL_REFLECTIVE_DAMAGE_SHIELD 35159
#define SPELL_REFLECTIVE_MAGIC_SHIELD 35158
#define SPELL_POLARITY_SHIFT 39096
#define SPELL_BERSERK         27680
#define NPC_NETHER_CHARGE     20405
#define SAY_AGRO1  "You should split while you can."
#define SAY_KILL "Damn, I'm good!"
#define SAY_DAMAGE_SHIELD "Think you can hurt me, huh? Think I'm afraid a' you?"
#define SAY_MAGIC_SHIELD "Go ahead, gimme your best shot. I can take it!"
#define SAY_DEATH1 "Bully!"

struct boss_mechano_lord_capacitusAI : public ScriptedAI
{
    boss_mechano_lord_capacitusAI(Creature* c) : ScriptedAI(c)
    {
        pInstance = c->GetInstanceData();
    }

    ScriptedInstance *pInstance;

	bool HeroicMode;
	bool isEnraged;
	uint32 Shift_or_Shield_Timer;
	uint32 Head_Crack_Timer;
	uint32 Berserk_Timer;
	uint32 Drop_Bomb_Timer;
	uint32 Position_Check_Timer;

	void Reset()
	{
	if(HeroicMode)
		Berserk_Timer=180000; 
		Shift_or_Shield_Timer=10000;
		isEnraged=false;
		//Position_Check_Timer=5000;
		Head_Crack_Timer=5000;
		Drop_Bomb_Timer=8000;
	}

            void KilledUnit(Unit* /*victim*/)
            {
                me->Yell(SAY_KILL,LANG_UNIVERSAL,NULL);
            }

            void JustDied(Unit* /*victim*/)
            {
                 me->Yell(SAY_DEATH1, LANG_UNIVERSAL,NULL);
            }

            void UpdateAI(uint32 const diff)
            {
                if (!UpdateVictim())
                    return;


				if (Head_Crack_Timer <= diff)
				{
                            DoCastVictim(SPELL_HEADCRACK);
							Head_Crack_Timer = 15000;
				} else Head_Crack_Timer -= diff;

				if (Drop_Bomb_Timer <= diff)
				{                     
					me->SummonCreature(20405, me->GetPositionX()-rand()%20+rand()%20, me->GetPositionY()-rand()%20+rand()%20, me->GetPositionZ(),0, TEMPSUMMON_TIMED_DESPAWN, 25000);
					Drop_Bomb_Timer = 5000+rand()%8000;
				} else Drop_Bomb_Timer -= diff;
				
		if(Shift_or_Shield_Timer<diff)
		{
			if(HeroicMode)
			{
				DoCastAOE(SPELL_POLARITY_SHIFT);
			}		
		 else

			switch(rand()%2)
			{
				case 0: DoCast(me,SPELL_REFLECTIVE_DAMAGE_SHIELD); break;
				case 1: DoCast(me,SPELL_REFLECTIVE_MAGIC_SHIELD); break;
			}
			Shift_or_Shield_Timer=10000+rand()%7000;
		}else Shift_or_Shield_Timer-=diff;

                if (Berserk_Timer <= diff)
					{
                          DoCast(me, SPELL_BERSERK);
					} else Berserk_Timer -= diff;

                DoMeleeAttackIfReady();
            }
        };

CreatureAI* GetAI_boss_mechano_lord_capacitus(Creature *_Creature)
{
    return new boss_mechano_lord_capacitusAI (_Creature);
}

//----------------------------------------------------------
//Nether Charge (entry 20405)
//#define SPELL_ARCANE_EXPLOSION     42629

#define SPELL_ARCANE_EXPLOSION 27082
struct mob_nether_chargeAI : public ScriptedAI
{
    mob_nether_chargeAI(Creature *c) : ScriptedAI(c){}
	
	uint32 Explosion_Timer;
	uint32 Remove_Timer;

	void Aggro(Unit* who) 
	{}

    void AttackStart(Unit* who) 
	{}

    void MoveInLineOfSight(Unit* who) 
	{}

	void Reset()
	{                 
        Explosion_Timer=16000; 
		Remove_Timer=18000;
		me->SetReactState(REACT_PASSIVE);
    }
	void UpdateAI(const uint32 diff){
         if(Explosion_Timer<diff) {
                  me->CastSpell(me, SPELL_ARCANE_EXPLOSION, true);
				  me->CastSpell(me, SPELL_ARCANE_EXPLOSION, true);
				  me->CastSpell(me, SPELL_ARCANE_EXPLOSION, true);
				  me->CastSpell(me, SPELL_ARCANE_EXPLOSION, true);
				  me->CastSpell(me, SPELL_ARCANE_EXPLOSION, true);
		 Explosion_Timer=5000;
		 }else Explosion_Timer-= diff;

		if(Remove_Timer<diff) 
		{
			me->DealDamage(me, 9000, NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
			Remove_Timer=7000;
		}else Remove_Timer-= diff;

	}  

};

CreatureAI* GetAI_mob_nether_charge(Creature *_Creature)
{
    return new mob_nether_chargeAI (_Creature);
}


void AddSC_boss_mechano_lord_capacitus()
{
    Script *newscript;
	newscript = new Script;
    newscript->Name="boss_mechano_lord_capacitus";
    newscript->GetAI = &GetAI_boss_mechano_lord_capacitus;
    newscript->RegisterSelf();

	newscript = new Script;
    newscript->Name="mob_nether_charge";
    newscript->GetAI = &GetAI_mob_nether_charge;
    newscript->RegisterSelf();
}