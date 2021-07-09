bagger  hex(  bagger bagged,  int d,  int N) {

/*
this function is called by hop(), minihop(), and laser()
its purpose is to move along hexagonal tiles
to update bagged.s.

I don't pass bagged.s by reference because I sometimes want to have multiple s's.

bagged.s is the state of Emi as defined in hop.h

d is a number 1 through 6 that defines which direction to alter bagged.s...
  1 is up, 2 is NE, 3 is SE, 4 is down, 5 is SW, and 6 is NW

N is the number of alteration to be made in that direction (negative N
makes alterations as if d were the opposite direction)

(c) 2019 Bradley Knockel
*/


switch(d){
case 1:
 bagged.s[1] += -2*N;
 break;
case 2:
 bagged.s[1] += -N;
 bagged.s[2] += N;
 break;
case 3:
 bagged.s[1] += N;
 bagged.s[2] += N;
 break;
case 4:
 bagged.s[1] += 2*N;
 break;
case 5:
 bagged.s[1] += N;
 bagged.s[2] += -N;
 break;
case 6:
 bagged.s[1] += -N;
 bagged.s[2] += -N;
 break;
}

bagged.s[0] = (lSize[1] * bagged.s[1] + bagged.s[2])/2 ;


return bagged;

}
