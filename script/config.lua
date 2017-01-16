local module  = {}

module.SSID = {}
module.SSID["Kofemolka"] = "Kofemolki.NET"
module.SSID["Kofe.NET"] = "12341234"

module.TelNet = {}
module.TelNet.Enabled = true
module.TelNet.Port = 23

module.MQTT = {}
module.MQTT.HOST = "192.168.0.104"
module.MQTT.PORT = 1883
module.MQTT.USER = "ESP"
module.MQTT.PWD = "1234"
--module.MQTT.HOST = "m20.cloudmqtt.com"
--module.MQTT.PORT = 17690
--module.MQTT.USER = "czaauaxu"
--module.MQTT.PWD = "Fg9Kmwf-1V31"
module.MQTT.ROOT = string.format("%06X", node.chipid())

return module
