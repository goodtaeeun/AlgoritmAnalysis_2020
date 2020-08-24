#include <stdio.h>
#include <stdlib.h>

long combi_DP(int n, int k){
  long **A;
  A = (long**) malloc ( sizeof(long*) * (n+1) );
  for(int i=0; i<n+1; i++){
      A[i] = (long*) malloc ( sizeof(long) * (k+1) );
  }

  for (int i=0;i<=n;i++){
    for(int j=0; j<=k; j++){
      
      if(j==0 || i==j)
        A[i][j] = 1;
      else if(i==0 && j>0)
        A[i][j]=0;
      else 
        A[i][j] = A[i-1][j-1] + A[i-1][j];
    }
  }

  

  return A[n][k];


}

long combi_Rec(int n, int k){
  if(k==0 || n==k)
    return 1;
  else if(n==0 && k>0)
    return 0;
  else
    return combi_Rec(n-1, k-1) + combi_Rec(n-1, k);
}





int main(void) {

  printf("Enter two integers n and k: ");
  int n, k;
  scanf("%d %d", &n, &k);

  long dp = combi_DP(n, k);
  printf("DP solution: %ld\n", dp);
  long rec = combi_Rec(n, k);
  printf("Recursive solution: %ld", rec );

  //printf("DP solution: %d\nRecursive solution: %d", dp, rec );
  return 0;
}