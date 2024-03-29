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

#include "AuthCodes.h"

namespace AuthHelper
{
    bool IsPreCataAcceptedClientBuild(int build)
    {
        int accepted_versions[] = PRE_CATA_ACCEPTED_CLIENT_BUILD;

        for (int i = 0; accepted_versions[i]; ++i)
            if (build == accepted_versions[i])
                return true;

        return false;
    }

    bool IsPostCataAcceptedClientBuild(int build)
    {
        int accepted_versions[] = POST_CATA_ACCEPTED_CLIENT_BUILD;

        for (int i = 0; accepted_versions[i]; ++i)
            if (build == accepted_versions[i])
                return true;

        return false;
    }

    bool IsAcceptedClientBuild(int build)
    {
        return (IsPostCataAcceptedClientBuild(build) || IsPreCataAcceptedClientBuild(build));
    }
};
