print("Boot...")

local config = require("config")
local app = require("app")

function wifiConnect()
	wifi.setmode(wifi.STATION)
	wifi.sta.config(config.SSID, config.SSPWD)

	print("SSID:")
	print(config.SSID)
	print("PWD:")
	print(config.SSPWD)
	
	wifi.sta.connect()
	tmr.delay(1000*1000)

	print("Connecting to WiFi...")

	local connected = false
	local retries = 10
	while( connected ~= true and retries > 0)
	do 
		tmr.delay(500*1000)
   
		local ip = wifi.sta.getip()	 
		if ip ~= nil
		then       
			print(ip)
			print(wifi.sta.status())
			connected = true
		end
   
		retries = retries - 1
	end
end

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

wifiConnect()

checkFirmware()

app.loop()