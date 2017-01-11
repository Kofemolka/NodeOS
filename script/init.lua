tmr.alarm(1, 3000, tmr.ALARM_SINGLE,
  function()
    if gpio.read(3) == gpio.HIGH then
      dofile("boot.lua")
    else
      print("Hardware stop! Reboot when ready")
    end
  end)
