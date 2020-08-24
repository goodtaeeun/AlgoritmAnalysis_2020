/*
Name: Tae Eun Kim
ID: 21400217
Algorithm Analysis HW7
*/

/*
Notice for TAs
gcc version:
I did not manage to compile this code with gcc version 6.3.0 or 7.4.0
However, This code worked fine when compiled with gcc version 6.4.0 and 7.5.0

output format:
Output for this program requires a wide screen size, due to the table size.
To see the results in the inteded format, it is better to have a wide window size. 
*/


#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#define INF 1000000000


int node_count = 0 ; //to count the number of nodes
char node_name[30][30] ; //array that stores the name of each node 
int m[30][30]; //adjacency matrix of graph 



typedef struct Node{
  int index;
  int length;
  struct Node* next;
  int inS;
  int precede;
} Node;
//data structure to implement nodes in the linked list.
Node* adj_list[30]; //adjacency list of graph

Node* create_node(int index, int length){
  Node* newNode = (Node* )malloc(sizeof(Node));
  newNode->length = length;
  newNode->index = index;
  newNode->next = NULL;
  newNode->inS = 0;
  newNode->precede = -1;

  return newNode;
}
//function to create a new node in adjacency list


void swap_node(Node** a, Node** b){ //function to swap elements of Heap
  Node* temp = *a;
  *a = *b;
  *b = temp;
  return;
}

void Min_Heapify(Node** A, int index, int size){ //function to turn the tree into a min heap from the given index.
  int smallest; //to store the index of the smallest key value
  int l = index*2; // left child of the given index
  int r = l+1; // right child of the given index
  
  //compare with the left child
  if ((l <= size) && (A[l]->length < A[index]->length)){ 
    smallest = l;
  }
  else {smallest = index;}

  //compare with the right child
  if ((r <= size) && (A[r]->length < A[smallest]->length)) {
    smallest = r;
  }
  
  // if given index is in invalid place, swap and move down with it.
  if (smallest != index){
    swap_node(&A[index], &A[smallest]);
    Min_Heapify( A, smallest, size );
  }

  return;
};


Node* Min_Heap_Extract(Node** A, int* size){ // function to return and delete the smallest element 
  if(*size<1){//check if queue is empty
    Node* dummy = create_node(-1, 0);
    return dummy; // to tell that the Queue is empty
  }

  Node* front = A[1];
  
  swap_node(&A[1], &A[*size]);
  (*size)--;

  //rearrange due to change
  Min_Heapify(A ,1, *size);
  
  
  return front;
};

void Min_Heap_Insert(Node** A, Node* new_node, int *size){//function to insert a new element in the heap and rearrange to maintain the min heap property.
  //check for queue size
  if(*size==30){//an arbitrary queue size limit
    return;
  }
  
  *size = *size+1; //increase the size
  
  A[*size] = new_node;
  int index = *size;
  //insert new data
  
  while(index > 1 && A[index]->length < A[index/2]->length){
    swap_node(&A[index], &A[index/2]);
    index = index/2;
  }//push it up to the right position
  
  return;
};



int distance[30][30];
//for Dijkstra's
  
void Dijkstra(int s){
  Node* real[30];
  Node* Q[30];
  int i, j;
  int heap_size = 0;
  
  for(i=0; i<node_count; i++){
    Node* newNode;
    if(i == s)
      newNode = create_node(i, 0);
    else
      newNode = create_node(i, INF);

    real[i] = newNode;
    Min_Heap_Insert(Q, newNode, &heap_size);
  }//initialize


  while(1){
    Node* now = Min_Heap_Extract(Q, &heap_size);
    if(now->index == -1)
      break; // Q is empty
    now->inS = 1;
    //put in S
    Node* t = adj_list[now->index];
    t = t->next;
    while(t != NULL){ // relaxation
      if(now->precede != t->index){
        if(real[t->index]->length > t->length + now->length){
          real[t->index]->length = t->length + now->length;
          real[t->index]->precede = now->index;
        }
      }
      t = t->next;
    }

    Min_Heapify(Q, 1, heap_size); // Heapify due to the distance change
  }

  for(i = 0; i<node_count; i++){
    distance[s][i] = real[i]->length;
  } // recored the results


}


