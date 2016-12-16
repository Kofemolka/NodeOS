print("Boot...")

local config = require("config")
local app = require("app")

local env = {
	conf = config,
}

function wifi_wait_ip()  
  if wifi.sta.getip()== nil then
    print("IP unavailable, Waiting...")
  else
    tmr.stop(1)
    print("\n====================================")
    print("ESP8266 mode is: " .. wifi.getmode())
    print("MAC address is: " .. wifi.ap.getmac())
    print("IP is "..wifi.sta.getip())
    print("====================================")
	mqtt_start()
    app.init(env)
  end
end

function wifi_start()        
    wifi.setmode(wifi.STATION);
    wifi.sta.config(config.SSID,config.SSPWD)
    wifi.sta.connect()
    print("Connecting to " .. config.SSID .. " ...")
    --config.SSID = nil  -- can save memory
    tmr.alarm(1, 2500, 1, wifi_wait_ip) 
end

function register_myself()  
    m:subscribe(config.ENDPOINT .. config.ID,0,function(conn)
        print("Successfully subscribed to data endpoint")
    end)
end

function mqtt_start()  
    m = mqtt.Client(wifi.ap.getmac(), 120)
	env.mqtt = m
    -- register message callback beforehand
    m:on("message", function(conn, topic, data) 
      if data ~= nil then
        print(topic .. ": " .. data)
        -- do something, we have received a message
      end
    end)
    -- Connect to broker
    m:connect(config.HOST, config.PORT, 0, 1, function(con) 
        register_myself()
        -- And then pings each 1000 milliseconds
        tmr.stop(6)
        tmr.alarm(6, 1000, 1, function() 
			m:publish("outbox/ping", node.chipid(),0,0)
		end)
    end) 

end

wifi_start()
  
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

