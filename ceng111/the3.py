from random import choice

################ hyphenate function ################

def hyphenate(inp_word):

	vowels = ['a', 'A', 'e', 'E', 'i', 'I', 'o', 'O', 'u', 'U']
	out = []
	pivot = 0

	while pivot < len(inp_word):
		if inp_word[pivot] in vowels:
			pivot += 1
			if pivot < len(inp_word):
				while inp_word[pivot] not in vowels:
					if pivot == len(inp_word) - 1:
						break
					else:
						pivot += 1
						if inp_word[pivot] not in vowels:
							continue
						else:
							out.append(inp_word[:pivot-1])
							inp_word = inp_word[pivot-1:]
							pivot = 0
							break
				else:
					out.append(inp_word[:pivot])
					inp_word = inp_word[pivot:]
					pivot = 0
			else:
				out.append(inp_word)
				break
		else:
			if pivot == len(inp_word) - 1:
				out.append(inp_word)
				break
			else:
				pivot += 1

	return out

################ helper converter function ################

def converter(inp_dict):

	if '.' in inp_dict and ' ' in inp_dict:
		inp_dict['.'] = list(set(inp_dict['.'].keys() + inp_dict[' '].keys()))
	elif '.' in inp_dict and ' ' not in inp_dict:
		inp_dict['.'] = inp_dict['.'].keys()

	for key in inp_dict:

		if key == '.':
			continue
		else:
			if len(inp_dict[key])>2:
				k = []
				f_most_repeated=max(inp_dict[key], key=inp_dict[key].get)
				for i in inp_dict[key]:
					if inp_dict[key][i] == inp_dict[key][f_most_repeated] and i != f_most_repeated:
						k.append(i)
					else:
						continue
				del inp_dict[key][f_most_repeated]
				if k == []:
					s_most_repeated = max(inp_dict[key], key=inp_dict[key].get)
					k.append(s_most_repeated)
					for i in inp_dict[key]:
						if inp_dict[key][i] == inp_dict[key][s_most_repeated] and i != s_most_repeated:
							k.append(i)
						else:
							continue
				inp_dict[key]= k + [f_most_repeated]
			else:
				inp_dict[key]=inp_dict[key].keys()

	return inp_dict

################ execute function ################

def execute():

################ getting input text ################

	n, m = map(int, raw_input().split())
	inp=[' ']
	lst=[]

	while inp[-1] != '=':
		inp.extend(raw_input().split())

	del inp[-1]

	for word in range(2, 2*len(inp)-2, 2):
		if inp[word-1][-1] == '.':
			inp.insert(word, '')
		else:
			inp.insert(word, ' ')

	for item in inp:
		if '.' in item:
			temp = item.split('.')
			for place in range(1, len(temp)*2-1, 2):
				temp.insert(place, '.')
			inp[inp.index(item)] = temp

	for item in inp:
		if type(item)==type(['']):
			inp=inp[:inp.index(item)]+item+inp[inp.index(item)+1:]
			
	for item in inp:
		lst.extend(hyphenate(item))

################ generating dictionary of dictionaries ################

	dict_of_dicts={}
	temp_dict={}

	for i in lst:
		temp_dict[i]=[]
		dict_of_dicts[i]={}

	for i in range(len(lst)-1):
		temp_dict[lst[i]].append(lst[i+1])

	for key in temp_dict:
		for item in temp_dict[key]:
			dict_of_dicts[key].update({item:temp_dict[key].count(item)})

################ generation output text ################

	dict_of_lists=converter(dict_of_dicts)
	a=[choice(dict_of_lists['.'])]

	while True:
		temp_str = ''.join(a)
		if len(temp_str)>m and (len(temp_str.split())>n or len(temp_str.split('.'))>n) and temp_str[-1]=='.':
			break
		a.append(choice(dict_of_lists[a[-1]]))

	print ''.join(a)
