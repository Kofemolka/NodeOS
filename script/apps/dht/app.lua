local module = {}

local tempTopic = "/temp"
local humTopic = "/hum"
local sensPin = 1

function module.init(env)
  tmr.alarm(3, 60000, tmr.ALARM_AUTO,
    function()
      status, temp, humi, temp_dec, humi_dec = dht.read(sensPin)
      if status == dht.OK then
        local resTemp = string.format("%d.%d", math.floor(temp),  temp_dec/100)
        local resHum = string.format("%d.%d", math.floor(humi), humi_dec/100)
        print("Temperature: " .. resTemp)
        print("Humidity: " .. resHum)

        pcall( function() env.broker:publish(env.conf.MQTT.ROOT .. tempTopic,resTemp,0,0, nil) end )
        pcall( function() env.broker:publish(env.conf.MQTT.ROOT .. humTopic,resHum,0,0, nil) end )
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
