"""Generates the code that uses a lookup table to find a piece's score its type, colour, and position."""

conversion = {'S':1, 'P': 5, 'R': 9, 'W': 13, 's': 3, 'p': 7, 'r': 11, 'w': 15}

index_to_line = [
        0,
        0,
        0,
        0,
        0,
        0,
        1,
        1,
        1,
        1,
        1,
        1,
        1,
        2,
        2,
        2,
        2,
        2,
        2,
        3,
        3,
        3,
        3,
        3,
        3,
        3,
        4,
        4,
        4,
        4,
        4,
        4,
        5,
        5,
        5,
        5,
        5,
        5,
        5,
        6,
        6,
        6,
        6,
        6,
        6]

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
                score *= 256
        else:
            score = -9 - i
            if i == 6:
                score *= 256
    
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

print("    int64_t pieceScores[1575] {")
for piece in pieces:
    for i in range(45):
        score = evaluate_piece(piece, index_to_line[i]) * 10
        print(f"        {score},")
for i in range(44):
    print("        0,")
print("        0")
print("    };")