local module  = {}

module.SSID = {}
module.SSID["Kofemolka"] = "Kofemolki.NET"
module.SSID["Kofe.NET"] = "12341234"

module.MQTT = {}
module.MQTT.HOST = "m20.cloudmqtt.com"
module.MQTT.PORT = 17690
module.MQTT.USER = "czaauaxu"
module.MQTT.PWD = "Fg9Kmwf-1V31"
module.MQTT.ROOT = string.format("%X", node.chipid())

return module
