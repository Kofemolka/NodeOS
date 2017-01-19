local module = {}

local modeTopic = "/mode"
local rgbTopic = "/rgb"
local delayTopic = "/delay"
local setMODE = "MODE"
local setRGB = "RGB"
local setDELAY = "DELAY"
local pinR = 1
local pinG = 2
local pinB = 6

local OFF = "off"
local CLR = "clr"
local RUN = "run"

function module.init(env)
  module.env = env

  pwm.setup(pinR, 1000, 1023)
  pwm.setup(pinG, 1000, 1023)
  pwm.setup(pinB, 1000, 1023)
  pwm.start(pinR)
  pwm.start(pinG)
  pwm.start(pinB)

  module.delay = env.set.get(setDELAY, 100)
  module.mode = env.set.get(setMODE, OFF)
  module.onModeChanged(module.mode)

  module.rgb = env.set.get(setRGB, "#FFFFFF")
  module.onRGBChanged(module.rgb)
end

function module.subscribe(sub)
    sub(modeTopic, module.onModeChanged)
    sub(rgbTopic, module.onRGBChanged)
    sub(delayTopic, module.onDelayChanged)
end

function module.onModeChanged(d)
  print("Mode: " .. d)
  if d == OFF then
      tmr.stop(4)
      module.setClr(0,0,0)
  elseif d == CLR then
      tmr.stop(4)
  elseif d == RUN then
    tmr.alarm(4, module.delay, tmr.ALARM_AUTO, module.doRun)
  else return end

  module.mode = d
  module.env.set.set(setMODE, d)

  module.onRGBChanged(module.rgb)
end

function module.onRGBChanged(d)
  if d == nil then return end
  module.rgb = d
  module.env.set.set(setRGB, d)

  if module.mode ~= CLR then return end

  r = tonumber(string.sub(d,2,3),16)
  g = tonumber(string.sub(d,4,5),16)
  b = tonumber(string.sub(d,6,7),16)

  module.setClr(r,g,b)
end

function module.onDelayChanged(d)
  local d = tonumber(d)
  if d == nil or d < 5 or d > 100 then return end

  module.delay = d
  module.env.set.set(setDELAY, d)
  module.onModeChanged(module.mode)
end

function module.setClr(r, g, b)
  pwm.setduty(pinR, r*4)
  pwm.setduty(pinG, g*4)
  pwm.setduty(pinB, b*4)
end

local runState = 1
local runR = 255
local runG = 0
local runB = 0
function module.doRun()
  if runState == 1 then -- 255 G+ 0
    runG = runG + 1
    if runG == 255 then runState = 2 end
  elseif runState == 2 then -- R- 255 0
    runR = runR - 1
    if runR == 0 then runState = 3 end
  elseif runState == 3 then -- 0 255 B+
    runB = runB + 1
    if runB == 255 then runState = 4 end
  elseif runState == 4 then -- 0 G- 255
    runG = runG - 1
    if runG == 0 then runState = 5 end
  elseif runState == 5 then -- R+ 0 255
    runR = runR + 1
    if runR == 255 then runState = 6 end
  elseif runState == 6 then -- 255 0 B-
    runB = runB - 1
    if runB == 0 then runState = 1 end
  end

  module.setClr(runR, runG, runB)
end

return module
