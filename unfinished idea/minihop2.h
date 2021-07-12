
void minihop2( unsigned char level[], std::vector<int> &ddStart, int &startD, std::vector<std::vector<int> > &endDDs, std::vector<std::vector<int> > &endGreensToGet, std::vector<int> &endScore, std::vector<bagger> &endBags, std::vector<can> &endSituations, std::vector<int> &endD, std::vector<std::vector<int> > &endPastIndices) {

/*
This function is called by hop2.h
Its purpose is to simulate making a move in Hex-a-hop in
  regions where Emi moves only on 3's and 5's
  (and 103's, 203's, 105's, and 205's).
Identical paths are removed.

In the future, I can make this code benefit more levels by having
it also walk on 4's and 6's
Currently it just works for 3's and 5's

level is the level array (not to be modified)

ddStart is a vector of all start d values (not to be modified)

startD is the start direction

The following are the outputs...
  endDDs          <-- vector of vectors of d values after ddStart
  endGreensToGet  <-- vector of greensToGet vectors
  endScore        <-- vector of scores
  endBags         <-- vector of Emi's states
  endSituations   <-- vector of level situations
  endD            <-- direction moving at end
  endPastIndices  <-- list of modified indices

(c) 2021 Bradley Knockel
*/



unsigned char v, vNew;      // value on level

wrap wBlank = {{true,true,true,true,true,true}};


int step = 0;
while (DDs.size()!=0) {
  step++;   
  size_t siz0 = DDs.size();  ////// maybe a siz0 cutoff will be necessary!

  // loop over scenarios that have not yet completed and take a step for each
  for (size_t j=0; j<DDs.size(); j++) {

    for (int k=1; k<8; k++) {  // loop over directions
      if (step==1 && k<startD) continue;
      int got = 0;   // 0 means not get; 1 means got anti-ice; 2 means got jump
      std::vector<int> newInd = pastIndices[j];


      // For skipping to a certain amount of progress
      if (ddStart.size() + DDs[j].size() + 1 == skipToDigits){

        // concatenate the vectors into ddNew1
        std::vector<int> ddNew1 = ddStart;
        ddNew1.insert( ddNew1.end(), DDs[j].begin(), DDs[j].end() );
        ddNew1.push_back(k);

        progress = 0;
        multiplier = 1;
        for(int i = skipToDigits-1; i>=0; i--) {
          progress += ddNew1[i]*multiplier;
          multiplier *= 10;
        }

        if (progress < skipTo)
          continue;

      }


      if ( k<7 && ws[j].notBlocked[k] ) continue;


      // get info regarding new location
      bagged0 = hex(bags[j],k,1);
      v = level( bagged0.s[0] );
      if (v>99){
        v += -100;
        if (v>99) {
          v += -100;
          if (v>2 && v<7) got++;
        }
        if (v>2 && v<7) got++;
      }

      // stop if green already once or blue already twice (Emi is now drowning)
      bool blueOnce = false;
      if ( (v==3||v==4) && std::find(newInd.begin(), newInd.end(), bagged0.s[0]) != newInd.end() )
        continue;
      if ( v==5 || v==6 ) {
        std::vector<int>::iterator it = std::find(newInd.begin(), newInd.end(), bagged0.s[0]);
        if ( it != newInd.end() ) {
          got=0;
          blueOnce = true;
          if ( std::find(it+1, newInd.end(), bagged0.s[0]) != newInd.end() )
            continue;
        }
      }


      // take care of jumps
      // Note that you can never jump on what was once an ice because
      //  anti-ice and jumps never appear on the same level
      if ( k==7 && !(bagged0.s[4]>0 && v==5)) ) continue;


      // take care of water or elevated tiles
      if (v==0 || v==2 || v==4 || v==6 || v==14) continue;


      if (v==8 || v==9) // take care of trampolines and ice
        bool notGo = false;
        while (true) {

          v = level[bagged0.s[0]];
          if (v>99){
            v += -100;
            if (v>99) {
              v += -100;
              if (v==8 || v==9) got++;
              if (v>2 && v<7) got++;
            }
            if (v==8 || v==9) {
              if ( std::find(newInd.begin(), newInd.end(), bagged0.s[0]) != newInd.end() )
                got=0;
              else {
                newInd.push_back(bagged0.s[0]);
                got++;
              }
            } else if (v>2 && v<7) got++;
          }

          if (v==0) {         // water
            notGo = true;
            break;
          } else if (v==8) {   // trampoline
            bagged0=hex(bagged0,k,1);
            vNew = level[bagged0.s[0]];
            if (bagged0.s[3]==0 && (vNew==2 || vNew==4 || vNew==6 || vNew==14)) {
                bagged0=hex(bagged0,d,-1);
                break;
            }else {
                bagged0=hex(bagged0,d,1);
                vNew = level[bagged0.s[0]];
                if (bagged0.s[1]>=lSize[0] || bagged0.s[1]<0 || bagged0.s[2]>=lSize[1] || bagged0.s[2]<0) {
                  // for if you bounce off of the level
                  notGo = true;
                  break;
                }
                if (bagged0.s[3]==0 && (vNew==2 || vNew==4 || vNew==6 || vNew==14) )
                    bagged0=hex(bagged0,d,-1);
            }

          } else if (v==9) {    // ice
            bagged0.s[3]=0;
            int num = std::count(newInd.begin(),newInd.end(), bagged0.s[0]);
            if (bagged0.s[5] > 0 && num<2) { //use anti-ice
                bagged0.s[5] += -1;
                newInd.push_back(bagged0.s[0]);
                if ( num == 0 )
                  newInd.push_back(bagged0.s[0]);  // having used anti-ice should give 2 occurrences
                break;
            } else if (num==2) {   // anti-ice already used
                v=5;
                break;
            } else if (num==3) {
                v=3;
                break;
            } else if (num==4) {
                notGo = true;
                break;
            } else {
                bagged0=hex(bagged0,d,1);
                vNew = level[bagged0.s[0]];
                if (vNew==2 || vNew==4 || vNew==6 || vNew==14){
                    bagged0=hex(bagged0,d,-1);
                    break;
                }
            }
          } else break;

        }
        if (notGo) continue;
      }



      // take care of bagged0
      bagged0.s[3] = 0;
      if (k==7) bagged0.s[4] += -1;
      if (got==0) {}
      else if (got==1) bagged0.s[5] += 1;
      else if (got==2) bagged0.s[4] += 1;
      else std::cout << "that some stank!" << std::endl;


      if ( v<3 || v>6 ) {

        endDDs.push_back(DDs[j]);
        endGreensToGet.push_back(greensToGets[j]);
        endScore.push_back(score[j]);
        endBags.push_back(bagged0);        // updated already this step
        endSituations.push_back(situations[j]);
        endD.push_back(k);                 // updated already this step
        endPastIndices.push_back(newInd);  // updated already this step

        continue;

      }

      ////// check if you've won (if so, add to endDDs and mark for removal via DDs)
      // Should I update nMax?
      if ( v==5 && !blueOnce ) {
        int count34 = 0;
      }


      // mark greens and blues as new location
      newInd.push_back(bagged0.s[0]);



      ////// check if 4's or 6's must be lowered (if so, set ws and situations, then add to end vectors, then continue to next iteration)


      ////// continue if nMax cannot be met by future hops


      ////// add to end of global vectors (note that score goes up by 11 for v=5 if first time landing on it) (update greensToGets)
      ////// Do not add to end of vectors if, for any new entries (those after first siz0 elements), Emi ended up in the same location
      //////   after walking on the same tiles. This is crucial and is the whole point of minihop2()



    }


  }

  ////// remove the first siz0 elements of global vectors

}


////// sort end vectors according to endDDs


}
