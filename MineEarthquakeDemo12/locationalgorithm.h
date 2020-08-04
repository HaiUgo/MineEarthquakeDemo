#ifndef LOCATIONALGORITHM_H
#define LOCATIONALGORITHM_H

#include <QTime>
#include <cstdlib>

class LocationAlgorithm{
public:
    LocationAlgorithm();
    ~LocationAlgorithm();
    static int locationAlgorithm(int value)
    {
        for(int i=0;i<9;i++){
            value = rand()+1000;
        }
        return value;
    }

};


#endif // LOCATIONALGORITHM_H
