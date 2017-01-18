local module = {}

local modeTopic = "/mode"
local rgbTopic = "/rgb"
local setMODE = "MODE"
local setRGB = "RGB"
local pinR = 2
local pinG = 1
local pinB = 4

local OFF = "off"
local CLR = "clr"
local RUN = "run"

function module.init(env)
  module.env = env

  pwm.setup(pinR, 1000, 256)
  pwm.setup(pinG, 1000, 256)
  pwm.setup(pinB, 1000, 256)
  pwm.start(pinR)
  pwm.start(pinG)
  pwm.start(pinB)

  module.mode = env.set.get(setMODE, OFF)
  module.onModeChanged(module.mode)

  module.rgb = env.set.get(setRGB, "#FFFFFF")
  module.onRGBChanged(module.rgb)
end

function module.subscribe(sub)
    sub(modeTopic, module.onModeChanged)
    sub(rgbTopic, module.onRGBChanged)
end

function module.onModeChanged(d)
  print("Mode: " .. d)
  if d == OFF then
      tmr.stop(3)
      module.setClr(0,0,0)
  elseif d == CLR then
      tmr.stop(3)
  elseif d == RUN then
    tmr.alarm(3, 10, tmr.ALARM_AUTO, module.doRun)
  else return end

  module.mode = d
  module.env.set.set(setMODE, d)

  module.onRGBChanged(module.rgb)
end

function module.onRGBChanged(d)
  module.rgb = d
  module.env.set.set(setRGB, d)

  if module.mode ~= CLR then return end

  r = tonumber(string.sub(d,2,3),16)
  g = tonumber(string.sub(d,4,5),16)
  b = tonumber(string.sub(d,6,7),16)

  module.setClr(r,g,b)
end

function module.setClr(r, g, b)
  --print("R: " .. r .. " G: " .. g .. " B: " .. b)

  pwm.setduty(pinR, r)
  pwm.setduty(pinG, g)
  pwm.setduty(pinB, b)
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
