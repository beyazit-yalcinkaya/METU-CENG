import sys
import random

input_type = None
theta = 0
gamma = 0
M = 0
N = 0
number_of_obstacles = 0
obstacles = []
number_of_pitfalls = 0
pitfalls = []
goal = [0, 0]
step_reward = 0
obstacle_reward = 0
pitfall_reward = 0
goal_reward = 0
number_of_episodes = 0
alpha = 0
epsilon = 0

U = None
Q = None

def pick_action(s):
    global input_type, theta, gamma, M, N, number_of_obstacles, obstacles, number_of_pitfalls, pitfalls, goal
    global step_reward, obstacle_reward, pitfall_reward, goal_reward, number_of_episodes, alpha, epsilon, U, Q
    return random.randint(0, 3) if random.random() < epsilon else get_best_action_based_on_q(s)

def reward(x, y):
    global input_type, theta, gamma, M, N, number_of_obstacles, obstacles, number_of_pitfalls, pitfalls, goal
    global step_reward, obstacle_reward, pitfall_reward, goal_reward, number_of_episodes, alpha, epsilon, U, Q
    if (x, y) == goal:
        return goal_reward
    if (x, y) in pitfalls:
        return pitfall_reward
    if (x, y) in obstacles:
        return obstacle_reward
    return step_reward

def execute_action(s, a):
    global input_type, theta, gamma, M, N, number_of_obstacles, obstacles, number_of_pitfalls, pitfalls, goal
    global step_reward, obstacle_reward, pitfall_reward, goal_reward, number_of_episodes, alpha, epsilon, U, Q
    x, y = s
    if a == 0 and y < M and (x, y + 1) not in obstacles:
        return (x, y + 1), reward(x, y + 1)
    elif a == 1 and x < N and (x + 1, y) not in obstacles:
        return (x + 1, y), reward(x + 1, y)
    elif a == 2 and y > 1 and (x, y - 1) not in obstacles:
        return (x, y - 1), reward(x, y - 1)
    elif a == 3 and x > 1 and (x - 1, y) not in obstacles:
        return (x - 1, y), reward(x - 1, y)
    return (x, y), obstacle_reward

def get_best_action_based_on_u(s):
    global input_type, theta, gamma, M, N, number_of_obstacles, obstacles, number_of_pitfalls, pitfalls, goal
    global step_reward, obstacle_reward, pitfall_reward, goal_reward, number_of_episodes, alpha, epsilon, U, Q
    u = float("-inf")
    a = []
    for i in range(4):
        s_p, _ = execute_action(s, i)
        if U[s_p] > u:
            u = U[s_p]
            a = [i]
        elif U[s_p] == u:
            a.append(i)
    return random.choice(a)

def get_best_action_based_on_q(s):
    global input_type, theta, gamma, M, N, number_of_obstacles, obstacles, number_of_pitfalls, pitfalls, goal
    global step_reward, obstacle_reward, pitfall_reward, goal_reward, number_of_episodes, alpha, epsilon, U, Q
    q = float("-inf")
    a = []
    for i in range(4):
        if Q[s][i] > q:
            q = Q[s][i]
            a = [i]
        elif Q[s][i] == q:
            a.append(i)
    return random.choice(a)

def value_iteration(output_file_name):
    global input_type, theta, gamma, M, N, number_of_obstacles, obstacles, number_of_pitfalls, pitfalls, goal
    global step_reward, obstacle_reward, pitfall_reward, goal_reward, number_of_episodes, alpha, epsilon, U, Q
    U_p = {(x, y):0 for x in range(1, N + 1) for y in range(1, M + 1)}
    while True:
        U = {s:U_p[s] for s in U_p.keys()}
        d = 0
        for s in U_p.keys():
            if s == goal:
                U_p[s] = goal_reward
            elif s in obstacles:
                U_p[s] = obstacle_reward
            elif s in pitfalls:
                U_p[s] = pitfall_reward
            else:
                s_p, _ = execute_action(s, get_best_action_based_on_u(s))
                U_p[s] = step_reward + gamma*U[s_p]
            if abs(U_p[s] - U[s]) > d:
                d = abs(U_p[s] - U[s])
        if d < theta:
            break
    f = open(output_file_name, "w+")
    for x in range(1, N + 1):
        for y in range(1, M + 1):
            f.write(str(x) + " " + str(y) + " " + str(get_best_action_based_on_u((x, y))) + "\n")   
    f.close()