int F[30][30];
int temp[2][30][30];
//for Floyd's
//use only one matrix to reduce space complexity.

void Floyd(){
  int i, j, k;

  for(i=0; i<node_count; i++){
    for(j=0; j<node_count; j++){
      F[i][j] = m[i][j];
    }
  }// initialize

  int now, past;
  for(k=0; k<node_count; k++){
    for(i = 0; i<node_count; i++){
      for(j = 0; j<node_count; j++){
        if(i==j)
          F[i][j] = 0;

        else if(i==k || j == k)
          continue;

        else if(F[i][j] > F[i][k] + F[k][j])
          F[i][j] = F[i][k] + F[k][j];
          
        else
          F[i][j] = F[i][j] ;
      }
    }
  }

  
  return;

}




int main(){

  int i, j, t;
  
  char buffer[200];
  FILE* fp = fopen("hw7_data.txt", "r") ;

  if(fp == NULL){
    printf("There is no input file\n");
    return 1;
  }

  fgets(buffer, 200, fp);
  for(i=0; i<strlen(buffer); i++){
    if(isspace(buffer[i]) == 0){
      j=0;
      while(isspace(buffer[i]) == 0)
        node_name[node_count][j++] = buffer[i++];
      node_name[node_count][j] = 0x0;
      node_count++;
    }
  }//count the number of nodes from the first line


  

  for (i = 0 ; i < node_count ; i++) { //row number
    fgets(buffer, 200, fp);
    t = 0; //column number
    char temp[10];
    int k;
    for (j = strlen(node_name[i]) ; j < strlen(buffer) ; j ++ ) { 
      // j is index in the buffer string, not the column number
      if(isspace(buffer[j]) == 0) { //if the character is not a white space
        if(buffer[j] == 'I'){
          m[i][t++] = INF;
          j+=3;
        }
        else{
          k=0;
          while(isspace(buffer[j]) == 0)
            temp[k++] = buffer[j++];
          temp[k] = 0x0;
          m[i][t++] = atoi(temp);
        }
      }
    }
  }
  fclose(fp) ;
  //read the file to create the adjacency matrix of the original graph
  


  Node* temp;

  for(i=0;i<node_count;i++){
    adj_list[i] = create_node(i, 0);
    temp = adj_list[i];
    for(j=0; j<node_count; j++){
      if((m[i][j] != INF) && (m[i][j] != 0)){
        temp->next = create_node(j, m[i][j]);
        temp = temp->next;
      }
    }
  } // make adjacency list



  clock_t start, end;
  
  start = clock();
  for(i=0; i<node_count; i++){
    Dijkstra(i);
  }
  end = clock();
  double D_length = (double)(end-start)/CLOCKS_PER_SEC;
  //perform and measure Dijkstra's algorithm.

  printf("It took %.8lf seconds to compute shortest path between cities with Dijkstra's algorithm as follows.\n", D_length );
  printf("           ");
  for(i=0; i<node_count; i++)
    printf("%10s ",node_name[i]);
  printf("\n");

  for(i=0;i<node_count;i++){
    printf("%10s ", node_name[i]);
    for(j=0; j<node_count; j++)
      printf("%10d ", distance[i][j]);
    printf("\n");
  }
  printf("\n");
  printf("\n");
  //print Dijkstra's algorithm's result



  start = clock();
  Floyd();
  end = clock();
  double F_length = (double)(end-start)/CLOCKS_PER_SEC;
  //perform and measure Floyd's algorithm.


  printf("It took %.8lf seconds to compute shortest path between cities with Floyd's algorithm as follows.\n", F_length );
  printf("           ");
  for(i=0; i<node_count; i++)
    printf("%10s ",node_name[i]);
  printf("\n");

  for(i=0;i<node_count;i++){
    printf("%10s ", node_name[i]);
    for(j=0; j<node_count; j++)
      printf("%10d ", F[i][j]);
    printf("\n");
  }
  //print Floyd's algorithm's result


  return 0;

}
 
    