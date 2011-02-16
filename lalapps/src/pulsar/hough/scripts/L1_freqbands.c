/*
*  Copyright (C) 2007 Alicia Sintes Olives
*
*  This program is free software; you can redistribute it and/or modify
*  it under the terms of the GNU General Public License as published by
*  the Free Software Foundation; either version 2 of the License, or
*  (at your option) any later version.
*
*  This program is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with with program; see the file COPYING. If not, write to the
*  Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston,
*  MA  02111-1307  USA
*/

/**
 * \file
 * \ingroup pulsarApps
 * \author Alicia Sintes Olives
 */

#include <stdio.h> 
#include <stdlib.h>
#include <string.h> 
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
int main(int argc, char *argv[]){
  
  int i, arg;
  float bandlist[172][2] = {
    {200.0, 201.0},
    {201.0, 202.0},
    {202.0, 203.0}, 
    {203.0, 204.0},
    {204.0, 205.0},
    {205.0, 206.0},
    {206.0, 207.0},
    {207.0, 207.9},
    {208.1, 209.0},
    {209.0, 210.3},
    {210.4, 212.0},
    {212.0, 213.0},
    {213.0, 214.0},
    {214.0, 215.0},
    {215.0, 216.0},
    {216.0, 217.0},
    {217.0, 218.0},
    {218.0, 219.0},
    {219.0, 220.0},
    {220.0, 221.1},
    {221.3, 222.6},
    {222.6, 224.0},
    {224.0, 225.0},
    {225.0, 226.0},
    {226.0, 227.0},
    {227.0, 228.0},
    {228.0, 229.0},
    {229.0, 230.0},
    {230.0, 231.0},
    {231.0, 232.0},
    {232.0, 233.0},
    {233.0, 234.0},
    {234.0, 235.0},
    {235.0, 236.0},
    {236.0, 237.0},
    {237.0, 238.0},
    {238.0, 239.6},
    {240.4, 242.0},
    {242.0, 243.0},
    {243.0, 244.0},
    {244.0, 245.0},
    {246.0, 247.0},
    {247.0, 248.0},
    {248.0, 248.7},
    {248.8, 249.9},
    {250.1, 251.0},
    {251.0, 252.0},
    {252.0, 253.0},
    {253.0, 253.7}, 
    {253.8, 255.0},
    {255.0, 256.0},
    {256.0, 257.0},
    {257.0, 258.0},
    {258.2, 259.0},
    {259.0, 260.0},
    {260.0, 261.0},
    {261.0, 262.0},
    {262.0, 263.0},
    {263.0, 264.0},
    {264.0, 265.0},
    {265.0, 266.0},
    {266.0, 267.0},
    {267.0, 268.0},
    {268.0, 269.0},
    {269.0, 270.0},
    {270.0, 271.0},
    {271.0, 272.0},
    {272.0, 273.0},
    {273.0, 274.0},
    {274.0, 275.2},
    {275.3, 276.0},
    {276.0, 277.0},
    {277.0, 278.0},
    {278.0, 279.0},
    {279.0, 280.0},
    {280.0, 281.0},
    {281.0, 281.9},
    {282.1, 283.0},
    {283.0, 283.9},
    {284.1, 285.0},
    {285.0, 285.9},
    {286.1, 287.1},
    {287.4, 287.9},
    {288.1, 289.0},
    {289.0, 290.0},
    {290.0, 291.0},
    {291.0, 292.0},
    {292.0, 293.0},
    {293.0, 294.0},
    {294.0, 294.9},
    {295.0, 296.0},
    {296.0, 297.0},
    {297.0, 298.0},
    {298.0, 299.5},
    {300.5, 301.0},
    {301.0, 302.0},
    {302.0, 303.0},
    {303.0, 304.0},
    {304.0, 305.0},
    {305.0, 305.9},
    {306.3, 307.9},
    {308.1, 309.0},
    {309.0, 309.9},
    {310.1, 311.0},
    {311.0, 312.0},
    {312.0, 313.0},
    {313.0, 314.0},
    {314.0, 315.0},
    {315.0, 316.0},
    {316.0, 317.0},
    {317.0, 317.9},
    {318.1, 319.2},
    {319.3, 319.9},
    {320.1, 321.0},
    {321.0, 321.9},
    {322.1, 323.0},
    {323.0, 323.9},
    {324.1, 325.0},
    {325.0, 325.9},
    {326.1, 327.4},
    {328.1, 329.1},
    {330.1, 331.2},
    {332.6, 334.0},
    {334.0, 335.0},
    {335.0, 335.9},
    {336.4, 337.2},
    {337.3, 338.2},
    {338.3, 339.0},
    {339.0, 340.0},
    {340.0, 341.0},
    {341.0, 342.0},
    {342.0, 342.8},
    {344.5, 345.5},
    {345.5, 346.4},
    {347.1, 348.0},
    {348.0, 349.0},
    {349.0, 350.0},
    {350.0, 351.0},
    {351.0, 352.0},
    {352.0, 353.0},
    {353.0, 354.0},
    {354.0, 355.0},
    {355.0, 356.0},
    {356.0, 357.0},
    {357.0, 358.3},
    {358.3, 359.6},
    {360.6, 362.0},
    {363.0, 363.0},
    {363.0, 364.0},
    {364.0, 365.0},
    {365.0, 365.9},
    {366.1, 367.4},
    {367.4, 368.6},
    {368.8, 369.7},
    {369.8, 371.2},
    {371.3, 374.4},
    {376.6, 378.0},
    {378.0, 379.0},
    {379.0, 380.0},
    {380.0, 381.2},
    {382.1, 383.0},
    {383.0, 383.9},
    {384.5, 385.7},
    {386.1, 387.0},
    {387.0, 388.2},
    {389.0, 389.9},
    {390.1, 391.4}, 
    {391.6, 392.4},
    {393.4, 394.8},
    {394.8, 396.2},
    {396.4, 397.4},
    {398.3, 399.9}
  };

  i = 0; 
  if (argc > 1) i = atoi( argv[argc-1] );
  if ((i>=1)&&(i<=172)) printf("%f  %f\n", bandlist[i-1][0], bandlist[i-1][1]);
 
  return 0;
}








