local module = {}

local tempTopic = "/temp"
local humTopic = "/hum"
local sensPin = 10

function module.init(env)
  tmr.alarm(3, 6000, tmr.ALARM_AUTO,
    function()
      status, temp, humi, temp_dec, humi_dec = dht.read(sensPin)
      if status == dht.OK then
        print(string.format("DHT Temperature:%d.%03d;Humidity:%d.%03d\r\n",
          math.floor(temp),
          temp_dec,
          math.floor(humi),
          humi_dec
        ))

        pcall( function() env.broker:publish(env.conf.MQTT.ROOT .. tempTopic,temp,0,0, nil) end )
        pcall( function() env.broker:publish(env.conf.MQTT.ROOT .. humTopic,humi,0,0, nil) end )
      elseif status == dht.ERROR_CHECKSUM then
          print( "DHT Checksum error." )
      elseif status == dht.ERROR_TIMEOUT then
          print( "DHT timed out." )
      end
    end)
end

function module.subscribe(env)

end

function module.onEvent(topic, data)

end

return module
