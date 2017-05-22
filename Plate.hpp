//
//  Plate.hpp
//  A. oris HTS Data Analysis
//
//  Created by Jason Gosschalk on 2/26/17.
//  Copyright © 2017 Jason Gosschalk. All rights reserved.
//

#ifndef Plate_hpp
#define Plate_hpp

#include <stdio.h>
#include <fstream>
#include <vector>
#include <string>
#include <iostream>
#include <math.h>
#include <sstream>
/**
        @class this class is an object type plate, which represents a physical 384-well plate, where each well of the plate has a data vlaue. in this case the data is an optical density reading, but could be anything (e.g. fluorescence) as long as the statistical needs are the same as we see here. 
        @brief this class can be called from main and handles all statistical and numberical information internally, including multiple vectors of type double that hold numerical decimal values that represented optical density reading. the class automatically divides input into three other vectors, so there are four total containers wihtin this container. One is all of the plate, one is just the wells of the negative control (positive growth, no AB, columns 1-2), one is just the small molecule wells (columns 3-22) and the third is the positive control (no growth due to antibiotic) (columns 23-24 of the plate)
 */
class Plate {
public:
    Plate(){};                          //default constructor does nothing
    void CreatePlate(std::string name); //creates the plate and divides the data up into the respective vectors of type double 
    void CalcData();
    void calculateZ();
    void determineHits();//anything greater than 3 SD away from average
    std::string printInfo();
    void makeList(std::string pass, std::string plateName, std::vector<std::string> &forCDD); //will convert a table type file into a list typ efile
    std::string checkControls();
    std::string printAllSamples(size_t location);
    void hitsForMSSR(std::string fileLocation, std::string plateName, std::vector<std::string> &forMSSR);  //prints to file all the hits in a library. called from main.
//    std::string printOneSample(std::string library, std::string plateNumber, std::vector<std::string> &forCDD);
    std:: string printAllNegConSamples(size_t location); //prints out all of the negative control values to a sample
    
private:
    std::string namePlate; //holds the name of the plate from a given library.
    std::vector<double> thePlate; //this will be a plate containing each of the values of the well as a float. 0 = well A1, 1 = A1, 2 = A3, etc.
    std::vector<double> thePlatePosCon;
    std::vector<double> thePlateNegCon;
    std::vector<double> thePlateData;
    std::vector<int> hits;
    std::vector<std::string> hitWells;
    std::vector<std::string> asList;

    double dataStandardDeviation = 0;
    double dataSum = 0;
    double dataSumSquares = 0;
    double dataMean = 0;
    double dataVariance = 0;
    
    double posConStandardDeviation = 0;
    double posConSum = 0;
    double posConSumSquares = 0;
    double posConMean = 0;
    double posConVariance = 0;
    
    double negConStandardDeviation = 0;
    double negConSum = 0;
    double negConSumSquares = 0;
    double negConMean = 0;
    double negConVariance = 0;
    
    double zFactor = 0;
    
};

