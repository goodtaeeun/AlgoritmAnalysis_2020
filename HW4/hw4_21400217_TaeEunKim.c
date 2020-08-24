/*
Name: Tae Eun Kim
ID: 21400217
Algorithm Analysis HW4
*/

/*
Notice for TAs
Algorithms:
My program works for greedy, DP, and branch and bound.

gcc version:
I did not manage to try compile this code with gcc version 6.3.0 or 7.4.0
However, This code worked fine when compiled with gcc version 6.4.0 and 7.5.0
*/

/*
Explanation:
This program solves knapsack problem in 3 algorithms, greedy, DP, and branch and bound.

Each algorithm is tested with same dataset which is randomly generated at the beginning of the program.
There are 9 cases of input size.

For branch and bound, I implemented it with priority queue implemented with heap.
So I reused the code I wrote with in HW1.

To reduce the running time, I implemented the multiprocessing property.
So I forked 9 child process to run each cases of algorithms.
For the communication between child and paren processes, I used pipe mechanism.

Since multiprocessing was possible, list in the parent process was always unsorted.
So, every case of greedy and BB algorithm includes the sorting time seperately. 
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

int pipes[2];
pid_t child_pid ;
char data[200];
//used for multiprocessing 

int input_size[9] = {10, 100, 500, 1000, 3000, 5000, 7000, 9000, 10000};
//9 cases of input size.

int final_benefit[3][9];
double speed[3][9];
//to store final results
double greedy_benefit[9];//to store final benefit of greedy algorithm. It needs to be in double type.

int weight[10001];
int benefit[10001];
double value[10001];
//to store the randomly generated dataset.


void parent_proc(){//function to receive data from each of the child processes 
  wait(0x0);
  read(pipes[0], data, 200);
  int algo, seq;
  double time, max_ben;
  sscanf(data, "%d %d %lf %lf", &algo, &seq, &time, &max_ben);
  if(algo == 0)
    greedy_benefit[seq] = max_ben;
  else
    final_benefit[algo][seq] = max_ben;
  speed[algo][seq] = time;

  return;
}


void swap(int a, int b){ // to swap 3 arrays when sorting the data with quick_sort()
  int temp;
  double temp_val;
  //swap value,benefit,weight 
  temp_val=value[a];
  value[a] = value[b];
  value[b] = temp_val;

  temp = benefit[a];
  benefit[a] = benefit[b];
  benefit[b] = temp;

  temp = weight[a];
  weight[a] = weight[b];
  weight[b] = temp;

  return;
}

int choose_pivot(double arr[], int left, int right){ // to choose pivot
  double pivot = arr[left]; 
  int low = left + 1;
  int high = right;

  while (low <= high){ 
    while (low <= right && pivot <= arr[low]) 
      low++; 
    while (high >= (left+1)  && pivot >= arr[high])  
      high--; 
    if (low <= high) 
      swap(low, high); 
  }
  swap(left, high); 
  return high; 
}
 
 
void quick_sort(double arr[], int left, int right){ //sort the dataset in descending order
  if (left <= right){
    int pivot = choose_pivot(arr, left, right); 
    quick_sort(arr, left, pivot - 1); 
    quick_sort(arr, pivot + 1, right);
  }
}


void greedy(int seq){
  clock_t start = clock(); // start count the time
  quick_sort(value, 1, input_size[seq]); // sort the list
  int size = input_size[seq];
  int max = size*40;
  int total_w=0;
  double total_b=0;
  //initialize

  for(int i=1; i<=size; i++){
    if(total_w + weight[i] > max){ //if it needs to be split
      total_b += (double)(max-total_w)*value[i];
      break;
    }
    else{ //if we can take it as whole
      total_b += benefit[i];
      total_w += weight[i];
    }
  }

  clock_t end = clock();
  double length = (double)(end-start)*1000 /CLOCKS_PER_SEC;

  sprintf(data, "%d %d %lf %lf", 0, seq, length, total_b);
  write(pipes[1], data, 200);
  // send data to parent

  exit(0x0);

}


void DynamicP(int seq){
  clock_t start = clock();

  int size = input_size[seq];
  int max_weight = size*40; //total weight
  int DP[max_weight+1][2]; //used to calculate Dynamic Programming
  for(int i=0; i<=max_weight; i++){
    DP[i][0] = 0;
  }//initialize the array
  
  int i, k;
  for(int item=1; item<=size; item++){//represents the actual number of the item
    i = item%2;// changes every other case
    k = (item+1)%2; // the remaing index of DP other than i, represents the previously used index 
    DP[0][i] = 0;
    for(int w=1; w<=max_weight; w++){// represents the weight at the moment
      if(weight[item]<=w){ //if we have space
        if(DP[w-weight[item]][k] + benefit[item] > DP[w][k]){ // compare benefit of taking it and not taking it
          DP[w][i] = DP[w-weight[item]][k] + benefit[item]; //take it
         }
         else DP[w][i] = DP[w][k]; // not take it
      }
      else DP[w][i] = DP[w][k]; // can't take it
    }
  }
  clock_t end = clock();
  double length = (double)(end-start)*1000/CLOCKS_PER_SEC;

  sprintf(data, "%d %d %lf %d", 1, seq, length, DP[max_weight][i]);
  write(pipes[1], data, 200);
  // send data to parent

  exit(0x0);
}

typedef struct{
  int b_ben;
  int b_weight;
  int item;
  double bound;
} Node; // structure for each node used in BB algorithm


void swap_node(Node* a, Node* b){ //function to swap elements of Heap
  Node temp = *a;
  *a = *b;
  *b = temp;
  return;
}

void Max_Heapify(Node* A, int index, int size){ //function to turn the tree into a max heap from the given index.
  int largest; //to store the index of the largest key value
  int l = index*2; // left child of the given index
  int r = l+1; // right child of the given index
  
  //compare with the left child
  if ((l <= size) && (A[l].bound > A[index].bound)){ 
    largest = l;
  }
  else {largest = index;}

  //compare with the right child
  if ((r <= size) && (A[r].bound > A[largest].bound)) {
    largest = r;
  }
  
  // if given index is in invalid place, swap and move down with it.
  if (largest != index){
    swap_node(&A[index], &A[largest]);
    Max_Heapify( A, largest, size );
  }

  return;
};


Node Max_Heap_Extract(Node* A, int* size){ // function to return and delete the biggest element 
  if(*size<1){//check if queue is empty
    Node dummy;
    dummy.item = 0;
    return dummy; // to tell that the Queue is empty
  }

  Node front = A[1];
  
  swap_node(&A[1], &A[*size]);
  A[*size] = A[*size-1];
  (*size)--;
  //rearrange due to change
  Max_Heapify(A ,1, *size);
  
  return front;
};

void Max_Heap_Insert(Node* A, Node new_node, int *size){//function to insert a new element in the heap and rearrange to maintain the max heap property.
  //check for queue size
  if(*size==100000){//an arbitrary queue size limit
    return;
  }
  
  *size = *size+1; //increase the size
  
  A[*size] = new_node;
  int index = *size;
  //insert new data
  
  while(index > 1 && A[index].bound > A[index/2].bound){
    swap_node(&A[index], &A[index/2]);
    index = index/2;
  }//push it up to the right position
  
  return;
};


int BB(int seq) {
  clock_t start = clock();
  quick_sort(value, 1, input_size[seq]); //sort the list
  
  int size = input_size[seq];
  int max_weight = size*40;
  int max_benefit = 0;
  int temp_weight = 0;
  int heap_size = 0;
  //initialize

  Node Q[100000]; // array to use as priority queue
  
  Node root; // the root node
  root.b_ben=0;
  root.b_weight=0;
  root.item=1;//indicate what choice should be made on child level
  root.bound=0;
  // initialize root node

  for(int i=root.item; i<=size; i++){
    if(temp_weight + weight[i] <= max_weight){
      temp_weight += weight[i];
      root.bound += benefit[i];
    }
    else{
      double frac;
      frac = max_weight-temp_weight;
      root.bound += frac * value[i];
      break;
    }
  }//calculate bound for the root node

  Max_Heap_Insert(Q, root, &heap_size); //now push the root into the queue
  Node top_node;

  while(1){
    top_node = Max_Heap_Extract(Q, &heap_size);
    if(top_node.item == 0) break;
    if(top_node.item > size+1) continue;

    Node left; // node that choose the item
    left.b_ben = top_node.b_ben + benefit[top_node.item];
    left.b_weight = top_node.b_weight + weight[top_node.item];
    left.item = top_node.item + 1;
    
    if(left.b_weight <= max_weight){
      left.bound = left.b_ben;
      temp_weight = left.b_weight;
      for(int i=left.item; i<=size; i++){
        if(temp_weight + weight[i] <= max_weight){
          left.bound += benefit[i];
          temp_weight += weight[i];
        }
        else{
          double frac;
          frac = max_weight-temp_weight;
          left.bound += frac * value[i];
          break;          
        }
      }
    }//calculate bound for the left node
    else{left.bound = 0;}


    Node right; // node that don't choose the item
    right.b_ben = top_node.b_ben;
    right.b_weight = top_node.b_weight;
    right.item = top_node.item + 1;

    right.bound = right.b_ben;
    temp_weight = right.b_weight;
    for(int i=right.item; i<=size; i++){
      if(temp_weight + weight[i] > max_weight){
        double frac;
        frac = max_weight-temp_weight;
        right.bound += frac * value[i];
        break;
      }
      else{
        right.bound += benefit[i];
        temp_weight += weight[i];
      }
    }//calculate bound for the right node
    
    if(left.b_weight <= max_weight){
      if(left.b_ben > max_benefit)
        max_benefit = left.b_ben;
      if(left.bound > max_benefit)
        Max_Heap_Insert(Q, left, &heap_size);
    }
    if(right.b_weight <= max_weight){
      if(right.bound > max_benefit)
        Max_Heap_Insert(Q, right, &heap_size);
    }// check and insert expandable nodes

    while(Q[heap_size].bound < max_benefit){
      heap_size--;
    }//minimize Q size regularly
  }
  clock_t end = clock();
  double length = (double)(end-start)*1000/CLOCKS_PER_SEC;

  sprintf(data, "%d %d %lf %d", 2, seq, length, max_benefit);
  write(pipes[1], data, 200);

  exit(0x0);
}




int main(void) {  
  srand(time(NULL));
  for(int i=1; i<=10000; i++){
    benefit[i] = rand()%300 +1;
    weight[i] = rand()%100 +1;
    value[i] = (double) benefit[i]/weight[i];
  }// make dataset
  
  if (pipe(pipes) != 0) {
		perror("Error") ;
		exit(1) ;
	} //open pipe


  for(int seq=0; seq<9; seq++){
    child_pid = fork() ;
    if (child_pid == 0) {
      greedy(seq);
    }
  }
  for(int i=0;i<9;i++)
    parent_proc();
//run greedy

  for(int seq=0; seq<9; seq++){
    child_pid = fork() ;
    if (child_pid == 0) {
      DynamicP(seq);
    }
  }
  for(int i=0;i<9;i++)
    parent_proc();
// run dP

  for(int seq=0; seq<9; seq++){
    child_pid = fork() ;
    if (child_pid == 0) {
      BB(seq);
    }
  }
  for(int i=0;i<9;i++)
    parent_proc();
//run BB


  FILE* fp = fopen("output.txt", "w");
  fprintf(fp,"Number of |    Processing time in milliseconds    /    Maximum benefit value    |\n");
  fprintf(fp,"  Items   |        Greedy        |         D. P.         |       B. & B.        |\n");
  fprintf(fp,"          |                      |                       |                      |\n");

  char data[9][200] = {0x0, };
  char buf[60] = {0x0, };
  for(int i=0; i<9;i++){
    for(int j=0; j<3; j++){
      if(j==0)
        sprintf(buf, "  %.3lf / %.3lf  |", speed[j][i], greedy_benefit[i]);
      else 
        sprintf(buf, "  %.3lf / %d  |", speed[j][i], final_benefit[j][i]);
      strcat(data[i], buf);
    }
  }// prepare the data to write on the file
  
  fprintf(fp,"     10   |%s\n", data[0]);
  fprintf(fp,"    100   |%s\n", data[1]);
  fprintf(fp,"    500   |%s\n", data[2]);
  fprintf(fp,"   1000   |%s\n", data[3]);
  fprintf(fp,"   3000   |%s\n", data[4]);
  fprintf(fp,"   5000   |%s\n", data[5]);
  fprintf(fp,"   7000   |%s\n", data[6]);
  fprintf(fp,"   9000   |%s\n", data[7]);
  fprintf(fp,"  10000   |%s\n", data[8]);
// print to output file

  printf("Number of |    Processing time in milliseconds    /    Maximum benefit value    |\n");
  printf("  Items   |        Greedy        |         D. P.         |       B. & B.        |\n");
  printf("          |                      |                       |                      |\n");  
  printf("     10   |%s\n", data[0]);
  printf("    100   |%s\n", data[1]);
  printf("    500   |%s\n", data[2]);
  printf("   1000   |%s\n", data[3]);
  printf("   3000   |%s\n", data[4]);
  printf("   5000   |%s\n", data[5]);
  printf("   7000   |%s\n", data[6]);
  printf("   9000   |%s\n", data[7]);
  printf("  10000   |%s\n", data[8]);
//print to screen




  return 0;
}
