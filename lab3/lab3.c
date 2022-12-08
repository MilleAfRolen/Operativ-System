#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef signed char BYTE;

typedef struct node {
    struct node *next;
    struct node *prev;
    int page_num;
    long physical;
} node;

typedef struct tlb{
    struct node *head;
    struct node *tail;
    int size;
} tlb;



tlb tlb_queue = {.head = NULL, .tail = NULL, .size = 0};


void insert(int page, long physicalAddr) {
    node *temp = malloc(sizeof(*temp));
    temp -> page_num = page;
    temp -> physical = physicalAddr;

    if (tlb_queue.size == 0) {
        tlb_queue.head = temp;
        tlb_queue.tail = temp;
        tlb_queue.size = 1;
        return;
    }

    node *head = tlb_queue.head;
    head -> prev = temp;
    temp -> next = head;
    temp -> prev = NULL;
    tlb_queue.head = temp;
    tlb_queue.size++;
}

void pop() {
    node *temp = tlb_queue.tail -> prev;
    temp -> next = NULL;
    //free(tlb_queue.tail);
    tlb_queue.tail = temp;
    tlb_queue.size--;    
}


int main(void) {
    FILE *address_ptr, *backing_ptr, *file_ptr;
    int mask = 0xffff;
    int getOffset = 0x00ff;
    size_t size = 0;
    ssize_t read;
    char *reader = NULL;
    int page_num, page_offset, number;
    char * char_ptr;
    size_t characters;
    int page_table[256];
    int page_ptr = 0;
    long physical;
    int pageCounter = 0;

    int missCounter = 0;
    int hitCounter = 0;
    bool hit = false;


    for(int i = 0; i < 256; i++) {
        page_table[i] = -1;
    }

    address_ptr = fopen("data/addresses.txt", "r");
    if (address_ptr == NULL) {
        return -1;
    }

    backing_ptr = fopen("data/BACKING_STORE.bin", "r");
    if (backing_ptr == NULL) {
        return -1;
    }

    file_ptr = fopen("solution.txt", "w");
    if (file_ptr == NULL) {
        return -1;
    }

    while (read = (getline(&reader, &size, address_ptr)) != -1) {
        hit = false;
        //read page number
        number = strtol(reader, &char_ptr, 10);
        page_offset = number & getOffset;
        page_num = (number & mask) >> 8;
        
        node *current = tlb_queue.head;
        for(int i = 0; i < tlb_queue.size; i++){
            if(current->page_num == page_num){
                physical = current->physical + page_offset;
                hit = true;
                hitCounter++;
                break;  
            }
            current = current->next;
        }
        

        if (!hit) {
            missCounter++;
            if(page_table[page_num] == -1) {
                page_table[page_num] = page_ptr;
                page_ptr += 256;
                pageCounter++;
            }

            physical = page_table[page_num] + page_offset;

            if(tlb_queue.size == 16) {
                pop();
            }
            insert(page_num, page_table[page_num]);
        }

        fseek(backing_ptr, physical, SEEK_SET);

        BYTE buffer;
        size_t bytes_read = 0;
        bytes_read = fread(&buffer, sizeof(BYTE), 1, backing_ptr);

        fprintf(file_ptr ,"Virtual address: %d Physical address: %ld Value: %d\n", 
            number, physical, buffer);
    }

     printf("TLB misses: %d\nTLB hits: %d\nPage Faults: %d\n", missCounter, hitCounter, pageCounter);

    fclose(address_ptr);
    fclose(backing_ptr);

    return 0;
}