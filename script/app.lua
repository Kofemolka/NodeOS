local module = {}

local subTopic = "/led"

function module.init(env)
	print("App start")

	gpio.mode(4,gpio.OUTPUT)
end

function module.subscribe(env)
	env.broker:subscribe(env.conf.MQTT.ROOT .. subTopic,0, nil)
end

function module.onEvent(topic, data)
	if topic == subTopic then
		if data == "1" then
			gpio.write(4,gpio.HIGH)
		else
			gpio.write(4,gpio.LOW)
		end
	end
end

return module
