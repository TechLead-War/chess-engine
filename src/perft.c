#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <locale.h>
#include "board.h"
#include "fen.h"
#include "movegen.h"
#include "san.h"
#include "bitboards.h"

typedef unsigned long long u64;
u64 perft(Board board, int depth, bool divide);
void parseArgs(int argc, char* argv[], int* depth, bool* extraOutput, int* cFens, bool* divide);

int NUM_FENS = 126;
char* FENS[250];
u64 RESULTS[5][126];

int main(int argc, char* argv[]) {
    initBitboards();
    initMoveGeneration();
    setlocale(LC_NUMERIC, ""); // For prettier number formatting

    int depth = 0;
    int cFens = NUM_FENS;
    bool showProgress = false;
    bool divide = false;
    parseArgs(argc, argv, &depth, &showProgress, &cFens, &divide);

    int correct = 0;
    u64 totalNodes = 0;
    clock_t start = clock();
    for (int i = 0; i < cFens;i++) {
        char* fen = FENS[i];

        Board board = {};
        setFen(&board, fen);

        u64 nodes = perft(board, depth, divide);
        totalNodes += nodes;
        u64 expected = RESULTS[depth-1][i];
        bool matches = nodes == expected;
        correct += matches ? 1 : 0;

        if (showProgress && matches) {
            printf("SUCCESS %d: %s\n", i, fen);
        } else if (!matches) {
            printf("FAIL %d: %s\n", i, fen);
        }
    }

    double timeSpent = (double) (clock() - start) / CLOCKS_PER_SEC;
    int nps = totalNodes / timeSpent;

    printf("\n");
    printf("Depth:          %d\n", depth);
    printf("Correct:        %d of %d\n", correct, cFens);
    printf("Time elapsed:   %.2fs\n", timeSpent);
    printf("Nps:            %'d\n", nps);
    printf("Total nodes:    %'llu\n", totalNodes);
    return 0;
}

u64 perft(Board board, int depth, bool divide) {
  u64 nodes = 0;
  Move moves[256];
  int numMoves = legalMoves(&board, moves);

  if (depth == 1)  {
    int legal = 0;

    for (int i = 0; i < numMoves; i++) {
      if (moves[i].validation == LEGAL) legal++;
    }

    return legal;
  }

  for (int i = 0; i < numMoves; i++) {
    if (moves[i].validation == LEGAL) {
      // Copy-make
      Board cpy = board;
      pushMove(&cpy, moves[i]);

      u64 count = perft(cpy, depth-1, false);
      if (divide) {
        char san[6];
        memset(&san, 0, sizeof(san));
        moveToSan(moves[i], san);
        printf("%s: %llu\n", san, count);
      }
      nodes += count;
    }
  }

  return nodes;
}

void parseArgs(int argc, char* argv[], int* depth, bool* extraOutput, int* cFens, bool* divide) {
    if (argc < 3) {
        printf("Depth must be provided using the -d flag\n");
        exit(0);
    }

    for (int i = 1; i < argc; i++) {
      char* flag = argv[i];

      if (strcmp("-d", flag) == 0) {
          *depth = atoi(argv[i+1]);
      }

      if (strcmp("-fens", flag) == 0) {
          *cFens = atoi(argv[i+1]);
      }

      if (strcmp("--show-progress", flag) == 0) {
          *extraOutput = true;
      }

      if (strcmp("--divide", flag) == 0) {
          *divide = true;
      }
    }

    if (*depth == 0) {
        printf("Depth should be within 1-5\n");
        exit(0);
    }
}

