print("Boot...")

local devTopic = "/dev/"
local resetTopic = devTopic .. "reset"
local heapTopic = devTopic .. "heap"
local lwtTopic = devTopic .. "lwt"
local firmTopic = devTopic .. "firmware"
local firmStatusTopic = firmTopic .. "/status"
local temp = "_temp_"

config = require("config")

pcall( function() app = require("app") end )

env = {
	conf = config,
	settings = require("settings"),
	broker = mqtt.Client(config.MQTT.ROOT, 120, config.MQTT.USER, config.MQTT.PWD)
}

pcall( function() app.init(env) end )

function cnc(fileName)
  local sum = 0
  if file.open(fileName, "r") == nil then
    return nil
  end

  while true
  do
    line = file.readline()
    if line == nil then
      break
    end

    for i = 1, string.len(line) do
      sum = sum + string.byte(line, i)
      if sum > 65535 then
        sum = sum - 65535
      end
    end
  end

  file.close()

  return sum
end

function updateFirmware(data)
  print("Updating...")
	local state, fileName, origCnc = nil
	local firstLine = true

	for line in string.gmatch(data, "[^\r\n]+") do
		if firstLine then
			local split = string.gmatch(line, "[^%s]+")
			state = split(0)
			fileName = split(1)
			origCnc = split(2)
			print("State:" .. state)
			print("File Name:" .. fileName)
			print("Origin CNC: " .. origCnc)

			firstLine = false

			if state == "b" then
				file.open(temp, "w")
			else
				file.open(temp, "a")
			end
		else
			file.writeline(line)
		end
	end

	file.close()

	if state == "e" then
			local fileCnc = cnc(temp)

			print("File CNC: " .. fileCnc)

			if tonumber(origCnc) == tonumber(fileCnc) then
				file.rename(temp, fileName)
				env.broker:publish(env.conf.MQTT.ROOT .. firmStatusTopic,"Updated: " .. fileName,0,0, nil)
				tmr.alarm(1, 1000, tmr.ALARM_SINGLE,
					function() node.restart() end)
			else
				env.broker:publish(env.conf.MQTT.ROOT .. firmStatusTopic,"Invalid CNC: O=" .. origCnc .. " F=" .. fileCnc,0,0, nil)
				file.remove(temp)
			end
	end
end

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
			print("Connecting to [" .. ssid .. "]")
	    wifi.sta.connect()
			tmr.alarm(1, 2500, tmr.ALARM_AUTO, wifiWaitIP)
			return
		end
	end

	tmr.alarm(1, 5000, tmr.ALARM_AUTO, wifiConnect)
end

function wifiConnect()
	print("WiFi Connect")
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
				print("MSG> " .. subTopic .. ":" .. data)
				if subTopic == resetTopic then node.restart() end
				if subTopic == firmTopic then updateFirmware(data) end

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

wifiConnect()
