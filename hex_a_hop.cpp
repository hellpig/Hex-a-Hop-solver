/*

This code requires
hop.h, minihop.h, hex.h, laser.h, minilaser.h, and levels.h
to solve Hex-a-hop levels to find the lowest possible score.

Scores will be output as they are obtained, and will progressively
decrease. When a level is finished, an output message will appear. The
last score is the lowest possible unless time limit is reached.

Along with the scores, a solution string will be output that uses...
  1 is up, 2 is NE, 3 is SE, 4 is down, 5 is SW, 6 is NW, and 7 is jump

A brute-force search is the main idea behind the code. Some levels take a
very long to run and some might take many thousands of years to finish.
Of the levels that finished, the longest running are...
  60, 19, 97, 86, 12, 95, 47
(approximately in order from longest to fastest)

This code is designed for the Hex-a-hop levels and may run incorrectly on
user-made levels because some possible combinations of map items are
never used in Hex-a-hop (such as an object [anti-ice or jump] in path of
a boat or an object near a grey tile), so I never coded these situations
in.

Printed progress statements from minihop() are denoted with a dash (-).
Because minihop() "looks ahead", do not use any printed
progress with a dash (-) for skipTo.
If progress is printed by minihop(), hop() will still print progress,
which causes some printed progress statements to have a length larger
than progressDepth.

(c) 2019 Bradley Knockel

here is a list of the levels and the optimal solutions that they produced

1: Mini Island (best: 34)             2212233241133354443555165566162441
2: Green Walls (best: 52)             2433235331132216651154515512664433544355112222422356
3: Trampolines (best: 26)             32442344664532332642166256
4: Orbital (best: 49)                 4345151121112343356443212112345435454321122234562
5: Weathervane (best: 22)             1224432244466635423444
6: Explorer (best: 37)                6161265534221123446453334321611654354
7: Hive (best: 24)                    434323212161654345616122
8: Triangular (best: 33)              211223333333555555566112223355154
9: Toughened Tiles (best: 124)        32234165543321325321244553221122346661261256
10: Archipelago (best: 30)            533564324532132345156326562265
11: Island (best: 361)                1212222333341666655554543544323222222114455555565611232522251
12: Stripes (best: 110)               1254223621155261132532253444453233556611
13: One Two One Two (best: 101)       32345654341654165666523332112215516554321
14: Deathtrap (best: 187)             154323432323216361612123632
15: Bridges (best: 29)                33332111666651365433332121666
16: Not a Knot (best: 63)             434213116512334222345156411122333423456455545116453456156611114
17: Winding Order (best: 65)          24454321335321312243211516615165345461511266444612444643544431112
18: Laser Tiles (best: 68)            3235561123212343212455126543
19: Sniper (best: 51)                 65663656666333331313141321463132621115652
20: Snowflake (best: 20)              22233466433221151641
?21: Trigger Happy (best: 216)         
22: Deadly Snowflake (best: 93)       31222333444456614145566
?23: A Mountain (best: 69)             
?24: More Stripes (best: 165)          
25: Gun Platform (best: 144)          1411125222523325431452561436123634
26: Nucleus (best: 119)               251236655546444143533323522213111411645
27: Eagerness (best: 107)             1125443332541434464122211262544452331
28: Wand (best: 81)                   114112523364335152551
29: There and Back (best: 32)         33216121234462233454561255543343
30: Turtle (best: 57)                 6115511266553353663442325523243226611
?31: Fetch Quest (best: 80)            
?32: A Strange Place (best: 57)        
33: Rolling Hexagons (best: 79)       131364112536125361253611122233445516446
34: Transport Hub (best: 35)          55211436664444435321633316111655441
35: Spinner Tiles (best: 29)          43653433314226311224343634654
36: Icy Tiles (best: 34)              3466611112345555612111654432116554
37: Small Cog (best: 19)              3361414146122344446
38: All About Preparation (best: 68)  33334332466666616611122333314222342165656655654332232354436654661623
39: A Slippery Situation (best: 45)   2363124212244464356432212
40: Bouncing Required (best: 30)      122242341655115453431232564666
41: Ferrying (best: 33)               232322114351144161361234616554441
42: Floating (best: 20)               21633411563246635326
43: Three Ways To Go (best: 32)       33325416661112461463444555525222
44: Lure (best: 16)                   3551212333565436
45: Refraction (best: 76)             33325432113444464555566231
46: Please Skate Safely (best: 50)    23216553425556612541
47: All Wound Up (best: 98)           44452323322155666121242213243345432111161461556565634561316434154353
?48: Maze (best: 291)                  
49: Three More Ways (best: 41)        32334615362563655645135221112234514416541
?50: Revolver Cannon (best: 167)       
51: Telephone (best: 59)              213165151655554312224416132434446565111
52: Green Honey (best: 103)           551656543234565611666622332232332454336665556666163433332556522
53: Switch (best: 51)                 2525216424245542522321345445643
54: Island Variation (best: 41)       23212334451165566554416234322116445665564
?55: Motion Sickness (best: 65)        
56: Tri Mesh (best: 116)              142632234355555111252332225222433513
?57: Deslippify (best: 85)             
58: A Little Light Lifting (best: 39) 224223334644445661555511121324324346125
?59: Leftovers (best: 79)              
60: Two Fish (best: 103)              3523445415422336211126333544221344435231112
61: Demolition (best: 80)             4353211224233244144566561322222235344466
62: Crooked (best: 27)                321243213266656113266555414
?63: Route Finder (best: 324)          
64: Lifts (best: 92)                  13611222146341532344443226341453
65: Clearance (best: 75)              261123556523163425143456233232321616446155611
66: Wheel (best: 45)                  111112421242333334446116555453235322222343125
67: More Lifting (best: 37)           6613546115166111112344463412212345345
?68: Spinners (best: 80)               
69: Carefully Does It (best: 61)      12252312232322436113115554434556151362125
-70: Breakthrough (best: 68)           
71: Opportunist (best: 38)            321146356621123467
72: Upper (best: 51)                  332111144424646544446461222433212227121654342345565
73: Radioactive Ice (best: 82)        22243513556654512223211331441351
74: Pro Skater (best: 62)             2422422254344116654624
75: No Swimming Allowed (best: 38)    12366135631632133234644465435616124243
76: Laser Safety (best: 92)           55464454325433232211111516646314
77: Sprocket (best: 33)               233343216611233513334552551126544
78: Overbuild (best: 36)              332514141413166152444616121116516445
79: Somewhat Constructive (best: 37)  3321512342643245123443232532155565565
80: Finishing Strike (best: 73)       2244433321166163611242233335344534555566337
81: Big Jumps (best: 105)             4441153332241772233367112211552554151122361655555557542
82: Icy Road (best: 18)               113446454321615111
?83: Only One Way Up (best: 119)       
?84: Commute (best: 65)                
85: House (best: 20)                  21123153465615564442
86: Burn Your Bridges (best: 43)      2423532313233451655611612225552554222165555
?87: Spinners II (best: 68)            
88: Wave Cannon (best: 100)           232335356436152411463144152411
?89: Forced Fire (best: 51)            
90: Roundabouts (best: 37)            2124212121135133161233344515431135454
91: Rental Boat (best: 26)            23134533414164461221644445
?92: Trampoline Retrieval (best: 84)   
93: Hunting (best: 52)                221432345616121654323222152131134455111151
94: Reversing Space (best: 135)       11412212526354334413612211316543433
95: Branching Pathway (best: 261)     3232354712656541443634525566111133344144376566111133321214664
?96: Beware Feedback Loops (best: 50)  
97: Laser Surgery (best: 37)          4216222243451511233353465356533444141
?98: Another Mountain (best: 70)       
99: Inversion (best: 23)              22325333632666654333323
100: Outposts (best: 11)              22541654344

? = displayed best score (and maybe solution) may not be the best since
   code never finished without reaching time limit

- = levels with ice and the possibility of 2 lasers being next
   to each other are cause for concern because of the game's quirk of not
   causing a huge explosion of laser A that has been shot by a laser if
   laser B, which is adjacent to laser A, has been shot "first." By
   "first," I am talking about a situation of having a shorter laser path
   from original laser to target. "Carefully Does It," "Breakthrough,"
   and "House" are cause for concern. On further examination, "House"
   does not allow lasers to be shot since that kills you, and "Carefully
   Does It" does not have ice that can create the situation of shooting
   two adjacent lasers. "Breakthrough" CAN cause this situation. I think
   I have coded laser.h appropriately, but I am not sure that my
   definition of which has been shot "first" is correct, and I am not too
   sure that my choice to make BOTH lasers explode when being shot at the
   same time is correct. In the end, it probably doesn't matter because
   the level won't run to completion in a reasonable amount of time.

Note that, if a solution was found then the code timed out,
I reran the level with a much larger time limit to let it finish!

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

// for sorting minihop() output
#include <algorithm>  // std::sort
#include <utility>    // std::pair
long count;  // increases as minihop() continues to "look ahead"

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

  






//// load in my functions (order matters)

#include "levels.h"
#include "hex.h"
#include "laser.h"
#include "minilaser.h"
#include "minihop.h"
#include "hop.h"





int main() {



  //// time for you to set parameters!

  //list level(s) to be solved
  int level[] = {1,2,27,71,85};
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

    // declare history vector and greensToGet
    std::vector<int> dd;
    std::vector<int> greensToGet;

    // initialize b.spinnerTracker for spinner tiles
    bag b = {{-1,-1}};

    // initialize w.notBlocked
    wrap w = {{true,true,true,true,true,true}};

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
    hop(wrapped, bagged, bagged, -1, 0, dd, false, greensToGet, false, b, w, c);

    // print
    timeTemp = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::cout << " * * * level " << i << " concluded * * *   " << ctime(&timeTemp) << std::endl;

  }


}
