/*
 * sys.c - Syscalls implementation
 */
#include <devices.h>

#include <utils.h>

#include <io.h>

#include <mm.h>

#include <mm_address.h>

#include <sched.h>

#include <system.h>



#define LECTURA 0
#define ESCRIPTURA 1

int check_fd(int fd, int permissions)
{
  if (fd!=1) return -9; /*EBADF*/
  if (permissions!=ESCRIPTURA) return -13; /*EACCES*/
  return 0;
}

int sys_ni_syscall()
{
	return -38; /*ENOSYS*/
}

int sys_getpid()
{
	return current()->PID;
}

int sys_fork()
{
  int PID=-1;

  // creates the child process
  
  return PID;
}

void sys_exit()
{  
}

int sys_write(int fd, char * buffer, int size){
    char entra[1024];
    int escrits = 0;

    if (size < 0) return -5; //IO error
    if (buffer == NULL) return -4; //-4; //Afegir mirar si la posició es valida

    int c = check_fd(fd, ESCRIPTURA);
    if (c != 0) return c;
    while (size > 1024) { 
        copy_from_user(buffer, entra, 1024);
        sys_write_console(entra, 1024);
        buffer = buffer + 1024;
        size = size - 1024;
        escrits = escrits +1024;
    }
    copy_from_user(buffer, entra, size);
    sys_write_console(entra, size);
    escrits = escrits + size;

    return escrits; 
}

int sys_gettime(){
printc('A');
    return zeos_ticks;
}
