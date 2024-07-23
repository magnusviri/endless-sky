/* LuaPlugin.h
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

#ifndef ES_LUA_PLUGIN_H_
#define ES_LUA_PLUGIN_H_

#include "PlayerInfo.h"
#include <filesystem>
#include <string>

extern "C"
{
	#include <lauxlib.h>
	#include <lua.h>
	#include <lualib.h>
}

using namespace std;

namespace fs = std::filesystem;

class DataNode;
class DataWriter;

class LuaPlugin
{
public:
	LuaPlugin(const std::string &name, const std::string &path, PlayerInfo *player);
	~LuaPlugin() = default;

	LuaPlugin(LuaPlugin&&) = default;
	LuaPlugin& operator=(LuaPlugin&&) = default;

	LuaPlugin(const LuaPlugin&) = delete;
	LuaPlugin& operator=(const LuaPlugin&) = delete;

	void runAddCrew(int crewCount);
	void runBoarding(int crewCount);
	void runDaily();
	void runInit();
	void runLoad(const DataNode &node);
	void runSave(DataWriter &out) const;

	const std::string& getName() const { return name; }

private:
	struct LuaDeleter {
		void operator()(lua_State* Lpointer) const {
			if (Lpointer) lua_close(Lpointer);
		}
	};

	std::unique_ptr<lua_State, LuaDeleter> Lpointer;
	fs::path plugin_dir;
	std::string name;
	int addCrew = LUA_NOREF;
	int boarding = LUA_NOREF;
	int daily = LUA_NOREF;
	int init = LUA_NOREF;
	int load = LUA_NOREF;
	int save = LUA_NOREF;
};

#endif // ES_LUA_PLUGIN_H_
