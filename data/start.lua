function tick()
--   print ( "hostname: " .. wifi_hostname .. " wifi status: " .. wifi_status )
--   print ( 'Hostname: ' .. wifi_hostname .. " wifi status " .. tostring(wifi_status) )
--   print ( '.' )

 --    local err,d_h,d_m,d_s,d_y,d_mn,d_d,dw = ntp.sync(2.0,1)
 --     print ( 'Date: ' .. d_y .. '-' .. d_mn .. '-' .. d_d .. ' ' .. d_h .. ':' .. d_m .. ':' .. d_s .. '---')
-- local sec,min,hr,day,month,year = rtc.get()

-- print ("Date: " .. year .. "-" .. month .. "-" .. day .. " " .. hr .. ":" .. min .. ":" .. sec)

end
function get_pow()
	local vrmsa,vrmsb,vrmsc,irmsa,irmsb,irmsc,apowera,apowerb,apoerc,th1,th2,th3 = top323.get()

end

local ssid = read_nvram("/wifi-ssid")
local pwd  = read_nvram("/wifi-password")

print ('LUA version: ' .. _VERSION )

print ( 'Hostname: ' .. wifi_hostname .. " wifi status: " .. tostring(wifi_status) .. ' SSID: ' .. ssid .. " wifi password: " .. pwd )
top323.init()

-- pass
-- local h = require("hello")
-- h.welcome("Petya") -- local "h"

-- pass
-- fp = io.open('/littlefs/test.txt','r') 
-- print(fp:read())
-- fp:close()

-- pass
-- local t = require "ntp"
-- t.begin()
-- ntp.begin()

-- pass
--  save_nvram('testram','12569')
--  local tread = read_nvram("testram")
--  print ("Test ram read: " .. tread )

-- pass
-- dofile('/littlefs/file.lua')

-- return interval run script
-- ntp.begin()


return 1000
