/*
 * BlizzLikeCore integrates as part of this file: CREDITS.md and LICENSE.md
 */

/* ScriptData
Name: Instance_Hellfire_Ramparts
Complete(%): 95
Comment:
Category: Hellfire Ramparts
EndScriptData */

#include "ScriptPCH.h"
#include "hellfire_ramparts.h"

#define ENCOUNTERS    3

struct instance_ramparts : public ScriptedInstance
{
    instance_ramparts(Map *map) : ScriptedInstance(map){Initialize();}

    uint32 Encounter[ENCOUNTERS];
    std::string str_data;

    void Initialize()
    {
        for (uint8 i = 0; i < ENCOUNTERS; i++)
            Encounter[i] = NOT_STARTED;
    }

    bool IsEncounterInProgress() const
    {
        for (uint8 i = 0; i < ENCOUNTERS; i++)
            if (Encounter[i] == IN_PROGRESS) return true;

        return false;
    }

    void OnCreatureCreate(Creature* /*pCreature*/, bool /*add*/) { }

    void OnGameObjectCreate(GameObject* /*pGo*/, bool /*add*/) { }

    Player* GetPlayerInMap()
    {
        Map::PlayerList const& players = instance->GetPlayers();

        if (!players.isEmpty())
        {
            for (Map::PlayerList::const_iterator itr = players.begin(); itr != players.end(); ++itr)
            {
                if (Player* plr = itr->getSource())
                    return plr;
            }
        }

        debug_log("BSCR: Instance Hellfire Ramparts: GetPlayerInMap, but PlayerList is empty!");
        return NULL;
    }

    void SetData(uint32 type, uint32 data)
    {
        switch(type)
        {
            case DATA_GARGOLMAR:
                if (Encounter[0] != DONE)
                    Encounter[0] = data;
                break;
            case DATA_OMOR:
                if (Encounter[1] != DONE)
                    Encounter[1] = data;
                break;
            case DATA_VAZRUDEN:
                if (Encounter[2] != DONE)
                    Encounter[2] = data;
				break;
        }

        if (data == DONE)
        {
            SaveToDB();
            OUT_SAVE_INST_DATA_COMPLETE;
        }
    }

    uint32 GetData(uint32 type)
    {
        switch(type)
        {
            case DATA_GARGOLMAR: return Encounter[0];
            case DATA_OMOR: return Encounter[1];
            case DATA_VAZRUDEN: return Encounter[2];
        }
        return false;
    }

    uint64 GetData64(uint32 /*identifier*/)
    {
        return 0;
    }

    std::string GetSaveData()
    {
        OUT_SAVE_INST_DATA;
        std::ostringstream saveStream;

        saveStream << Encounter[0] << " " << Encounter[1] << " " << Encounter[2];

        char* out = new char[saveStream.str().length() + 1];
        strcpy(out, saveStream.str().c_str());
        if (out)
        {
            OUT_SAVE_INST_DATA_COMPLETE;
            return out;
        }

        return str_data.c_str();
    }

    void Load(const char* in)
    {
        if (!in)
        {
            OUT_LOAD_INST_DATA_FAIL;
            return;
        }

        OUT_LOAD_INST_DATA(in);

        std::istringstream loadStream(in);
        loadStream >> Encounter[0] >> Encounter[1] >> Encounter[2];

        for (uint8 i = 0; i < ENCOUNTERS; ++i)
            if (Encounter[i] == IN_PROGRESS)
                Encounter[i] = NOT_STARTED;

        OUT_LOAD_INST_DATA_COMPLETE;
    }
};
InstanceData* GetInstanceData_instance_ramparts(Map* pMap)
{
    return new instance_ramparts(pMap);
}

void AddSC_instance_ramparts()
{
    Script* pNewScript;
    pNewScript = new Script;
    pNewScript->Name = "instance_ramparts";
    pNewScript->GetInstanceData = &GetInstanceData_instance_ramparts;
    pNewScript->RegisterSelf();
}
