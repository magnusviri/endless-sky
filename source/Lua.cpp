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
#include "Files.h"
#include "GameData.h"
#include "Logger.h"
#include "LuaImpl.h"
#include "LuaPlugin.h"

#include "PlayerInfo.h"
#include "System.h"


#include <iostream>
#include <vector>
#include <filesystem>

using namespace std;

namespace fs = std::filesystem;

namespace {
	lua_State *L;

	vector<LuaPlugin> plugins;
}

void report_errors(lua_State *L, int status) {
    if (status == 0) {
        return;
    }

    std::cerr << "[LUA ERROR] " << lua_tostring(L, -1) << std::endl;

    // remove error message from Lua state
    lua_pop(L, 1);
}

void Lua::dumpstack()
{
	printf("Dumping Stack:\n");
	int top = lua_gettop(L);
	for(int i = 1; i <= top; i++)
	{
		printf("%d\t%s\t", i, luaL_typename(L, i));
		switch(lua_type(L, i))
		{
		case LUA_TNUMBER:
			printf("%g\n", lua_tonumber(L, i));
			break;
		case LUA_TSTRING:
			printf("%s\n", lua_tostring(L, i));
			break;
		case LUA_TBOOLEAN:
			printf("%s\n", (lua_toboolean(L, i) ? "true" : "false"));
			break;
		case LUA_TNIL:
			printf("%s\n", "nil");
			break;
		default:
			printf("%p\n", lua_topointer(L, i));
			break;
		}
	}
}

lua_State *Lua::get()
{
	return L;
}

bool Lua::registerPlayer(PlayerInfo &player)
{
	luabridge::push(L, &player);
	lua_setglobal(L, "player");
	return true;
}

// System* GetSystemWrapper(const PlayerInfo& playerInfo) {
//     return new System(playerInfo.GetSystem());
// }

bool Lua::init()
{
	L = luaL_newstate();
	luaL_openlibs(L);

	LuaImpl::registerAll();

    luabridge::getGlobalNamespace(L)
		.beginNamespace("ES")
			.beginClass<System>("System")
				.addFunction("Name", &System::Name)
			.endClass()
			.beginClass<PlayerInfo>("PlayerInfo")
				.addFunction("IsDead", &PlayerInfo::IsDead)
				.addFunction("FirstName", &PlayerInfo::FirstName)
				.addFunction("LastName", &PlayerInfo::LastName)
				.addFunction("GetSystem", &PlayerInfo::GetSystem)
// 				.addFunction("GetPreviousSystem", &PlayerInfo::GetPreviousSystem)
// 				.addFunction("GetPlanet", &PlayerInfo::GetPlanet)
// 				.addFunction("GetStellarObject", &PlayerInfo::GetStellarObject)
// 				.addFunction("Land", &PlayerInfo::Land)
// 				.addFunction("TakeOff", &PlayerInfo::TakeOff)
// 				.addFunction("SetSystem", &PlayerInfo::SetSystem)
// 				.addFunction("SetPlanet", &PlayerInfo::SetPlanet)
// 				.addFunction("SetName", &PlayerInfo::SetName)
// 				.addFunction("IncrementDate", &PlayerInfo::IncrementDate)
			.endClass()
        .endNamespace();
	return true;
}

void Lua::close()
{
	lua_close(L);
}

bool Lua::loadSource(const string &name, const string &path)
{
	fs::path p = path;
	std::string script_path = p.parent_path().parent_path().string() + "/scripts/"+ name;


	std::cout << "script: " << script_path << std::endl;

	if(Files::Exists(script_path))
	{
		const auto truePath = script_path;
		const auto loaded = Files::Read(truePath);
		luaL_loadstring(L, loaded.c_str());
		lua_call(L, 0, 1);

		plugins.emplace_back();
	} else {
		std::cout << "Could not find " << script_path << std::endl;
	}
	return true;
}

void Lua::runDailyScripts()
{
	for(auto &plugin : plugins)
	{
		luabridge::LuaRef addAndDouble = luabridge::getGlobal(L, "addAndDouble");
		luabridge::LuaResult res = addAndDouble(15, 12);

		plugin.runDaily();
	}
}

void Lua::runInitScripts()
{
	for(auto &plugin : plugins)
	{
		luabridge::LuaRef addAndDouble = luabridge::getGlobal(L, "addAndDouble");
		luabridge::LuaResult res = addAndDouble(15, 12);


		plugin.runInit();
	}
}
