from sys import setrecursionlimit
setrecursionlimit(7400)
def query(decision_tree,variable_value):
	func_dict = {'==' : lambda lst : lst[0] == lst[1], '!=' : lambda lst : lst[0] != lst[1], '<' : lambda lst : lst[0] < lst[1], '>' : lambda lst : lst[0] > lst[1], 'in' : lambda lst : lst[0] in lst[1], 'and' : lambda lst: reduce(lambda x, y : x and y, lst), 'or' : lambda lst: reduce(lambda x, y : x or y, lst), 'not' : lambda lst : not lst[0]}
	var_dict = {}
	for i in range(len(variable_value)):
		var_dict.update({variable_value[i][0] : variable_value[i][1]})
	def eval_func(lst):
		if type(lst) == type([]) and lst[0] in func_dict.keys():
			return func_dict[lst[0]](map(eval_func, lst[1:]))
		else:
			if type(lst) == type('string') and lst in var_dict.keys():
				return var_dict[lst]
			else:
				return lst
	def action(inp):
		if type(inp) != type([]):
			return inp
		if eval_func(inp[0]) == True:
			return action(inp[1])
		else:
			return action(inp[2])
	return action(decision_tree)