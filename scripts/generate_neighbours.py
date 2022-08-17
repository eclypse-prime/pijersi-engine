"""A helper script to generate the switch case in Board::neighbours(int index)
I wanted to hardcode this so I created this script to generate all the cases in C++ code.
"""

def coords_index(i: int, j: int) -> int:
    if i % 2 == 0:
        index = 13 * i // 2 + j
    else:
        index = 6 + 13 * (i-1) // 2 + j
    return index


def find_neighbours(i, j):
    neighbours = []
    index = coords_index(i,j)
    if j > 0 or i % 2 == 0:
        if j > 0:
            neighbours.append(index - 1)
        if i > 0:
            neighbours.append(index - 7)
        if i < 6:
            neighbours.append(index + 6)

    if (i % 2 == 0) or (i % 2 == 1 and j < 6):
        if (i%2 == 0 and j < 5) or (i % 2 == 1 and j < 6):
            neighbours.append(index + 1)
        if i > 0:
            neighbours.append(index - 6)
        if i < 6:
            neighbours.append(index + 7)

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