//this needs to create a new plate object
void Plate::CreatePlate(std::string name){
    namePlate = name;
    //name ="/Users/jasongosschalk/Desktop/College/Clubb Lab/A. oris/A. oris HTS Data Analysis/test.txt"; //replace this later so that user input dictates the file name.
    double newValue; //this will be the numerical value taken in after it's converted to float
    std::string stringValue;//will take in the value as a string to be converted
    std::ifstream plateFile(name);
    std::string::size_type sz;

    while(getline(plateFile, stringValue)){
        std::string tempWord; //handles individual numbers when there are multiple numbers per line.
        
        /**
         for loop that will fill the vector with all the numbers in a line in a given text file
         for example if the file is: 
        12 23 17 
        18 17 34
         then the program will intake the line 12 23 17 and each number will be made into a double and become an element of the vector thePlate
         */
        for(size_t i = 0; i < stringValue.size(); ++i){
//            if(stringValue[i] == '\"' || stringValue[i] == ' '){
//                std::cout <<"SKIPPED! " << std::endl;
//                continue;
//            }
            
            if(stringValue[i] != '	' && (thePlate.size()+1)%24 != 0){  //if the value is not a space, meaning the number has ended
                tempWord = tempWord + stringValue[i]; //creates a string of just the one number by adding the next digit in the string
            }
            
            else if ((thePlate.size()+1)%24 == 0){
                
                if(!stringValue[i+2]){
                    if(i == stringValue.size()){
//                    std::cout << "its last" << tempWord << std::endl;
                    newValue = std::stod(tempWord, &sz);    //convert the string to a double
                    thePlate.push_back(newValue);           //adds the value to the vector
                    tempWord.clear();
                    }
                    else{
                        tempWord = tempWord + stringValue[i]; //creates a string of just the one number by adding the next digit in the string

                    }
                }
                if(stringValue[i+2]){
                    if(stringValue[1+i] == '0' && stringValue[2+i] == '.'){
//                        std::cout << "its divisible" << tempWord << std::endl;
                        newValue = std::stod(tempWord, &sz);    //convert the string to a double
                        thePlate.push_back(newValue);           //adds the value to the vector
                        tempWord.clear();
                    }
                    else{
                        tempWord = tempWord + stringValue[i]; //creates a string of just the one number by adding the next digit in the string
                    }
                }
            }
            
            
            else { //if it's a space, the number is done.
//                std::cout << "its" << tempWord << std::endl;
                newValue = std::stod(tempWord, &sz);    //convert the string to a double
                thePlate.push_back(newValue);           //adds the value to the vector
                tempWord.clear();                       //resets the temproary word so we can add the next number in the line.
        }
}
        
        //this code adds the final number in the line, because it gets skipped in the code above.
        newValue = std::stod(tempWord, &sz);    //
        thePlate.push_back(newValue); //adds the value to the vector

    }


    
    plateFile.close();  //closes the file.
    
    
    //create vectors for positive control, negative control, main data.
    for(size_t i = 0; i < thePlate.size(); ++i){
        if( i == 0 || i == 1 || i == 24 || i == 25 || i == 48 || i == 49 || i == 72 || i == 73 || i == 96 || i == 97 || i == 120 || i == 121 || i == 144 || i == 145 || i == 168 || i == 169 || i == 192 || i == 193 || i == 216 || i == 217 || i == 240 || i == 241 || i == 264 || i == 265 || i == 288 || i == 289 || i == 312 || i == 313 || i == 336 || i == 337 || i == 360 || i == 361){
        thePlateNegCon.push_back(thePlate[i]);
        }
        else if (i == 22 || i == 23 || i == 46 || i == 47 || i == 70  || i == 71  || i == 94 || i == 95 || i == 118 || i == 119 || i == 142 || i == 143 || i == 166 || i == 167 || i == 190 || i == 191 || i == 214 || i == 215 || i == 238 || i == 239 || i == 262 || i == 263 || i == 286 || i == 287 || i == 310 || i == 311 || i == 334 || i == 335 || i == 358 || i == 359 || i == 382 || i == 383){
            thePlatePosCon.push_back(thePlate[i]);
        }
        else{
            thePlateData.push_back(thePlate[i]);
        }
    }

//    for(size_t i = 0, j = thePlateData.size(); i<j; ++i){
//            std::cout << thePlateData[i] << std::endl;
//        }

    return;
}
/**
 @about this function calculates the statistical values for the positive control, negative control, and data.
 */