def q_learning(output_file_name):
    global input_type, theta, gamma, M, N, number_of_obstacles, obstacles, number_of_pitfalls, pitfalls, goal
    global step_reward, obstacle_reward, pitfall_reward, goal_reward, number_of_episodes, alpha, epsilon, U, Q
    Q = {(x, y):[0]*4 for x in range(1, N + 1) for y in range(1, M + 1)}
    states = []
    for x in range(1, N + 1):
        for y in range(1, M + 1):
            if (x, y) not in obstacles and (x, y) not in pitfalls and (x, y) != goal:
                states.append((x, y))
    for i in range(number_of_episodes):
        s = random.choice(states)
        while s != goal:
            a = pick_action(s)
            s_p, r = execute_action(s, a)
            a_p = get_best_action_based_on_q(s_p)
            Q[s][a] += alpha*(r + gamma*Q[s_p][a_p] - Q[s][a])
            s = s_p
    f = open(output_file_name, "w+")
    for x in range(1, N + 1):
        for y in range(1, M + 1):
            f.write(str(x) + " " + str(y) + " " + str(get_best_action_based_on_q((x, y))) + "\n")
    f.close()

def read_input(input_file_name):
    global input_type, theta, gamma, M, N, number_of_obstacles, obstacles, number_of_pitfalls, pitfalls, goal
    global step_reward, obstacle_reward, pitfall_reward, goal_reward, number_of_episodes, alpha, epsilon, U, Q
    f = open(input_file_name, "r")
    content = map(lambda x: x.strip(), f.readlines())
    f.close()
    input_type = content[0]
    if input_type == "V":
        theta = float(content[1])
        gamma = float(content[2])
        M, N = map(lambda x: int(x), content[3].split(" "))
        number_of_obstacles = int(content[4])
        obstacles = [tuple(map(lambda x: int(x), content[i].split(" "))) for i in range(5, 5 + number_of_obstacles)]
        number_of_pitfalls = int(content[5 + number_of_obstacles])
        pitfalls = [tuple(map(lambda x: int(x), content[i].split(" "))) for i in range(6 + number_of_obstacles, 6 + number_of_obstacles + number_of_pitfalls)]
        goal = tuple(map(lambda x: int(x), content[6 + number_of_obstacles + number_of_pitfalls].split(" ")))
        step_reward, obstacle_reward, pitfall_reward, goal_reward = map(lambda x: float(x), content[7 + number_of_obstacles + number_of_pitfalls].split(" "))
    elif input_type == "Q":
        number_of_episodes = int(content[1])
        alpha = float(content[2])
        gamma = float(content[3])
        epsilon = float(content[4])
        M, N = map(lambda x: int(x), content[5].split(" "))
        number_of_obstacles = int(content[6])
        obstacles = [tuple(map(lambda x: int(x), content[i].split(" "))) for i in range(7, 7 + number_of_obstacles)]
        number_of_pitfalls = int(content[7 + number_of_obstacles])
        pitfalls = [tuple(map(lambda x: int(x), content[i].split(" "))) for i in range(8 + number_of_obstacles, 8 + number_of_obstacles + number_of_pitfalls)]
        goal = tuple(map(lambda x: int(x), content[8 + number_of_obstacles + number_of_pitfalls].split(" ")))
        step_reward, obstacle_reward, pitfall_reward, goal_reward = map(lambda x: float(x), content[9 + number_of_obstacles + number_of_pitfalls].split(" "))

def main():
    global input_type, theta, gamma, M, N, number_of_obstacles, obstacles, number_of_pitfalls, pitfalls, goal
    global step_reward, obstacle_reward, pitfall_reward, goal_reward, number_of_episodes, alpha, epsilon, U, Q
    read_input(sys.argv[1])
    if input_type == "V":
        value_iteration(sys.argv[2])
    elif input_type == "Q":
        q_learning(sys.argv[2])

main()
