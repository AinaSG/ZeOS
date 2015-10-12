#include <libc.h>

char buff[24];

int pid;

void clear(){
	int i, j;
	write(1, "\n", 1);
	for (i=0; i<33; ++i){
		for(j=0; j<80; ++j){
			write(1, " ", 1);
		}
	}
}

int add(int par1, int par2){
	int ret;
	__asm__("add %%ebx, %%eax"
		: "=a" (ret)
		: "a" (par1), "b" (par2)
		);
	return ret;
}
long inner (long n)
{
	int i;
	long suma;
	suma = 0;
	for (i=0; i<n; ++i) suma = suma +i;
	return suma;
}

long outer (long n)
{
	int i;
	long acum;
	acum = 0;
	for (i=0; i<n; i++) acum = acum + inner(i);
	return acum;
}
 
int __attribute__ ((__section__(".text.main")))
  main(void)
{
    /* Next line, tries to move value 0 to CR3 register. This register is a privileged one, and so it will raise an exception */
     /* __asm__ __volatile__ ("mov %0, %%cr3"::"r" (0) ); */
	int t = 0;
	char ts[11];

   	long count, acum;
	count = 75;
	acum = 0;
 	acum = outer(count);
	write(1, "Pepe ", 5);
	fork();

	while(1){}
	return 0;
}
