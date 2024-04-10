#include<iostream>
using namespace std;
int main()
{
    int a,n,p,fact=1;
    
    cin>>n;
     if(n==1) fact=0;
    for(int i=1;i<=n;i++)
    {
        fact=fact*i;
    }
    // cout<<fact<<;
    a=fact+1;
    cout<<a;
    return 0;



}