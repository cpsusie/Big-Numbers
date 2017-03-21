#include "stdafx.h"

static double meanSum[7][7];
static int    bestMinChoise[7];

#define PLAYCOUNT 5000000

#define kastTerning() randInt(1,6);

static double playRound(int diceCount, int minChoise) {
  if(diceCount == 1) {
    return kastTerning();
  }
  int sum       = 0;
  int maxK      = 0;

  for(int i = diceCount; i--;) {
    const int k = kastTerning();
    if(k >= minChoise) { // we remove at least one
      sum += k;
      diceCount--;
      for(;i; i--) { // no need to find maxK. Continue in this loop, and return
        const int k = kastTerning();
        if(k >= minChoise) {
          sum += k;
          diceCount--;
        }
      }
      if(diceCount) {
        return meanSum[diceCount][bestMinChoise[diceCount]] + sum;
      } else {
        return sum;
      }
    } else if(k > maxK) {
      maxK = k;
    }
  }

  return meanSum[diceCount-1][bestMinChoise[diceCount-1]] + maxK; // if we get here diceCount is unchanged => none removed
}

static int playRealRound(int diceCount) {
  if(diceCount == 1) {
    return kastTerning();
  }
  int sum       = 0;
  int dicesLeft = diceCount;
  int maxK      = 0;

  const int minChoise = bestMinChoise[diceCount];

  for(int i = 0; i < diceCount; i++) {
    const int k = kastTerning();
    if(k >= minChoise) {
      sum += k;
      dicesLeft--;
    } else if(k > maxK) {
      maxK = k;
    }
  }
  if(dicesLeft == diceCount) { // none has been selected
    return maxK + playRealRound(diceCount-1);
  } else if(dicesLeft == 0) {
    return sum;
  } else {
    return sum  + playRealRound(dicesLeft  );
  }
}

int main(int argc, char **argv) {
  randomize();
  for(int diceCount = 0; diceCount < 7; diceCount++) {
    for(int minChoise = 0; minChoise < 7; minChoise++) {
      meanSum[diceCount][minChoise] = 0;
    }
    bestMinChoise[diceCount] = 0;
  }

  for(int diceCount = 1; diceCount <= 6; diceCount++) {
    printf("Dices:%d:", diceCount);
    for(int minChoise = 1; minChoise <= 6; minChoise++) {
      double sum = 0;
      for(int count = 0; count < PLAYCOUNT; count++) {
        sum += playRound(diceCount, minChoise);
      }
      meanSum[diceCount][minChoise] = sum / PLAYCOUNT;
      printf("%6.3lf ", meanSum[diceCount][minChoise]);
    }
    int bestChoise = 1;
    for(int minChoise = 2; minChoise <= 6; minChoise++) {
      if(meanSum[diceCount][minChoise] > meanSum[diceCount][bestChoise]) {
        bestChoise = minChoise;
      }
    }
    bestMinChoise[diceCount] = bestChoise;
    printf("    Best minChoise = %d\n", bestChoise);
  }

  const int bestChoise = bestMinChoise[6];
  unsigned int scoreCount[37];
  memset(scoreCount,0,sizeof(scoreCount));
//  bestMinChoise[5] = 5;

#define MAXPLAY 20000000
  for(int i = 0; i < MAXPLAY; i++) {
    if(i % 200000 == 0) {
      printf("%d/%d:%3.0lf%%\r", i, MAXPLAY, (double)i/MAXPLAY*100);
    }
    scoreCount[playRealRound(6)]++;
  }
  int akkumulated = 0;
  for(int score = 36; score > 0; score--) {
    if(scoreCount[score]) {
      akkumulated += scoreCount[score];
      printf("Score:%2d. Count:%7d %6.1lf%% %6.1lf%%\n"
            ,score
            ,scoreCount[score]
            ,(double)scoreCount[score]/MAXPLAY*100
            ,(double)akkumulated/MAXPLAY*100
            );
    }
  }
  return 0;
}
