#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../headers/dumalloc.h"

#define HEAP_SIZE (128*8)
#define MANAGED_LIST_SIZE (HEAP_SIZE / 8)

void duInitMalloc(int searchType);
void* duMalloc(int count, int whichHeap);
void duFree(void* ptr);

unsigned char heap[3][HEAP_SIZE];
void* managedList[MANAGED_LIST_SIZE];
int managedListSize;
int fitType;
int currentHeap;

typedef struct memoryBlockHeader {
    int free;   
    int size;
    int managedIndex; 
    int survivalAmt; // the number of times the block has moved between young heaps
    struct memoryBlockHeader* next;
} memoryBlockHeader;

memoryBlockHeader* freeList[3];

void duInitMalloc(int searchType){
    
    currentHeap = 0;
    
    for(int j = 0; j < 3; j++){
        for(int i = 0; i < HEAP_SIZE; i++){
            heap[j][i] = 0;
        }
    }

    memoryBlockHeader* currentBlock = (memoryBlockHeader*)heap[currentHeap];
    currentBlock -> size = HEAP_SIZE-sizeof(memoryBlockHeader);
    currentBlock -> next = NULL;
    currentBlock -> free = 1;

    memoryBlockHeader* currentBlock2 = (memoryBlockHeader*)heap[currentHeap+1];
    currentBlock2 -> size = HEAP_SIZE-sizeof(memoryBlockHeader);
    currentBlock2 -> next = NULL;
    currentBlock2 -> free = 1;

    memoryBlockHeader* currentBlock3 = (memoryBlockHeader*)heap[currentHeap+2];
    currentBlock3 -> size = HEAP_SIZE-sizeof(memoryBlockHeader);
    currentBlock3 -> next = NULL;
    currentBlock3 -> free = 1;

    freeList[currentHeap] = currentBlock;
    freeList[currentHeap+1] = currentBlock2;
    freeList[currentHeap+2] = currentBlock3;

    fitType = searchType;
}

void duManagedInitMalloc(int searchType){
    duInitMalloc(searchType);
    managedListSize = 0;

    for(int i = 0; i< managedListSize; i++){
        managedList[i] = NULL;
    }

}

void memoryBlockDump(int printHeap){

    printf("Memory Block\n");

    char stringMemoryRep[129];
    char* stringMemoryRepP = stringMemoryRep;

    int freeChar = 97;
    int usedChar = 65;

    int currentChar = 97;

    memoryBlockHeader* allMemory = (memoryBlockHeader*)heap[printHeap];

    while((char *)allMemory < (char *)((memoryBlockHeader*)heap[printHeap])+1024){
        
        if(allMemory -> free == 1){
            printf("Free at %p, size %d, surv: %d\n",allMemory, allMemory -> size, allMemory -> survivalAmt);
            currentChar = freeChar;
            freeChar++;
        }else if(allMemory -> free == 0){
            printf("Used at %p, size %d, surv: %d\n",allMemory, allMemory -> size, allMemory -> survivalAmt);
            currentChar = usedChar;
            usedChar++;
        }

        for(int i = 0; i < ((allMemory -> size + sizeof(memoryBlockHeader))/ 8); i++){
            *stringMemoryRepP = (char)currentChar;
            stringMemoryRepP++;
        }
        
        int size = allMemory -> size;
        char* ptr = (char *)allMemory;
        ptr += size + sizeof(memoryBlockHeader);
        allMemory = (memoryBlockHeader*)ptr;
        
    }
    stringMemoryRep[128] = '\0';
    printf("%s\n",stringMemoryRep);
}


void freeListDump(int printHeap){
    printf("Free List\n");
    
    memoryBlockHeader* temp2 = freeList[printHeap];

    while(temp2 != NULL){
        printf("Block at %p, size %d\n",temp2, temp2 -> size);
        temp2 = temp2 -> next;
    }
}

void managedListDump(){
    printf("Managed List\n");

    for(int i = 0; i < managedListSize; i++){
        if(managedList[i] == NULL)
            printf("Managed List[%d] = (nil)\n", i);
        else
            printf("Managed List[%d] = %p\n", i, managedList[i]);
    }
}

