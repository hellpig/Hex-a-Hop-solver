
bool hop2( wrapper wrapped,  bagger bagged,  bagger baggedOld,  int n,  int d, std::vector<int> dd,  std::vector<int> greensToGet, bag b, can c,  std::vector<int> WW) {

/*
this function is called by hex_a_hop2.cpp
its purpose is to simulate making a move in Hex-a-hop and then see if a
solution has been found

wrapped.l is the level array

bagged.s is the state of Emi: {index, row, column, elevated, jumps, anti-ice}

baggedOld.s is state before.
Used when calling minihop2()

n is the score (not more than nMax)

d is the direction Emi hopped (1 through 6, 0 for start, and 7 for jump).
Directions 1 through 6 are defined in hex.h and hex_a_hop.cpp

dd is an array of all previous d values

greensToGet is to stop certain infinite loops involving grey tiles:
  {index of grey, indices of greens created around the grey}
This helps level 96 a lot.

b.spinnerTracker[] is {index of spinner, counter} to try to prevent
infinite loops. Index is set to -1 if progress was made since spinner.
This helps levels 55 and 70 a lot.

c.levelSituation[] is a 5-element boolean array saying if the level map has...
  {any 4's, any 6's, 7's and 9's, any anti-ice, any jumps}
Except for the 0th element, once set to false, these are no longer updated.
It is not very important that these are set to false when they should be false
  (for example, the first 2 elements are not set to false when stepping off of
  the last 4 or the last 6), but, when set to false, it must be correct.
Note that 6's and 11's can make 4's
  (see level 84 for an example of 6's changing c.levelSituation[0]).

WW has to do with preventing Emi from going in infinite loops.
It is reset when a tile that does not make it grow is landed on.
WW is a vector of indices of tiles that both (1) Emi finished a hop on
that are white, laser that destroyed nothing, a trampoline, or a grey tile
that did nothing and (2) did not have an anti-ice or jump.

hop2() returns true if should keep calling hop2().
This is only false after minihop2() is called,
to prevent redoing what has been done already.

(c) 2021 Bradley Knockel
*/


bool notGot = true;   // used for picking up objects
bool notAdded = true; // for grey tiles (should I even include 11's in WW?)

// for each iteration of while loop
int iStart;   // start location
unsigned char v;      // value at start location

unsigned char vNew;   // value at new location

std::vector<int> greensToGetOld = greensToGet;


//// do the hop!!!

while(true) {

    // the start values
    iStart = bagged.s[0];
    v=wrapped.l[iStart];

    //do different things depending on v
    if (v==0) {   // 0 is water
        return true;
    } else if(v>199) {       //acquire jump
        notGot=false;
        bagged.s[4]++;
        wrapped.l[iStart] += -200;
    } else if(v>99) {     //acquire anti-ice
        notGot=false;
        bagged.s[5]++;
        wrapped.l[iStart] += -100;
    } else if (v==3) {  // green unelevated

        if (greensToGet.size()>0) {
          bool notInList = true;
          for(size_t k=1; k<greensToGet.size(); k++)
            if (iStart==greensToGet[k]){  // remove from list
              greensToGet[k] = greensToGet.back();
              greensToGet.pop_back();
              notInList = false;
              break;
            }
          if (notInList)
            greensToGet.resize(0);   // clear list
        }

        bagged.s[3]=0;
        break;
    } else if (v==2 || v==4 || v==6 || v==14) { //elevated tiles
        if (bagged.s[3]==-1) { //if you start level on tile
            bagged.s[3]=1;
            break;
        }else if (bagged.s[3]==0) { //if you collide into tile
            return true;
        } else { //if you successfully land
            if (v==14){ //elevated lift
                bagged.s[3]=0;
                wrapped.l[iStart]=13;
            }
            break;
        }
    } else if (v==7 && d>0) {   // laser

        laser(wrapped.l,bagged.s[1],bagged.s[2],d,n);  // updates list and n

        // modify wrapped.l based on list
        for (size_t i1=0; i1<list.size(); i1++){
            vNew = wrapped.l[list[i1]];
            if (vNew>199)     wrapped.l[list[i1]]=200;
            else if (vNew>99) wrapped.l[list[i1]]=100;
            else              wrapped.l[list[i1]]=0;
        }

        if(wrapped.l[iStart]==0) return true;
        bagged.s[3]=0;

        break;
    } else if (v==8 && d>0) {   //trampoline
        bagged=hex(bagged,d,1);
        vNew = wrapped.l[bagged.s[0]];
        if (bagged.s[3]==0 && (vNew==2 || vNew==4 || vNew==6 || vNew==14)) {
            bagged=hex(bagged,d,-1);
            break;
        }else {
            bagged=hex(bagged,d,1);
            vNew = wrapped.l[bagged.s[0]];
            if (bagged.s[1]>=lSize[0] || bagged.s[1]<0 || bagged.s[2]>=lSize[1] || bagged.s[2]<0)
              // for if you bounce off of the level
              return true;
            if (bagged.s[3]==0 && (vNew==2 || vNew==4 || vNew==6 || vNew==14) )
                bagged=hex(bagged,d,-1);
        }
    } else if (v==9 && d>0) {  //ice
        bagged.s[3]=0;
        if (bagged.s[5] > 0) { //use anti-ice
            bagged.s[5] += -1;
            wrapped.l[iStart] = 5;
            break;
        }else{
            bagged=hex(bagged,d,1);
            vNew = wrapped.l[bagged.s[0]];
            if (vNew==2 || vNew==4 || vNew==6 || vNew==14){
                bagged=hex(bagged,d,-1);
                break;
            }
        }
    } else if (v==10 && d>0) {  //spinner
        bagged.s[3]=0;

        // generate array new6
        for (int k=1; k<=6; k++)
            new6[k] = 0;
        for (int k=1; k<=6; k++) {
            bagged0=hex(bagged,k,1);
            vNew = wrapped.l[bagged0.s[0]];
            int next = k%6 + 1;
            if (vNew>199) { //jumps don't rotate with tile
                new6[k] += 200;
                new6[next] += vNew-200;
            }else if (vNew>99) { //anti-ice don't rotate with tile
                new6[k] += 100;
                new6[next] += vNew-100;
            }else{
                new6[next] += vNew;
            }
        }

        // Stop infinite loops.
        //   b.spinnerTracker[0] is iStart of spinner
        //   b.spinnerTracker[1] is counter that counts down each spin
        // Note that you cannot win by landing on a spinner when
        //   b.spinnerTracker[0] > -1
        if (iStart == b.spinnerTracker[0]) {
            if (b.spinnerTracker[1] == 1) return true;
            b.spinnerTracker[1]--;
        } else {

            b.spinnerTracker[0] = iStart;

            // Initialize counter depending on symmetry.
            // Don't worry about optimizing for jumps and anti-ice.
            if (new6[1]==new6[3] && new6[3]==new6[5] && new6[2]==new6[4] && new6[4]==new6[6])
              b.spinnerTracker[1] = 2;
            else if (new6[1]==new6[4] && new6[2]==new6[5] && new6[3]==new6[6])
              b.spinnerTracker[1] = 3;
            else
              b.spinnerTracker[1] = 6;

        }

        //use new6 to change the level
        for (int k=1; k<=6; k++) {
            bagged0=hex(bagged,k,1);
            wrapped.l[bagged0.s[0]] = new6[k];
        }

        break;
    } else if (v==11 && d>0) { //grey (note that anti-ice and jumps never appear near grey)

        // Stop infinite loops
        // Note that you cannot win by landing on a grey.
        if (greensToGet.size()==1 && greensToGet[0]==iStart) return true;
        greensToGet.resize(0);
        greensToGet.push_back(iStart);

        //int count4=0;
        for(int k=1; k<=6; k++){
            bagged0=hex(bagged,k,1);
            vNew = wrapped.l[bagged0.s[0]];
            if (vNew==3) {
              wrapped.l[bagged0.s[0]] = 4;
              c.levelSituation[0] = true;
              notAdded = false;
            } else if (vNew==0) {
              wrapped.l[bagged0.s[0]] = 3;
              notAdded = false;
              greensToGet.push_back(bagged0.s[0]);
            }
            //else if (vNew==4) count4++;
        }
        //if (count4==5 && !notAdded) return true;

        bagged.s[3]=0;
        break;
    } else if (v==12 && d>0) {  //boat
        bagged=hex(bagged,d,1);
        bagged.s[3]=0;
        vNew = wrapped.l[bagged.s[0]];
        if (bagged.s[1]>=lSize[0] || bagged.s[1]<0 || bagged.s[2]>=lSize[1] || bagged.s[2]<0)
          //for when your boat falls off edge of map
          return true;
        if (vNew == 0) {
            wrapped.l[iStart] = 0;
            wrapped.l[bagged.s[0]] = 12;
        }else{
            bagged=hex(bagged,d,-1);
            break;
        }
    } else if (v==13 && d>0) {  // lift initially UNelevated
        bagged.s[3]=1;
        wrapped.l[iStart]=14;
        break;
    } else {  // v=1 or v=5
        bagged.s[3]=0;
        break;
    }
}


// take care of WW
if ( d>0 && notGot && (v==1 || v==2 || (v==7 && list.size()==0) || v==8 || (v==11 && notAdded)) ) {

  for(size_t i=0; i<WW.size(); i++)
    if (WW[i]==iStart)
      return true;
  WW.push_back(iStart);

} else {

  WW.clear();

  // progress was made so update some things
  if (v!=10) b.spinnerTracker[0] = -1;
  if (greensToGet.size()>0 && v!=3 && v!=11) greensToGet.resize(0);

}



//increase n by 1 and take care of dd
n++;
if(d>0) dd.push_back(d);



//// Let's create some useful things
// Having 6 different versions of the code
//   is only a linear speedup (not exponential like pruning paths).
//   Should I also test for 4's to create 12 different versions?
// Note that items never appear on 4's (or 6's)
// Note that there are never both anti-ice and jump items on the same level

int count34 = 0;   // count 3's and 4's

can cOld = c;
if (c.levelSituation[4]) {  // jumps
  if (c.levelSituation[2]) {

    c.levelSituation[4] = false;
    c.levelSituation[2] = false;   // is anything a 9?
    for(int i1=0; i1<lSize[2]; i1++) {
      vNew = wrapped.l[i1];
      if (vNew == 3 || vNew == 203 || vNew == 4) count34++;
      if (vNew > 199) c.levelSituation[4] = true;
      else if(vNew == 9) c.levelSituation[2] = true;
    }

  } else {

    c.levelSituation[4] = false;
    for(int i1=0; i1<lSize[2]; i1++) {
      vNew = wrapped.l[i1];
      if (vNew == 3 || vNew == 203 || vNew == 4) count34++;
      if (vNew > 199) c.levelSituation[4] = true;
    }

  }
} else if (c.levelSituation[3]) {  // anti-ice
  if (c.levelSituation[2]) {

    c.levelSituation[3] = false;
    c.levelSituation[2] = false;   // is anything a 9?
    for(int i1=0; i1<lSize[2]; i1++) {
      vNew = wrapped.l[i1];
      if (vNew == 3 || vNew == 103 || vNew == 4) count34++;
      if (vNew == 9) c.levelSituation[2] = true;
      else if(vNew > 99) c.levelSituation[3] = true;
    }

  } else {

    c.levelSituation[3] = false;
    for(int i1=0; i1<lSize[2]; i1++) {
      vNew = wrapped.l[i1];
      if (vNew == 3 || vNew == 103 || vNew == 4) count34++;
      if (vNew > 99) c.levelSituation[3] = true;
    }

  }
} else {   // no items
  if (c.levelSituation[2]) {

    c.levelSituation[2] = false;   // is anything a 9?
    for(int i1=0; i1<lSize[2]; i1++) {
      vNew = wrapped.l[i1];
      if (vNew == 3 || vNew == 4) count34++;
      else if(vNew == 9) c.levelSituation[2] = true;
    }

  } else {

    for(int i1=0; i1<lSize[2]; i1++) {
      vNew = wrapped.l[i1];
      if (vNew == 3 || vNew == 4) count34++;
    }

  }
}



//// check to see if we've won!
if (count34==0) {
  if (nMax>=n) { //this is ALMOST unnecessary

    nMax = n-1;

    // print
    timeTemp = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::cout << "Solution found with score of " << n << ":  " << ctime(&timeTemp);
    for(size_t k=0; k<dd.size(); k++)
      std::cout << dd[k];
    std::cout << std::endl;

  }
  return true;
}




// Should we allow minihop2 to take over for a bit?
if ( d>0 && notGot && (v==3 || v==5) ) {

  // get the dd before this hop
  dd.pop_back();

  // reset global vectors
  score.resize(0);        // the score
  situations.resize(0);   // the level situation
  ws.resize(0);           // blocked directions when lowering tiles
  DDs.resize(0);          // all the dd's
  bags.resize(0);         // the state of Emi
  greensToGets.resize(0); // greensToGet
  pastIndices.resize(0);  // the main list (helps remove identical paths)

  // initialize global vectors
  // used internally within minihop2()
    wrap w = {{true,true,true,true,true,true}};
    std::vector<int> blank;  // blank
  score.push_back(n-1);
  situations.push_back(cOld);
  ws.push_back(w);
  DDs.push_back(blank);
  bags.push_back(baggedOld);
  greensToGets.push_back(greensToGetOld);
  pastIndices.push_back(blank);

  // declare vectors for minihop2() output
  // for example, each endScore[] will be passed to hop2()
  std::vector<std::vector<int> > endDDs;
  std::vector<std::vector<int> > endGreensToGet;
  std::vector<int> endScore;
  std::vector<bagger> endBags;
  std::vector<can> endSituations;
  std::vector<int> endD;
  std::vector<std::vector<int> > endPastIndices;
  
  // call minihop2
  minihop2(wrapped.l, dd, d, endDDs, endGreensToGet, endScore, endBags, endSituations, endD, endPastIndices);

  // create wrappedNew and ddNew then call hop2()
  ////// inside this loop, print progress!
  for(size_t i=0; i<endScore.size(); i++) {

    wrapper wrappedNew = wrapped;
    bagged0 = bagged;
    for(size_t j=0; j<endPastIndices[i].size(); j++) {

      iStart = endPastIndices[i][j];
      v = wrappedNew.l[iStart];
      if (v>99){
        v += -100;
        if (v>99) v += -100;
      }

      if (v==3 || v==4)
        wrappedNew.l[iStart] = 0;
      else if (v==5 || v==6)
        wrappedNew.l[iStart] = 3;
      else if (v==8)    // item on trampoline
        wrappedNew.l[iStart] = 8;
      else if (v==9) {  // item on ice
        if (wrappedNew.l[iStart] > 9)  // acquire item
          wrappedNew.l[iStart] = 9;
        else                           // use item
          wrappedNew.l[iStart] = 5;
      } else
        std::cout << "well that's just stanky" << std::endl;

    }

    if (c[0] && !endSituations[i][0])  // lower 4's?
      for(int i1=0; i1<lSize[2]; i1++)
        if(wrappedNew.l[i1] == 4) wrappedNew.l[i1] = 3;
    if (c[1] && !endSituations[i][1])  // lower 6's?
      for(int i1=0; i1<lSize[2]; i1++)
        if(wrappedNew.l[i1] == 6) wrappedNew.l[i1] = 5;

    std::vector<int> ddNew = dd;
    ddNew.insert( ddNew.end(), endDDs[i].begin(), endDDs[i].end() );

    hop2(wrappedNew, endBags[i], endBags[i], endScore[i], endD[i], ddNew, endGreensToGet[i], b, endSituations[i], WW);   // 3rd argument should never be used
  }

return false;
}





// For skipping to a certain amount of progress
if (dd.size() == skipToDigits){

  progress = 0;
  multiplier = 1;
  for(int i = skipToDigits-1; i>=0; i--) {
    progress += dd[i]*multiplier;
    multiplier *= 10;
  }

  if (progress < skipTo) // to start at specific amount of progress
    return true;

}

// For printing progress
if (dd.size() == progressDepth) {

  timeTemp = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
  for(size_t k=0; k<dd.size(); k++)
    std::cout << dd[k];
  std::cout << "   " << ctime(&timeTemp) << std::flush;

}






//// return if nMax cannot be met by future hops

if (n>=nMax)
    return true;

if (   ( !l7 || (bagged.s[4]==0 && !c.levelSituation[4] && !c.levelSituation[2]) )   &&   n > nMax-count34+1   )
    return true;
//Stop if "n>nMax-length(find(l==3|l==103|l==203|l==4))+1" when
//  (1) there are no lasers or (2) when there are no jumps and no ice.
//Note that exploding another laser surrounded by green is not score advantageous.



//// Take care of some final stuff
// Changing the order of the following things can break the code or just
//   make it slower.


//take care of stepping off of green/blue tiles
if (v==3 || v==4) {
    wrapped.l[iStart] = 0;
}else if (v==5) {
    wrapped.l[iStart] = 3;
    n += 10;
}else if (v==6) {
    wrapped.l[iStart] = 4;
    c.levelSituation[0] = true;
    n += 10;
}




// create l3 and l5
// Having 9 different versions of the code is
//   only a linear speedup (not exponential like pruning paths)
// Note that no level has an item on a 5 and also has 6's.

bool l3 = false;  // is anything a 3?
bool l5 = false;  // is anything a 5?
// Do I have to initialize these?

if (c.levelSituation[4]) {  // jumps
  if (c.levelSituation[1] && c.levelSituation[0]) {  // 6's and 4's

    for(int i1=0; i1<lSize[2]; i1++) {
      vNew = wrapped.l[i1];
      if (vNew == 3 || vNew == 203) l3 = true;
      else if(vNew == 5 || vNew == 205) l5 = true;
    }

  } else if (c.levelSituation[0]) {  // 4's

    for(int i1=0; i1<lSize[2]; i1++) {
      vNew = wrapped.l[i1];
      if (vNew == 3 || vNew == 203) {
        l3 = true;
        break;
      }
    }

  } else if (c.levelSituation[1]) {  // 6's

    for(int i1=0; i1<lSize[2]; i1++) {
      vNew = wrapped.l[i1];
      if (vNew == 5|| vNew == 205) {
        l5 = true;
        break;
      }
    }

  }
} else if (c.levelSituation[3]) {  // anti-ice
  if (c.levelSituation[1] && c.levelSituation[0]) {  // 6's and 4's

    for(int i1=0; i1<lSize[2]; i1++) {
      vNew = wrapped.l[i1];
      if (vNew == 3 || vNew == 103) l3 = true;
      else if(vNew == 5 || vNew == 105) l5 = true;
    }

  } else if (c.levelSituation[0]) {  // 4's

    for(int i1=0; i1<lSize[2]; i1++) {
      vNew = wrapped.l[i1];
      if (vNew == 3 || vNew == 103) {
        l3 = true;
        break;
      }
    }

  } else if (c.levelSituation[1]) {  // 6's

    for(int i1=0; i1<lSize[2]; i1++) {
      vNew = wrapped.l[i1];
      if (vNew == 5 || vNew == 105) {
        l5 = true;
        break;
      }
    }

  }
} else {   // no items
  if (c.levelSituation[1] && c.levelSituation[0]) {  // 6's and 4's

    for(int i1=0; i1<lSize[2]; i1++) {
      vNew = wrapped.l[i1];
      if (vNew == 3) l3 = true;
      else if(vNew == 5) l5 = true;
    }

  } else if (c.levelSituation[0]) {  // 4's

    for(int i1=0; i1<lSize[2]; i1++)
      if (wrapped.l[i1] == 3) {
        l3 = true;
        break;
      }

  } else if (c.levelSituation[1]) {  // 6's

    for(int i1=0; i1<lSize[2]; i1++) {
      if (wrapped.l[i1] == 5) {
        l5 = true;
        break;
      }
    }

  }
}



//check the time
timeTemp = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
if ( timeTemp-timeStart > max_time) {
    nMax = -2;
    std::cout << "Time limit reached! Aborting search. " << ctime(&timeTemp) << std::flush;
    return true;
}


//take care of lowering raised green/blue tiles
// Note that no level has items on 4's or 6's

wrap w = {{true,true,true,true,true,true}};
//w.notBlocked[] is a 6-element array saying if you can go in the 6 directions.
//These can be false if you just lowered a 4 or a 6.

if (v==7 || v==11) {

  // if not any 3's, change all 4's to 3's
  if (c.levelSituation[0] && !l3) {
    c.levelSituation[0] = false;
    for(int i1=0; i1<lSize[2]; i1++)
      if(wrapped.l[i1] == 4) wrapped.l[i1] = 3;
  }

  // if not any 5's, change all 6's to 5's
  if (v==7 && c.levelSituation[1] && !l5) {
    c.levelSituation[1] = false;
    for(int i1=0; i1<lSize[2]; i1++)
      if(wrapped.l[i1] == 6) wrapped.l[i1] = 5;
  }

}else if (v==3 && c.levelSituation[0] && !l3) {

  // update w.notBlocked[] and change all 4's to 3's
  for(int i1=0; i1<6; i1++) {
    bagged0 = hex(bagged,i1+1,1);
    if (wrapped.l[bagged0.s[0]] == 4)
      w.notBlocked[i1] = false;
  }
  c.levelSituation[0] = false;
  for(int i1=0; i1<lSize[2]; i1++)
    if(wrapped.l[i1] == 4) wrapped.l[i1] = 3;

}else if (v==5 && c.levelSituation[1] && !l5) {

  // update w.notBlocked[] and change all 6's to 5's
  for(int i1=0; i1<6; i1++) {
    bagged0 = hex(bagged,i1+1,1);
    if (wrapped.l[bagged0.s[0]] == 6)
      w.notBlocked[i1] = false;
  }
  c.levelSituation[1] = false;
  for(int i1=0; i1<lSize[2]; i1++)
    if(wrapped.l[i1] == 6) wrapped.l[i1] = 5;

}





//// take next 6 hops and maybe jump

bool keepGoing = true;

if (w.notBlocked[0])
  keepGoing = hop2(wrapped,hex(bagged,1,1),bagged,n,1,dd,greensToGet,b,c,WW);
if (keepGoing && w.notBlocked[1])
  keepGoing = hop2(wrapped,hex(bagged,2,1),bagged,n,2,dd,greensToGet,b,c,WW);
if (keepGoing && w.notBlocked[2])
  keepGoing = hop2(wrapped,hex(bagged,3,1),bagged,n,3,dd,greensToGet,b,c,WW);
if (keepGoing && w.notBlocked[3])
  keepGoing = hop2(wrapped,hex(bagged,4,1),bagged,n,4,dd,greensToGet,b,c,WW);
if (keepGoing && w.notBlocked[4])
  keepGoing = hop2(wrapped,hex(bagged,5,1),bagged,n,5,dd,greensToGet,b,c,WW);
if (keepGoing && w.notBlocked[5])
  keepGoing = hop2(wrapped,hex(bagged,6,1),bagged,n,6,dd,greensToGet,b,c,WW);

if (bagged.s[4]>0 && (v==5 || v==6 || v==7 || v==10 || v==11 || v==13 || v==14) ) {
  bagged.s[4]--;
  hop2(wrapped,bagged,bagged,n,7,dd,greensToGet,b,c,WW);  // 3rd argument should never be used
}



return true;

}
