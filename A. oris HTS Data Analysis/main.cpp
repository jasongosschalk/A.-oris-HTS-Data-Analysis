//
//  main.cpp
//  A. oris HTS Data Analysis
//
//  Created by Jason Gosschalk on 2/25/17.
//  Copyright © 2017 Jason Gosschalk. All rights reserved.
//

#include <iostream>
#include <string>
#include "Plate.hpp"
#include "Analysis.hpp"


int main() {
    std::string response;
    std::cout << "Data input or analysis? write 'input' or 'analysis'" << std::endl;
    std::cin >> response;
    
    
    if(response == "input"){
        
    std::vector<Plate> compoundLibrary; //this will hold the library, so each plate is an object within this container.
    std::string fileFoundation = "/Users/jasongosschalk/Desktop/College/Clubb Lab/A. oris/Screen Data/051317 LIPID/Text Files/"; //This can be changed depending on where the file comes from. option to make this a reader input to be able to work with different files without touching the source code.
    std::string intake;             //this will be used to read in the name of the library, e.g. TAR
    int startingPlateNumber;        //this is if you only want to look at part of a library or you don't do an entire library at once, also required if you want to look at an entire library.
    int endingPlateNumber;
    
    //following code takes in from the user the range of plates they're working with from a library.
    std::cout << "Enter name of library" << std::endl;
    std::cin >> intake;
    intake += "-";
    fileFoundation += intake;
    std::cout << "Enter starting plate number" << std::endl;
    std::cin >> startingPlateNumber;
    std::cout << "Enter last plate number" << std::endl;
    std::cin >> endingPlateNumber;
 
    //this for loop goes through and fills a vector with Plate objects, each object is a plate form the given library. option later to scale this up by making a vector of libraries, so a vector of vectors, if we want to do larger data work
    for(int i = startingPlateNumber, j =endingPlateNumber; i<=j; ++i){
        std::string toOpen = fileFoundation;    //temporary string so that we can slightly change the thing we are opening.
        toOpen += std::to_string(i);            //sets to the appropriate plate number  //this makes it such that its, e.g. TAR-1...TAR-2...TAR-3...TAR-i etc.
        toOpen += ".txt";                       //adds .txt file ending to the name.
        std::cout << "Opening \n" << toOpen << std::endl; //informs the user that it's opening a text file, doesn't read this to a file, reads to console.
        Plate newPlate;                         //creates a new plate
        newPlate.CreatePlate(toOpen);           // transfers the plate to ownership outside of the for loop, so it's not lost.
        compoundLibrary.push_back(newPlate);    //adds to the library
        toOpen = fileFoundation;                //resets the temporary string so that it can be remade.
    }

    //this for loop goes through a given vector of plates and calculates statistical information, then determines if there were any hits on that plate.
    for(size_t i = 0, j = compoundLibrary.size(); i<j; ++i){
        compoundLibrary[i].CalcData();
        compoundLibrary[i].determineHits();
    }
    
    
    
    
    std::string forDataCumulation = fileFoundation + "Cumulative";
    std::string allData;
    
    //creates a string that contains all the moleucle data (no controls) of a library for use in excel, graphs, etc
    for(size_t i =0; i<320; ++i){
        for(size_t k = 0, l = compoundLibrary.size(); k<l; ++k){
            allData += compoundLibrary[k].printAllSamples(i);
        }
        allData += "\n";//creates new line after each plate has been gone through. 
    }
    
    
    //create a file of all SAMPLE data from a plate if you want to manipulate it later for graphs, stats, etc.
    std::ofstream compiledData;              // creates ofstream to read to a file
    compiledData.open(forDataCumulation);
    compiledData << allData;
    compiledData.close();
    
    std::string NegConCumulation = fileFoundation + "Negative Control Cumulative";
    std::string allNegConData;
    
    //creates a string that contains all the moleucle data (no controls) of a library for use in excel, graphs, etc
    for(size_t i =0; i<32; ++i){
        for(size_t k = 0, l = compoundLibrary.size(); k<l; ++k){
            allNegConData += compoundLibrary[k].printAllNegConSamples(i);
        }
        allNegConData += "\n";//creates new line after each plate has been gone through.
    }
    
    
    //create a file of all SAMPLE data from a plate if you want to manipulate it later for graphs, stats, etc.
    std::ofstream compiledNegConData;              // creates ofstream to read to a file
    compiledNegConData.open(NegConCumulation);
    compiledNegConData << allNegConData;
    compiledNegConData.close();
    
    
    
    
    //this handles output of information to a file
    std::string dataSummary = fileFoundation + "Data Summary"; //will name anything "Library-Data Summary" e.g. TAR-Data Summary
    std::ofstream readOut;              // creates ofstream to read to a file
    readOut.open(dataSummary);       //opens the file so that it can be used, also creates it if not present.
    

    //this for loop goes through the vector of a library and then for each element (E.g. each physical plate) sends the information about that plate to the endpoint file in the folder that all the data is located
    for(size_t i = 0, j = compoundLibrary.size(); i<j; ++i){
        readOut << compoundLibrary[i].printInfo();  //sends to file
    }
    readOut.close();    //closes out the file
    
    std::string toMSSR = fileFoundation + "Hits for MSSR"; //will name anything "Library-Data Summary" e.g. TAR-Data Summary
    std::ofstream MSSRFile;              // creates ofstream to read to a file
    MSSRFile.open(toMSSR);       //opens the file so that it can be used, also creates it if not present.
    
    
    std::vector<std::string> MSSRVector; //vector that will contain every data value with plate and well descriptor information in the same element. e.g. element 0 would be = "TAR-1 A1 0.679"
    
    //creates a vector for each plate containing the data in format: "plate well/number data value"
    for(size_t i = 0, j = compoundLibrary.size(); i<j; ++i){
        std::string sourceName = fileFoundation + "-" + std::to_string(1+i);
        std::string plateName = intake + std::to_string(1+i);
        compoundLibrary[i].hitsForMSSR(sourceName, plateName, MSSRVector);  //passes CDD vector by reference, adds to the vector which will contain whole library information and also makes a file for every single plate.
        //CDDVector.push_back(intake + std::to_string(i) + compoundLibrary[i].)
    }
    
    //adds all elements to the file for upload to CDD
    for(size_t i = 0, j = MSSRVector.size(); i<j; ++i){
        MSSRFile << MSSRVector[i] << "\n";
    }

    MSSRFile.close();
    
    
    std::cout << std::endl <<std::endl <<std::endl; //why is this here lol
    
    //for loop that checks each control to make sure tht it's not too high positive control that is not actually no growth)
    for(size_t i = 0, j = compoundLibrary.size(); i<j; ++i){
        std::cout << compoundLibrary[i].checkControls();  //sends to file
    }
    
    //opens file to put CDD upload data in
    std::string forCDD = fileFoundation + "Data for CDD.csv";
    std::ofstream cdd;
    cdd.open(forCDD);
    std::vector<std::string> CDDVector; //vector that will contain every data value with plate and well descriptor information in the same element. e.g. element 0 would be = "TAR-1 A1 0.679"
    
    //creates a vector for each plate containing the data in format: "plate well/number data value"
    for(size_t i = 0, j = compoundLibrary.size(); i<j; ++i){
        std::string sourceName = fileFoundation + "-" + std::to_string(1+i);
        std::string plateName = intake + std::to_string(1+i);
        compoundLibrary[i].makeList(sourceName, plateName, CDDVector);  //passes CDD vector by reference, adds to the vector which will contain whole library information and also makes a file for every single plate.
        //CDDVector.push_back(intake + std::to_string(i) + compoundLibrary[i].)
    }
    
    
    //adds all elements to the file for upload to CDD
    for(size_t i = 0, j = CDDVector.size(); i<j; ++i){
        cdd << CDDVector[i] << "\n";
    }
    cdd.close();
    }
    
    else if (response == "analysis"){
        //This can be changed depending on where the file comes from. option to make this a reader input to be able to work with different files without touching the source code.
        std::cout << "at analysis" << std::endl;
        std::string hitsFile = "/Users/jasongosschalk/Desktop/College/Clubb Lab/A. oris/Screen Data/Cumulative Data Analysis/041417/All hits up to 041917 plate only.txt";
        std::cout << "Opening" << hitsFile << std::endl;
        
//        std::cout << "Enter hits file location" << std::endl;
//        std::getline(std::cin, hitsFileIntake);
//        std::string hitsFile = hitsFileIntake + "/"; //necessary for mac, must be removed on PC.
        
//        std::cout << std::endl << hitsFile << std::endl;
        Cumulative theHits;
        theHits.create(hitsFile);
        //theHits.output();
        
        Cumulative removeBacteria;
        removeBacteria.create("/Users/jasongosschalk/Desktop/College/Clubb Lab/A. oris/Screen Data/Cumulative Data Analysis/041417/Data from other bacterial screen.txt");
        //removeBacteria.output();
        
        theHits.removeEntries(removeBacteria);
        
        std::cout << "The new list is: " << std::endl;
        theHits.output();
        
    }
    return 0;

}
