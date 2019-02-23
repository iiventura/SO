#include <stdio.h>

typedef struct {
    char data[4096];
    int key;
} item;

item array[] = {
    {"bill", 3},
    {"neil", 1},
    {"john", 2},
    {"rick", 5},
    {"alex", 1},
};

void sort(item *a, int n) {
    int i, j = 0;
    // int s = 1;
    item* p;

    for(i = 0; i < n; i++) {
        //s = 0; // Quitamos s
        p = a;
        j = n-1;
        //printf("array[%d] = {%i}\n", i, (p+i)->key);
        do {
            if((p+i)->key > (p+j)->key) { // p -> p+i y p+1 -> p+j
                item t = *(p+i);
                *(p+i) = *(p+j);
                *(p+j) = t;
                //s++;
            }
        } while ( --j >= i ); // Se comprueba hasta i,  no hasta 0
    }
}

int main() {
    int i;
    sort(array,5);
    for(i = 0; i < 5; i++)
        printf("array[%d] = {%s, %d}\n",
                i, array[i].data, array[i].key);
    return 0;
}
