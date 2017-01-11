print("Boot...")

local devTopic = "/dev/"
local resetTopic = devTopic .. "reset"
local heapTopic = devTopic .. "heap"
local lwtTopic = devTopic .. "lwt"
local firmTopic = devTopic .. "firmware"
local appFile = "app.lua"

config = require("config")

pcall( function() app = require("app") end )

env = {
	conf = config,
	broker = mqtt.Client(config.MQTT.ROOT, 120, config.MQTT.USER, config.MQTT.PWD)
}

pcall( function() app.init(env) end )

function wifiWatchDog()
	tmr.alarm(1, 30000, tmr.ALARM_AUTO,
		function()
			if wifi.sta.getip()== nil then
				wifiConnect()
			end
		end)
end

function wifiWaitIP()
  if wifi.sta.getip()== nil then
    print("Waiting for IP ...")
  else
    tmr.stop(1)
    print("MAC: " .. wifi.ap.getmac())
    print("IP: ".. wifi.sta.getip())

		mqttInit()
		wifiWatchDog()
  end
end

function findAP(t)
	for ssid,v in pairs(t) do
		if config.SSID[ssid] ~= nil then
			wifi.sta.config(ssid,config.SSID[ssid])
			print("Connecting to " .. ssid .. " ...")
	    wifi.sta.connect()
			tmr.alarm(1, 2500, tmr.ALARM_AUTO, wifiWaitIP)
			return
		end
	end

	tmr.alarm(1, 5000, tmr.ALARM_AUTO, wifiConnect)
end

function wifiConnect()
	print("WiFi Connect ...")
  wifi.setmode(wifi.STATION);
  wifi.sta.getap(findAP)
end

local once = false
function mqttInit()
	env.broker:lwt(env.conf.MQTT.ROOT .. lwtTopic, "offline", 1, 1)
	env.broker:on("message",
		function(conn, topic, data)
			if data ~= nil then
				local subTopic = string.sub(topic, string.len(config.MQTT.ROOT)+1)
				if subTopic == resetTopic then node.restart() end
				if subTopic == firmTopic then update(data) end 

				pcall( function() app.onEvent(subTopic, data) end )
			end
		end)

env.broker:on("connect",
	function(con)
		print("MQTT connected")
		env.broker:subscribe(env.conf.MQTT.ROOT .. resetTopic,0, nil)
		env.broker:subscribe(env.conf.MQTT.ROOT .. firmTopic,0, nil)
		pcall( function() app.subscribe(env) end )

		tmr.alarm(2, 60000, tmr.ALARM_AUTO,
			function()
				pcall( function() env.broker:publish(env.conf.MQTT.ROOT .. heapTopic,node.heap(),0,0, nil) end )
			end)
	end)

	if not once then
		print("MQTT connect...")
		env.broker:connect(config.MQTT.HOST, config.MQTT.PORT, 0, 1, nil)

		once = true
	end
end

local function update(data)
  print("Updating...")
  file.open(appFile, "w")
  file.write(data)
  file.close()

  local countdown = 5
  tmr.alarm(2, 1000, tmr.ALARM_AUTO,
    function()
      print("Restarting in " .. countdown)
      countdown = countdown - 1
      if countdown < 0 then
        node.restart()
      end
    end)
end

wifiConnect()
