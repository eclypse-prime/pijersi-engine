conversion = {'S':1, 'P': 5, 'R': 9, 'W': 13, 's': 3, 'p': 7, 'r': 11, 'w': 15}

def piece_to_int(piece: str):
    if len(piece) == 1:
        return conversion[piece]
    else:
        return conversion[piece[0]] + conversion[piece[1]]*16

def evaluate_piece(piece, i):
    score = 0
    if piece[0] not in ['W', 'w']:
        if piece[0] in ['S', 'P', 'R']:
            score = 15 - i
            if i == 0:
                score *= 100
        else:
            score = -9 - i
            if i == 6:
                score *= 100
    
    elif piece[0] == 'W':
        score = 8
    elif piece[0] == 'w':
        score = -8
    
    if len(piece) == 2:
        score = score * 2
        if piece[0] in ['S', 'P', 'R', 'W']:
            score += 3
        else:
            score -= 3
    return score
    

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


# print("        switch (i)")
# print("        {")
# for i in range(7):
#     print(f"        case {i}:")
#     print(f"            switch (piece)")
#     print("            {")
#     for piece in pieces:
#         print(f"            case {piece_to_int(piece)}:")
#         print(f"                return {evaluate_piece(piece, i)};")
#     print(f"            default:")
#     print(f"                return 0;")
#     print("            }")
# print(f"        default:")
# print(f"            return 0;")
# print("        }")

print(f"        switch (piece)")
print("        {")
for piece in pieces:
    print(f"            case {piece_to_int(piece)}:")
    print("            switch (i)")
    print("            {")
    for i in range(7):
        print(f"            case {i}:")
        print(f"                return {evaluate_piece(piece, i)};")
    print(f"            default:")
    print(f"                return 0;")
    print("            }")
print(f"        default:")
print(f"            return 0;")
print("        }")