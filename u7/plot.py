#!/usr/bin/python3

import matplotlib.pyplot as plt
import csv


with open("test.csv", "r") as csvfile:
    plots = csv.reader(csvfile, delimiter=",")
    obj_num = 0
    for row in plots:
        if int(row[0]) >= obj_num:
            obj_num += 1
        else:
            break

obj_lists = [[[],[]] for x in range(obj_num)]

with open("test.csv", "r") as csvfile:
    plots = csv.reader(csvfile, delimiter=",")
    for row in plots:
        obj_lists[int(row[0])][0].append(float(row[1]))
        obj_lists[int(row[0])][1].append(float(row[2]))


for graphs in obj_lists:
    plt.plot(graphs[0], graphs[1])

# plt.ylabel('some numbers')
plt.show()
