// Example Input Tensors

const int8_t ic_bench_example_input_tensors[] = {
    -119, 104,  17,   34,   -7,   110,  -117, -24,  25,   54,   100,  -98,  57,   58,   115,  -66,
    -4,   -94,  -73,  59,   110,  111,  -14,  6,    -120, -40,  -26,  -94,  88,   99,   -35,  -109,
    80,   -73,  60,   -113, -21,  63,   124,  4,    -38,  87,   -78,  82,   -63,  -34,  91,   -1,
    -51,  -18,  -116, 111,  98,   71,   -82,  -46,  -3,   -39,  56,   -108, -30,  98,   -121, 37,
    -68,  22,   -23,  -62,  123,  126,  -70,  61,   -65,  9,    11,   -106, -56,  30,   115,  36,
    58,   -3,   -41,  6,    -9,   -91,  -1,   95,   15,   116,  -31,  78,   63,   -122, -83,  42,
    -19,  51,   -58,  25,   44,   37,   -80,  62,   126,  19,   -123, 126,  19,   78,   -47,  45,
    -83,  33,   -39,  39,   60,   -52,  -115, 86,   -121, 119,  78,   28,   -96,  39,   -115, -4,
    -4,   -53,  47,   -52,  -51,  81,   -27,  105,  102,  -100, 32,   -45,  -22,  53,   91,   -25,
    4,    -99,  10,   -25,  102,  -25,  107,  -58,  -83,  -119, 40,   -76,  101,  -122, -109, 79,
    105,  56,   51,   -21,  56,   -91,  -52,  -70,  12,   -90,  -3,   -7,   50,   -8,   63,   -84,
    121,  3,    -20,  102,  -119, 12,   16,   123,  90,   -20,  -38,  -38,  -125, 89,   96,   -96,
    88,   -125, -116, 31,   84,   2,    -35,  49,   88,   -13,  83,   60,   8,    90,   12,   27,
    -95,  101,  103,  -8,   32,   -88,  -101, 120,  -3,   -77,  -77,  68,   -90,  -12,  83,   -86,
    -96,  -31,  14,   72,   -111, -52,  114,  -100, 109,  64,   -55,  63,   119,  -60,  21,   -7,
    123,  82,   -10,  -92,  71,   -71,  -119, 116,  10,   96,   -46,  -73,  18,   -55,  -79,  -75,
    70,   5,    -30,  -103, -57,  -64,  -38,  -122, 52,   59,   111,  -60,  111,  111,  -55,  95,
    -56,  -15,  52,   -26,  -80,  11,   49,   -35,  123,  30,   -45,  -98,  -82,  -55,  19,   -8,
    22,   -58,  -60,  -7,   19,   -11,  -81,  -18,  100,  -9,   -50,  -119, -92,  -75,  -64,  -88,
    53,   58,   34,   -66,  19,   77,   24,   -79,  29,   111,  42,   -34,  69,   10,   -20,  -24,
    -31,  102,  13,   108,  -3,   -11,  -21,  -35,  -80,  -51,  -122, -53,  25,   -24,  90,   4,
    115,  67,   121,  46,   105,  -114, 64,   61,   -7,   17,   124,  119,  92,   63,   -5,   89,
    -98,  30,   -58,  94,   104,  -17,  6,    107,  116,  20,   -116, 16,   -36,  40,   31,   31,
    28,   78,   121,  61,   -116, -119, 56,   -124, -121, -50,  -93,  95,   -48,  9,    -107, -75,
    110,  105,  -64,  63,   85,   124,  -44,  -22,  41,   82,   -18,  -5,   110,  42,   -9,   -122,
    47,   65,   -82,  -122, -9,   -63,  77,   46,   81,   31,   89,   89,   41,   -15,  -28,  120,
    94,   126,  63,   123,  -78,  -41,  109,  83,   104,  -100, 65,   90,   -88,  -105, 124,  50,
    -17,  40,   -94,  56,   -122, 98,   36,   -20,  77,   90,   60,   6,    11,   81,   -53,  -59,
    -79,  -74,  20,   107,  46,   40,   12,   53,   124,  97,   -9,   36,   -113, -120, 53,   -105,
    -7,   84,   -110, 70,   -5,   92,   -83,  -105, 74,   -119, 67,   11,   -56,  -85,  67,   -93,
    47,   -29,  13,   53,   21,   118,  70,   -119, -77,  -106, -69,  34,   -4,   0,    -28,  -37,
    32,   112,  -21,  -113, 0,    6,    -126, -44,  -49,  44,   -96,  -9,   -48,  98,   60,   -65,
    125,  106,  -44,  -21,  92,   -39,  34,   -12,  -60,  125,  119,  -63,  -54,  -77,  34,   87,
    -83,  118,  -103, 36,   41,   68,   -67,  58,   39,   111,  93,   107,  124,  -93,  94,   48,
    -69,  -25,  -72,  89,   -59,  101,  119,  -106, 39,   -59,  3,    67,   96,   105,  100,  61,
    -100, -102, -93,  -127, 20,   65,   -96,  -87,  56,   -25,  -63,  113,  26,   -7,   -79,  89,
    105,  52,   125,  -71,  97,   -29,  71,   108,  -37,  6,    -32,  -110, -82,  -92,  -111, 17,
    46,   65,   -72,  -22,  -34,  -101, -58,  -77,  53,   64,   24,   88,   66,   72,   -70,  -98,
    40,   -6,   41,   -126, 37,   14,   101,  -36,  25,   16,   47,   98,   -63,  28,   -47,  -54,
    -70,  -90,  -50,  98,   -76,  -48,  95,   107,  112,  -79,  -117, 8,    118,  -69,  16,   24,
    -3,   57,   38,   84,   -43,  -74,  -19,  84,   42,   -101, -76,  -81,  -44,  8,    95,   -118,
    -116, 89,   22,   51,   52,   59,   28,   107,  62,   -85,  -111, -83,  -93,  76,   50,   -109,
    -54,  -31,  -104, 110,  56,   112,  -48,  40,   -56,  46,   -115, -5,   100,  110,  15,   -97,
    -66,  98,   -104, -86,  -46,  114,  -92,  -30,  -48,  68,   -114, -89,  -23,  -46,  13,   24,
    107,  56,   -112, -89,  70,   -38,  -63,  -18,  -126, -107, -118, -127, -4,   39,   -26,  120,
    -18,  30,   -95,  -6,   -83,  19,   18,   5,    -71,  -63,  98,   -45,  11,   54,   -102, -49,
    -10,  -112, -10,  -47,  -23,  -37,  60,   123,  49,   33,   71,   -5,   20,   -32,  41,   102,
    -54,  -77,  -32,  -39,  -89,  -9,   -97,  8,    -29,  -91,  -101, 8,    -34,  -13,  -112, 109,
    -101, 43,   51,   -61,  49,   -98,  -120, -72,  20,   13,   -8,   48,   13,   66,   -79,  7,
    64,   -35,  -60,  15,   -58,  -53,  34,   10,   -67,  -87,  -5,   -117, -117, -18,  -91,  16,
    26,   -24,  86,   -47,  -118, -4,   74,   -21,  103,  -73,  -7,   5,    -96,  24,   119,  -66,
    42,   43,   57,   -103, 50,   67,   -119, -34,  29,   94,   94,   124,  33,   -27,  -80,  60,
    -36,  109,  58,   45,   -39,  101,  108,  -120, -35,  123,  38,   45,   60,   -51,  -7,   64,
    95,   93,   -47,  111,  113,  -119, 114,  -107, 0,    -86,  79,   22,   103,  85,   -101, 53,
    -108, -63,  95,   -2,   -101, 41,   23,   119,  -39,  49,   84,   35,   -8,   -87,  92,   -21,
    26,   -44,  78,   27,   -15,  -59,  -15,  -125, 35,   58,   -78,  125,  122,  -121, 18,   109,
    -23,  50,   120,  46,   -12,  82,   -114, -108, 111,  75,   115,  7,    61,   93,   -77,  -121,
    -71,  55,   105,  -64,  -65,  -82,  12,   106,  9,    59,   -30,  -83,  57,   57,   -96,  42,
    -56,  -15,  -106, -49,  41,   63,   20,   13,   86,   92,   33,   -54,  -109, -57,  109,  45,
    98,   94,   -14,  91,   -111, 38,   -3,   -68,  81,   -15,  35,   112,  112,  3,    -39,  -33,
    -35,  -94,  -11,  40,   27,   64,   -88,  114,  77,   101,  61,   75,   -45,  1,    -51,  -3,
    27,   93,   -73,  -46,  -113, 52,   8,    -30,  -71,  95,   -52,  96,   1,    -121, -1,   52,
    -76,  -8,   95,   25,   -46,  -74,  63,   -45,  37,   -87,  -10,  98,   53,   94,   42,   -41,
    -112, -84,  -98,  -98,  -46,  -42,  0,    52,   126,  108,  62,   23,   61,   78,   36,   -110,
    106,  3,    -54,  57,   47,   -8,   85,   65,   -25,  -54,  -85,  31,   -101, -63,  -20,  46,
    10,   -73,  -100, 79,   124,  64,   -98,  -100, 99,   -85,  -104, -126, -17,  55,   66,   -47,
    -80,  -93,  104,  93,   -77,  106,  -3,   96,   19,   113,  99,   -62,  -34,  94,   59,   41,
    -35,  60,   42,   -55,  -47,  88,   47,   89,   -10,  33,   -114, -96,  51,   104,  78,   -87,
    24,   85,   -37,  -93,  -37,  -35,  65,   5,    59,   64,   13,   85,   39,   40,   40,   13,
    -17,  113,  -70,  115,  90,   -32,  115,  55,   -94,  -96,  -13,  57,   27,   21,   -74,  -76,
    97,   -115, 3,    -39,  -48,  -62,  -39,  56,   -44,  27,   -80,  -118, -94,  -27,  -116, -40,
    -123, 21,   -121, -79,  -23,  15,   -124, 30,   43,   -75,  -89,  83,   -109, -57,  92,   -39,
    -89,  92,   -45,  -81,  -123, 83,   -101, -23,  47,   -109, -112, 9,    111,  108,  114,  -64,
    -21,  7,    47,   -6,   89,   5,    -39,  0,    -84,  65,   -119, -57,  67,   117,  52,   -43,
    0,    -92,  -41,  49,   -6,   -58,  -95,  38,   93,   -17,  -58,  -100, 57,   -109, -39,  -79,
    61,   2,    -100, -98,  62,   -48,  -115, 8,    75,   13,   -36,  50,   2,    94,   22,   -35,
    35,   -44,  -69,  9,    85,   68,   99,   101,  65,   36,   -28,  85,   -109, -81,  -16,  7,
    22,   -29,  -99,  22,   -109, 93,   -101, 88,   70,   85,   -41,  100,  48,   -79,  78,   -82,
    -92,  -15,  52,   -3,   -118, 64,   -74,  -2,   121,  44,   58,   -34,  92,   115,  -85,  -11,
    -106, -61,  -103, -101, -66,  -99,  74,   89,   -89,  -98,  83,   7,    -17,  -43,  38,   34,
    120,  -98,  108,  24,   -2,   -51,  -113, -105, -120, 99,   -35,  -36,  89,   89,   13,   -62,
    -94,  94,   11,   -111, 117,  122,  -112, 23,   119,  65,   -18,  -87,  38,   -28,  -47,  104,
    -97,  -124, -121, 47,   -62,  -113, -106, -24,  63,   11,   53,   90,   64,   -122, -24,  -37,
    -29,  -33,  3,    -121, 17,   58,   -22,  -127, -71,  -37,  -94,  -100, -40,  -55,  16,   -24,
    27,   123,  -81,  82,   54,   101,  -113, -116, -3,   -62,  54,   100,  -70,  78,   -125, 27,
    -124, 5,    69,   -83,  -44,  -76,  7,    119,  86,   -43,  4,    -72,  74,   -50,  -65,  42,
    95,   -70,  -95,  -126, 82,   -69,  12,   11,   79,   21,   111,  -122, -74,  -34,  -29,  -87,
    -20,  -69,  -10,  -19,  11,   95,   119,  -24,  70,   -126, -50,  -72,  111,  -118, 26,   -80,
    -75,  126,  -92,  -123, 23,   -90,  69,   52,   14,   36,   -26,  7,    -28,  109,  121,  -106,
    82,   -92,  124,  -32,  112,  -30,  -83,  -61,  -48,  90,   25,   78,   -61,  -3,   -72,  70,
    -119, -98,  38,   13,   -113, -90,  99,   100,  121,  -107, -38,  -3,   28,   4,    -47,  -86,
    -53,  22,   114,  91,   118,  -16,  28,   -100, 37,   120,  53,   -124, 98,   11,   90,   104,
    85,   116,  19,   41,   -119, -84,  -62,  -89,  -33,  -63,  88,   -27,  -12,  107,  29,   -9,
    -63,  -77,  75,   -34,  16,   -101, 98,   -82,  -26,  5,    -52,  58,   10,   -115, 113,  68,
    -94,  68,   78,   94,   -82,  -42,  42,   -104, -117, 83,   15,   60,   -18,  70,   -127, -33,
    -118, -18,  -66,  61,   -42,  102,  58,   56,   -30,  106,  11,   14,   52,   9,    -124, 107,
    4,    51,   76,   -63,  -41,  -19,  -92,  -13,  -78,  91,   -1,   -71,  -60,  -63,  -102, -99,
    -8,   -63,  112,  70,   37,   118,  -103, 55,   -13,  -67,  -29,  32,   -92,  -78,  -5,   -47,
    -7,   29,   15,   -123, -36,  95,   80,   51,   -21,  60,   126,  109,  -61,  69,   92,   39,
    -113, -36,  -26,  -65,  -22,  -38,  -118, 94,   8,    -51,  54,   -102, -32,  -19,  -113, -95,
    46,   38,   -1,   16,   117,  106,  -63,  100,  1,    60,   43,   -55,  30,   11,   -43,  87,
    72,   -87,  -119, 8,    22,   -29,  39,   120,  -90,  -20,  -119, -127, 31,   -116, 68,   -66,
    -111, 92,   -95,  -113, -86,  23,   123,  -84,  35,   83,   -83,  9,    67,   -100, -72,  -94,
    -113, -54,  -116, 114,  -41,  -40,  -116, 81,   76,   -99,  -4,   -30,  116,  45,   73,   -63,
    90,   100,  86,   60,   88,   53,   57,   36,   21,   -82,  -97,  -4,   112,  61,   -3,   118,
    88,   20,   -38,  55,   -54,  59,   -125, -50,  -65,  20,   95,   -5,   15,   123,  -76,  116,
    -67,  82,   -123, -110, -60,  -60,  -106, 78,   66,   -112, 77,   -104, 54,   104,  113,  82,
    17,   18,   97,   -105, 12,   -121, -41,  68,   -18,  25,   87,   -57,  114,  -22,  -65,  -5,
    7,    -50,  -43,  -6,   35,   102,  -3,   54,   -6,   -21,  90,   -2,   126,  34,   -56,  113,
    0,    68,   22,   -1,   60,   -67,  111,  -91,  -41,  58,   123,  -22,  -2,   -110, -55,  56,
    43,   -1,   119,  102,  108,  111,  30,   87,   104,  57,   6,    -20,  -51,  108,  89,   -50,
    115,  39,   118,  -83,  -89,  -109, 108,  35,   -120, -75,  22,   -78,  62,   27,   21,   63,
    9,    105,  -47,  123,  -97,  -65,  108,  -10,  78,   20,   -119, 2,    -90,  -32,  -17,  -29,
    -18,  31,   -6,   90,   26,   -48,  33,   25,   -59,  -93,  58,   -80,  63,   -82,  68,   -11,
    92,   -123, 51,   -13,  38,   14,   -37,  54,   -94,  112,  0,    -115, 83,   88,   51,   20,
    -48,  112,  88,   -68,  30,   28,   115,  -51,  -102, 3,    -13,  -25,  -94,  -93,  -2,   -24,
    72,   63,   17,   4,    -75,  103,  -26,  -39,  56,   25,   8,    30,   59,   -87,  45,   -20,
    95,   -100, -119, 111,  -105, 89,   91,   28,   -59,  56,   16,   90,   106,  -92,  -56,  57,
    -41,  -85,  -85,  -12,  -58,  69,   40,   -75,  -34,  -63,  40,   -120, -70,  -102, 117,  56,
    15,   40,   65,   37,   60,   45,   74,   51,   97,   -43,  -16,  6,    53,   0,    105,  -78,
    -93,  -67,  26,   19,   36,   -65,  33,   44,   -28,  91,   -88,  -124, 88,   -61,  77,   -32,
    45,   -35,  57,   -105, -44,  98,   95,   -122, -14,  -80,  38,   -4,   110,  -24,  -91,  98,
    27,   31,   -45,  6,    -42,  97,   -100, 100,  -46,  -27,  -104, 86,   121,  -87,  -31,  121,
    -68,  119,  -58,  -35,  60,   86,   -103, -22,  39,   -74,  90,   -83,  -73,  -109, -38,  -8,
    -24,  76,   19,   -43,  -106, 110,  -47,  -123, -4,   13,   -98,  -57,  -44,  110,  6,    84,
    15,   97,   38,   87,   63,   74,   87,   48,   -93,  -36,  -6,   121,  -123, 94,   -91,  -62,
    -52,  112,  -106, 51,   28,   33,   83,   44,   65,   99,   -107, -122, 32,   95,   -54,  -9,
    3,    -41,  -44,  71,   -23,  40,   -42,  -88,  74,   120,  114,  -80,  -11,  -65,  120,  -80,
    -93,  61,   26,   -69,  71,   60,   34,   91,   9,    36,   -26,  -106, -95,  36,   46,   -75,
    24,   -101, -85,  97,   29,   55,   26,   -85,  9,    -101, -32,  70,   42,   -38,  13,   -39,
    5,    3,    53,   79,   -84,  91,   -123, 74,   -85,  -102, 74,   -61,  -101, 17,   -78,  -33,
    44,   -97,  24,   76,   -39,  -87,  -118, -84,  -41,  -53,  101,  -77,  114,  74,   -31,  17,
    15,   -83,  59,   -89,  -95,  -52,  46,   -56,  66,   64,   115,  -31,  69,   110,  125,  -8,
    -121, -120, 57,   31,   -26,  -124, -101, 46,   -94,  49,   -12,  -14,  -10,  -27,  -97,  -127,
    12,   -10,  -92,  21,   -96,  -7,   50,   -14,  -18,  -24,  42,   24,   27,   -2,   9,    95,
    -54,  -61,  112,  123,  88,   -15,  -1,   96,   -76,  -67,  73,   28,   -71,  -52,  24,   -84,
    39,   56,   18,   64,   -110, 59,   -82,  -30,  103,  -100, -83,  23,   16,   78,   20,   -91,
    -62,  -54,  -117, 117,  -42,  93,   92,   -57,  83,   114,  4,    -69,  15,   108,  -102, -76,
    101,  121,  23,   85,   -91,  1,    82,   48,   -20,  89,   22,   -20,  -45,  -101, 93,   93,
    100,  -120, -30,  -69,  9,    -47,  -90,  125,  -43,  -80,  -18,  108,  -33,  108,  5,    -94,
    -49,  -28,  -18,  -109, 99,   -48,  1,    11,   -19,  -1,   84,   7,    -103, -89,  17,   -74,
    -28,  84,   -73,  -67,  -52,  -77,  -109, -32,  -42,  62,   47,   -33,  -93,  -124, 85,   -85,
    19,   65,   -81,  33,   91,   -58,  100,  1,    4,    77,   17,   -96,  106,  -36,  -56,  19,
    -19,  -106, -39,  -47,  -32,  115,  -112, -24,  -108, 21,   37,   -28,  -123, 76,   91,   47,
    35,   -60,  21,   -48,  92,   30,   -16,  -111, -6,   -58,  -68,  -97,  -42,  45,   -106, 3,
    5,    30,   120,  -108, 78,   -87,  0,    -81,  16,   107,  12,   -90,  -60,  -78,  4,    -4,
    20,   66,   -111, -107, -123, 73,   -34,  76,   -63,  25,   -17,  -30,  36,   112,  45,   -97,
    80,   -90,  95,   -74,  39,   54,   79,   108,  121,  33,   -122, 72,   -93,  -16,  117,  -16,
    46,   -1,   32,   -67,  -78,  -77,  8,    -7,   22,   38,   -27,  -3,   31,   84,   23,   53,
    -44,  -15,  -87,  47,   -87,  31,   -3,   -90,  14,   -4,   -46,  111,  -74,  107,  -20,  -126,
    -54,  -15,  112,  -19,  64,   -120, -84,  25,   16,   -44,  -29,  48,   -32,  -79,  -98,  55,
    47,   83,   65,   -83,  -98,  -58,  4,    37,   -25,  -115, 22,   117,  -55,  124,  110,  122,
    92,   90,   -30,  -14,  47,   -31,  45,   -12,  -40,  -120, 124,  -110, -66,  -66,  5,    -85,
    -124, -114, -95,  80,   86,   -107, 43,   19,   98,   70,   85,   -69,  88,   8,    -61,  -124,
    -42,  -61,  -58,  -51,  121,  -85,  -78,  -127, -124, -118, -23,  75,   -64,  -53,  -19,  -94,
    53,   124,  -104, -64,  -84,  -113, 34,   108,  -17,  72,   -26,  -51,  6,    -7,   -5,   -52,
    -86,  60,   100,  126,  59,   -106, -58,  43,   -91,  -42,  -110, 47,   47,   51,   -3,   116,
    -2,   -120, 36,   -52,  56,   3,    78,   115,  -48,  74,   59,   121,  103,  35,   -85,  88,
    -27,  60,   56,   -29,  -27,  -3,   -78,  26,   -65,  110,  116,  -109, 38,   -113, 105,  -79,
    -91,  59,   117,  -39,  -64,  -60,  -52,  99,   45,   -54,  82,   -13,  66,   -45,  -73,  -21,
    76,   104,  46,   -116, -121, -47,  -77,  122,  -4,   -83,  -38,  73,   99,   74,   -73,  18,
    16,   -88,  -47,  -29,  45,   104,  -66,  21,   26,   -3,   39,   -36,  -61,  -83,  -69,  102,
    82,   -4,   69,   -105, -27,  -77,  -46,  -21,  -109, -58,  -122, -31,  -27,  -63,  20,   -48,
    14,   -94,  82,   -114, 73,   -93,  103,  -4,   72,   -5,   117,  94,   -89,  38,   118,  -43,
    -49,  39,   -42,  -75,  50,   60,   -107, 116,  -94,  -66,  46,   -107, -115, 23,   104,  52,
    -91,  79,   0,    100,  82,   97,   79,   -16,  36,   -90,  110,  7,    89,   75,   -35,  44,
    86,   12,   -127, 63,   -22,  -10,  119,  80,   26,   29,   114,  99,   -65,  -18,  -33,  -11,
    -92,  14,   -49,  96,   -81,  3,    -61,  36,   65,   20,   79,   -54,  -23,  -71,  95,   58,
    -73,  76,   -32,  12,   -9,   72,   -100, -49,  68,   -125, -3,   15,   48,   81,   100,  -78,
    69,   -45,  39,   -32,  30,   22,   92,   74,   -120, -42,  -117, 32,   -105, 45,   30,   -98,
    64,   112,  -12,  -93,  37,   105,  59,   -38,  35,   -116, 46,   81,   93,   -123, 36,   1,
    23,   -12,  65,   -115, -52,  28,   90,   -45,  -21,  87,   31,   90,   -18,  52,   -10,  -56,
    18,   -40,  -23,  98,   50,   117,  -77,  -95,  109,  -44,  -116, -7,   119,  27,   -17,  -30,
    77,   -88,  35,   111,  106,  5,    9,    -11,  87,   -4,   47,   110,  27,   77,   -100, 103,
    -114, 18,   -119, -121, 7,    -45,  75,   108,  -102, -29,  -109, -49,  122,  -117, 66,   -61,
    -106, -1,   19,   -92,  -58,  52,   -124, -40,  -90,  70,   -27,  -81,  107,  -8,   12,   20,
    10,   -86,  52,   64,   -125, 73,   105,  -13,  6,    -125, 68,   36,   39,   -116, -96,  -40,
    -27,  66,   119,  -59,  70,   2,    24,   74,   -114, 101,  97,   -8,   -9,   -65,  -47,  36,
    4,    -2,   80,   125,  -60,  123,  86,   -3,   36,   46,   1,    106,  73,   32,   -104, 108,
    92,   28,   19,   9,    -84,  -25,  -119, -63,  84,   -28,  -34,  -26,  109,  37,   -114, -108,
    -103, -114, 32,   -117, -108, -19,  18,   -92,  -89,  126,  -122, 115,  -123, 104,  15,   -95,
    -39,  97,   -120, 87,   -43,  14,   121,  69,   -93,  75,   41,   -71,  -27,  -115, 59,   -51,
    69,   -100, -126, -85,  -80,  118,  79,   35,   -12,  20,   -55,  -48,  -102, 32,   108,  27,
    -91,  -69,  37,   109,  33,   11,   22,   -6,   43,   101,  24,   -108, -17,  -17,  59,   81,
    -8,   -67,  -90,  125,  -90,  68,   97,   -99,  40,   -19,  45,   -1,   45,   -1,   123,  -119,
    -82,  -63,  119,  -108, -55,  31,   77,   104,  -120, 4,    76,   95,   124,  -64,  -44,  20,
    -85,  112,  49,   38,   -96,  -68,  -34,  -108, 21,   126,  -100, 59,   5,    94,   75,   -104,
    79,   -32,  86,   -37,  -30,  27,   66,   -105, 123,  59,   72,   -71,  -61,  104,  -72,  -7,
    -21,  25,   108,  -25,  44,   -104, 76,   -8,   24,   109,  23,   43,   -68,  8,    -54,  -39,
    24,   -81,  23,   95,   -67,  54,   69,   -39,  -103, -110, 119,  -52,  -81,  15,   35,   -79,
    -102, 110,  -7,   -43,  45,   -111, 21,   94,   49,   -35,  -57,  77,   126,  74,   -66,  -24,
    -108, -58,  -53,  5,    59,   -118, -48,  77,   101,  -123, -2,   -80,  42,   89,   -90,  -79};

const int8_t ic_bench_example_output_tensors[] = {-128, -128, -128, -126, -128,
                                                  -128, 126,  -128, -128, -128};
