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

#ifndef _REALMLIST_H
#define _REALMLIST_H

#include <ace/Singleton.h>
#include <ace/Null_Mutex.h>
#include "Common.h"

// Storage object for a realm
struct Realm
{
    std::string address;
    std::string name;
    uint8 icon;
    uint8 color;
    uint8 timezone;
    uint32 m_ID;
    AccountTypes allowedSecurityLevel;
    float populationLevel;
    uint32 gamebuild;
};

/// Storage object for the list of realms on the server
class RealmList
{
public:
    typedef std::map<std::string, Realm> RealmMap;

    RealmList();
    ~RealmList() {}

    void Initialize(uint32 updateInterval);

    void UpdateIfNeed();

    void AddRealm(Realm NewRealm) {m_realms[NewRealm.name] = NewRealm;}

    RealmMap::const_iterator begin() const { return m_realms.begin(); }
    RealmMap::const_iterator end() const { return m_realms.end(); }
    uint32 size() const { return m_realms.size(); }

private:
    void UpdateRealms(bool init=false);
    void UpdateRealm(uint32 ID, const std::string& name, const std::string& address, uint32 port, uint8 icon, uint8 color, uint8 timezone, AccountTypes allowedSecurityLevel, float popu, uint32 build);

    RealmMap m_realms;
    uint32   m_UpdateInterval;
    time_t   m_NextUpdateTime;
};

#define sRealmList ACE_Singleton<RealmList, ACE_Null_Mutex>::instance()
#endif
