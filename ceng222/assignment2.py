from math import *

def phi(x):
	return (1.0 + erf(x / sqrt(2.0))) / 2.0

###################### Question 2 ######################
print "Question 2"

#### 8 ####
data = [10.5,1.2,6.3,3.1,10.2,5.9,2.7,4.9,6.8,2.6,1.9,1.0,3.7,12.6,8.6,4.1,0.9,7.1,3.3,4.5,8.8,7.2,0.8,2.2,2.1,2.4,13.6,15.2,4.0,7.5,0.5,6.4,1.7,11.6,5.1,2.8,4.3,8.0,0.2,4.4,2.3,2.9,11.7,0.6,5.3,5.4,1.4,5.0,4.8,2.0,8.0,3.5,9.6,5.5,6.9,11.4,3.8,3.9,1.8,4.7,4.6,0.3,3.2,0.7]
data = sorted(data)
lmbd = len(data) / sum(data)
print len(data), sum(data), lmbd
partition = []
for i in range(8):
	partition.append(data[(i*8):(i*8+8)])
#p = [1 - e**((-1) * lmbd * 1.2), (1 - e**((-1) * lmbd * 2.3)) - (1 - e**((-1) * lmbd * 1.2)), (1 - e**((-1) * lmbd * 3.3)) - (1 - e**((-1) * lmbd * 2.3)), (1 - e**((-1) * lmbd * 4.4)) - (1 - e**((-1) * lmbd * 3.3)), (1 - e**((-1) * lmbd * 5.3)) - (1 - e**((-1) * lmbd * 4.4)), (1 - e**((-1) * lmbd * 7.1)) - (1 - e**((-1) * lmbd * 5.3)), (1 - e**((-1) * lmbd * 10.2)) - (1 - e**((-1) * lmbd * 7.1)), 1 - (1 - e**((-1) * lmbd * 10.2))]
p = [0.213372, 0.155344, 0.114432, 0.102068, 0.068328, 0.104742, 0.111685, 0.130029]
exp = [i * len(data) for i in p]
obs = [len(i) for i in partition]
chi = 0
for i in range(8):
	chi += ((obs[i] - exp[i])**2) / (exp[i])
print "N = ", len(p), "Chi = ", chi, "d.f. = ", len(p) - 1 - 1
print obs
print exp

#### 7 ####
partition = [data[:9], data[9:18], data[18:27], data[27:36], data[36:45], data[45:54], data[54:]]
p = [1 - e**((-1) * lmbd * partition[1][0])] + [(1 - e**((-1) * lmbd * partition[i][0])) - (1 - e**((-1) * lmbd * partition[i - 1][0])) for i in range(2, 7)] + [1 - (1 - e**((-1) * lmbd * partition[6][0]))]
exp = [i * len(data) for i in p]
obs = [len(i) for i in partition]
chi = 0
for i in range(7):
	chi += ((obs[i] - exp[i])**2) / (exp[i])
print "N = ", len(p), "Chi = ", chi, "d.f. = ", len(p) - 1 - 1

#### 6 ####
partition = [data[:11], data[11:22], data[22:32], data[32:42], data[42:52], data[52:]]
p = [1 - e**((-1) * lmbd * 1.8), (1 - e**((-1) * lmbd * 3.1)) - (1 - e**((-1) * lmbd * 1.8)), (1 - e**((-1) * lmbd * 4.4)) - (1 - e**((-1) * lmbd * 3.1)), (1 - e**((-1) * lmbd * 5.5)) - (1 - e**((-1) * lmbd * 4.4)), (1 - e**((-1) * lmbd * 8)) - (1 - e**((-1) * lmbd * 5.5)), 1 - (1 - e**((-1) * lmbd * 8))]
exp = [i * len(data) for i in p]
obs = [len(i) for i in partition]
chi = 0
for i in range(6):
	chi += ((obs[i] - exp[i])**2) / (exp[i])
print "N = ", len(p), "Chi = ", chi, "d.f. = ", len(p) - 1 - 1

#### 5 ####
partition = [data[:13], data[13:25], data[25:38], data[38:51], data[51:]]
p = [1 - e**((-1) * lmbd * partition[1][0])] + [(1 - e**((-1) * lmbd * partition[i][0])) - (1 - e**((-1) * lmbd * partition[i - 1][0])) for i in range(2, 5)] + [1 - (1 - e**((-1) * lmbd * partition[-1][0]))]
exp = [i * len(data) for i in p]
obs = [len(i) for i in partition]
chi = 0
for i in range(5):
	chi += ((obs[i] - exp[i])**2) / (exp[i])
