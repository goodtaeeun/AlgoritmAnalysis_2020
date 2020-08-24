/* This assignment was conducted on repl.it */ 

/*
Name: Tae Eun Kim
ID: 21400217
Algorithm Analysis HW1
*/

/*
Explanation:
This Priority queue is implemented by heap structure.
I implemented Heap with a structure that has three elements.

First, an array of intergers that holds the key values.
Second, a 2D array that holds the strings for the names.
Last, a interger that keeps track of the size.

I specially implemented swap function to sycronize the movements
between the key index and the name index.
Thus, whenever a certain key value is moved from one place the another, the corresponding name moves to its exact corresponding position.

All the other functions are implemented to support features of the priority queue. The specific details are commented within the code.

*/


#include <stdio.h>
#include <string.h>
#include <stdlib.h>



typedef struct {//structure of our Heap
    int key[31];
    char name[31][21];
    int size;
} Heap;

void swap(Heap* A, int a, int b){ //function to swap elements of Heap
  int temp_key;
  char temp_name[100];
  //copy a to temp
  temp_key = A->key[a];
  strcpy(temp_name,A->name[a]);
  //copy b to a 
  A->key[a] = A->key[b];
  strcpy(A->name[a],A->name[b]);
  //copy temp to b
  A->key[b] = temp_key;
  strcpy(A->name[b],temp_name);

  return;
}

void Max_Heapify(Heap* A, int index){ //function to turn the tree into a max heap from the given index.
  int largest; //to store the index of the largest key value`
  int l = index*2; // left child of the given index
  int r = l+1; // right child of the given index
  
  //compare with the left child
  if ((l <= A->size) && (A->key[l] > A->key[index])){ 
    largest = l;
  }
  else {largest = index;}

  //compare with the right child
  if ((r <= A->size) && (A->key[r] > A->key[largest])) {
    largest = r;
  }
  
  // if given index is in invalid place, swap and move down with it.
  if (largest != index){
    swap(A, index, largest);
    Max_Heapify( A , largest );
  }

  return;
};


void Heap_Extract_Max(Heap* A){ // function to return and delete the biggest element
  
  //check if queue is empty
  if(A->size<1){
    printf("This queue is empty! Try other operations.\n\n");
    return;
  }


  //return value by sending out a message
  printf("[%s, %d] is deleted\n\n.",A->name[1],A->key[1]);
  //delete by sending it to the last node and reducing the heap size
  swap(A, 1, A->size);
  A->size =A->size-1;
  //rearrange due to change
  Max_Heapify(A ,1);
  
  return;
};

void Heap_Increase_Key(Heap* A, int index, int newKey){ //function to increase the key value of the given index and rearrange the tree to make it a max heap.

 //check for invalid inputs
  if(index > A->size || index<1){
    printf("Invalid index. Try again.\n\n");
    return;
  }
  if(newKey>10 || newKey<1){
    printf("Invalid key value! Try again.\n\n");
    return;
  }
  if(newKey < A->key[index]){
    printf("New Key is smaller than the old key. Try again.\n\n");
    return;
  }
    
  
  //increase the key value
  A->key[index]=newKey;
  //push it up to the right position
  while(index > 1 && A->key[index] > A->key[index/2]){
    swap(A, index, index/2);
    index = index/2;
  }

};

void Max_Heap_Insert(Heap* A,int newKey, char* newValue){//function to insert a new element in the heap and rearrange to maintain the max heap property.

  //check for name length
  if(strlen(newValue)>20){
    printf("The name is too long! Try again.\n\n");
    return;
  }
  //check for queue size
  if(A->size==30){
    printf("The queue is full! Try other operations.\n\n");
    return;
  }
  //check for invlid inputs
  if(newKey>10 || newKey<1){
    printf("Invalid key value! Try again.\n\n");
    return;
  }

  //increase the size
  A->size = A->size+1;
  //insert new data
  A->key[A->size] = newKey;
  strcpy(A->name[A->size],newValue);
  //push it up to the right position
  Heap_Increase_Key(A, A->size, newKey);
  printf("New element [%s, %d] is inserted.\n\n",newValue, newKey);
  
  return;
};


int main(void) {
  char buffer[1024]; //for any input string
  char menu; //for the menu input
  char numbuf[1024]; //for number specification
  int key; //for key input

  Heap A; //our Heap;
  A.size=0; //initialize the size

  
  
  while(1){
  
  menu = '\0';
  key=0;
  //The menu
  printf("*********** MENU ***********\n");
  printf("I : Insert new element into queue.\n");
  printf("D : Delete element with largest key from queue.\n");
  printf("R : Retrieve element with largest key from queue.\n");
  printf("K : Increase key of element in queue.\n");
  printf("P : Print out all elements in queue.\n");
  printf("Q : Quit.\n\n");
  printf("Choose menu: ");
  scanf("%s", buffer);

  if(strlen(buffer)>1){ //Checking if the menu was a string
    printf("Invalid menu! Please try again\n\n");
    continue;
  }

  menu = buffer[0];//Take only the first letter of any input string

  if(menu == 'I'){
    //receive input
    printf("Enter name of element: ");
    scanf("%s", buffer);
    printf("Enter key value of element: ");
    scanf("%s", numbuf);
    key=atoi(numbuf);
    
    

    //insert to heap
    Max_Heap_Insert(&A, key, buffer);
    
    continue;
        
  }
  else if(menu == 'D'){
    
    //extract the biggest element
    Heap_Extract_Max(&A);
    continue;

  }
  else if(menu == 'R'){
    //check if queue is empty
    if(A.size==0){
      printf("This queue is empty! Try other operations.\n\n");
      continue;
    }
    //return the biggest element
    printf("[%s, %d]\n\n",A.name[1],A.key[1]);
    continue;
  }
  else if(menu == 'K'){ 
    int index, newKey; //for index input and new key value input
    
    //receive inputs
    printf("Enter index of element: ");
    scanf("%s", numbuf);
    index=atoi(numbuf);   
    printf("Enter new key value: ");
    scanf("%s", numbuf);
    newKey=atoi(numbuf);

    // increase key value of element of the given index
    Heap_Increase_Key(&A, index, newKey);
    printf("\n");
  
    continue;

  }
  else if(menu == 'P'){ 
    //print all the elements

    //check if queue is empty
    if(A.size==0){
      printf("This queue is empty! Try other operations.\n\n");
      continue;
    }


    for(int i=1;i<=A.size;i++){
      printf("[%s, %d] ",A.name[i], A.key[i]);
    }
    printf("\n\n");
    continue;

  }
  else if(menu == 'Q'){
    //quit the program
    printf("Thank you. Bye!");
    break;
  }
  else{
    printf("Invalid menu! Please try again\n\n");
    continue;
  }

  }


  return 0;
}