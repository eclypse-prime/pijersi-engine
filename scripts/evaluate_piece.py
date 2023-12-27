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

CELL_TO_SCORE = [
    8.546530691405348,
    -2.4248153537871096,
    -9.603649901634746,
    3.220673112897795,
    -2.3225240837845558,
    -1.6928949918623704,
    9.044688821614217,
    4.454803325548141,
    10.45759044645208,
    0.37278336992654015,
    9.67064653687067,
    9.574487904389493,
    9.440166320426936,
    -3.213172789508703,
    1.7713172374947987,
    -14.960192314514053,
    6.784432789129085,
    -2.249917703480553,
    -3.6114512177543534,
    11.621915831563234,
    28.267194666875504,
    21.447035977930767,
    20.034368693287714,
    7.038946708509679,
    21.348905754363972,
    10.849632201222969,
    16.97428194932498,
    13.057519596010415,
    3.3606325590128003,
    -11.771492936858277,
    22.07291268509233,
    25.65994172899924,
    2.363771999141353,
    -15.212508453616453,
    4.967321156754354,
    -5.295964478093077,
    1.3815592415913682,
    2.164429079366725,
    2.934300356828555,
]

WISE_SCORE = 4.265081603543751
STACK_SCORE = 18.83345911883383

for i, _ in enumerate(CELL_TO_SCORE):
    CELL_TO_SCORE[i] *= 100
WISE_SCORE *= 100
STACK_SCORE *= 100

def piece_to_int(piece: str):
    if len(piece) == 1:
        return CHAR_TO_PIECE[piece]
    else:
        return CHAR_TO_PIECE[piece[0]] + CHAR_TO_PIECE[piece[1]]*16

def evaluate_piece(piece, index):
    score = 0
    if piece[0] not in ['W', 'w']:
        if piece[0] in ['S', 'P', 'R']:
            if INDEX_TO_LINE[index] == 0:
                score = 512*1024
            else:
                score = CELL_TO_SCORE[44-index]
        else:
            if INDEX_TO_LINE[index] == 6:
                score = -512*1024
            else:
                score = CELL_TO_SCORE[index]
    
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
    return round(score)
    

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
    for index in range(45):
        score = evaluate_piece(piece, index)
        print(f"        {score},")
for index in range(44):
    print("        0,")
print("        0")
print("    };")