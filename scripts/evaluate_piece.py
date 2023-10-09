"""Generates the code that uses a lookup table to find a piece's score its type, colour, and position."""

CHAR_TO_PIECE = {'S':1, 'P': 5, 'R': 9, 'W': 13, 's': 3, 'p': 7, 'r': 11, 'w': 15}

INDEX_TO_LINE = [
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

LINE_TO_SCORE = [63, 100, 110, 115, 130, 135, 150]
WISE_SCORE = 70
STACK_SCORE = -10


def piece_to_int(piece: str):
    if len(piece) == 1:
        return CHAR_TO_PIECE[piece]
    else:
        return CHAR_TO_PIECE[piece[0]] + CHAR_TO_PIECE[piece[1]]*16

def evaluate_piece(piece, i):
    score = 0
    if piece[0] not in ['W', 'w']:
        if piece[0] in ['S', 'P', 'R']:
            score = LINE_TO_SCORE[6-i]
            if i == 0:
                score *= 256
        else:
            score = -LINE_TO_SCORE[i]
            if i == 6:
                score *= 256
    
    elif piece[0] == 'W':
        score = WISE_SCORE
    elif piece[0] == 'w':
        score = -WISE_SCORE
    
    if len(piece) == 2:
        score = score * 2
        if piece[0] in ['S', 'P', 'R', 'W']:
            score += STACK_SCORE
        else:
            score -= STACK_SCORE
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

print("    const int64_t pieceScores[1575] {")
for piece in pieces:
    for i in range(45):
        score = evaluate_piece(piece, INDEX_TO_LINE[i])
        print(f"        {score},")
for i in range(44):
    print("        0,")
print("        0")
print("    };")