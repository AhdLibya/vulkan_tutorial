#pragma once
#include<iostream>

#ifndef DEBUG
#define LOG(X) std::cout<<X<<'\n'
#else
#define LOG(X) 
#endif // DEBUG

//typedef ui  unsigned int;