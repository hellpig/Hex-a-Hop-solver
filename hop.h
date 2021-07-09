
bool hop( wrapper wrapped,  bagger bagged,  bagger baggedOld,  int n,  int d, std::vector<int> dd, bool printNow, std::vector<int> greensToGet, bool dontGoBack, bag b, wrap w, can c) {

/*
this function is called by hex_a_hop.cpp
its purpose is to simulate making a move in Hex-a-hop and then see if a
solution has been found

wrapped.l is the level array

bagged.s is the state of Emi: {index, row, column, elevated, jumps, anti-ice}

baggedOld.s is state before.
Used when calling minihop()

n is the score (not more than nMax)

d is the direction Emi hopped (1 through 6, 0 for start, and 7 for jump).
Directions 1 through 6 are defined in hex.h and hex_a_hop.cpp

dd is an array of all previous d values

printNow will force a print if minihop() reached progressDepth.
This will cause some progress statements to have a larger progressDepth.

greensToGet is to stop certain infinite loops involving grey tiles:
  {index of grey, indices of greens created around the grey}
This helps level 96 a lot.

dontGoBack is true if miniHop() should not hop back to the original tile.
I think this could speed up certain levels.
In case it does, I keep this code!

b.spinnerTracker[] is {index of spinner, counter} to try to prevent
infinite loops. Index is set to -1 if progress was made since spinner.
This helps levels 55 and 70 a lot.

w.notBlocked[] is a 6-element array saying if you can go in the 6 directions.
These can be false if you just lowered a 4 or a 6.
This needs to be passed to hop() for calling minihop().

c.levelSituation[] is a 5-element boolean array saying if the level map has...
  {any 4's, any 6's, 7's and 9's, any anti-ice, any jumps}
Except for the 0th element, once set to false, these are no longer updated.
It is not very important that these are set to false when they should be false
  (for example, the first 2 elements are not set to false when stepping off of
  the last 4 or the last 6), but, when set to false, it must be correct.
Note that 6's and 11's can make 4's
  (see level 84 for an example of 6's changing c.levelSituation[0]).

hop() returns true if should keep calling hop().
This is only false after minihop() is called,
to prevent redoing what has been done already.

(c) 2019 Bradley Knockel
*/


bool notGot = true;   // used for picking up objects
bool notAdded = true; // for grey tiles (should I even include 11's in minihop?)

// for each iteration of while loop
int iStart;   // start location
unsigned char v;      // value at start location

unsigned char vNew;   // value at new location


// for minihop()
int nOld = n;


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



// if needed, call minihop() to take over for hop() for a bit
if ( d>0 && notGot && (v==1 || v==2 || (v==7 && list.size()==0) || v==8 || (v==11 && notAdded)) ) {

  // all these vectors will be the same length
  std::vector<int> endInd, endDir, endScore, endS1, endS2;
  std::vector<std::vector<int> > toDoList;
  std::vector<long> counts;
  // do I even need endScore? doesn't doDoList[i].size() have enough info?


  //// fill the above vectors

  count = 0;  // increased by 1 each time vectors are modified (needed for sorting)

  // to speed up running certain levels, quickly redo previous directions so that minihop won't end up there
  for(int i=1; i<d; i++) {
    bagged0 = hex(baggedOld,i,1);
    std::vector<int> ddNew;  // initialize empty vector
    if (w.notBlocked[i-1]){
      iStart = bagged0.s[0];
      v = wrapped.l[iStart];
      if (v==0 || v==8 || v==9)
          continue;   // I'm too lazy to worry about including trampolines or ice
      else if (bagged0.s[3]==0 && (v==2 || v==4 || v==6 || v==14))
          continue; // hit elevated tiles
      endInd.push_back(iStart);
      endDir.push_back(i);
      endScore.push_back(nOld);
      toDoList.push_back(ddNew);
      endS1.push_back(bagged0.s[1]);
      endS2.push_back(bagged0.s[2]);
      counts.push_back(++count);  // increment count before appending it to counts
    }
  }
  int numToRemove = count;  // sorting won't effect location of things to remove

  // call minihop
  dd.pop_back();  // we want the "old" dd before this hop
  for(int i=d; i<7; i++) {
    bagged0 = hex(baggedOld,i,1);
    if (w.notBlocked[i-1]){
      std::vector<int> ddNew, WW;  // initialize history vectors
      if (dontGoBack) WW.push_back(baggedOld.s[0]);
      minihop(wrapped.l,bagged0,nOld,i,dd,ddNew,WW,c,count34,endInd,endDir,endScore,toDoList,endS1,endS2,counts);
    }
  }
  for(int i1=0; i1<6; i1++)  // necessary?
    w.notBlocked[i1] = true;


  //// sort the output of minihop() so that skipTo can work.

  std::vector<int> endIndOld(endInd.begin()+numToRemove, endInd.end());
  std::vector<int> endDirOld(endDir.begin()+numToRemove, endDir.end());
  std::vector<int> endScoreOld(endScore.begin()+numToRemove, endScore.end());
  std::vector<int> endS1Old(endS1.begin()+numToRemove, endS1.end());
  std::vector<int> endS2Old(endS2.begin()+numToRemove, endS2.end());
  std::vector<std::vector<int> > toDoListOld(toDoList.begin()+numToRemove, toDoList.end());
  // do I need endInd now? Aren't endS1 and endS2 enough?

  std::vector< std::pair<long,int> > vectPair;
  for (size_t i=numToRemove; i<counts.size(); i++)
    vectPair.push_back( std::make_pair(counts[i],i-numToRemove) );

  std::sort(vectPair.begin(), vectPair.end());

  for (size_t i=0; i<endIndOld.size(); i++) {
    int j = vectPair[i].second;  // the index of sort
    endInd[i] = endIndOld[j];
    endDir[i] = endDirOld[j];
    endScore[i] = endScoreOld[j];
    endS1[i] = endS1Old[j];
    endS2[i] = endS2Old[j];
    toDoList[i] = toDoListOld[j];
  }


  //// do toDoList
  for(size_t i=0; i<endIndOld.size(); i++){

    std::vector<int> ddNew = dd;
    ddNew.insert( ddNew.end(), toDoList[i].begin(), toDoList[i].end() );

    // update bagged
    bagged.s[0] = endInd[i];
    bagged.s[1] = endS1[i];
    bagged.s[2] = endS2[i];
    vNew = wrapped.l[endInd[i]];
    if ( vNew==4 || vNew==6 || vNew==14 )
      bagged.s[3] = 1;
    else
      bagged.s[3] = 0;

    if ( dd.size() < progressDepth && ddNew.size() >= progressDepth)
      printNow = true;
    else
      printNow = false;

    hop(wrapped,bagged,bagged,endScore[i],endDir[i],ddNew,printNow,greensToGet,false,b,w,c);  // 3rd argument should never be used
  }


  return false;
}

// Progress has been made, so turn off b.spinnerTracker. 
if (v!=10) b.spinnerTracker[0] = -1;



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
if (dd.size() == progressDepth || printNow) {

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

for(int i1=0; i1<6; i1++)
  w.notBlocked[i1] = true;

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





// a little something to try to speed up certain levels
if (v==1 && (d==0 || !notGot))
  dontGoBack = true;
else
  dontGoBack = false;


// Take care of greensToGet!
// This code comes after minihop() is called in case Emi needs to 
//   briefly walk over a v==1 to get the greens.
if (greensToGet.size()>0 && v!=3 && v!=11) greensToGet.resize(0);



//// take next 6 hops and maybe jump

bool keepGoing = true;

if (w.notBlocked[0])
  keepGoing = hop(wrapped,hex(bagged,1,1),bagged,n,1,dd,false,greensToGet,dontGoBack,b,w,c);
if (keepGoing && w.notBlocked[1])
  keepGoing = hop(wrapped,hex(bagged,2,1),bagged,n,2,dd,false,greensToGet,dontGoBack,b,w,c);
if (keepGoing && w.notBlocked[2])
  keepGoing = hop(wrapped,hex(bagged,3,1),bagged,n,3,dd,false,greensToGet,dontGoBack,b,w,c);
if (keepGoing && w.notBlocked[3])
  keepGoing = hop(wrapped,hex(bagged,4,1),bagged,n,4,dd,false,greensToGet,dontGoBack,b,w,c);
if (keepGoing && w.notBlocked[4])
  keepGoing = hop(wrapped,hex(bagged,5,1),bagged,n,5,dd,false,greensToGet,dontGoBack,b,w,c);
if (keepGoing && w.notBlocked[5])
  keepGoing = hop(wrapped,hex(bagged,6,1),bagged,n,6,dd,false,greensToGet,dontGoBack,b,w,c);

if (bagged.s[4]>0 && (v==5 || v==6 || v==7 || v==10 || v==11 || v==13 || v==14) ) {
  bagged.s[4]--;
  hop(wrapped,bagged,bagged,n,7,dd,false,greensToGet,false,b,w,c);  // 3rd argument should never be used
}



return true;

}
