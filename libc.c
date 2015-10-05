/*
 * libc.c 
 */

#include <libc.h>

#include <types.h>

int errno;

void perror(){
    write(1, "ERROR: ", 7);
    int e = errno;
    switch (e){
        case 4:
            write(1, "No s ha passat un buffer", 24);
            break;
        case 5:
            write(1, "Tamany negatiu", 14);
            break;
        case 13:
            write(1, "Sense permisos per realitzar la operacio", 40);
            break;
        case 9:
            write(1, "El File descriptor no es 1", 26);
            break;
        default:
            write(1, "Error no contemplat", 19);
            break;
    }
}


void itoa(int a, char *b)
{
  int i, i1;
  char c;
  
  if (a==0) { b[0]='0'; b[1]=0; return ;}
  
  i=0;
  while (a>0)
  {
    b[i]=(a%10)+'0';
    a=a/10;
    i++;
  }
  
  for (i1=0; i1<i/2; i1++)
  {
    c=b[i1];
    b[i1]=b[i-i1-1];
    b[i-i1-1]=c;
  }
  b[i]=0;
}

int strlen(char *a)
{
  int i;
  
  i=0;
  
  while (a[i]!=0) i++;
  
  return i;
}

// Wraper de write, retorna numero de caracters escrits
int write ( int fd, char *buffer, int size){
    int registre_eax;
    __asm__ volatile( "int $0x80"
             :"=a" (registre_eax),
             "+b" (fd),
             "+c" (buffer),
             "+d" (size)
             :"a" (4)
            );
     if (registre_eax <0){
        errno = -registre_eax;
        registre_eax = -1;
        perror();
    }
    else registre_eax = registre_eax;
    return registre_eax;
}

//Wraper del gettime, retorna ticks des del boot del sistema operatiu
int gettime(){
    int registre_eax;
    __asm__ volatile( "int $0x80"
                      :"=a" (registre_eax)
                      :"a" (10)
                    );
    return registre_eax;
}

