local module = {}

local settingKey = "LedState"
local subTopic = "/led"
local ledPin = 0

function module.init(env)
  module.env = env

  local state = env.settings.get(settingKey, gpio.LOW)

  gpio.mode(ledPin,gpio.OUTPUT)
  gpio.write(ledPin,state)
end

function module.subscribe(env)
--  env.broker:subscribe(env.conf.MQTT.ROOT .. subTopic,0, nil)
end

function module.onEvent(topic, data)
  if topic == subTopic then
    local state = gpio.LOW
    if data == "ON" then
      state = gpio.HIGH
    end

    gpio.write(ledPin,state)
    module.env.settings.set(settingKey, state)
  end
end

return module
