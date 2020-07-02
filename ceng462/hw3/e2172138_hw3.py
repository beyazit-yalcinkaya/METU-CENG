"""
    Beyazit Yalcinkaya
    2172138
"""

def is_variable(x):
    return (("+" not in x)
        and ("(" not in x)
        and (")" not in x)
        and ("," not in x)
        and ("~" not in x)
        and x.islower())

def is_constant(x):
    return (("+" not in x)
        and ("(" not in x)
        and (")" not in x)
        and ("," not in x)
        and ("~" not in x)
        and x.isupper())

def is_parenthesis_matching(x):
    stack = 0
    for i in x:
        if i == "(":
            stack += 1
        elif i == ")":
            stack -= 1
    return stack == 0

def parse(x, c):
    s = ""
    out = []
    for i in x:
        if i == c and is_parenthesis_matching(s):
            out.append(s)
            s = ""
        else:
            s += i
    out.append(s)
    return out

def get_args(x):
    return parse(x[x.index("(") + 1: x.rindex(")")], ",")

def subs(x, theta):
    for s in theta:
        x = x.replace(*s)
    return x

def unify(x, y, theta=[]):
    if (is_variable(x)
     or is_constant(x)
     or is_variable(y)
     or is_constant(y)):
        if x == y:
            return True
        elif is_variable(x) and is_variable(y):
            if x in y:
                return False
            else:
                theta.append((y, x))
                return True
        elif is_variable(x):
            if x in y:
                return False
            else:
                theta.append((x, y))
                return True
        elif is_variable(y):
            if y in x:
                return False
            else:
                theta.append((y, x))
                return True
        else:
            return False
    elif x[:x.index("(")] != y[:y.index("(")]:
        return False
    else:
        x_args = get_args(x)
        y_args = get_args(y)
        if len(x_args) != len(y_args):
            return False
        for i in range(len(x_args)):
            s = []
            if not unify(x_args[i], y_args[i], s):
                return False
            if s != []:
                x_args = map(lambda t: subs(t, s), x_args)
                y_args = map(lambda t: subs(t, s), y_args)
                theta.extend(s)
        return True

def unify_for_resolution(x, y, theta=[]):
    x = parse(x, "+")
    y = parse(y, "+")
    for x_i in x:
        neg_x_i = 0
        for i in x_i:
            if i == "~":
                neg_x_i += 1
            else:
                break
        for y_i in y:
            if x_i == y_i:
                continue
            neg_y_i = 0
            for i in y_i:
                if i == "~":
                    neg_y_i += 1
                else:
                    break
            if (((neg_x_i - neg_y_i) % 2 == 1)
             and unify(x_i[neg_x_i:], y_i[neg_y_i:], theta)):
                return True
    return False

def resolution(x, y, theta=[]):
    x = subs(x, theta)
    y = subs(y, theta)
    s = list(set(parse(x, "+") + parse(y, "+")))
    resolvent = []
    for s_i in s:
        neg_s_i = 0
        for i in s_i:
            if i == "~":
                neg_s_i += 1
            else:
                break
        f = True
        for s_j in s:
            if s_i == s_j:
                continue
            neg_s_j = 0
            for i in s_j:
                if i == "~":
                    neg_s_j += 1
                else:
                    break
            if (((neg_s_i - neg_s_j) % 2 == 1)
             and (s_i[neg_s_i:] == s_j[neg_s_j:])):
                f = False
                break
        if f:
            resolvent.append(s_i)
    if resolvent == []:
        return "empty"
    return "+".join(resolvent)

def eliminate_tautologies(s):
    s = list(set(filter(lambda x: x != "", s)))
    new_s = []
    for p in s:
        p = list(set(parse(p, "+")))
        new_p = []
        for p_i in p:
            f = True
            neg_p_i = 0
            for i in p_i:
                if i == "~":
                    neg_p_i += 1
                else:
                    break
            for p_j in p:
                if p_i == p_j:
                    continue
                neg_p_j = 0
                for i in p_j:
                    if i == "~":
                        neg_p_j += 1
                    else:
                        break
                if (((neg_p_i - neg_p_j) % 2 == 1)
                 and (p_i[neg_p_i:] == p_j[neg_p_j:])):
                    f = False
                    break
            if f:
                new_p.append(p_i)
        if new_p != []:
            new_s.append("+".join(new_p))
    return new_s

def is_subsumed(p, q):
    p = set(parse(p, "+"))
    q = set(parse(q, "+"))
    if set(map(lambda x: x[:x.index("(")], p)) >= set(map(lambda x: x[:x.index("(")], q)):
        q_subsumes = set()
        for p_i in p:
            for q_i in q:
                theta = []
                if unify(p_i, q_i, theta):
                    if p_i != subs(q_i, theta):
                        return False
                    else:
                        q_subsumes.add(q_i)
        return q == q_subsumes
    return False

def eliminate_subsumptions(s):
    new_s = []
    for p in s:
        f = True
        for q in s:
            if p != q:
                if is_subsumed(p, q) and is_subsumed(q, p):
                    f = False
                    if p not in new_s and q not in new_s:
                        new_s.append(p)
                    break
                elif is_subsumed(p, q):
                    f = False
                    break
        if f:
            new_s.append(p)
    return new_s

def theorem_prover(knowledge_base, negated_goals):
    resolutions = []
    set_of_support = eliminate_subsumptions(eliminate_tautologies(negated_goals))
    knowledge_base = eliminate_subsumptions(eliminate_tautologies(knowledge_base + negated_goals))
    if set_of_support == []:
        return ("yes", [])
    while True:
        new_set_of_support = []
        for p in set_of_support:
            for q in knowledge_base:
                if p == q:
                    continue
                theta = []
                if unify_for_resolution(p, q, theta):
                    r = resolution(p, q, theta)
                    resolutions.append(p + "$" + q + "$" + r)
                    if r == "empty":
                        return ("yes", resolutions)
                    if r not in set_of_support:
                        new_set_of_support.append(r)
        if new_set_of_support == []:
            break;
        knowledge_base = eliminate_subsumptions(eliminate_tautologies(knowledge_base + set_of_support))
        set_of_support = eliminate_subsumptions(eliminate_tautologies(new_set_of_support))
    return ("no", [])

