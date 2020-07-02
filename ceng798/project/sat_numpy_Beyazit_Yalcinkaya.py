import numpy as np

def check(x):
    global N, constraint, variables, l
    temp = constraint
    assignment = format(x, '0' + str(l) + 'b')
    for i in range(len(assignment)):
        temp = temp.replace(variables[i], "True" if assignment[i] == "1" else "False")
    return 1 if eval(temp) else 0

def oracle(s):
    global N, constraint, variables, l
    return {i:((-1)**check(i))*s[i] for i in s}#this oracle can be used for all logical expressions; however, it takes N time

def grover():
    global N, constraint, variables, l
    state = {i:1/np.sqrt(N) for i in range(N)}#states after Hadamard transform
    M = np.matrix([[2/N for i in range(N)] for j in range(N)]) - np.identity(N)#(H)(U_{0}^{\perp})(H) matrix
    for i in range(int(np.sqrt(N)) if N != 4 else 1):#Grover's iteration
        state = oracle(state)#applying the phase inversion operator
        A = np.matrix([[state[i]] for i in state])#matrix with alpha values
        temp = M*A#(H)(U_{0}^{\perp})(H)(A) matrix
        state = {i:temp.item(i, 0) for i in state}#applying the inversion about the mean operator
    solution = format(max(state, key=state.get), '0' + str(l) + 'b')#measure the solution
    temp = constraint
    for i in range(l):
        temp = temp.replace(variables[i], solution[i])
    if eval(temp):#try solution
        for i in range(l):
            print(variables[i], "=", solution[i])#print the solution
    else:
        print("Not Satisfiable")#solution does not exist

def main():
    global N, constraint, variables, l
    constraint = input()#get logical expression as input
    variables = sorted(list(set(filter(lambda x: "x_" in x, constraint.replace("(", "").replace(")", "").split()))))#parse variables
    l = len(variables)#save number of varibales
    N = 2**l#N for Grover's Algorithm
    grover()#Call routine for Grover's Algorithm

if __name__ == '__main__':
    main()
