//nov22-825

#include "PC_FileIO.c" //for the file

//degrees the mill rotates
const float MILL = 90;
// length from color sensor to end of belt
const float ENC_LIMIT = 13 * (180/(PI * 1.75));
// length to check for 1 skittle
const float ENC_LIMITMINUS1 = 12 * (180/(PI * 1.75));
const int SPEED1=25;
const int BELT_SPEED=15;
const int END=16000;

bool shouldcontinue = true, twoskittles = true;
// array for positioning of cups
const int ColourAngles[6] = {0, 0, 140, 0, -140, 0};

void rotateMill(int rotations)
{ //rotates mill by MILL to drop skittle onto the belt
	for(int index = rotations; index > 0; index--)
  	{
		nMotorEncoder[motorC] = 0;
	    motor[motorC] = SPEED1;
    	while (nMotorEncoder[motorC] < MILL)
	    {}
    	motor[motorC]=0;
  	}
}

void moveToSensor(int & distance)
{
	nMotorEncoder[motorD] = 0;
  motor[motorD] = BELT_SPEED;
  while(SensorValue[S1] != (int) colorBlack)
	{}
	while((SensorValue[S1] != (int) colorRed) &&
		(SensorValue[S1] != (int)colorBlue) &&
		(SensorValue[S1] != (int)colorYellow) &&
		(nMotorEncoder[motorD] < ENC_LIMITMINUS1))
	{} // checks for either a skittle or to know there is only one skittle
	motor[motorD] = 0;

  	if (nMotorEncoder[motorD] < ENC_LIMIT &&
			nMotorEncoder[motorD] >= ENC_LIMITMINUS1)
  		twoskittles=false; // this means there is only one skittle

	distance = nMotorEncoder[motorD];
	/*the distance between two skittles so that
	they are dropped into the right cups*/
}

int readColour()
{/*this could have been added to moveToSensor but
	due to using the timer for a stop function
	and emergency function, this was made into its own function*/

 	if(SensorValue[S1] != 1 && SensorValue[S1] != 3)
		time1[T1]=0; //checks for inactivity
 	return SensorValue[S1]; // returns the color of skittle
}

void moveToCup(int distance)
{ // moves the skittle and drops it in the cup
	nMotorEncoder[motorD] = 0;
	motor[motorD] = BELT_SPEED;
  	while(nMotorEncoder[motorD] < distance)
 	{}
  	motor[motorD] = 0;
  	nMotorEncoder[motorD] = 0;
}

void rotateCups(int previousColour, int viewColour)
{ /* rotates the cup platform until the skittle color
	matchs the specific degree of rotation in the array for that colour

	cords got tangled if it had to rotate more then 180 degress so
	based on the previous position and the position it was
  supposed to turn too*/
	if(viewColour != previousColour)
  {
    if (ColourAngles[previousColour] < ColourAngles[viewColour])
    {
    	motor[motorA] = SPEED1;
    	motor[motorB] = -SPEED1;
    	while(nMotorEncoder[motorA] < ColourAngles[viewColour])
    	{}
    	motor[motorA] = motor[motorB] = 0;
    }
    else
    {
    	motor[motorA]=-SPEED1;
    	motor[motorB]=SPEED1;
    	while(nMotorEncoder[motorA]>ColourAngles[viewColour])
    	{}
    	motor[motorA]=motor[motorB]=0;
    }
  }
}

void addToTotals(int skittleColour, int & totRed,
	int & totBlue, int & totYel) // R = 5, B = 2, Y = 4
{ // totals of each colour for output data
	if (skittleColour == (int)colorRed)
	{
		totRed++;
	}
	else if (skittleColour == (int)colorBlue)
	{
		totBlue++;
	}
	else if (skittleColour == (int)colorYellow)
	{
		totYel++;
	}
}


void maxSkittles(string & colourReturn, int & max,
	int totRed, int totBlue, int totYel)
{ // checks for whoch colour had the most skittles
	if (totRed > totBlue)
	{
		max = totRed;
		colourReturn = "Red";
	}
	else
	{
		max = totBlue;
		colourReturn = "Blue";
	}
	if (totYel > max)
	{
		max = totYel;
		colourReturn = "Yellow";
	}
}

void minSkittles(string & colourReturn, int & min,
	int totRed, int totBlue, int totYel)
{ //checks for which color had the least skittles
	if (totRed < totBlue)
	{
		min = totRed;
		colourReturn = "Red";
	}
	else
	{
		min = totBlue;
		colourReturn = "Blue";
	}
	if (totYel < min)
	{
		min = totYel;
		colourReturn = "Yellow";
	}
}

