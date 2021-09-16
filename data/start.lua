function tick()
--   print ( "hostname: " .. wifi_hostname .. " wifi status: " .. wifi_status )
--   print ( 'Hostname: ' .. wifi_hostname .. " wifi status " .. tostring(wifi_status) )
--   print ( '.' )
end

local ssid = read_nvram("/wifi-ssid")
local pwd  = read_nvram("/wifi-password")

print ( 'Hostname: ' .. wifi_hostname .. " wifi status " .. tostring(wifi_status) .. ' SSID: ' .. ssid .. " wifi password: " .. pwd )

save_nvram('testram','12569')
local tread = read_nvram("testram")
print ("Test ram read: " .. tread )

-- return interval run script
return 50
