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

#include "Lua.h"

#include <string>

class LuaPlugin
{
public:
	LuaPlugin();

	void runDaily();
	void runInit();
	void runDie();

private:
	int daily = LUA_NOREF;
	int init = LUA_NOREF;
	int die = LUA_NOREF;
};
