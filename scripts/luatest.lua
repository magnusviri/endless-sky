local inspect = require('inspect')

local luatest = {}
function luatest.es_init()
    es_debug("Loaded plugin luatest.lua :)")
	luatest.days = 0
	print(inspect(_G))
end

function luatest.es_die()
    es_debug("Die")
	print("Die")
end

function luatest.es_daily()
	luatest.days = luatest.days + 1
	system_name = player:GetSystem():Name()
	player_name = player:FirstName().." "..player:LastName()
    es_addMsg("Lua: day(s): "..tostring(luatest.days)..", Name: "..player_name..", System: "..system_name, 0)

	print(player:IsDead())
	print(player:GetPreviousSystem():Name())
	print(player:GetPlanet():Name())
	player:SetName("Captain", "Jeng")
	account = player:Accounts()
	print(account:Credits())
	print(account:AddCredits(10))
	print(account:Credits())
end

function luatest.es_add_crew(crew_count)
	system_name = player:GetSystem():Name()
	print("Hired crew:", crew_count, system_name)
end
return luatest
