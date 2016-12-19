print("Boot...")

config = require("config")
app = require("app")

env = {
	conf = config,
	broker = mqtt.Client(wifi.ap.getmac(), 120, config.MQTT.USER, config.MQTT.PWD)
}

app.init(env)

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

		mqtt_init()
    end)
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
    env.broker:subscribe("os",0, nil)
end

function mqtt_init()
	env.broker:on("message",
		function(conn, topic, data)
			if data ~= nil then
				app.onEvent(topic, data)
			end
		end)

	env.broker:connect(config.MQTT.HOST, config.MQTT.PORT, 0, 1,
		function(con)
		    register_myself()
				app.subscribe(env)
		end)
end

wifi_start()
