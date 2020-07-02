import numpy as np
import scipy.stats

def mean_confidence_interval(n, m, se, confidence=0.95):
	confidence = 1 - confidence
	h = se * scipy.stats.t.ppf((1 + confidence) / 2., n - 1)
	return m - h, m + h, h


f = open("experimentScripts/s/exp1.txt", "r")
exp1 = list(map(lambda x: float(x.strip().split(" = ")[1]), f.readlines()))
f.close()
f = open("experimentScripts/s/exp2.txt", "r")
exp2 = list(map(lambda x: float(x.strip().split(" = ")[1]), f.readlines()))
f.close()
f = open("experimentScripts/s/exp3.txt", "r")
exp3 = list(map(lambda x: float(x.strip().split(" = ")[1]), f.readlines()))
f.close()
print("Emulated Delay, N, Mean, Standard Deviation, Lower Limit, Upper Limit, Margin of Error")
N = int(exp1[0])
mean = exp1[1] * 1000
std = exp1[2] * 1000
lower, upper, margin_of_error = mean_confidence_interval(N, mean, std, 0.95)
print("20, " + str(N) + ", " + str(mean) + ", " + str(std) + ", " + str(lower) + ", " + str(upper) + ", " + str(margin_of_error))
N = int(exp2[0])
mean = exp2[1] * 1000
std = exp2[2] * 1000
lower, upper, margin_of_error = mean_confidence_interval(N, mean, std, 0.95)
print("40, " + str(N) + ", " + str(mean) + ", " + str(std) + ", " + str(lower) + ", " + str(upper) + ", " + str(margin_of_error))
N = int(exp3[0])
mean = exp3[1] * 1000
std = exp3[2] * 1000
lower, upper, margin_of_error = mean_confidence_interval(N, mean, std, 0.95)
print("50, " + str(N) + ", " + str(mean) + ", " + str(std) + ", " + str(lower) + ", " + str(upper) + ", " + str(margin_of_error))