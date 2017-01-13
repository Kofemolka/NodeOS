gpio.mode(0,gpio.OUTPUT)
gpio.write(0,0)

tmr.alarm(1, 1000, tmr.ALARM_SINGLE,
  function()
    gpio.write(0,1)
    if gpio.read(3) == gpio.HIGH then
      dofile("boot.lua")
    else
      print("Hardware stop! Reboot when ready")
    end
  end)