print "N = ", len(p), "Chi = ", chi, "d.f. = ", len(p) - 1 - 1





###################### Question 3 ######################
print "Question 3"

#### 8 ####
data = [-2.434, -1.331, -1.052, -0.588, -0.373, -0.103, 0.251, 0.590, 0.978, 1.360, -2.336, -1.269, -1.051, -0.584, -0.344, -0.101, 0.261, 0.638, 1.005, 1.370, -2.192, -1.229, -1.032, -0.496, -0.280, -0.033, 0.308, 0.652, 1.013, 1.681, -2.010, -1.227, -0.938, -0.489, -0.246, -0.011, 0.343, 0.656, 1.039, 1.721, -1.967, -1.174, -0.884, -0.473, -0.239, 0.033, 0.357, 0.673, 1.072, 1.735, -1.707, -1.136, -0.847, -0.453, -0.211, 0.110, 0.463, 0.772, 1.168, 1.779, -1.678, -1.127, -0.846, -0.427, -0.188, 0.139, 0.477, 0.775, 1.185, 1.792, -1.563, -1.124, -0.716, -0.395, -0.155, 0.143, 0.482, 0.776, 1.263, 1.881, -1.476, -1.120, -0.644, -0.386, -0.149, 0.218, 0.489, 0.787, 1.269, 1.903, -1.388, -1.073, -0.625, -0.386, -0.112, 0.218, 0.545, 0.969, 1.297, 2.009]
data = sorted(data)
partition = [data[:12], data[12:24], data[24:36], data[36:48], data[48:61], data[61:74], data[74:87], data[87:]]
#### A ####
p = [phi(partition[1][0])] + [phi(partition[i][0]) - phi(partition[i - 1][0]) for i in range(2, len(partition))] + [1 - phi(partition[-1][0])]
exp = [i * len(data) for i in p]
obs = [len(i) for i in partition]
chi = 0
for i in range(len(partition)):
	chi += ((obs[i] - exp[i])**2) / (exp[i])
print "N = ", len(partition), "Part a) Chi = ", chi, "d.f. = ", len(p) - 1
#### B ####
p = [(1.0 / 6.0) * (partition[1][0] + 3)] + [(1.0 / 6.0) * (partition[i][0] - partition[i - 1][0]) for i in range(2, len(partition))] + [(1.0 / 6.0) * (3 - partition[-1][0])]
exp = [i * len(data) for i in p]
obs = [len(i) for i in partition]
chi = 0
for i in range(len(partition)):
	chi += ((obs[i] - exp[i])**2) / (exp[i])
print "N = ", len(partition), "Part b) Chi = ", chi, "d.f. = ", len(p) - 1


#### 7 ####
data = [-2.434, -1.331, -1.052, -0.588, -0.373, -0.103, 0.251, 0.590, 0.978, 1.360, -2.336, -1.269, -1.051, -0.584, -0.344, -0.101, 0.261, 0.638, 1.005, 1.370, -2.192, -1.229, -1.032, -0.496, -0.280, -0.033, 0.308, 0.652, 1.013, 1.681, -2.010, -1.227, -0.938, -0.489, -0.246, -0.011, 0.343, 0.656, 1.039, 1.721, -1.967, -1.174, -0.884, -0.473, -0.239, 0.033, 0.357, 0.673, 1.072, 1.735, -1.707, -1.136, -0.847, -0.453, -0.211, 0.110, 0.463, 0.772, 1.168, 1.779, -1.678, -1.127, -0.846, -0.427, -0.188, 0.139, 0.477, 0.775, 1.185, 1.792, -1.563, -1.124, -0.716, -0.395, -0.155, 0.143, 0.482, 0.776, 1.263, 1.881, -1.476, -1.120, -0.644, -0.386, -0.149, 0.218, 0.489, 0.787, 1.269, 1.903, -1.388, -1.073, -0.625, -0.386, -0.112, 0.218, 0.545, 0.969, 1.297, 2.009]
data = sorted(data)
partition = [data[:14], data[14:28], data[28:42], data[42:56], data[56:70], data[70:85], data[85:]]
#### A ####
p = [phi(partition[1][0])] + [phi(partition[i][0]) - phi(partition[i - 1][0]) for i in range(2, len(partition))] + [1 - phi(partition[-1][0])]
exp = [i * len(data) for i in p]
obs = [len(i) for i in partition]
chi = 0
for i in range(len(partition)):
	chi += ((obs[i] - exp[i])**2) / (exp[i])
