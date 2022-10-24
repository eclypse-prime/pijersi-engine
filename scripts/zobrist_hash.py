conversion = {'S':1, 'P': 5, 'R': 9, 'W': 13, 's': 3, 'p': 7, 'r': 11, 'w': 15}

def piece_to_int(piece: str):
    if len(piece) == 1:
        return conversion[piece]
    else:
        return conversion[piece[0]] + conversion[piece[1]]*16

pieces = []

for bottom in ['S', 'P', 'R']:
    for top in ['S', 'P', 'R']:
        pieces.append(top + bottom)

for top in ['S', 'P', 'R', 'W']:
    pieces.append(top + 'W')

pieces += ['S', 'P', 'R', 'W']

for bottom in ['s', 'p', 'r']:
    for top in ['s', 'p', 'r']:
        pieces.append(top + bottom)

for top in ['s', 'p', 'r', 'w']:
    pieces.append(top + 'w')

pieces += ['s', 'p', 'r', 'w']

n = 0

# print(f"        switch (piece)")
# print("        {")
for piece in pieces:
    # print(f"            case {piece_to_int(piece)}:")
    # print("            switch (i)")
    # print("            {")
    n += 1
    # for i in range(7):
        # print(f"            case {i}:")
#     print(f"            default:")
#     print(f"                return 0;")
#     print("            }")
# print(f"        default:")
# print(f"            return 0;")
# print("        }")

print(n)