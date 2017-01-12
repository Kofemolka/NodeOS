local module = {}

local firmTopic = "/dev/firmware"
local appFile = "app.lua"

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

function module.subscribe(env)
  env.broker:subscribe(env.conf.MQTT.ROOT .. firmTopic,0, nil)
end

function module.onEvent(topic, data)
  if topic == firmTopic then
    update(data)
    return true
  end

  return false
end

return module
