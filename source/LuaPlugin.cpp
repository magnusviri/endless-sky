/* LuaPlugin.cpp
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

#include "LuaPlugin.h"

#include "Logger.h"
#include <functional>

using namespace std;

namespace {
	using ParamAdder = std::function<int(lua_State*)>;

	int lua_fn_field_ref(lua_State *L, const char *fieldname)
	{
		const auto field_ty = lua_getfield(L, 1, fieldname);
		if(field_ty == LUA_TFUNCTION)
			return luaL_ref(L, LUA_REGISTRYINDEX);
		else if(field_ty != LUA_TNIL)
			Logger::LogError(string("Expected lua fn in field ") + fieldname + ", got: " + lua_typename(L, field_ty));
		return LUA_NOREF;
	}

	void runRawChecked(lua_State *L, int raw, const char* funcName, ParamAdder paramAdder = nullptr) {
		if(raw != LUA_NOREF)
		{
			lua_rawgeti(L, LUA_REGISTRYINDEX, raw);
			int parameterCount = paramAdder ? paramAdder(L) : 0;
			int result = lua_pcall(L, parameterCount, 0, 0);
			if (result != LUA_OK)
			{
				const char* errorMsg = lua_tostring(L, -1);
				Logger::LogError(string("Error calling ") + funcName + ": " + (errorMsg ? errorMsg : "Unknown error"));
				lua_pop(L, 1);
			}
		}
	}
}

LuaPlugin::LuaPlugin()
{
	auto L = Lua::get();
	this->daily = lua_fn_field_ref(L, "es_daily");
	this->init = lua_fn_field_ref(L, "es_init");
	this->addCrew = lua_fn_field_ref(L, "es_add_crew");
}

void LuaPlugin::runDaily()
{
	runRawChecked(Lua::get(), daily, "es_daily");
}

void LuaPlugin::runInit()
{
	runRawChecked(Lua::get(), init, "es_init");
}

void LuaPlugin::runAddCrew(int crewCount)
{
	auto addCrewParams = [crewCount](lua_State *L) -> int {
		lua_pushinteger(L, crewCount);
		return 1;
	};

	runRawChecked(Lua::get(), addCrew, "es_add_crew", addCrewParams);
}
