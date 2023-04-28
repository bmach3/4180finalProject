//=================================================================
// Implementation for DLL module.
//
// Copyright 2021 Georgia Tech.  All rights reserved.
// The materials provided by the instructor in this course are for
// the use of the students currently enrolled in the course.
// Copyrighted course materials may not be further disseminated.
// This file must not be made publicly available anywhere.
//=================================================================
#include <stdlib.h>
#include <stdio.h>
#include "doubly_linked_list.h"
#include "globals.h"
#include "hardware.h"
char buffer2[20];
int highscore;
LLNode* create_llnode(void* data) {
  // Allocates memory on the heap for the Node
  LLNode* newNode = (LLNode*)malloc(sizeof(LLNode));
  pc.printf("Node creation\n");
  // Default values of the Node
  newNode->data = data;
  newNode->prev = NULL;
  newNode->next = NULL;
  return newNode;
}

DLinkedList* create_dlinkedlist(void) {
  DLinkedList* newList = (DLinkedList*)malloc(sizeof(DLinkedList));
  newList->head = NULL;
  newList->tail = NULL;
  newList->size = 0;
  return newList;
}

void insertHead(DLinkedList* dLinkedList, void* data){
  // Calls the create node method that will allocate memory on the heap.
  LLNode* newNode = create_llnode(data);
  // Has only 2 cases of adding to an empty list or a non-empty list
  if (dLinkedList->size == 0) {
    dLinkedList->head = newNode;
    dLinkedList->tail = newNode;
  } else {
    newNode->next = dLinkedList->head;
    dLinkedList->head->prev = newNode;
    dLinkedList->head = newNode;
  }
  dLinkedList->size++;
}

void deleteNode(DLinkedList* dLinkedList, LLNode* Node){
  // Has 2 main cases of larger than 1 or equals to one
  // Note** We dont need to do anything if size is zero, so we just leave it out
  if (dLinkedList->size > 1) {
    // Has inner case of removing from the head
    if (Node == dLinkedList->head) {
      dLinkedList->head = dLinkedList->head->next;
      dLinkedList->head->prev = NULL;
    // Has the case of removing from the tail
    } else if (Node == dLinkedList->tail) {
      dLinkedList->tail = dLinkedList->tail->prev;
      dLinkedList->tail->next = NULL;
    // The case of removing from the middle
    } else {
      Node->next->prev = Node->prev;
      Node->prev->next = Node->next;
    }
    // Decrements the size field of the list and frees memory on the heap afterwards
    dLinkedList->size--;
    free(Node->data);
    free(Node);
  // Case of having only 1 where we will create an empty list afterwards and free memory on the heap
  } else if (dLinkedList->size == 1) {
    dLinkedList->head = NULL;
    dLinkedList->tail = NULL;
    dLinkedList->size--;
    free(Node->data);
    free(Node);
  }
}

void destroyList(DLinkedList* dLinkedList){
  LLNode* Node = dLinkedList->head;
  LLNode* Next;
  while(Node){
    free(Node->data);
    Next = Node->next;
    free(Node);
    Node = Next;
    }
  free(dLinkedList);
}

int getSize(DLinkedList* dLinkedList){
  return dLinkedList->size;
}


void startScreen(void) {
    FILE* fp = fopen("/sd/dir/sdtest.txt", "r"); // open sd file to read previous high scores, if any
    uLCD.color(GREEN);
    uLCD.locate(0,3);
    uLCD.text_height(2);
    uLCD.text_width(2);
    uLCD.printf("W");
    wait(.2);
    uLCD.printf("E");
    wait(.2);
    uLCD.printf("L");
    wait(.2);
    uLCD.printf("C");
    wait(.2);
    uLCD.printf("O");
    wait(.2);
    uLCD.printf("M");
    wait(.2);
    uLCD.printf("E");
    uLCD.text_bold(OFF);
    uLCD.text_height(1);
    uLCD.text_width(1);
    uLCD.printf("\n\n\n");
    uLCD.locate(2,6);
    uLCD.color(BLUE);
    if (fp == NULL) { 
        uLCD.locate(3, 6);
        uLCD.set_font_size(8, 8);
        uLCD.printf("Highscore: ");
        uLCD.text_bold(ON);
        uLCD.color(RED);
        uLCD.printf("XXX");
        uLCD.text_bold(OFF);
    } else {
        while (!feof(fp)) {
            fgets(buffer2, 64, fp);
            highscore = atoi(buffer2);
        }
        uLCD.locate(3,6);
        uLCD.set_font_size(8, 8);
        uLCD.printf("Highscore: ");
        uLCD.color(RED);
        uLCD.text_bold(ON);
        uLCD.printf("%d \n\n\n", highscore);
        uLCD.text_bold(OFF);
        uLCD.locate(1, 8);
        uLCD.printf("TRY TO BEAT IT!\n\n");
        fclose(fp);
    }
}
