print(package.path)
print(package.cpath)

local inspect = require('inspect')

local luatest = {}
function luatest.es_init()
	es_debug("Loaded luatest.lua from data dir :)")
	luatest.days = 0
	print(inspect(_G))
end

function luatest.es_daily()
	luatest.days = luatest.days + 1
	system_name = player:GetSystem():Name()
	player_name = player:FirstName().." "..player:LastName()
	es_addMsg("Lua: day(s): "..tostring(luatest.days)..", Name: "..player_name..", System: "..system_name, 0)

--	print(player:IsDead())
--	print(player:GetPreviousSystem():Name())
--	print(player:GetPlanet():Name())
--	player:SetName("Captain", "Jeng")
--	account = player:Accounts()
--	print(account:Credits())
--	print(account:AddCredits(10))
--	print(account:Credits())
end

function luatest.es_add_crew(crew_count)
	system_name = player:GetSystem():Name()
	print("Hired crew:", crew_count, system_name)
end

function luatest.es_load(node)
	print(node:Token(0))
	print(node:Token(1))
	print(node:HasChildren())
	print(node:Size())
	print(node.BoolValue)
	print(node.Tokens)
	print(node.PrintTrace)
	print(node.Value) -- why is this a nil value?
	print(node.IsNumber) -- why is this a nil value?
	print(node.IsBool) -- why is this a nil value?
	print(node.GetChildren) -- why is this a nil value?

--	local children = node:GetChildren()
--	for i, child in ipairs(children) do
--		print(child:Token(0))
--	end

end

function luatest.es_save(out)
	out:WriteString("Hello")

--local writer = ES.DataWriter("output.txt")
--writer:WriteString("Hello")
--writer:WriteDouble(3.14)
--writer:BeginChild()
--writer:WriteInt(42)
--writer:EndChild()
--writer:WriteComment("This is a comment")
--writer:SaveToPath("newoutput.txt")

end

return luatest