void outputToFile(TFileHandle fout, int totRed,
	int totBlue, int totYel,int totalSortTime)
{ // writes all the data to the file
	string colReturn = "";
	int maximumSkittles = 0;
	int minimumSkittles = 0;
	writeTextPC(fout, "Skittle Sorter Data:");
	writeEndlPC(fout);

	writeTextPC(fout, "Total Number of Skittles: ");
	writeLongPC(fout, (totBlue+totRed+totYel));
	writeEndlPC(fout);

	writeTextPC(fout, "Total Number of Red Skittles: ");
	writeLongPC(fout, totRed);
	writeEndlPC(fout);

	writeTextPC(fout, "Total Number of Blue Skittles: ");
	writeLongPC(fout, totBlue);
	writeEndlPC(fout);

	writeTextPC(fout, "Total Number of Yellow Skittles: ");
	writeLongPC(fout, totYel);
	writeEndlPC(fout);

	maxSkittles(colReturn, maximumSkittles, totRed, totBlue, totYel);
	writeTextPC(fout, "Most Skittles are the colour: ");
	writeTextPC(fout, colReturn);
	writeTextPC(fout, " with ");
	writeLongPC(fout, maximumSkittles);
	writeTextPC(fout, " Skittles");
	writeEndlPC(fout);

	minSkittles(colReturn, minimumSkittles, totRed, totBlue, totYel);
	writeTextPC(fout, "Least Skittles are the colour: ");
	writeTextPC(fout, colReturn);
	writeTextPC(fout, " with ");
	writeLongPC(fout, minimumSkittles);
	writeTextPC(fout, " Skittles");
	writeEndlPC(fout);

	writeTextPC(fout, "Total Seconds to sort Skittles: ");
	writeFloatPC(fout, totalSortTime/1000.0);
	writeEndlPC(fout);

	writeTextPC(fout, "Average Time to sort a single Skittle: ");
	writeFloatPC(fout, (totalSortTime/1000.0)/(totBlue+totRed+totYel);
	writeEndlPC(fout);
}

task main()
{
	TFileHandle fout;
	bool okay = openWritePC(fout, "skittles_data.txt");

	if (!okay)
	{
		displayString(0, "Error!");
		wait1Msec(5000);
	}

	int totalRed = 0, totalBlue = 0, totalYellow = 0;
	int firstskittle = 0, secondskittle = 0, lastcolour = 0;
	int rotationstostart = 3;
	int distance = 0;

	SensorType[S1] = sensorEV3_Color;
	wait1Msec(50);
	SensorMode[S1] = modeEV3Color_Color;
	wait1Msec(50);

	displayString(5, "Press Button to Start Sorting");
  while(!getButtonPress(buttonAny))
	{}
	while(getButtonPress(buttonAny))
	{}
	eraseDisplay();
	displayString(5, "Sorting...");

	time1[T1]= 0;
	time1[T2] = 0;

	while (shouldcontinue)
	{// makes sure that the program is still okay to continue
 	   rotateMill(rotationstostart);
	    if (rotationstostart == 3)
   		 	rotationstostart -= 2;
        //only needs to rotate 90 degrees after the first time
    	moveToSensor(distance);
		wait1Msec(2000);

		if(time1[T1] > END && (lastcolour == 1 || lastcolour == 3))
			shouldcontinue=false; /*this whill stop the robot once it knows
			that it is done sorting or there is an emergency*/

    	if (shouldcontinue)
    	{
    		firstskittle = readColour();
      	addToTotals(firstskittle, totalRed, totalBlue, totalYellow);
    		moveToSensor(distance);

    		if (twoskittles)
    		{// function calls for when there are two skittles on the belt
	    		secondskittle = readColour();
      	  addToTotals(firstskittle, totalRed, totalBlue, totalYellow);
	    		wait1Msec(2000);
	    		rotateCups(lastcolour, firstskittle);
	    		moveToCup(ENC_LIMIT - distance);
	    		wait1Msec(2000);
	    		lastcolour = secondskittle;

	    		wait1Msec(2000);
	    		rotateCups(firstskittle, secondskittle);
	    		moveToCup(distance * 5);
		    }

	    	else
	  		{ // code for only one skittle being dropped onto the belt
	  			wait1Msec(2000);
	  			rotateCups(lastcolour, firstskittle);
	  			moveToCup(ENC_LIMIT - distance);
	      		wait1Msec(2000);
	      		lastcolour = firstskittle;
	  		}
	  		twoskittles = true;
		}
	}
	int sortTime = time1[T2];
	eraseDisplay();
	displayString(5, "Done Sorting");
	//outputs final data
  outputToFile(fout, totalRed, totalBlue, totalYellow, sortTime);
  closeFilePC(fout);
	wait1Msec(2000);
} // program end
