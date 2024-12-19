#include <stdio.h>
#include "hashmap.h"

int main()
{
    HashMap *map = createHashMap(10);
    insert(map, "001122", "003d0");
    insert(map, "0x0011220", "0x003d02");
    insert(map, "001122", "003d1");
    insert(map, "0x0011221", "0x003d03");

    printf("enbid: %s\n", get(map, "001122"));
    printf("cellid: %s\n", get(map, "0x0011220")); 
    
    
    freeHashMap(map);
    return 0;
}
