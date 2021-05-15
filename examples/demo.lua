print("-= Demo script for lua4gba =-\n")
print("*** Math test")
print("sqrt(101) = " .. math.sqrt(101))
print("2^100 = " .. 2 ^ 100 .. "\n")
print("*** Function assignement test")
test = function(a, b) return a * 2 + b * 2 end
print("test(5, 7) = " .. test(5, 7))
print("type(test) = " .. type(test) .. "\n")
print("*** Table test")
t = { "ab", "cd", 5, 8, 13 }
print("t = " .. tostring(t))
print("type(t) = " .. type(t))
table.foreachi(t, print)