print "N = ", len(partition), "Part a) Chi = ", chi, "d.f. = ", len(p) - 1
#### B ####
p = [(1.0 / 6.0) * (partition[1][0] + 3)] + [(1.0 / 6.0) * (partition[i][0] - partition[i - 1][0]) for i in range(2, len(partition))] + [(1.0 / 6.0) * (3 - partition[-1][0])]
exp = [i * len(data) for i in p]
obs = [len(i) for i in partition]
chi = 0
for i in range(len(partition)):
	chi += ((obs[i] - exp[i])**2) / (exp[i])
print "N = ", len(partition), "Part b) Chi = ", chi, "d.f. = ", len(p) - 1


#### 6 ####
data = [-2.434, -1.331, -1.052, -0.588, -0.373, -0.103, 0.251, 0.590, 0.978, 1.360, -2.336, -1.269, -1.051, -0.584, -0.344, -0.101, 0.261, 0.638, 1.005, 1.370, -2.192, -1.229, -1.032, -0.496, -0.280, -0.033, 0.308, 0.652, 1.013, 1.681, -2.010, -1.227, -0.938, -0.489, -0.246, -0.011, 0.343, 0.656, 1.039, 1.721, -1.967, -1.174, -0.884, -0.473, -0.239, 0.033, 0.357, 0.673, 1.072, 1.735, -1.707, -1.136, -0.847, -0.453, -0.211, 0.110, 0.463, 0.772, 1.168, 1.779, -1.678, -1.127, -0.846, -0.427, -0.188, 0.139, 0.477, 0.775, 1.185, 1.792, -1.563, -1.124, -0.716, -0.395, -0.155, 0.143, 0.482, 0.776, 1.263, 1.881, -1.476, -1.120, -0.644, -0.386, -0.149, 0.218, 0.489, 0.787, 1.269, 1.903, -1.388, -1.073, -0.625, -0.386, -0.112, 0.218, 0.545, 0.969, 1.297, 2.009]
data = sorted(data)
partition = [data[:16], data[16:32], data[32:49], data[49:66], data[66:83], data[83:]]
#### A ####
p = [phi(partition[1][0])] + [phi(partition[i][0]) - phi(partition[i - 1][0]) for i in range(2, len(partition))] + [1 - phi(partition[-1][0])]
exp = [i * len(data) for i in p]
obs = [len(i) for i in partition]
chi = 0
for i in range(len(partition)):
	chi += ((obs[i] - exp[i])**2) / (exp[i])
print "N = ", len(partition), "Part a) Chi = ", chi, "d.f. = ", len(p) - 1
#### B ####
p = [(1.0 / 6.0) * (partition[1][0] + 3)] + [(1.0 / 6.0) * (partition[i][0] - partition[i - 1][0]) for i in range(2, len(partition))] + [(1.0 / 6.0) * (3 - partition[-1][0])]
exp = [i * len(data) for i in p]
obs = [len(i) for i in partition]
chi = 0
for i in range(len(partition)):
	chi += ((obs[i] - exp[i])**2) / (exp[i])
print "N = ", len(partition), "Part b) Chi = ", chi, "d.f. = ", len(p) - 1


