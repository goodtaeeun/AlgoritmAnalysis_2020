/*
Name: Tae Eun Kim
ID: 21400217
Algorithm Analysis HW6
*/

/*
Notice for TAs
gcc version:
I did not manage to compile this code with gcc version 6.3.0 or 7.4.0
However, This code worked fine when compiled with gcc version 6.4.0 and 7.5.0
*/


#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>

typedef struct Node{
  char name;
  int index;
  struct Node* next;
} Node;
//data structure to implement nodes in the linked list.

Node* create_node(char name, int index){
  Node* newNode = (Node* )malloc(sizeof(Node));
  newNode->name = name;
  newNode->index = index;
  newNode->next = NULL;

  return newNode;
}
//function to create a new node in adjacency list


int main(){

  int node_count = 0 ; //to count the number of nodes
  char node_name[20]; //array that stores the name of each node 
  int m[20][20]; //adjacency matrix of original graph 
  Node* adj_list[20]; //adjacency list of original graph 
  Node* t_adj_list[20]; //adjacency list of transposed graph

  int i, j, t;
  
  
  char buffer[200];
  FILE* fp = fopen("hw6_data.txt", "r") ;

  if(fp == NULL){
    printf("There is no input file\n");
    return 1;
  }

  fgets(buffer, 200, fp);
  for(i=0; i<strlen(buffer); i++){
    if(isspace(buffer[i]) == 0){
      node_name[node_count++] = buffer[i];
    }
  }//count the number of nodes from the first line
  for (i = 0 ; i < node_count ; i++) { //row number
    fgets(buffer, 200, fp);
    t = 0; //column number
    for (j = 1 ; j < strlen(buffer) ; j ++ ) { //index in the string, not the column number
      if(isspace(buffer[j]) == 0) //if the character is not a white space
        m[i][t++] = buffer[j] -'0';
    }
  }
  fclose(fp) ;
  //reading the file to create the adjacency matrix of the original graph



  for(i=0;i<node_count;i++){
    adj_list[i] = create_node(node_name[i], i);
    t_adj_list[i] = create_node(node_name[i], i);    
  } //initialize adjacecy lists

  Node* temp;

  for(i=0;i<node_count;i++){
    temp = adj_list[i];
    for(j=0; j<node_count; j++){
      if(m[i][j] == 1){
        temp->next = create_node(node_name[j], j);
        temp = temp->next;
      }
    }
  } // make original adjacency list


  printf("Adjacency list of origianl graph\n");
  for(i=0;i<node_count;i++){
    temp = adj_list[i];
    printf("%c-> ", temp->name);
    while(temp->next != NULL){
      temp = temp->next;
      printf("%c ", temp->name);
    }
    printf("\n");
  }
  printf("\n");
  // print adjacency list of origianl graph


  Node* temp_trans[node_count];
  for(i=0; i<node_count; i++)
    temp_trans[i] = t_adj_list[i];
  //to be used as a temporary pointer

  for(i=0;i<node_count;i++){
    temp = adj_list[i];
    while(temp->next != NULL){
      temp = temp->next;
      j = temp->index;
      temp_trans[j]->next = create_node(node_name[i], i);
      temp_trans[j] = temp_trans[j]->next;
    }
  }//transposing a graph



  printf("Adjacency list of transposed graph\n");
  for(i=0;i<node_count;i++){
    temp = t_adj_list[i];
    printf("%c-> ", temp->name);
    while(temp->next != NULL){
      temp = temp->next;
      printf("%c ", temp->name);
    }
    printf("\n");
  }
  printf("\n");
  // print adjacency list of transposed graph

  return 0;
}
 
    