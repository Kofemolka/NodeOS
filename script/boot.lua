print("Boot...")

local config = require("config")
local app = require("app")

local env = {
	conf = config,
}

app.init(env)

tmr.alarm(1,1000,tmr.ALARM_AUTO, 
 function()
	app.loop()
 end)
  
function checkFirmware()
	local http = require("socket.http")
	local remoteVer = http.request(config.SERVER .. "ver")
	
	if not remoteVer 
	then
		return
	end

	local verFile = io.open("ver", "r")
	local localVer = 0
	if verFile ~= nil
	then
		localVer = verFile:read()
		verFile:close()
	end
	
	if localVer == remoteVer
	then
		return
	end

	local body = http.request(config.SERVER .. "app.lua")
	if not body 
	then
		return
	end

	local f = assert(io.open("app.lua", "w")) -- open in "binary" mode
	f:write(body)
	f:close()

	verFile = assert(io.open("ver", "w"))
	verFile:write(remoteVer)
	verFile:close()

	node.compile("app.lua")
	node.restart()
end

