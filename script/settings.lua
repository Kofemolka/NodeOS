local m  = {}
local st = "kvstorage"
local c = {}
local function loadStorage()
  if file.open(st,"r")==nil then return end
  while true
  do
    l=file.readline()
    if l==nil then
      break
    end
    k,v=l:match("([^,]+),([^,]+)")
    if k~=nil then
      c[k]=string.gsub(v, "\n", "")
    end
  end
  file.close()
end
function m.save()
  file.open(st, "w")
  for k,v in pairs(c) do
    file.writeline(k .. "," .. v)
  end
  file.close()
end
function m.set(k, v)
  local u=c[k]~=v
  c[k]=v
  if u then m.save() end
end
function m.get(k,d)
  if c[k]==nil then m.set(k, d) end
  return c[k]
end
loadStorage()
return m
