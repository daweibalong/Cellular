num = 1; num2 = 2
function func1()
	global = 1
	print("func1", global, num, num2, num3)
	num = math.random() 
	num2 = math.random()
	base_func()
end

function func2()
	print("func2", global, num, num2, num3)
end