void duMemoryDump(){
    printf("MEMORY DUMP\n");
    printf("Current heap (0/1 young): %d\n", currentHeap);
    printf("Young Heap (only the current one)\n");
    memoryBlockDump(currentHeap);
    freeListDump(currentHeap);
    printf("Old Heap\n");
    memoryBlockDump(2);
    freeListDump(2);
    managedListDump();
}

void* duMalloc(int count,int whichHeap){
    int totalBytes  = 0;
    if(count % 8 != 0){
        totalBytes = (((count/8)+1)*8);
    }else{
        totalBytes = count;
    }

    memoryBlockHeader* bestBlock = NULL;
    memoryBlockHeader* bestBlockPrev = NULL;
    memoryBlockHeader* freeBlock = freeList[whichHeap];
    memoryBlockHeader* prevBlock = NULL;

    if (fitType == FIRST_FIT) {
        while (freeBlock != NULL) {
            if (freeBlock->size >= totalBytes) {
                bestBlock = freeBlock;
                bestBlockPrev = prevBlock;
                break;
            }
            prevBlock = freeBlock;
            freeBlock = freeBlock -> next;
        }
    } else if (fitType == BEST_FIT) {
        while (freeBlock != NULL) {
            if (freeBlock->size >= totalBytes && (bestBlock == NULL || freeBlock->size < bestBlock->size)) {
                bestBlock = freeBlock;
                bestBlockPrev = prevBlock;
            }
            prevBlock = freeBlock;
            freeBlock = freeBlock -> next;
        }
    }

    if (bestBlock == NULL) {
        return NULL; 
    }

    int size = bestBlock -> size;
    bestBlock -> size = totalBytes;
    bestBlock -> free = 0;
    bestBlock -> survivalAmt = 0;

    if (size > totalBytes + sizeof(memoryBlockHeader)) {
        memoryBlockHeader* newBlock = (memoryBlockHeader*)((unsigned char*)bestBlock + sizeof(memoryBlockHeader) + totalBytes);
        newBlock -> size = size - totalBytes - sizeof(memoryBlockHeader);
        newBlock -> next = bestBlock -> next;
        newBlock -> free = 1;

        bestBlock -> next = newBlock;
    }

    if (bestBlockPrev == NULL) {
        freeList[whichHeap] = bestBlock -> next;
    } else {
        bestBlockPrev -> next = bestBlock -> next;
    }

    return (void*)(bestBlock + 1);
}

void** duManagedMalloc(int size){
    void* location = duMalloc(size,currentHeap);
    if(location != NULL){
        managedList[managedListSize] = location;
        memoryBlockHeader* block = (memoryBlockHeader*)location-1;
        block -> managedIndex = managedListSize;
        managedListSize++;
        return (managedList+(managedListSize-1));    
    }else{
        return (managedList+(managedListSize));  
    }

    
    
}

void duFree(void* ptr){

    int whatHeap;

    if((char*)ptr >= (char *)((memoryBlockHeader*)heap[2]) && ((char*)ptr < (char *)((memoryBlockHeader*)heap[2])+1024)){
        whatHeap = 2;
    }else{
        whatHeap = currentHeap;
    }

    memoryBlockHeader* header = (memoryBlockHeader*)ptr-1;
    header -> free = 1;
    header -> survivalAmt = 0;

    memoryBlockHeader* freeBlock = freeList[whatHeap];
    memoryBlockHeader* prevBlock = NULL;


    while(freeBlock != NULL && freeBlock < header){
        prevBlock = freeBlock;
        freeBlock = freeBlock -> next;
    }

    if(prevBlock == NULL){
        header -> next = freeList[whatHeap];
        freeList[whatHeap] = header;
    } else{
        prevBlock -> next = header;
        header -> next = freeBlock;
    }
}

void duManagedFree(void** mptr){
    duFree(*mptr);
    memoryBlockHeader* header = (memoryBlockHeader*)(*mptr)-1;
    managedList[header -> managedIndex] = NULL;
}


