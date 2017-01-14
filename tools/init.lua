wifi.setmode(wifi.STATION);
wifi.sta.config("Kofemolka","Kofemolki.NET")
wifi.sta.connect()
tmr.alarm(1,500,tmr.ALARM_AUTO,function()
if wifi.sta.getip()~=nil then
  print("IP:".. wifi.sta.getip())
  require("telnet").start()
  tmr.stop(1)
end end)
