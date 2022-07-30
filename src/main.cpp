#include <../headers/blob.hpp>
#include <sys/stat.h>
#include <sys/types.h>

// will move to class later
void init()
{
    mkdir("./.gitAtHome", 0777);
    mkdir("./.gitAtHome/objects", 0777);
}   

int main()
{   
    init();
    Blob blob("test2.txt");
    blob.serialize();
    blob.print();
    return 0;
}