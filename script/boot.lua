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

wifiConnect()

app.loop()