void Plate::CalcData(){
    //calculate sum
    for (size_t i = 0; i < thePlateData.size(); ++i){
        dataSum = dataSum + thePlateData[i];
    }
    dataMean = dataSum / double(thePlateData.size());
    
    for (size_t i = 0; i < thePlateData.size(); ++i){
        double temp = fabs(thePlateData[i] - dataMean);
        dataSumSquares += pow((temp), 2.0);   //squares each number before adding.
    }
    
    dataVariance = dataSumSquares / double(thePlateData.size()); //is the variance of the data
    dataStandardDeviation = sqrt(dataVariance); //is the standard deviation
    
    //for neg con
    for (size_t i = 0; i < thePlateNegCon.size(); ++i){
        negConSum = negConSum + thePlateNegCon[i];
    }
    negConMean = negConSum / double(thePlateNegCon.size());
    
    for (size_t i = 0; i < thePlateNegCon.size(); ++i){
        double temp = fabs(thePlateNegCon[i] - negConMean);
        negConSumSquares += pow((temp), 2.0);   //squares each number before adding.
    }
    
    negConVariance = negConSumSquares / double(thePlateNegCon.size()); //is the variance of the data
    negConStandardDeviation = sqrt(negConVariance); //is the standard deviation
    
    
    //for pos con
    for (size_t i = 0; i < thePlatePosCon.size(); ++i){
        posConSum = posConSum + thePlatePosCon[i];
    }
    posConMean = posConSum / double(thePlatePosCon.size());
    
    for (size_t i = 0; i < thePlatePosCon.size(); ++i){
        double temp = fabs(thePlatePosCon[i] - posConMean);
        posConSumSquares += pow((temp), 2.0);   //squares each number before adding.
    }
    
    posConVariance = posConSumSquares / double(thePlatePosCon.size()); //is the variance of the data
    posConStandardDeviation = sqrt(posConVariance); //is the standard deviation
    
    calculateZ(); //calculates z factor for a given plate.
    return;
}

/**
    @about this function calculates the z factor of the plate. */
void Plate::calculateZ(){
    zFactor = 1 - (3*(posConStandardDeviation+negConStandardDeviation)/(fabs((posConMean-negConMean)))); //equation for Z factor.
    return;
}


void Plate::determineHits(){
    //this for loop goes through the small molecule wells and calculates whether they are below or above 3Xstandard deviation
    for(size_t i = 0, j = thePlateData.size(); i<j; ++i){
        //use negative control statistics
        if(thePlateData[i] < (negConMean - 3*negConStandardDeviation)){   // || thePlateData[i] > (dataMean + 3*dataStandardDeviation if it's either 3 above or 3 below standard deviation of the plate
            hits.push_back(i);  //add the location of the hit into the vector that holds the location of all the hit wells.
            //i know it's bad that i converted a size_t to an int there but whatever
        }
    }
    std::string tempWord; //this is used to store the name of the well that the hit is in.
    //this for loop goes through and assigns a well name to the hit, depending on its location. adds a letter and a number by casting a double to a string, requires C++11
    for(size_t i = 0, j = hits.size(); i<j; ++i){
        if(hits[i] < 20){
            tempWord = "A";
            tempWord += std::to_string(hits[i]+3);
            hitWells.push_back(tempWord);
        }
        else if(hits[i] < 40){
            tempWord = "B";
            tempWord += std::to_string(hits[i]-17);
            hitWells.push_back(tempWord);
        }
        else if(hits[i] < 60){
            tempWord = "C";
            tempWord += std::to_string(hits[i]-37);
            hitWells.push_back(tempWord);
        }
        else if(hits[i] < 80){
            tempWord = "D";
            tempWord += std::to_string(hits[i]-57);
            hitWells.push_back(tempWord);
        }
        else if(hits[i] < 100){
            tempWord = "E";
            tempWord += std::to_string(hits[i]-77);
            hitWells.push_back(tempWord);
        }
        else if(hits[i] < 120){
            tempWord = "F";
            tempWord += std::to_string(hits[i]-97);
            hitWells.push_back(tempWord);
        }
        else if(hits[i] < 140){
            tempWord = "G";
            tempWord += std::to_string(hits[i]-117);
            hitWells.push_back(tempWord);
        }
        else if(hits[i] < 160){
            tempWord = "H";
            tempWord += std::to_string(hits[i]-137);
            hitWells.push_back(tempWord);
        }
        else if(hits[i] < 180){
            tempWord = "I";
            tempWord += std::to_string(hits[i]-157);
            hitWells.push_back(tempWord);
        }
        else if(hits[i] < 200){
            tempWord = "J";
            tempWord += std::to_string(hits[i]-177);
            hitWells.push_back(tempWord);
        }
        else if(hits[i] < 220){
            tempWord = "K";
            tempWord += std::to_string(hits[i]-197);
            hitWells.push_back(tempWord);
        }
        else if(hits[i] < 240){
            tempWord = "L";
            tempWord += std::to_string(hits[i]-217);
            hitWells.push_back(tempWord);
        }
        else if(hits[i] < 260){
            tempWord = "M";
            tempWord += std::to_string(hits[i]-237);
            hitWells.push_back(tempWord);
        }
        else if(hits[i] < 280){
            tempWord = "N";
            tempWord += std::to_string(hits[i]-257);
            hitWells.push_back(tempWord);
        }
        else if(hits[i] < 300){
            tempWord = "O";
            tempWord += std::to_string(hits[i]-277);
            hitWells.push_back(tempWord);
        }
        else if(hits[i] < 320){
            tempWord = "P";
            tempWord += std::to_string(hits[i]-297);
            hitWells.push_back(tempWord);
        }
    }
    return;
}


