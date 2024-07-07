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

#include "Files.h"
#include "Logger.h"
#include "LuaImpl.h"
#include "PlayerInfo.h"

#include <filesystem>
#include <functional>
#include <luabridge3/LuaBridge/LuaBridge.h>

namespace {
	using ParamAdder = std::function<int(lua_State*)>;

	void setLuaPackagePaths(lua_State* L, const std::filesystem::path& plugin_dir)
	{
		// Get Lua version
		std::string lua_version = LUA_VERSION_MAJOR "." LUA_VERSION_MINOR;

		std::filesystem::path scripts_dir = plugin_dir / "scripts";

		// Construct package.path
// 		std::string packagePath = "package.path = package.path .. ';";
		std::string packagePath = "package.path = '";
		packagePath += (scripts_dir / "share/lua" / lua_version / "?.lua").string() + ";";
		packagePath += (scripts_dir / "share/lua" / lua_version / "?/init.lua").string() + ";";
		packagePath += (scripts_dir / "lib/lua" / lua_version / "?.lua").string() + ";";
		packagePath += (scripts_dir / "lib/lua" / lua_version / "?/init.lua").string() + ";";
		packagePath += (scripts_dir / "?.lua").string() + ";";
		packagePath += (scripts_dir / "?/init.lua").string() + "'";

		// Set package.path
		luaL_dostring(L, packagePath.c_str());

		// Construct package.cpath (for C modules)
// 		std::string packageCPath = "package.cpath = package.cpath .. ';";
		std::string packageCPath = "package.cpath = '";
		packageCPath += (scripts_dir / "lib/lua" / lua_version / "?.so").string() + ";";
		packageCPath += (scripts_dir / "lib/lua" / lua_version / "?.dll").string() + "'";

		// Set package.cpath
		luaL_dostring(L, packageCPath.c_str());
	}

	int lua_fn_field_ref(lua_State *L, const char *fieldname)
	{
		const auto field_ty = lua_getfield(L, 1, fieldname);
		if(field_ty == LUA_TFUNCTION)
			return luaL_ref(L, LUA_REGISTRYINDEX);
		else if(field_ty != LUA_TNIL)
			Logger::LogError("Expected lua fn in field " + std::string(fieldname) + ", got: " + lua_typename(L, field_ty));
		return LUA_NOREF;
	}

	void runRawChecked(lua_State *L, int raw, const char* funcName, ParamAdder paramAdder = nullptr)
	{
		if (!L || lua_status(L) != LUA_OK)
		{
			Logger::LogError("Lua state is bad");
			return;
		}
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

LuaPlugin::LuaPlugin(const std::string &name, const std::string &path, PlayerInfo *player)
	: Lpointer(luaL_newstate())
{
	if (!Lpointer) {
		Logger::LogError("Failed to create Lua state");
		return;
	}
	lua_State* L = Lpointer.get();

	plugin_dir = fs::path(path).parent_path().parent_path();
	fs::path script_path = plugin_dir / "scripts" / name;

	if (! Files::Exists(script_path.string()))
	{
		Logger::LogError("Error, could not find Lua script: " + script_path.string());
		return;
	}

	try {
		luaL_openlibs(L);
		LuaImpl::registerAll(L);
		const auto loaded = Files::Read(script_path.string());
		setLuaPackagePaths(L, plugin_dir);

		// Load the script
		if (luaL_loadstring(L, loaded.c_str()) != LUA_OK) {
			Logger::LogError("Error loading Lua script: " + std::string(lua_tostring(L, -1)));
			lua_pop(L, 1);
			return;
		}

		// Run the script
		if (lua_pcall(L, 0, 1, 0) != LUA_OK) {
			Logger::LogError("Error executing Lua script: " + std::string(lua_tostring(L, -1)));
			lua_pop(L, 1);
			return;
		}

		// Set global player variable
		auto result = luabridge::Stack<PlayerInfo*>::push(L, player);
		if (!result) {
			Logger::LogError("Failed to set global 'player' variable in Lua state: " + result.message());
			return;
		}
		lua_setglobal(L, "player");

		// Safely get function references
		this->init = lua_fn_field_ref(L, "es_init");
		this->daily = lua_fn_field_ref(L, "es_daily");
		this->addCrew = lua_fn_field_ref(L, "es_add_crew");

	} catch (const std::exception& e) {
		Logger::LogError("LuaPlugin initialization error: " + std::string(e.what()));
		lua_close(L);
		Lpointer.release();
	}
}

void LuaPlugin::runDaily()
{
	runRawChecked(Lpointer.get(), daily, "es_daily");
}

void LuaPlugin::runInit()
{
	runRawChecked(Lpointer.get(), init, "es_init");
}

void LuaPlugin::runAddCrew(int crewCount)
{
	auto addCrewParams = [crewCount](lua_State *L) -> int {
		lua_pushinteger(L, crewCount);
		return 1;
	};
	runRawChecked(Lpointer.get(), addCrew, "es_add_crew", addCrewParams);
}
