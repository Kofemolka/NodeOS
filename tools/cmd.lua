--list files
for k,v in pairs(file.list()) do l = string.format("%-15s",k) print(l.."   "..v.." bytes") end

--cat file
file.open( "telnet.lua", "r") while true do line = file.readline() if (line == nil) then break end print(line) end file.close()