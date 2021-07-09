
void minihop( unsigned char level[],  bagger bagged,  int n,  int d, std::vector<int> &ddStart, std::vector<int> dd,  std::vector<int> WW,  can &c,  int &count34,  std::vector<int> &endInd,  std::vector<int> &endDir,  std::vector<int> &endScore,  std::vector<std::vector<int> > &toDoList,  std::vector<int> &endS1,  std::vector<int> &endS2, std::vector<long> &counts) {

/*
this function is called by hop.h
its purpose is to simulate making a move in Hex-a-hop in
regions where Emi moves without changing the level map.

level is the level array (not to be modified)

bagged.s is the state of Emi: {index, row, column, elevated, jumps, anti-ice}

n is the score

d is the direction Emi hopped (1 through 6, and 7 for jump).
Directions 1 through 6 are defined in hex.h and hex_a_hop.cpp

ddStart is a vector of all start d values (not to be modified)

dd is a vector of all d values after ddStart

WW has to do with preventing Emi from going in infinite loops.
It is reset when a tile that does not make it grow is landed on.
WW is a vector of indices of tiles that both (1) Emi finished a hop on
that are white, laser that destroyed nothing, a trampoline, or a grey tile
that did nothing and (2) did not have an anti-ice or jump.

c.levelSituation[] and count34 are features of the level map (not to be modified)

The following are the output vectors (will be the same length)...
 - endInd is a vector of end indices
 - endDir is a vector of end direction
 - endScore is a vector of end scores (does not include score of end move)
 - toDoList is a vector of dd vectors (does not include d of end move)
 - endS1 is a vector of bagged.s[1]
 - endS2 is a vector of bagged.s[2]
 - counts is a vector of count
They will grow if a new end situation is found, or will be
updated if a better score is found for an end situation.

(c) 2019 Bradley Knockel
*/


bool notGot = true;
bool notAdded = true; // for grey tiles
bool notDestroyed;   // for minilaser

// for each iteration of while loop
int iStart;   // start location
unsigned char v;      // value at start location

unsigned char vNew;   // value at new location


//// do the hop without changing bagged.s[4] and bagged.s[5] and level

while(true) {

    // the start values
    iStart = bagged.s[0];
    v = level[iStart]; 

    //do different things depending on v
    if (v==0)
        return;
    else if (v>99) {     //acquire jump or anti-ice
        notGot = false;
        break;
    } else if (v==2 || v==4 || v==6 || v==14) { //elevated tiles
        if (bagged.s[3]==0) return; // collide into tile
        break;
    } else if (v==7) {   //laser

        if (d==7)
          notDestroyed = false;
        else {
          notDestroyed = minilaser(level,bagged.s[1],bagged.s[2],d);
          if(d==-1) return;
          bagged.s[3]=0;
        }

        break;
    } else if (v==8) {   //trampoline
        bagged=hex(bagged,d,1);
        vNew = level[bagged.s[0]];
        if (bagged.s[3]==0 && (vNew==2 || vNew==4 || vNew==6 || vNew==14)) {
            bagged=hex(bagged,d,-1);
            break;
        }else {
            bagged=hex(bagged,d,1);
            vNew = level[bagged.s[0]];
            if (bagged.s[1]>=lSize[0] || bagged.s[1]<0 || bagged.s[2]>=lSize[1] || bagged.s[2]<0)
              // for if you bounce off of the level
              return;
            if (bagged.s[3]==0 && (vNew==2 || vNew==4 || vNew==6 || vNew==14) )
                bagged=hex(bagged,d,-1);
        }
    } else if (v==9) {  //ice
        bagged.s[3]=0;
        if (bagged.s[5] > 0) { //use anti-ice
            notGot = false;
            break;
        }else{
            bagged=hex(bagged,d,1);
            vNew = level[bagged.s[0]];
            if (vNew==2 || vNew==4 || vNew==6 || vNew==14){
                bagged=hex(bagged,d,-1);
                break;
            }
        }
    } else if (v==10 || v==12 || v==13) {
        break;
    } else if (v==11) {
        for(int k=1; k<=6; k++){
            bagged0=hex(bagged,k,1);
            vNew = level[bagged0.s[0]];
            if (vNew==3 || vNew==0) {
              notAdded = false;
              break;
            }
        }
        bagged.s[3]=0;
        break;
    }else {  //v=1, v=3, or v=5
        bagged.s[3]=0;
        break;
    }
}






// take care of WW else end this path
if ( notGot && (v==1 || v==2 || (v==7 && notDestroyed) || v==8 || (v==11 && notAdded)) ) {

  for(size_t i=0; i<WW.size(); i++)
    if (WW[i]==iStart)
      return;
  WW.push_back(iStart);

}else{  // end the path

  count++;
  if (count==2000000000)
    std::cout << "Warning! Count reaching a value too large for long data type." << std::endl;

  if (v==7 || v==12) {    // check for previous occurrence by index and direction 
    for(size_t i=0; i<endInd.size(); i++)
      if (endInd[i]==iStart && endDir[i]==d) {
        if(endScore[i]>n) {
          endScore[i] = n;
          toDoList[i] = dd;
          counts[i] = count;
        }
        return;
      }
  } else {                // check for previous occurrence by index
    for(size_t i=0; i<endInd.size(); i++)
      if (endInd[i]==iStart) {
        if(endScore[i]>n) {
          endScore[i] = n;
          toDoList[i] = dd;
          endDir[i] = d;
          counts[i] = count;
        }
        return;
      }
  }

  // since no previous occurrence, add an occurrence
  endInd.push_back(iStart);
  endDir.push_back(d);
  endScore.push_back(n);
  toDoList.push_back(dd);
  endS1.push_back(bagged.s[1]);
  endS2.push_back(bagged.s[2]);
  counts.push_back(count);

  return;

}




//increase n by 1 and increase length of dd
n++;
dd.push_back(d);




// For skipping to a certain amount of progress
if (ddStart.size() + dd.size() == skipToDigits){

  // concatenate the vectors into ddNew1
  std::vector<int> ddNew1 = ddStart;
  ddNew1.insert( ddNew1.end(), dd.begin(), dd.end() );

  progress = 0;
  multiplier = 1;
  for(int i = skipToDigits-1; i>=0; i--) {
    progress += ddNew1[i]*multiplier;
    multiplier *= 10;
  }

  if (progress < skipTo) // to start at specific amount of progress
    return;

}

// For printing progress
if (ddStart.size() + dd.size() == progressDepth){

  // concatenate the vectors into ddNew
  std::vector<int> ddNew = ddStart;
  ddNew.insert( ddNew.end(), dd.begin(), dd.end() );

  // print progress
  timeTemp = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
  for(size_t k=0; k<ddNew.size(); k++)
    std::cout << ddNew[k];
  std::cout << " - " << ctime(&timeTemp) << std::flush;

}







//// return if nMax cannot be met by future hops

if (n>=nMax)
    return;

if (   ( !l7 || (bagged.s[4]==0 && !c.levelSituation[4] && !c.levelSituation[2]) )   &&   n>nMax-count34+1   )
    return;










//// take next 6 hops
for(int i=1; i<7; i++)
  minihop(level,hex(bagged,i,1),n,i,ddStart,dd,WW,c,count34,endInd,endDir,endScore,toDoList,endS1,endS2,counts);

// hop
if (bagged.s[4]>0 && v==7) {
  bagged.s[4]--;
  minihop(level,bagged,n,7,ddStart,dd,WW,c,count34,endInd,endDir,endScore,toDoList,endS1,endS2,counts);
}


}