std::string Plate::printInfo(){
    
//    if(readOut.is_open()){
//        std::cout << "I'm open.";
//    }
    std::string toReturn;
    
    toReturn += "Plate " + namePlate + "\n";
    toReturn += "The Plate Positive Control mean is " + std::to_string(posConMean) + "\n";
    toReturn += "The Plate Positive Control SD is " + std::to_string(posConStandardDeviation) + "\n";
    toReturn += "The Plate Negative Control mean is " + std::to_string(negConMean) + "\n";
    toReturn += "The Plate Negative ControlSD is " + std::to_string(negConStandardDeviation) + "\n";
    toReturn += "The Plate mean is " + std::to_string(dataMean) + "\n";
    toReturn += "The Plate SD is " + std::to_string(dataStandardDeviation) + "\n";
    toReturn += "The Plate Z score is " + std::to_string(zFactor) + "	" + " Total hits:	" + std::to_string(hits.size()) + "\n";
    
    if (hitWells.size() == 0){
        toReturn += ("no hits \n");
    }
    else{
        toReturn += "The hits are: \n";
        for (size_t i = 0, j = hitWells.size(); i<j; ++i){
            //pick up here, need to some how print the value of the hit per plate.
            //            double temp = hitWells[i];
            toReturn +=  hitWells[i] +  "	" + std::to_string(thePlateData[hits[i]]) + "\n"; //" at OD " +
        }
    }
    return toReturn;
}



