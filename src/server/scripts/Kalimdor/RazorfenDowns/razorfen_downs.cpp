/*
 * Copyright (C) 2005 - 2011 MaNGOS <http://www.getmangos.org/>
 *
 * Copyright (C) 2008 - 2011 TrinityCore <http://www.trinitycore.org/>
 *
 * Copyright (C) 2011 ArkCORE <http://www.arkania.net/>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "ScriptPCH.h"
#include "razorfen_downs.h"

/*###
# npc_henry_stern
####*/

enum eEnums
{
    SPELL_GOLDTHORN_TEA                         = 13028,
    SPELL_TEACHING_GOLDTHORN_TEA                = 13029,
    SPELL_MIGHT_TROLLS_BLOOD_POTION             = 3451,
    SPELL_TEACHING_MIGHTY_TROLLS_BLOOD_POTION   = 13030,
    GOSSIP_TEXT_TEA_ANSWER                      = 2114,
    GOSSIP_TEXT_POTION_ANSWER                   = 2115,
};

#define GOSSIP_ITEM_TEA     "Teach me the cooking recipe"
#define GOSSIP_ITEM_POTION  "Teach me the alchemy recipe"

class npc_henry_stern : public CreatureScript
{
public:
    npc_henry_stern() : CreatureScript("npc_henry_stern") { }

    bool OnGossipSelect (Player* player, Creature* creature, uint32 /*uiSender*/, uint32 uiAction)
    {
        player->PlayerTalkClass->ClearMenus();
        if (uiAction == GOSSIP_ACTION_INFO_DEF + 1)
        {
            player->CastSpell(player, SPELL_TEACHING_GOLDTHORN_TEA, true);
            player->SEND_GOSSIP_MENU(GOSSIP_TEXT_TEA_ANSWER, creature->GetGUID());
        }

        if (uiAction == GOSSIP_ACTION_INFO_DEF + 2)
        {
            player->CastSpell(player, SPELL_TEACHING_MIGHTY_TROLLS_BLOOD_POTION, true);
            player->SEND_GOSSIP_MENU(GOSSIP_TEXT_POTION_ANSWER, creature->GetGUID());
        }

        return true;
    }

    bool OnGossipHello (Player* player, Creature* creature)
    {
        if (player->GetBaseSkillValue(SKILL_COOKING) >= 175 && !player->HasSpell(SPELL_GOLDTHORN_TEA))
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_TEA, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

        if (player->GetBaseSkillValue(SKILL_ALCHEMY) >= 180 && !player->HasSpell(SPELL_MIGHT_TROLLS_BLOOD_POTION))
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_POTION, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);

        player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
        return true;
    }

};

/*######
## go_gong
######*/

class go_gong : public GameObjectScript
{
public:
    go_gong() : GameObjectScript("go_gong") { }

    bool OnGossipHello(Player* /*player*/, GameObject* pGO)
    {
        //basic support, not blizzlike data is missing...
        InstanceScript* pInstance = pGO->GetInstanceScript();

        if (pInstance)
        {
            pInstance->SetData(DATA_GONG_WAVES, pInstance->GetData(DATA_GONG_WAVES)+1);
            return true;
        }

        return false;
    }

};

enum eTombCreature
{
    SPELL_WEB                   = 745
};

class npc_tomb_creature : public CreatureScript
{
public:
    npc_tomb_creature() : CreatureScript("npc_tomb_creature") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_tomb_creatureAI (creature);
    }

    struct npc_tomb_creatureAI : public ScriptedAI
    {
        npc_tomb_creatureAI(Creature* creature) : ScriptedAI(creature)
        {
            pInstance = creature->GetInstanceScript();
        }

        InstanceScript* pInstance;

        uint32 uiWebTimer;

        void Reset()
        {
            uiWebTimer = urand(5000, 8000);
        }

        void UpdateAI(const uint32 uiDiff)
        {
            if (!UpdateVictim())
                return;

            //from acid
            if (me->GetEntry() == CREATURE_TOMB_REAVER)
            {
                if (uiWebTimer <= uiDiff)
                {
                    DoCast(me->getVictim(), SPELL_WEB);
                    uiWebTimer = urand(7000, 16000);
                } else uiWebTimer -= uiDiff;
            }

            DoMeleeAttackIfReady();
        }

        void JustDied(Unit* /*killer*/)
        {
            if (pInstance)
                pInstance->SetData(DATA_GONG_WAVES, pInstance->GetData(DATA_GONG_WAVES)+1);
        }
    };

};

void AddSC_razorfen_downs()
{
    new npc_henry_stern();
    new go_gong();
    new npc_tomb_creature();
}
