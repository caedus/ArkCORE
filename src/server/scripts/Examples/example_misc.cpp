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
#include "ScriptMgr.h"

enum eSay
{
    SAY_HI  = -1999925
};

class AT_example_areatrigger : public AreaTriggerScript
{
    public:

        AT_example_areatrigger()
            : AreaTriggerScript("example_areatrigger")
        {
        }

        bool OnTrigger(Player* player, AreaTriggerEntry const* /*trigger*/)
        {
            DoScriptText(SAY_HI, player);
            return true;
        }
};

class ItemUse_example_item : public ItemScript
{
    public:

        ItemUse_example_item()
            : ItemScript("example_item")
        {
        }

        bool OnUse(Player* /*player*/, Item* /*item*/, SpellCastTargets const& /*targets*/)
        {
            sScriptMgr->LoadDatabase();
            return true;
        }
};

class GOHello_example_go_teleporter : public GameObjectScript
{
    public:

        GOHello_example_go_teleporter()
            : GameObjectScript("example_go_teleporter")
        {
        }

        bool OnGossipHello(Player* player, GameObject* /*go*/)
        {
            player->TeleportTo(0, 1807.07f, 336.105f, 70.3975f, 0.0f);
            return false;
        }
};

void AddSC_example_misc()
{
    new AT_example_areatrigger();
    new ItemUse_example_item();
    new GOHello_example_go_teleporter();
}
