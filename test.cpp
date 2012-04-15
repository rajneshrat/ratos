#include <iostream>
using namespace std;

struct test
{
   char a1:1;
   char a2:1;
   char a3:1;
   char a4:1;
   char a5:1;
   char a6:1;
   char a7:1;
   char a8:1;
}var[10];

int main()
{
   int i;
   var[0].a1 = 1;
   var[0].a2 = 1;
   var[0].a3 = 1;
   var[0].a4 = 1;
   var[0].a5 = 0;
   var[0].a6 = 1;
   var[0].a7 = 1;
   var[0].a8 = 1;
   unsigned char *ab = (unsigned char *)&var[0];
   unsigned char a = *ab;
   cout<<"Size is = "<<sizeof(a);
   return 0;
   if(a^0xff)
   {
      for(i=0;i<8;i++)
      {
          if(a&0x01)
          cout<<"Set for "<<i<<"\n";
          cout<<"a = "<<(int)a<<"\n";
          a = a/2;
      }
   }
   a = 4;
 //  int b = (*a)&0xff;
//&0xff;	
   //cout << (int)var[0].a4;
}