char* FENS[250] = {
  "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
  "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1",
  "4k3/8/8/8/8/8/8/4K2R w K - 0 1",
  "4k3/8/8/8/8/8/8/R3K3 w Q - 0 1",
  "4k2r/8/8/8/8/8/8/4K3 w k - 0 1",
  "r3k3/8/8/8/8/8/8/4K3 w q - 0 1",
  "4k3/8/8/8/8/8/8/R3K2R w KQ - 0 1",
  "r3k2r/8/8/8/8/8/8/4K3 w kq - 0 1",
  "8/8/8/8/8/8/6k1/4K2R w K - 0 1",
  "8/8/8/8/8/8/1k6/R3K3 w Q - 0 1",
  "4k2r/6K1/8/8/8/8/8/8 w k - 0 1",
  "r3k3/1K6/8/8/8/8/8/8 w q - 0 1",
  "r3k2r/8/8/8/8/8/8/R3K2R w KQkq - 0 1",
  "r3k2r/8/8/8/8/8/8/1R2K2R w Kkq - 0 1",
  "r3k2r/8/8/8/8/8/8/2R1K2R w Kkq - 0 1",
  "r3k2r/8/8/8/8/8/8/R3K1R1 w Qkq - 0 1",
  "1r2k2r/8/8/8/8/8/8/R3K2R w KQk - 0 1",
  "2r1k2r/8/8/8/8/8/8/R3K2R w KQk - 0 1",
  "r3k1r1/8/8/8/8/8/8/R3K2R w KQq - 0 1",
  "4k3/8/8/8/8/8/8/4K2R b K - 0 1",
  "4k3/8/8/8/8/8/8/R3K3 b Q - 0 1",
  "4k2r/8/8/8/8/8/8/4K3 b k - 0 1",
  "r3k3/8/8/8/8/8/8/4K3 b q - 0 1",
  "4k3/8/8/8/8/8/8/R3K2R b KQ - 0 1",
  "r3k2r/8/8/8/8/8/8/4K3 b kq - 0 1",
  "8/8/8/8/8/8/6k1/4K2R b K - 0 1",
  "8/8/8/8/8/8/1k6/R3K3 b Q - 0 1",
  "4k2r/6K1/8/8/8/8/8/8 b k - 0 1",
  "r3k3/1K6/8/8/8/8/8/8 b q - 0 1",
  "r3k2r/8/8/8/8/8/8/R3K2R b KQkq - 0 1",
  "r3k2r/8/8/8/8/8/8/1R2K2R b Kkq - 0 1",
  "r3k2r/8/8/8/8/8/8/2R1K2R b Kkq - 0 1",
  "r3k2r/8/8/8/8/8/8/R3K1R1 b Qkq - 0 1",
  "1r2k2r/8/8/8/8/8/8/R3K2R b KQk - 0 1",
  "2r1k2r/8/8/8/8/8/8/R3K2R b KQk - 0 1",
  "r3k1r1/8/8/8/8/8/8/R3K2R b KQq - 0 1",
  "8/1n4N1/2k5/8/8/5K2/1N4n1/8 w - - 0 1",
  "8/1k6/8/5N2/8/4n3/8/2K5 w - - 0 1",
  "8/8/4k3/3Nn3/3nN3/4K3/8/8 w - - 0 1",
  "K7/8/2n5/1n6/8/8/8/k6N w - - 0 1",
  "k7/8/2N5/1N6/8/8/8/K6n w - - 0 1",
  "8/1n4N1/2k5/8/8/5K2/1N4n1/8 b - - 0 1",
  "8/1k6/8/5N2/8/4n3/8/2K5 b - - 0 1",
  "8/8/3K4/3Nn3/3nN3/4k3/8/8 b - - 0 1",
  "K7/8/2n5/1n6/8/8/8/k6N b - - 0 1",
  "k7/8/2N5/1N6/8/8/8/K6n b - - 0 1",
  "B6b/8/8/8/2K5/4k3/8/b6B w - - 0 1",
  "8/8/1B6/7b/7k/8/2B1b3/7K w - - 0 1",
  "k7/B7/1B6/1B6/8/8/8/K6b w - - 0 1",
  "K7/b7/1b6/1b6/8/8/8/k6B w - - 0 1",
  "B6b/8/8/8/2K5/5k2/8/b6B b - - 0 1",
  "8/8/1B6/7b/7k/8/2B1b3/7K b - - 0 1",
  "k7/B7/1B6/1B6/8/8/8/K6b b - - 0 1",
  "K7/b7/1b6/1b6/8/8/8/k6B b - - 0 1",
  "7k/RR6/8/8/8/8/rr6/7K w - - 0 1",
  "R6r/8/8/2K5/5k2/8/8/r6R w - - 0 1",
  "7k/RR6/8/8/8/8/rr6/7K b - - 0 1",
  "R6r/8/8/2K5/5k2/8/8/r6R b - - 0 1",
  "6kq/8/8/8/8/8/8/7K w - - 0 1",
  "6KQ/8/8/8/8/8/8/7k b - - 0 1",
  "K7/8/8/3Q4/4q3/8/8/7k w - - 0 1",
  "6qk/8/8/8/8/8/8/7K b - - 0 1",
  "6KQ/8/8/8/8/8/8/7k b - - 0 1",
  "K7/8/8/3Q4/4q3/8/8/7k b - - 0 1",
  "8/8/8/8/8/K7/P7/k7 w - - 0 1",
  "8/8/8/8/8/7K/7P/7k w - - 0 1",
  "K7/p7/k7/8/8/8/8/8 w - - 0 1",
  "7K/7p/7k/8/8/8/8/8 w - - 0 1",
  "8/2k1p3/3pP3/3P2K1/8/8/8/8 w - - 0 1",
  "8/8/8/8/8/K7/P7/k7 b - - 0 1",
  "8/8/8/8/8/7K/7P/7k b - - 0 1",
  "K7/p7/k7/8/8/8/8/8 b - - 0 1",
  "7K/7p/7k/8/8/8/8/8 b - - 0 1",
  "8/2k1p3/3pP3/3P2K1/8/8/8/8 b - - 0 1",
  "8/8/8/8/8/4k3/4P3/4K3 w - - 0 1",
  "4k3/4p3/4K3/8/8/8/8/8 b - - 0 1",
  "8/8/7k/7p/7P/7K/8/8 w - - 0 1",
  "8/8/k7/p7/P7/K7/8/8 w - - 0 1",
  "8/8/3k4/3p4/3P4/3K4/8/8 w - - 0 1",
  "8/3k4/3p4/8/3P4/3K4/8/8 w - - 0 1",
  "8/8/3k4/3p4/8/3P4/3K4/8 w - - 0 1",
  "k7/8/3p4/8/3P4/8/8/7K w - - 0 1",
  "8/8/7k/7p/7P/7K/8/8 b - - 0 1",
  "8/8/k7/p7/P7/K7/8/8 b - - 0 1",
  "8/8/3k4/3p4/3P4/3K4/8/8 b - - 0 1",
  "8/3k4/3p4/8/3P4/3K4/8/8 b - - 0 1",
  "8/8/3k4/3p4/8/3P4/3K4/8 b - - 0 1",
  "k7/8/3p4/8/3P4/8/8/7K b - - 0 1",
  "7k/3p4/8/8/3P4/8/8/K7 w - - 0 1",
  "7k/8/8/3p4/8/8/3P4/K7 w - - 0 1",
  "k7/8/8/7p/6P1/8/8/K7 w - - 0 1",
  "k7/8/7p/8/8/6P1/8/K7 w - - 0 1",
  "k7/8/8/6p1/7P/8/8/K7 w - - 0 1",
  "k7/8/6p1/8/8/7P/8/K7 w - - 0 1",
  "k7/8/8/3p4/4p3/8/8/7K w - - 0 1",
  "k7/8/3p4/8/8/4P3/8/7K w - - 0 1",
  "7k/3p4/8/8/3P4/8/8/K7 b - - 0 1",
  "7k/8/8/3p4/8/8/3P4/K7 b - - 0 1",
  "k7/8/8/7p/6P1/8/8/K7 b - - 0 1",
  "k7/8/7p/8/8/6P1/8/K7 b - - 0 1",
  "k7/8/8/6p1/7P/8/8/K7 b - - 0 1",
  "k7/8/6p1/8/8/7P/8/K7 b - - 0 1",
  "k7/8/8/3p4/4p3/8/8/7K b - - 0 1",
  "k7/8/3p4/8/8/4P3/8/7K b - - 0 1",
  "7k/8/8/p7/1P6/8/8/7K w - - 0 1",
  "7k/8/p7/8/8/1P6/8/7K w - - 0 1",
  "7k/8/8/1p6/P7/8/8/7K w - - 0 1",
  "7k/8/1p6/8/8/P7/8/7K w - - 0 1",
  "k7/7p/8/8/8/8/6P1/K7 w - - 0 1",
  "k7/6p1/8/8/8/8/7P/K7 w - - 0 1",
  "3k4/3pp3/8/8/8/8/3PP3/3K4 w - - 0 1",
  "7k/8/8/p7/1P6/8/8/7K b - - 0 1",
  "7k/8/p7/8/8/1P6/8/7K b - - 0 1",
  "7k/8/8/1p6/P7/8/8/7K b - - 0 1",
  "7k/8/1p6/8/8/P7/8/7K b - - 0 1",
  "k7/7p/8/8/8/8/6P1/K7 b - - 0 1",
  "k7/6p1/8/8/8/8/7P/K7 b - - 0 1",
  "3k4/3pp3/8/8/8/8/3PP3/3K4 b - - 0 1",
  "8/Pk6/8/8/8/8/6Kp/8 w - - 0 1",
  "n1n5/1Pk5/8/8/8/8/5Kp1/5N1N w - - 0 1",
  "8/PPPk4/8/8/8/8/4Kppp/8 w - - 0 1",
  "n1n5/PPPk4/8/8/8/8/4Kppp/5N1N w - - 0 1",
  "8/Pk6/8/8/8/8/6Kp/8 b - - 0 1",
  "n1n5/1Pk5/8/8/8/8/5Kp1/5N1N b - - 0 1",
  "8/PPPk4/8/8/8/8/4Kppp/8 b - - 0 1",
  "n1n5/PPPk4/8/8/8/8/4Kppp/5N1N b - - 0 1",
};

