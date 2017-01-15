print("<Boot>")

local _DEV="/dev/"
local _RESET=_DEV .. "reset"
local _HEAP=_DEV .. "heap"
local _STATUS=_DEV .. "status"
local _TN=_DEV .. "telnet"
local _IP=_DEV .. "ip"

pcall(function() app=require("app") end)

cfg = require("config")
broker = mqtt.Client(cfg.MQTT.ROOT,120,cfg.MQTT.USER,cfg.MQTT.PWD)
telnet = require("telnet")
subs={}

function sub(t,c)
	subs[t]=c
	broker:subscribe(cfg.MQTT.ROOT .. t,0,nil)
	--print("sub>" .. t)
end

function pub(t,d,q,r)
	if r==nil then r=0 end
	if q==nil then q=0 end
	pcall(function() broker:publish(cfg.MQTT.ROOT .. t,d,q,r,nil) end)
	--print("pub>" .. t .. ":" .. d)
end

env={
	set = require("settings"),
	sub = sub,
	pub = pub
}

pcall(function() app.init(env) end)

function wifiWatchDog()
	tmr.alarm(1,30000,tmr.ALARM_AUTO,
		function()
			if wifi.sta.getip()==nil then
				wifiConnect()
			end
		end)
end

function wifiWaitIP()
  if wifi.sta.getip()==nil then
    print("Registering...")
  else
    tmr.stop(1)
    print("MAC:" .. wifi.ap.getmac())
    print("IP:".. wifi.sta.getip())
		mqttInit()
		wifiWatchDog()
		enableTelNet(cfg.TelNetEnabled)
  end
end

function findAP(t)
	for ssid,v in pairs(t) do
		if cfg.SSID[ssid]~=nil then
			wifi.sta.config(ssid,cfg.SSID[ssid])
			print("AP [" .. ssid .. "]")
	    wifi.sta.connect()
			tmr.alarm(1,2500,tmr.ALARM_AUTO,wifiWaitIP)
			return
		end
	end

	tmr.alarm(1,5000,tmr.ALARM_AUTO,wifiConnect)
end

function wifiConnect()
	print("WiFi Connect")
  wifi.setmode(wifi.STATION);
	wifi.sta.sethostname("Node-" .. cfg.MQTT.ROOT)
  wifi.sta.getap(findAP)
end

function enableTelNet(enable)
	if enable then
		telnet.start()
 	 pub(_STATUS, "TelNet ON")
  else
 	 telnet.stop()
 	 pub(_STATUS, "TelNet OFF")
  end
end

local once=0
function mqttInit()
	broker:lwt(cfg.MQTT.ROOT .. _STATUS,"offline",1,1)
	broker:on("message",
		function(c,t,d)
			local st=string.sub(t,string.len(cfg.MQTT.ROOT)+1)
			if subs[st]~=nil then
				subs[st](d)
			end
		end)

broker:on("connect",
	function(con)
		print("MQTT connected")
		sub(_RESET,function(d) node.restart() end)
		sub(_TN,function(d) enableTelNet(d=="on") end)
		pcall(function() app.subscribe(sub) end)

		pub(_STATUS,"online")
		pub(_IP,tostring(wifi.sta.getip()),0,1)
		tmr.alarm(2,60000,tmr.ALARM_AUTO,
			function()
				pub(_HEAP,node.heap())
			end)
	end)

	if once==0 then
		print("MQTT connect...")
		broker:connect(cfg.MQTT.HOST,cfg.MQTT.PORT,0,1,nil)
		once=1
	end
end

wifiConnect()
