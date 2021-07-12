/*

This code requires
hop2.h, minihop2.h, hex.h, laser.h, and levels.h
to solve Hex-a-hop levels to find the lowest possible score.

Scores will be output as they are obtained, and will progressively
decrease. When a level is finished, an output message will appear. The
last score is the lowest possible unless time limit is reached.

Along with the scores, a solution string will be output that uses...
  1 is up, 2 is NE, 3 is SE, 4 is down, 5 is SW, 6 is NW, and 7 is jump

A brute-force search is the main idea behind the code. Some levels take a
very long to run and some might take many thousands of years to finish.

This code is designed for the Hex-a-hop levels and may run incorrectly on
user-made levels because some possible combinations of map items are
never used in Hex-a-hop (such as an object [anti-ice or jump] in path of
a boat or an object near a grey tile), so I never coded these situations
in.


Unlike hex_a_hop.cpp, this code uses minihop2() to take care of regions
with green&blue tiles (not elevated). Whereas hex_a_hop.cpp uses minihop()
to take care of regions where Emi moves without changing the level map.

See hex_a_hop.cpp for results.


(c) 2021 Bradley Knockel

*/



//// load in libraries and create some global variables

#include <iostream>
#include <chrono>
#include <ctime>

// to be able to "pass by value"
#include <vector>   // for variable-length (on heap) 1D arrays
struct wrapper{ unsigned char l[210]; };   // level
struct bagger{ int s[6]; };            // state of Emi
struct bag{ int spinnerTracker[2]; };  // for spinners to prevent infinite loops
struct wrap{ bool notBlocked[6]; };   // for lowering 4's and 6's on level map
struct can{ bool levelSituation[5]; };  // for telling which types of items still exist on level map (for speeding up runtime)

// for counting in minihop2()
#include <bits/stdc++.h>    // std::count
#include <algorithm>  // std::sort and std::find
#include <utility>    // std::pair

// made global for recursively-called hop() to be coded more easily
int lSize[3];      // set by my levels() function
std::time_t timeStart;  // when level started running
long max_time;     // stop if this many seconds elapses
int nMax;          // score not to exceed this value
int progressDepth; // to set how many progress statements are printed
long long skipTo;       // for skipping to a certain amount of progress
int skipToDigits;  // how many digits in skipTo
bool l7;        // is anything in the level a 7 (for speeding up runtime)

// Used in recursively-called hop() and/or laser().
// Made global to try to speed up code.
// I think this is especially important for certain std::vector objects
//   so that a vector of ideal size can be already reserved/allocated
//   (making global requires method .resize(0) to be called).
bagger bagged0;       // for storing locations other than Emi's
std::time_t timeTemp; // for storing the current time
std::vector<int> list; // stores results of laser()
std::vector<int> listTemp, ice1, ice2, shot, shots, go1, go1b, go2;  // for laser()
int new6[7];          // for spinners (new6[0] never used)
long long multiplier, progress;  // for checking progress

// Used in minihop2().
// Made global to try to speed up code.
std::vector<int> score;       // the score
std::vector<can> situations;    // the level situation
std::vector<wrap> ws;           // blocked directions when lowering tiles
std::vector<std::vector<int> > DDs;          // all the dd's
std::vector<bagger> bags;                   // the state of Emi
std::vector<std::vector<int> > greensToGets;  // greensToGet vector
std::vector<std::vector<int> > pastIndices;  // the main list (helps remove identical paths)






//// load in my functions (order matters)

#include "levels.h"
#include "hex.h"
#include "laser.h"
#include "minihop2.h"
#include "hop2.h"