u64 RESULTS[5][126] = {
    // Depth 1 results
    { 20,48,15,16,5,5,26,5,12,15,3,4,26,25,25,25,26,25,25,5,5,15,16,5,26,3,4,12,15,26,26,25,25,
      25,25,25,14,11,19,3,17,15,16,4,17,3,17,21,21,7,6,17,7,21,19,36,19,36,2,2,6,22,2,6,3,3,1,
      1,7,1,1,3,3,5,2,2,3,3,5,8,8,4,3,3,5,8,8,4,4,5,5,4,5,4,3,4,5,4,5,4,5,4,5,4,5,4,5,4,5,5,7,
      5,4,5,4,5,5,7,11,24,18,24,11,24,18,24,
    },
    // Depth 2 results
    { 400,2039,66,71,75,80,112,130,38,65,32,49,568,567,548,547,583,560,560,75,80,66,71,130,112,
      32,49,38,65,568,583,560,560,567,548,547,195,156,289,51,54,193,180,68,54,51,278,316,144,143,
      106,309,143,144,275,1027,275,1027,36,36,35,43,36,35,7,7,3,3,35,3,3,7,7,35,8,8,9,9,25,61,61,15,
      9,9,25,61,61,15,19,19,22,16,22,16,15,16,19,19,22,16,22,16,15,16,22,16,22,16,25,25,49,22,16,22,
      16,25,25,49,97,421,270,496,97,421,270,496,
    },
    // Depth 3 results
    { 8902,97862,1197,1287,459,493,3189,782,564,1018,134,243,13744,14095,13502,13579,14252,13592,13607,459,
      493,1197,1287,782,3189,134,243,564,1018,13744,14252,13592,13607,14095,13502,13579,2760,1636,4442,345,835,
      2816,2290,1118,835,345,4607,5744,3242,1416,1829,5133,1416,3242,5300,29215,5300,29227,143,143,495,1015,143,
      495,43,43,12,12,210,12,12,43,43,182,44,44,57,57,180,483,411,90,57,57,180,411,483,89,117,116,139,101,139,
      101,84,101,117,117,139,101,139,101,102,101,139,101,139,101,161,161,378,139,101,139,101,161,161,378,887,7421,4699,9483,887,7421,4699,9483,
    },
    // Depth 4 results
    { 197281,4085603,7059,7626,8290,8897,17945,22180,2219,4573,2073,3991,314346,328965,312835,316214,334705,317324,
      320792,8290,8897,7059,7626,22180,17945,2073,3991,2219,4573,314346,334705,317324,320792,328965,312835,316214,
      38675,20534,73584,5301,5910,40039,24640,16199,5910,5301,76778,93338,32955,31787,31151,93603,31787,32955,104342,
      771461,104342,771368,3637,3637,8349,4167,3637,8349,199,199,80,80,1091,80,80,199,199,1091,282,282,360,360,1294,
      3213,3213,534,360,360,1294,3213,3213,537,720,716,877,637,877,637,573,637,720,712,877,637,877,637,569,637,877,637,
      877,637,1035,1035,2902,877,637,877,637,1035,1035,2902,8048,124608,79355,182838,8048,124608,79355,182838,
    },
    // Depth 5 results
    { 4865609,193690690,133987,145232,47635,52710,532933,118882,37735,80619,10485,20780,7594526,8153719,7736373,7878456,
      8198901,7710115,7848606,47635,52710,133987,145232,118882,532933,10485,20780,37735,80619,7594526,8198901,7710115,7848606,8153719,
      7736373,7878456,570726,223507,1198299,38348,92250,582642,288141,281190,92250,38348,1320507,1713368,787524,310862,530585,1591064,
      310862,787524,2161211,20506480,2161211,20521342,14893,14893,166741,105749,14893,166741,1347,1347,342,342,7028,342,342,1347,1347,
      5408,1814,1814,1969,1969,8296,23599,21637,3450,1969,1969,8296,21637,23599,3309,4661,4786,6112,4354,6112,4354,3013,4271,5014,4658,
      6112,4354,6112,4354,4337,4271,6112,4354,6112,4354,7574,7574,24122,6112,4354,6112,4354,7574,7574,24122,90606,2193768,1533145,3605103,90606,2193768,1533145,3605103,
    }
};


