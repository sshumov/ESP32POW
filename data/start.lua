function tick()
--   print ( "hostname: " .. wifi_hostname .. " wifi status: " .. wifi_status )
--   print ( 'Hostname: ' .. wifi_hostname .. " wifi status " .. tostring(wifi_status) )
--   print ( '.' )
end
-- not pass


local ssid = read_nvram("/wifi-ssid")
local pwd  = read_nvram("/wifi-password")

print ('LUA version: ' .. _VERSION )

print ( 'Hostname: ' .. wifi_hostname .. " wifi status: " .. tostring(wifi_status) .. ' SSID: ' .. ssid .. " wifi password: " .. pwd )

local h = require("hello")
h.welcome("Petya") -- local "h"

-- pass
fp = io.open('/littlefs/test.txt','r') 
print(fp:read())
fp:close()

-- pass
--  save_nvram('testram','12569')
--  local tread = read_nvram("testram")
--  print ("Test ram read: " .. tread )

-- pass
-- dofile('/spiffs/t.lua')

-- return interval run script
return 1000
