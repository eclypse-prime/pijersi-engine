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

CELL_TO_SCORE = starting_weights = [
    -0.7342300141336119,
    1.4127028233662038,
    -0.7735221162858075,
    0.5065524399589618,
    -3.100515147541645,
    0.14947866329641646,
    -1.0892599081920806,
    1.434796292901909,
    -0.280773139107028,
    3.7035826148515234,
    -2.3252255160436652,
    -2.1003591970065516,
    5.346034235133284,
    0.28681528783369714,
    -0.8605728220789117,
    -0.8642533681170914,
    0.050324545243607605,
    0.7582389492850461,
    0.6798979362426254,
    -0.3421162581736944,
    1.4878559220604237,
    -3.129716249178368,
    4.6000636949987985,
    -0.2362470905260537,
    -3.3615524862476986,
    1.0999358779420039,
    -0.5597137913189794,
    1.0929167279566427,
    -3.595599707919011,
    1.1572439030441155,
    1.0332480379050293,
    -0.24438708604040177,
    -2.1302464301623014,
    -2.1663460716771596,
    2.2961793419439314,
    0.991333558466001,
    2.0490491594649143,
    1.8774504344436478,
    1.807785699967697,
    0.8433027808212338,
]

WISE_SCORE = 5.271905905896208
STACK_SCORE = 0.8433027808212338

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