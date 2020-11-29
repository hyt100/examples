import matplotlib.pyplot as plt
import numpy as np
import random

siz = 1000

x  = [i for i in range(siz)]
y1 = [random.randint(20, 40) for i in range(siz)]
y2 = []
j = 0

for i,v in enumerate(y1):
    if j == 0:
        j = v
    else:
        j += (v - j) / 16.0  # Y(n) = aX(n) + (1-a)Y(n-1), a=1/16.0
    y2.append(j)

plt.plot(x, y1, color='r', linestyle='-')
plt.plot(x, y2, color='b', linestyle='-')
plt.show()
