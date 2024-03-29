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
#include "old_hillsbrad.h"

#define SAY_ENTER                   -1560000
#define SAY_TAUNT1                  -1560001
#define SAY_TAUNT2                  -1560002
#define SAY_SLAY1                   -1560003
#define SAY_SLAY2                   -1560004
#define SAY_DEATH                   -1560005

#define SPELL_HOLY_LIGHT            29427
#define SPELL_CLEANSE               29380
#define SPELL_HAMMER_OF_JUSTICE     13005
#define SPELL_HOLY_SHIELD           31904
#define SPELL_DEVOTION_AURA         8258
#define SPELL_CONSECRATION          38385

class boss_captain_skarloc : public CreatureScript
{
public:
    boss_captain_skarloc() : CreatureScript("boss_captain_skarloc") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_captain_skarlocAI (creature);
    }

    struct boss_captain_skarlocAI : public ScriptedAI
    {
        boss_captain_skarlocAI(Creature* c) : ScriptedAI(c)
        {
            pInstance = c->GetInstanceScript();
        }

        InstanceScript *pInstance;

        uint32 Holy_Light_Timer;
        uint32 Cleanse_Timer;
        uint32 HammerOfJustice_Timer;
        uint32 HolyShield_Timer;
        uint32 DevotionAura_Timer;
        uint32 Consecration_Timer;

        void Reset()
        {
            Holy_Light_Timer = 20000 + rand()%10000;
            Cleanse_Timer = 10000;
            HammerOfJustice_Timer = 20000 + rand()%15000;
            HolyShield_Timer = 240000;
            DevotionAura_Timer = 3000;
            Consecration_Timer = 8000;
        }

        void EnterCombat(Unit* /*who*/)
        {
            //This is not correct. Should taunt Thrall before engage in combat
            DoScriptText(SAY_TAUNT1, me);
            DoScriptText(SAY_TAUNT2, me);
        }

        void KilledUnit(Unit* /*victim*/)
        {
            DoScriptText(RAND(SAY_SLAY1, SAY_SLAY2), me);
        }

        void JustDied(Unit* /*victim*/)
        {
            DoScriptText(SAY_DEATH, me);

            if (pInstance && pInstance->GetData(TYPE_THRALL_EVENT) == IN_PROGRESS)
                pInstance->SetData(TYPE_THRALL_PART1, DONE);
        }

        void UpdateAI(const uint32 diff)
        {
            //Return since we have no target
            if (!UpdateVictim())
                return;

            //Holy_Light
            if (Holy_Light_Timer <= diff)
            {
                DoCast(me, SPELL_HOLY_LIGHT);
                Holy_Light_Timer = 30000;
            } else Holy_Light_Timer -= diff;

            //Cleanse
            if (Cleanse_Timer <= diff)
            {
                DoCast(me, SPELL_CLEANSE);
                Cleanse_Timer = 10000;
            } else Cleanse_Timer -= diff;

            //Hammer of Justice
            if (HammerOfJustice_Timer <= diff)
            {
                DoCast(me->getVictim(), SPELL_HAMMER_OF_JUSTICE);
                HammerOfJustice_Timer = 60000;
            } else HammerOfJustice_Timer -= diff;

            //Holy Shield
            if (HolyShield_Timer <= diff)
            {
                DoCast(me, SPELL_HOLY_SHIELD);
                HolyShield_Timer = 240000;
            } else HolyShield_Timer -= diff;

            //Devotion_Aura
            if (DevotionAura_Timer <= diff)
            {
                DoCast(me, SPELL_DEVOTION_AURA);
                DevotionAura_Timer = 45000 + rand()%10000;
            } else DevotionAura_Timer -= diff;

            //Consecration
            if (Consecration_Timer <= diff)
            {
                //DoCast(me->getVictim(), SPELL_CONSECRATION);
                Consecration_Timer = 5000 + rand()%5000;
            } else Consecration_Timer -= diff;

            DoMeleeAttackIfReady();
        }
    };

};

void AddSC_boss_captain_skarloc()
{
    new boss_captain_skarloc();
}
