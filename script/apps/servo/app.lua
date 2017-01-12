local module = {}

local settingKey = "ServoPos"
local subTopic = "/pos"
local servoPin = 2

function module.init(env)
  module.env = env

  local pos = env.settings.get(settingKey, 70)

  pwm.setup(servoPin, 50, pos)
  pwm.start(servoPin)
end

function module.subscribe(env)
  env.broker:subscribe(env.conf.MQTT.ROOT .. subTopic,0, nil)
end

function module.onEvent(topic, data)
  if topic == subTopic then
    local duty = tonumber(data)
    if duty >= 0 and duty <= 123 then
      pwm.setduty(servoPin, duty)
      module.env.settings.set(settingKey, duty)
    end
  end
end

return module
