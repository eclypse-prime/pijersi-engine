"""A helper script to generate the switch case in Board::neighbours2(int index)
I wanted to hardcode this so I created this script to generate all the cases in C++ code.
"""

from typing import List


def coords_index(i: int, j: int) -> int:
    if i % 2 == 0:
        index = 13 * i // 2 + j
    else:
        index = 6 + 13 * (i-1) // 2 + j
    return index


def find_neighbours(i: int, j: int) -> List[int]:
    neighbours = []
    index = coords_index(i,j)
    if j > 1 or (i % 2 == 0 and j > 0):
        if j > 1:
            neighbours.append(index - 2)
        if i > 1:
            neighbours.append(index - 14)
        if i < 5:
            neighbours.append(index + 12)

    if (i%2 == 0 and j < 5) or (i % 2 == 1 and j < 6):
        if (i%2 == 0 and j < 4) or (i % 2 == 1 and j < 5):
            neighbours.append(index + 2)
        if i > 1:
            neighbours.append(index - 12)
        if i < 5:
            neighbours.append(index + 14)

    neighbours.sort()
    return neighbours



for i in range(7):
    if i % 2 == 0:
        for j in range(6):
            index = coords_index(i,j)
            print(f"case {index}:")
            line = "    return vector<int>({"
            neighbours = find_neighbours(i,j)
            n = len(find_neighbours(i,j))
            for k in range(n):
                line += str(neighbours[k])
                if k < n-1:
                    line += ","
            line += "});"
            print(line)
    else:
        for j in range(7):
            index = coords_index(i,j)
            print(f"case {index}:")
            line = "    return vector<int>({"
            neighbours = find_neighbours(i,j)
            n = len(find_neighbours(i,j))
            for k in range(n):
                line += str(neighbours[k])
                if k < n-1:
                    line += ","
            line += "});"
            print(line)
