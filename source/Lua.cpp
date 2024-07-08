/* Lua.cpp
Copyright (c) 2023 by Daniel Yoon

Endless Sky is free software: you can redistribute it and/or modify it under the
terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later version.

Endless Sky is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
this program. If not, see <https://www.gnu.org/licenses/>.
*/

#include "Lua.h"

#include "ConditionsStore.h"
#include "GameData.h"
#include "Logger.h"
#include "LuaPlugin.h"

#include <iostream>
#include <vector>

using namespace std;

namespace {
	vector<LuaPlugin> plugins;
	PlayerInfo* _player;
}

void Lua::registerPlayer(PlayerInfo &player)
{
	_player = &player;
}

void Lua::loadSource(const std::string &name, const std::string &path)
{
    plugins.emplace_back(name, path, _player);
}

void Lua::runAddCrew(int crewCount)
{
	for(auto &plugin : plugins)
	{
		plugin.runAddCrew(crewCount);
	}
}

void Lua::runDailyScripts()
{
	for(auto &plugin : plugins)
	{
		plugin.runDaily();
	}
}

void Lua::runInitScripts()
{
	for(auto &plugin : plugins)
	{
		plugin.runInit();
	}
}

void Lua::runLoadScripts(const DataNode &node)
{
	for(auto &plugin : plugins)
	{
		plugin.runLoad(node);
	}
}

void Lua::runSaveScripts(DataWriter &out)
{
	for(auto &plugin : plugins)
	{
		plugin.runSave(out);
	}
}
