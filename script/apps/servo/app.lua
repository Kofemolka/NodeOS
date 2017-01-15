local module = {}

local settingKey = "ServoPos"
local subTopic = "/pos"
local servoPin = 2

function module.init(env)
  module.env = env
  local pos = env.set.get(settingKey, 70)

  pwm.setup(servoPin, 50, pos)
  pwm.start(servoPin)
end

function module.subscribe(sub)
  sub(subTopic, module.moveServo)
end

function module.moveServo(d)
  local duty = tonumber(d)  
  if duty >= 0 and duty <= 123 then
    pwm.setduty(servoPin, duty)
    module.env.set.set(settingKey, duty)
  end
end

return module
