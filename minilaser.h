bool minilaser( unsigned char level[],  int row,  int col,  int &d) {

/*
This function is called by minihop.h
Its purpose is to fire a laser and deal with any ice.
If something is destroyed, return immediately!
For the simple case of another laser being hit immediately,
d is set to -1 (used as an output). 

The boolean output is true if the laser didn't hit anything!

level is the level array

row and col are where Emi is

d is the direction of the laser (1 through 6)

(c) 2019 Bradley Knockel
*/


unsigned char v;
int i;


// make a quick check to see if anything is immediately hit
bagged0.s[1] = row ;
bagged0.s[2] = col ;
bagged0 = hex(bagged0, d, 1);
v = level[bagged0.s[0]];
if (v==7) {        // laser
  d = -1;
  return false;
} else if (v==9) {  // do nothing for ice
} else if ( v>0 && v<15 ) {
  return false;
}


// arrays that makes sure no infinite loops occur by keeping
// track of which ice have fired in which directions.
ice1.resize(0);  // the index of the ice on the level array
ice2.resize(0);  // the direction it fired



//// create things where each element is a laser

go1.resize(0);   // contains the row
go1b.resize(0);  // contains the col
go2.resize(0);   // contains d

go1.push_back(row);
go1b.push_back(col);
go2.push_back(d);




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


    if ( v==9 ) {    //ice

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
    } else if ( v>0 && v<15 ) {
        return false;
    }


  }

  // remove from go1, go1b, and go2 things marked for deletion
  for (int i=go1.size()-1; i>=0; i--){
    if (go1[i]==-1) {
      go1[i] = go1.back();
      go1b[i] = go1b.back();
      go2[i] = go2.back();
      go1.pop_back();
      go1b.pop_back();
      go2.pop_back();
    }
  }

}


return true;


}
