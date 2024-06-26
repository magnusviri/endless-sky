local luatest = {}
function luatest.es_init()
    es_debug("Loaded plugin luatest.lua :)")
	luatest.days = 0
end

function luatest.es_daily()
	luatest.days = luatest.days + 1
	system_name = player:GetSystem():Name()
	player_name = player:FirstName().." "..player:LastName()
    es_addMsg("Lua: day(s): "..tostring(luatest.days)..", Name: "..player_name..", System: "..system_name, 0)

	print(player:IsDead())
	print(player:GetPreviousSystem():Name())
	print(player:GetPlanet():Name())
--	print(player:GetSystem():VisibleNeighbors())
end

return luatest
