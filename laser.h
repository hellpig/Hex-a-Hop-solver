void laser( unsigned char level[],  int row,  int col,  int d,  int &N) {

/*
This function is called by hop.h
Its purpose is to fire a laser and deal with all the insanity that can
ensue.

Global vector "list" is primary output.

N is the score.
N is passed by reference because its updated value is an output of laser().

level is the level array

row and col are where Emi is

d is the direction of the laser (1 through 7)

(c) 2019 Bradley Knockel
*/

// listTemp is a vector of indices of tiles to be destroyed
// (repeats will be removed when creating list)
listTemp.resize(0);

// arrays that makes sure no infinite loops occur by keeping
// track of which ice have fired in which directions.
ice1.resize(0);  // the index of the ice on the level array
ice2.resize(0);  // the direction it fired


// create lists of indices of lasers that have been destroyed
// (this is necessary for level "Breakthrough" to run correctly)
shot.resize(0); // lasers destroyed before current step
shots.resize(0); // lasers destroyed during current step



//// create things where each element is a laser

go1.resize(0);   // contains the row
go1b.resize(0);  // contains the col
go2.resize(0);   // contains d

if (d==7) {  //when jumping on laser, fire in all 6 directions
  go1.assign(6, row);
  go1b.assign(6, col);
  for (int i=1; i<=6; i++) 
    go2.push_back(i);
} else {
  go1.push_back(row);
  go1b.push_back(col);
  go2.push_back(d);
}


unsigned char v,v2;
int i,i2;


//keep stepping until all lasers hit something other than v=0,100,200
while (go1.size()!=0) {
  for (size_t j=0; j<go1.size(); j++) {

    //take a step
    bagged0.s[1] = go1[j] ;
    bagged0.s[2] = go1b[j] ;
    bagged0 = hex(bagged0, go2[j], 1);
    go1[j] = bagged0.s[1];
    go1b[j] = bagged0.s[2];

    //for when laser gets to edge of screen
    if (bagged0.s[1]>=lSize[0] || bagged0.s[1]<0 || bagged0.s[2]>=lSize[1] || bagged0.s[2]<0) {
      go1[j] = -1;
      continue;
    }
    
    i = bagged0.s[0];  //index of location
    v = level[i];     //value at location

    if (v>99){
      v += -100;
      if (v>99) v += -100;
    }

    bool a0 = false; // is our location a destroyed laser?
    for(size_t k=0; k<shot.size(); k++)
      if(shot[k]==i) a0 = true;

    if (v==3 || v==4) { //green tiles
        listTemp.push_back(i);
        go1[j] = -1;
    } else if ( v==1 || v==2 || v==5 || v==6 || v==8 || v==10 || v==11 || v==12 || v==13 || v==14 ) {
        listTemp.push_back(i);
        N += 10;
        go1[j] = -1;
    } else if ( v==7 && !a0 ) { //laser
        shots.push_back(i);
        N += 10;
        listTemp.push_back(i);
        for (int k=1; k<=6; k++) {

            bagged0.s[1] = go1[j];
            bagged0.s[2] = go1b[j];
            bagged0 = hex(bagged0, k, 1);

            i2 = bagged0.s[0];
            v2 = level[i2];

            if (v2>99) {
                v2 += -100;
                if (v>99) v2 += -100;
            }
            if (v2==1 || v2==2 || v2==5 || v2==6 || v2==7 || v2==8 || v2==9 || v2==10 || v2==11 || v2==12 || v2==13)
              N += 10;
            if (v2==7) shots.push_back(i2);
            if (v2>0) listTemp.push_back(i2);
        }
        go1[j] = -1;
    } else if ( v==9 ) {    //ice

        int d1 = go2[j]%6 + 1;
        int d2 = (go2[j]+4)%6 + 1;

        bool a1 = true; // should we pursue d1?
        bool a2 = true; // should we pursue d2?
        for(size_t k=0; k<ice1.size(); k++){
          if(ice1[k]==i){
            if(ice2[k]==d1) a1 = false;
            if(ice2[k]==d2) a2 = false;
          }
        }

        if (a1) {
            ice1.push_back(i);
            ice2.push_back(d1);
            go1.push_back(go1[j]);
            go1b.push_back(go1b[j]);
            go2.push_back(d1);
        }
        if (a2) {
            ice1.push_back(i);
            ice2.push_back(d2);
            go1.push_back(go1[j]);
            go1b.push_back(go1b[j]);
            go2.push_back(d2);
        }
        go1[j] = -1;
    }
  }

  // remove from go1, go1b, and go2 things marked for deletion
  for (int i=go1.size()-1; i>=0; i--){
    if (go1[i]==-1) {

      /* slow
      go1.erase(go1.begin() + i);
      go1b.erase(go1b.begin() + i);
      go2.erase(go2.begin() + i);
      */

      // back-swap trick is probably faster
      go1[i] = go1.back();
      go1b[i] = go1b.back();
      go2[i] = go2.back();
      go1.pop_back();
      go1b.pop_back();
      go2.pop_back();

    }
  }

  // update shot and shots
  for (size_t i=0; i<shots.size(); i++)
    shot.push_back(shots[i]);
  shots.resize(0);

}


////time to deal with repeats in listTemp

list.resize(0);
for(int i=0; i<lSize[2]; i++) {
  int k=0;  // how many times i is in listTemp
  for(size_t j=0; j<listTemp.size(); j++)
    if (listTemp[j]==i) {
      k++;
      if (k==1) list.push_back(i);  // put first instance in list
      else if (level[i]!=3 && level[i]!=4) N += -10;  // reduce score by 10
    }
}


}
