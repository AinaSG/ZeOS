#include <libc.h>

void print_stats(int i){
     struct stats st;
     get_stats(getpid(), &st);
     char b;
     
     write(1, "\nStats for the process ", 23);
     itoa(i, &b);
     write(1, &b, strlen(&b));
     
     itoa(st.user_ticks, &b);
     write(1, "\nUser Ticks: ", 13);
     write(1, &b, strlen(&b));
     
     itoa(st.system_ticks, &b);
     write(1, "\nSystem Ticks: ", 15);
     write(1, &b, strlen(&b));
     
     itoa(st.blocked_ticks, &b);
     write(1, "\nBlocked Ticks: ", 16);
     write(1, &b, strlen(&b));
     
     itoa(st.ready_ticks, &b);
     write(1, "\nReady Ticks: ", 14);
     write(1, &b, strlen(&b));
}



void WL1(){
  //write(1, "\n\n\n\n\n", 5);
  print_stats(getpid());
  exit();
}

void WL2(){
  //write(1, "\n\n\n\n\n\n\n\n\n\n", 10);
  print_stats(getpid());
  exit();
}

void WL3(){
  //write(1, "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n", 15);
  print_stats(getpid());
  exit();
}


int __attribute__ ((__section__(".text.main")))
  main(void)
{
    /* Next line, tries to move value 0 to CR3 register. This register is a privileged one, and so it will raise an exception */
     /* __asm__ __volatile__ ("mov %0, %%cr3"::"r" (0) ); */
     
     int i = 0;
     for (i = 0; i<100000; ++i);
     
     int ret_fork;
     
     ret_fork = -1;
     ret_fork = fork();
     if (ret_fork == 0){
       write(1, "fork1!", 6);
       WL1();
     }
     
     ret_fork = -1;
     ret_fork = fork();
     if (ret_fork == 0){
       write(1, "fork2!", 6);
       WL2();
     }
     
     ret_fork = -1;
     ret_fork = fork();
     if (ret_fork == 0){
       write(1, "fork3!", 6);
       WL3();
     }
     
     print_stats(getpid());

  while (1);
}
