#include <libc.h>

void write_int(int i){
  char b;
  itoa(i, &b);
  write(1, &b, strlen(&b));
}

void print_stats(int i){
     struct stats st;
     get_stats(getpid(), &st);
     char b;
     
     write(1, "\nStats for the process ", 23);
     write_int(i);
     
     write(1, "\nUser Ticks: ", 13);
     write_int(st.user_ticks);

     write(1, "\nSystem Ticks: ", 15);
     write_int(st.system_ticks);

     write(1, "\nBlocked Ticks: ", 16);
     write_int(st.blocked_ticks);

     write(1, "\nReady Ticks: ", 14);
     write_int(st.ready_ticks);
}



void WL1(){
  //write(1, "\n\n\n\n\n", 5);
  //WL1 nomes fa un loop normal no bloquejant
  int i = 0;
  for (i = 0; i<1000000000; ++i);
  //print_stats(getpid());
  exit();
}

void WL2(){
  //write(1, "\n\n\n\n\n\n\n\n\n\n", 10);
  //WL2 fa un bloqueig relativament gran
  char *inutilbuf;
  read(0, &inutilbuf, 100000);
  print_stats(getpid());
  exit();
}

void WL3(){
  //write(1, "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n", 15);
  //WL3 fa molts bloquejos petits
  print_stats(getpid());
  char *inutilbuf;
  int i = 0;
  for (i = 0; i<1000; ++i){
   read(0, &inutilbuf, 100);
  }
  print_stats(getpid());
  exit();
}


int __attribute__ ((__section__(".text.main")))
  main(void)
{
  
  set_sched_policy(1);
     //int i = 0;
     //for (i = 0; i<100000; ++i);
     
     int ret_fork;
     
     ret_fork = -1;
     ret_fork = fork();
     
     //write(1, "\n1: ", 4);
     //write_int(ret_fork);
     
     if (ret_fork == 0){
       write(1, "fork1!", 6);
       WL1();
     }
     
     ret_fork = -1;
     ret_fork = fork();
     
     //write(1, "\n2: ", 4);
     //write_int(ret_fork);
     
     if (ret_fork == 0){
       write(1, "fork2!", 6);
       WL2();
     }
     
     ret_fork = -1;
     ret_fork = fork();
     
     //write(1, "\n3: ", 4);
     //write_int(ret_fork);
     
     
     if (ret_fork == 0){
       write(1, "fork3!", 6);
       WL3();
     }
     //char *inutilbuf;
     //read(0, &inutilbuf, 1000);
     //print_stats(getpid());
     
     while (1);
}
