import copy

is_ida_star = False
m = 0
k = 0
initial_state = []
goal_state = []

class Node(object):
    def __init__(self, state, parent, path_cost, f):
        self.state = state
        self.parent = parent
        self.path_cost = path_cost
        self.f = f

def manhattan_distance(state):
    global k, goal_state
    total = 0
    n = k * k
    for i in range(n):
        temp = state[i]
        if temp == '_':
            continue
        j = goal_state.index(temp)
        total += abs((i % k) - (j % k)) + abs((i // k) - (j // k))
    return total

def get_input():
    global is_ida_star, m, k, initial_state, goal_state
    if raw_input() == "IDA*":
        is_ida_star = True
    else:
        is_ida_star = False
    m = int(raw_input())
    k = int(raw_input())
    for i in range(k):
        temp = raw_input().split()
        initial_state.extend(temp)
    for i in range(k):
        temp = raw_input().split()
        goal_state.extend(temp)

def print_path(node):
    global k
    path = []
    while node:
        path.append(node.state)
        node = node.parent
    path = path[::-1]
    for state in path:
        for i in range(k):
            for j in range(k):
                print state[i * k + j], 
            print
        print

def get_current_node(open_list):
    current_node = open_list[0]
    for i in open_list:
        if i.f < current_node.f:
            current_node = i
    return current_node

def get_children_states(state):
    global k
    blank = state.index('_')
    children = []
    i = blank % k
    j = blank // k
    if j < k - 1:
        # up exists
        temp = copy.deepcopy(state)
        pos = ((blank // k) + 1) * k + (blank % k)
        temp[blank], temp[pos] = temp[pos], temp[blank]
        children.append(temp)
    if j > 0:
        # down exists
        temp = copy.deepcopy(state)
        pos = ((blank // k) - 1) * k + (blank % k)
        temp[blank], temp[pos] = temp[pos], temp[blank]
        children.append(temp)
    if i < k - 1:
        # left exists
        temp = copy.deepcopy(state)
        pos = blank + 1
        temp[blank], temp[pos] = temp[pos], temp[blank]
        children.append(temp)
    if i > 0:
        # right exists
        temp = copy.deepcopy(state)
        pos = blank - 1
        temp[blank], temp[pos] = temp[pos], temp[blank]
        children.append(temp)
    return children

def dfs_with_fmax(start_node, fmax, current_new_f):
    global goal_state
    if start_node.state == goal_state:
        print "SUCCESS\n"
        print_path(start_node)
        return -1
    children_states = get_children_states(start_node.state)
    for child_state in children_states:
        child_node = Node(state=child_state,
                          parent=start_node,
                          path_cost=start_node.path_cost+1,
                          f=start_node.path_cost+1+manhattan_distance(child_state))
        if child_node.f <= fmax:
            temp = dfs_with_fmax(child_node, fmax, current_new_f)
            if temp == -1:
                return -1
            if temp < current_new_f:
                current_new_f = temp
        else:
            if child_node.f < current_new_f:
                return child_node.f
            else:
                return current_new_f
    return current_new_f

def ida_star_search():
    global m, initial_state
    fmax = manhattan_distance(initial_state)
    while fmax < m:
        fmax = dfs_with_fmax(Node(state=initial_state,
                                  parent=None,
                                  path_cost=0,
                                  f=manhattan_distance(initial_state)),
                             fmax, m + 1)
        if fmax == -1:
            return
    print "FAILURE"
    return

def a_star_search():
    global m, initial_state, goal_state
    open_list = [Node(state=initial_state,
                      parent=None,
                      path_cost=0,
                      f=manhattan_distance(initial_state))]
    close_list = []
    while open_list:
        current_node = get_current_node(open_list)
        open_list.remove(current_node)
        if current_node.state == goal_state:
            print "SUCCESS\n"
            print_path(current_node)
            return
        children_states = get_children_states(current_node.state)
        for child_state in children_states:
            f = current_node.path_cost + 1 + manhattan_distance(child_state)
            if f > m:
                continue
            add_to_open_list = True
            for i in open_list:
                if child_state == i.state:
                    if f < i.f:
                        i.parent = current_node
                        i.path_cost = current_node.path_cost+1
                        i.f = f
                    add_to_open_list = False
                    break
            if add_to_open_list:
                temp = None
                for i in close_list:
                    if child_state == i.state:
                        if f >= i.f:
                            add_to_open_list = False
                        else:
                            temp = i
                        break
                if temp:
                    close_list.remove(temp)
            if add_to_open_list:
                open_list.append(Node(state=child_state,
                                      parent=current_node,
                                      path_cost = current_node.path_cost+1,
                                      f=f))
        close_list.append(current_node)
    print "FAILURE"
    return

def search():
    global is_ida_star
    if is_ida_star:
        ida_star_search()
    else:
        a_star_search()

def main():
    get_input()
    search()

main()

