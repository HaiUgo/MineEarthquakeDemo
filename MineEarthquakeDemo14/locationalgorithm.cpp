#include "locationalgorithm.h"

LocationAlgorithm::LocationAlgorithm()
{

}

LocationAlgorithm::~LocationAlgorithm()
{

}


int LocationAlgorithm::locationAlgorithm(int value)
{
    for(int i=0;i<9;i++){
        value = rand()+1000;
    }
    return value;
}