#### 5 ####
data = [-2.434, -1.331, -1.052, -0.588, -0.373, -0.103, 0.251, 0.590, 0.978, 1.360, -2.336, -1.269, -1.051, -0.584, -0.344, -0.101, 0.261, 0.638, 1.005, 1.370, -2.192, -1.229, -1.032, -0.496, -0.280, -0.033, 0.308, 0.652, 1.013, 1.681, -2.010, -1.227, -0.938, -0.489, -0.246, -0.011, 0.343, 0.656, 1.039, 1.721, -1.967, -1.174, -0.884, -0.473, -0.239, 0.033, 0.357, 0.673, 1.072, 1.735, -1.707, -1.136, -0.847, -0.453, -0.211, 0.110, 0.463, 0.772, 1.168, 1.779, -1.678, -1.127, -0.846, -0.427, -0.188, 0.139, 0.477, 0.775, 1.185, 1.792, -1.563, -1.124, -0.716, -0.395, -0.155, 0.143, 0.482, 0.776, 1.263, 1.881, -1.476, -1.120, -0.644, -0.386, -0.149, 0.218, 0.489, 0.787, 1.269, 1.903, -1.388, -1.073, -0.625, -0.386, -0.112, 0.218, 0.545, 0.969, 1.297, 2.009]
data = sorted(data)
partition = [data[:20], data[20:40], data[40:60], data[60:80], data[80:]]
#### A ####
p = [phi(partition[1][0])] + [phi(partition[i][0]) - phi(partition[i - 1][0]) for i in range(2, len(partition))] + [1 - phi(partition[-1][0])]
exp = [i * len(data) for i in p]
obs = [len(i) for i in partition]
chi = 0
for i in range(len(partition)):
	chi += ((obs[i] - exp[i])**2) / (exp[i])
print "N = ", len(partition), "Part b) Chi = ", chi, "d.f. = ", len(p) - 1
print p
print exp
print obs
#### B ####
p = [(1.0 / 6.0) * (partition[1][0] + 3)] + [(1.0 / 6.0) * (partition[i][0] - partition[i - 1][0]) for i in range(2, len(partition))] + [(1.0 / 6.0) * (3 - partition[-1][0])]
exp = [i * len(data) for i in p]
obs = [len(i) for i in partition]
chi = 0
for i in range(len(partition)):
	chi += ((obs[i] - exp[i])**2) / (exp[i])
print "N = ", len(partition), "Part b) Chi = ", chi, "d.f. = ", len(p) - 1
print p
print exp
print obs

#### 5 ####
data = [-2.434, -1.331, -1.052, -0.588, -0.373, -0.103, 0.251, 0.590, 0.978, 1.360, -2.336, -1.269, -1.051, -0.584, -0.344, -0.101, 0.261, 0.638, 1.005, 1.370, -2.192, -1.229, -1.032, -0.496, -0.280, -0.033, 0.308, 0.652, 1.013, 1.681, -2.010, -1.227, -0.938, -0.489, -0.246, -0.011, 0.343, 0.656, 1.039, 1.721, -1.967, -1.174, -0.884, -0.473, -0.239, 0.033, 0.357, 0.673, 1.072, 1.735, -1.707, -1.136, -0.847, -0.453, -0.211, 0.110, 0.463, 0.772, 1.168, 1.779, -1.678, -1.127, -0.846, -0.427, -0.188, 0.139, 0.477, 0.775, 1.185, 1.792, -1.563, -1.124, -0.716, -0.395, -0.155, 0.143, 0.482, 0.776, 1.263, 1.881, -1.476, -1.120, -0.644, -0.386, -0.149, 0.218, 0.489, 0.787, 1.269, 1.903, -1.388, -1.073, -0.625, -0.386, -0.112, 0.218, 0.545, 0.969, 1.297, 2.009]
data = sorted(data)
partition = [data[:25], data[25:50], data[50:75], data[75:]]
#### A ####
p = [phi(partition[1][0])] + [phi(partition[i][0]) - phi(partition[i - 1][0]) for i in range(2, len(partition))] + [1 - phi(partition[-1][0])]
exp = [i * len(data) for i in p]
obs = [len(i) for i in partition]
chi = 0
for i in range(len(partition)):
	chi += ((obs[i] - exp[i])**2) / (exp[i])
print "N = ", len(partition), "Part b) Chi = ", chi, "d.f. = ", len(p) - 1

#### B ####
p = [(1.0 / 6.0) * (partition[1][0] + 3)] + [(1.0 / 6.0) * (partition[i][0] - partition[i - 1][0]) for i in range(2, len(partition))] + [(1.0 / 6.0) * (3 - partition[-1][0])]
exp = [i * len(data) for i in p]
obs = [len(i) for i in partition]
chi = 0
for i in range(len(partition)):
	chi += ((obs[i] - exp[i])**2) / (exp[i])
print "N = ", len(partition), "Part b) Chi = ", chi, "d.f. = ", len(p) - 1