void Plate::makeList(std::string fileName, std::string plateName, std::vector<std::string> &forCDD){
    
    for(size_t i = 0, j = thePlate.size(); i<j; ++i){
        std::string tempWord;   //will contain the list
        if(i < 24){
            tempWord = "A"; //GIVES WILL LETTER
            tempWord += std::to_string(i+1); //gives will number
            tempWord += ", " + std::to_string(thePlate[i]); //gives well OD value
            asList.push_back(tempWord); //adds "A1  0.7" to the vector asList. 
        }
        else if(i < 48){
            tempWord = "B";
            tempWord += std::to_string(i-23);
            tempWord += ", " + std::to_string(thePlate[i]);
            asList.push_back(tempWord);
        }
        else if(i < 72){
            tempWord = "C";
            tempWord += std::to_string(i-47);
            tempWord += ", " + std::to_string(thePlate[i]);
            asList.push_back(tempWord);
        }
        else if(i < 96){
            tempWord = "D";
            tempWord += std::to_string(i-71);
            tempWord += ", " + std::to_string(thePlate[i]);
            asList.push_back(tempWord);
        }
        else if(i < 120){
            tempWord = "E";
            tempWord += std::to_string(i-95);
            tempWord += ", " + std::to_string(thePlate[i]);
            asList.push_back(tempWord);
        }
        else if(i < 144){
            tempWord = "F";
            tempWord += std::to_string(i-119);
            tempWord += ", " + std::to_string(thePlate[i]);
            asList.push_back(tempWord);
        }
        else if(i < 168){
            tempWord = "G";
            tempWord += std::to_string(i-143);
            tempWord += ", " + std::to_string(thePlate[i]);
            asList.push_back(tempWord);
        }
        else if(i < 192){
            tempWord = "H";
            tempWord += std::to_string(i-167);
            tempWord += ", " + std::to_string(thePlate[i]);
            asList.push_back(tempWord);
        }
        else if(i < 216){
            tempWord = "I";
            tempWord += std::to_string(i-191);
            tempWord += ", " + std::to_string(thePlate[i]);
            asList.push_back(tempWord);
        }
        else if(i < 240){
            tempWord = "J";
            tempWord += std::to_string(i-215);
            tempWord += ", " + std::to_string(thePlate[i]);
            asList.push_back(tempWord);
        }
        else if(i < 264){
            tempWord = "K";
            tempWord += std::to_string(i-239);
            tempWord += ", " + std::to_string(thePlate[i]);
            asList.push_back(tempWord);
        }
        else if(i < 288){
            tempWord = "L";
            tempWord += std::to_string(i-263);
            tempWord += ", " + std::to_string(thePlate[i]);
            asList.push_back(tempWord);
        }
        else if(i < 312){
            tempWord = "M";
            tempWord += std::to_string(i-287);
            tempWord += ", " + std::to_string(thePlate[i]);
            asList.push_back(tempWord);
        }
        else if(i < 336){
            tempWord = "N";
            tempWord += std::to_string(i-311);
            tempWord += ", " + std::to_string(thePlate[i]);
            asList.push_back(tempWord);
        }
        else if(i < 360){
            tempWord = "O";
            tempWord += std::to_string(i-335);
            tempWord += ", " + std::to_string(thePlate[i]);
            asList.push_back(tempWord);
        }
        else if(i < 384){
            tempWord = "P";
            tempWord += std::to_string(i-359);
            tempWord += ", " + std::to_string(thePlate[i]);
            asList.push_back(tempWord);
        }
    }
    
    //std::ofstream readOut;
    //readOut.open(fileName);
    //readOut << plateName << std::endl;  //sends to file

    for(size_t i = 0, j = asList.size(); i<j; ++i){
        //readOut << plateName << "	" << asList[i] << std::endl;  //sends to file
        forCDD.push_back(plateName + ", " + asList[i]);
    }
    //readOut.close();

    
}

std::string Plate::checkControls(){
    std::string toReturn;
    for(size_t i = 0, j = thePlatePosCon.size(); i<j; ++i){
        if(thePlatePosCon[i] > 0.1){
            toReturn += "ERROR, CONTROL > 0.1: " + namePlate + std::to_string(thePlatePosCon[i]) + "\n";
        }
    }
    return toReturn;
}

std::string Plate::printAllSamples(size_t location){
    
    
    //std::cout << thePlateData.size();
    std::string pass;
    pass = std::to_string(thePlateData[location]) + "	";
    return pass;
}

std:: string Plate::printAllNegConSamples(size_t location){
    std::string pass;
    pass = std::to_string(thePlateNegCon[location]) + "	";
    return pass;
}


/**
 @function this function prints out to a file all of the hits for testing in the double knockout strain, in a format compatible with automation. 
*/
void Plate::hitsForMSSR(std::string fileLocation, std::string plateName, std::vector<std::string> &forMSSR){
    //these are all the hit wells per plate. for loop runs through a vector that contains all the hit wells.
    for (size_t i = 0, j = hitWells.size(); i<j; ++i){
        //adds to vector the plate name-number, the letter/number of well, and then the OD value of that well. calls on the vector hitwells, which contains the information about the well name. the hits and hitwells vectors are the same size, and hits vector contains the index of the hit in the plate data, or the vector that contains all of the sample values in a hit. so hitwells[i] corresponds to hits[i], which informs us where the value is in thePlateData vector. therefore we can combine the two vectors here to assign well name to well value. no one said it's beautiful. but it works.  
        forMSSR.push_back(plateName +  "	" + hitWells[i] +  "	" + std::to_string(thePlateData[hits[i]])); //" at OD " +
    }
    
    
}

#endif /* Plate_hpp */

