local t = {}
t.defaultname = "world"
function t.welcome(name)
    print("Hello, " .. (name or t.defaultname) .. "!")
end
return t
