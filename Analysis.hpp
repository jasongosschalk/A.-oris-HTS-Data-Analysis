//
//  Analysis.hpp
//  A. oris HTS Data Analysis
//
//  Created by Jason Gosschalk on 4/25/17.
//  Copyright © 2017 Jason Gosschalk. All rights reserved.
//

#ifndef Analysis_hpp
#define Analysis_hpp

#include <vector>
#include<iostream>
#include<fstream>


class Cumulative{
public:
    Cumulative() {};
    void output();
    void removeEntries(Cumulative otherObject);
    void create(std::string input);
private:
    std::vector<std::string> content;
    
};


void Cumulative::output(){
    for(size_t i=0, j = content.size(); i<j; ++i){
        std::cout << content[i] << std::endl;
    }
}
void Cumulative::removeEntries(Cumulative otherObject){
    std::vector<std::string> temp;
    bool determine = 0;
    for(size_t i = 0, j= content.size(); i<j; ++i){
       // std::cout << "First is " << content[i];
        determine = 0;
        for(size_t k = 0, l = otherObject.content.size(); k<l; ++k){
           // std::cout << "the size is " << otherObject.content.size();
           // std::cout << "Second is " << otherObject.content[k];
            
            if(content[i] == otherObject.content[k]){
                //std::cout << "erasing " << content[i] << std::endl;
                content[i].erase();
                determine = 1;
            }
        }
        if(determine == 0){
            temp.push_back(content[i]);
        }
    }
    
    content = temp;
}

void Cumulative::create(std::string fileName){
    std::cout << "got here" << std::endl;

    std::ifstream toOpen;
    toOpen.open(fileName);
    if(toOpen.is_open()){
        std::cout << "it opened" << std::endl;
    }
    else{
        std::cout << "Not open."<< std::endl;
    }
    std::string intake;
    while(std::getline(toOpen, intake)){
        std::cout << "intaking " << intake << std::endl;
        content.push_back(intake);
        intake = "";
    }
    toOpen.close();
}




#endif /* Analysis_hpp */