int main() {



  //// time for you to set parameters!

  //list level(s) to be solved
  int level[] = {1,2,85};
  //these are positive integers not greater than 100

  //make sure below reflects the best scores listed above
  int best[]={34,52,26,49,22,37,24,33,124,30,361,110,101,187,29,63,65,68,51,20,
    216,93,69,165,144,119,107,81,32,57,80,57,79,35,29,34,19,68,45,30,33,
    20,32,16,76,50,98,291,41,167,59,103,51,41,65,116,85,39,79,103,80,
    27,324,92,75,45,37,80,61,68,38,51,82,62,38,92,33,36,37,73,105,18,
    119,65,20,43,68,100,51,37,26,84,52,135,261,50,37,70,23,11};

  // if you don't want to force quit in the case a level is taking
  // too long to run, use this...
  //max_time = 1e4;    // almost 3 hours
  //max_time = 1e5;    // almost 28 hours
  max_time = 1e7;    // about 1/3 of a year lol
  // This is max time in seconds that the code will run on a level.
  // Unless you have ECC memory (pricy!) and a backup generator/battery,
  // don't go crazy with this unless progress is printed often enough.

  // Set to -1 for no progress statements.
  // Set to 4 for a reasonable amount of progress on most levels,
  //   though a larger number is often needed.
  progressDepth = 4;

  // Set skipToDigits to -1 for no skipping.
  // If skipping, be sure that skipToDigits matches skipTo.
  // Not larger than 19 digits (for a 64-bit long long skipTo).
  skipTo = -1;
  skipToDigits = -1;
  // If skipping past a solution that was found, the code may find
  //   another solution with the same score and may make progress
  //   more slowly (unless you reduce best[level[j]+1] by 1).
  // If skipToDigits is larger than progressDepth, you can see
  //   some initial progress statements that are less than skipTo,
  //   but just ignore them.




  //// do it!

  std::cout << '\n';

  for(int j=0; j<(*(&level + 1) - level); j++){

    // the level number
    int i = level[j];

    // retrieve the level information into wrapped.l[], start[], and lSize[]
    wrapper wrapped;
    int start[2];
    levels(i, wrapped.l, start);

    // Get l7 for the level.
    // Note that l7 cannot change because you can never destroy the final laser
    //   that you are standing on.
    // Note that l9 can change (see levels 31, 52, and 69).
    l7 = false;
    for(int i1=0; i1<lSize[2]; i1++) {
      if(wrapped.l[i1] == 7 || wrapped.l[i1] == 107 || wrapped.l[i1] == 207) l7 = true;
    }

    // Speed up certain levels!
    // Do this only if you're sure that 2 or more greens can never be
    //   destroyed at the same time in a score advantageous way.
    // Doing this is only ever helpful if level contains jump items or ice.
    // Note that level 72 finishes within several hours without this assistance.
    // Note that level 93 finishes within a couple hours without this assistance.
    // You should feel very nervous when adding to this list!
    if (i==72 || i==93)
      l7 = false;

    // state array
    bagger bagged = {{0,0,0,-1,0,0}};
    bagged.s[0] = (lSize[1]*start[0] + start[1]) / 2;
    bagged.s[1] = start[0];
    bagged.s[2] = start[1];

    // declare history vectors and greensToGet
    std::vector<int> dd, WW;
    std::vector<int> greensToGet;

    // initialize b.spinnerTracker for spinner tiles
    bag b = {{-1,-1}};

    // Initialize c.levelSituation
    //   {true if 4, true if 6, true unless !l7, true if anti-ice, true if jump}
    can c = {{false, false, true, false, false}};
    if (!l7) c.levelSituation[2] = false;  // only care about ice if there are lasers
    for(int i1=0; i1<lSize[2]; i1++) {
      if (wrapped.l[i1] > 199) c.levelSituation[4] = true;
      else if (wrapped.l[i1] > 99) c.levelSituation[3] = true;
      else if (wrapped.l[i1] == 6) c.levelSituation[1] = true;
      else if (wrapped.l[i1] == 4) c.levelSituation[0] = true;
    }

    // give nMax an initial value
    nMax = best[i-1];
    // nMax is the current maximum_score, which is either the value set here
    //   or the value of the best score found so far minus one

    // print
    timeStart = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()); 
    std::cout << " ___ running level " << i << " ___        " << ctime(&timeStart) << std::flush;

    // optimally solve the level
    hop2(wrapped, bagged, bagged, -1, 0, dd, greensToGet, b, c, WW);

    // print
    timeTemp = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::cout << " * * * level " << i << " concluded * * *   " << ctime(&timeTemp) << std::endl;

  }


}