void minorCollection(){
    int nextHeap = (currentHeap + 1) % 2;
    memoryBlockHeader* newLocation = (memoryBlockHeader*)heap[nextHeap];
    memoryBlockHeader* prev = NULL;
    int totalSize = 1000;
    for(int i = 0; i < managedListSize; i++){
        if(managedList[i] != NULL){

            memoryBlockHeader* current = (memoryBlockHeader*)managedList[i]-1;
            current -> survivalAmt++;

            if(current -> survivalAmt < 3){
                if(prev != NULL){
                    newLocation = (memoryBlockHeader*)((char*)prev + prev->size + sizeof(memoryBlockHeader));
                    prev -> next = newLocation;                    
                }
                memcpy(newLocation, managedList[i] - sizeof(memoryBlockHeader), sizeof(memoryBlockHeader));
                memcpy((void*)(newLocation + 1), managedList[i], newLocation->size);
                prev = newLocation;
                managedList[i] = (void*)(newLocation + 1);   
                totalSize -= current -> size + 24;
            }else{
                memoryBlockHeader* tempDUMalloc = (memoryBlockHeader*)duMalloc(current -> size,2)-1;
                memcpy(tempDUMalloc, managedList[i] - sizeof(memoryBlockHeader), sizeof(memoryBlockHeader));
                memcpy((void*)(tempDUMalloc+1), managedList[i], current->size);
                managedList[i] = (void*)(tempDUMalloc + 1); //May need adjusting
            }
            
            
        }
    }

    if (prev != NULL) {
        prev->next = NULL;
    }

    memoryBlockHeader* lastBlock = (memoryBlockHeader*)heap[nextHeap];
    while(lastBlock -> next != NULL){
        lastBlock = lastBlock->next;
        printf("%d\n",lastBlock -> size);
    }
    memoryBlockHeader* temp = (memoryBlockHeader*)((char*)lastBlock + sizeof(memoryBlockHeader) + lastBlock->size);
    temp -> free = 1;
    temp -> size = totalSize;
    temp -> next = NULL;
    temp -> survivalAmt = 0;

    freeList[nextHeap] = temp;
    lastBlock -> next = temp;   
    currentHeap = nextHeap;
}

void majorCollection(){
    memoryBlockHeader* allMemory = (memoryBlockHeader*)heap[2];

    while((char *)allMemory < (char *)((memoryBlockHeader*)heap[2])+1024){

        if(allMemory -> free == 1){
            int size = allMemory -> size;
            char* ptr = (char *)allMemory;
            ptr += size + sizeof(memoryBlockHeader);
            memoryBlockHeader* nextBlock = (memoryBlockHeader*)ptr;

            if(nextBlock -> free == 1){
                allMemory -> size += sizeof(memoryBlockHeader) + nextBlock -> size;
                if(nextBlock -> next != NULL){
                    allMemory -> next = allMemory -> next -> next;
                    managedList[allMemory -> managedIndex] = allMemory;
                }else{
                    freeList[2] = allMemory;
                }

            } else if(nextBlock -> free == 0){
                
                managedList[allMemory -> managedIndex] = NULL;
                memcpy(allMemory-1, nextBlock-1, sizeof(memoryBlockHeader));
                memcpy(allMemory, nextBlock, nextBlock->size);

                int thisSize = allMemory -> size;
                char* ptr = (char *)allMemory;
                ptr += thisSize + sizeof(memoryBlockHeader);
                nextBlock = (memoryBlockHeader*)ptr;

                nextBlock -> free = 1;
                nextBlock -> size = size;
                nextBlock -> survivalAmt = 0;

                managedList[0] = NULL;

                int otherSize = allMemory -> size;
                char* ptr2 = (char *)allMemory;
                ptr2 += otherSize + sizeof(memoryBlockHeader);
                allMemory = (memoryBlockHeader*)ptr2;

                
            }
        }else{

            int size = allMemory -> size;
            char* ptr = (char *)allMemory;
            ptr += size + sizeof(memoryBlockHeader);
            allMemory = (memoryBlockHeader*)ptr;
        }
        
    }

    
}