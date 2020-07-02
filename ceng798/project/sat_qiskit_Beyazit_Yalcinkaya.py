from qiskit import BasicAer
from qiskit.aqua import QuantumInstance
from qiskit.aqua.algorithms import Grover
from qiskit.aqua.components.oracles import LogicalExpressionOracle

def solve():
    global constraint
    variables = sorted(list(set(filter(lambda x: "x_" in x, constraint.replace("(", "").replace(")", "").split()))))#parse variables
    number_of_variables = len(variables)#save number of variables
    clauses = constraint.split(" and ")#parse clauses
    number_of_clauses = len(clauses) + 1#save number of clauses
    inp = "p cnf " + str(number_of_variables) + " " + str(number_of_clauses) + "\n"#initialize input for automatic oracle generation from logical expression
    for c in clauses:#parse input for automatic oracle generation from logical expression
        temp = c.replace("(", "").replace(")", "").split()
        is_negated = False
        for x in temp:
            if x == "not":
                is_negated = True
            elif "x_" in x:
                if is_negated:
                    inp += "-" + x.split("_")[1] + " "
                else:
                    inp += x.split("_")[1] + " "
                is_negated = False
        inp += "0\n"
    oracle = LogicalExpressionOracle(inp)#create oracle
    grover = Grover(oracle)#initialize Grover's Algorithm
    backend = BasicAer.get_backend('qasm_simulator')#initialize backend simulator
    quantum_instance = QuantumInstance(backend, shots=1024)#create quamtum instance
    result = grover.run(quantum_instance)#run Grover's Algorithm
    solution = result["result"]#get solution
    temp = constraint
    for x in variables:
        temp = temp.replace(x, "True" if int(x.split("_")[1]) in solution else "False")
    if eval(temp):#try solution
        for x in variables:
            print(x, "=", "1" if int(x.split("_")[1]) in solution else "0")#print the solution
    else:
        print("Not Satisfiable")#solution does not exist

def main():
    global constraint
    constraint = input()#get logical expression as input
    solve()#Call routine for the solution

if __name__ == '__main__':
    main()
