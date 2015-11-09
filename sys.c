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

 #include <entry.h>



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

  //a. obtenir task_struct lliure (si no hi ha -> error)  //fet
  //b. copiar task_union del pare al fill (copy_data) //fet
  //c. inicialitzar dir_pages_baseAddr usant allocate_DIR //fet
  //d. buscar pagines fisiques on posar pagines logiques per data+stack del fill (alloc_frames) (error si no hi ha)
  //e. heredar userdata
      //e1. nou espai d'adreces. accedit a la taula de pagines del fill a través del directori (get_PT)
        //e1.1 entrades taula de pagines del codi sistema i dades i codi usuari copiar pare ->fill
        //e1.2 dades d'usuari i stack han d'apuntar a pagines noves
      //e2. copiar userdata+stack del pare al fill. Com que no es pot anar fent pare-fill pare-fiil fem:
        //e2.1 usar temporalment entrades lliures del pare (set_ss_pag i del_ss_pag)
        //e2.2 copiar dades+stack
        //e2.3 alliberem les entrades temporals a la taula de pagines i fem flush de la TLB per que el pare no pugui accedir al fill
  //f. assignem PID nou al fill. (diferent a la posició de la task array)
  //g. inicialitzar els camps de la task_struct que no son comúns al fill.
      //g1 modifica els continguts dels registres al stack de sistema per que els registres siguin correctes al restaurar l'estat
  //h. preparar la pila del fill amb un contingut que emuli el resultat d'una crida a task_switch
      //h1 restaurar context de hardware
      //h2 continuar la execusió de proc´es d'usuari(crear rutina ret_from_fork que faci eso)
  //inserir el procés a la cua de ready
  //retornar el PID del procés fill
  /////////////////////////////////////////////////////////


  if(list_empty(&freequeue)) return -50; //Sense PCBs lliure

  //Pillem  task_struct lliure
  struct list_head *list_head_fill = list_first(&freequeue); //a
  list_del(list_head_fill); //borrem de lliures

  union task_union * task_union_fill = (union task_union*)list_head_to_task_struct(list_head_fill);

  int registre_ebp = 0;
  unsigned int pos_ebp = 0; //posició ebp

  __asm__ __volatile__( "mov %%ebp, %0;"
                        : "=r" (registre_ebp)
                      );
  pos_ebp =( (unsigned int) registre_ebp - (unsigned int)current());

  //Busquem pagines fisiques per donarli al fill
  //d
 
  int i;

  int frames[NUM_PAG_DATA];
  for (i=0; i<NUM_PAG_DATA; i++){

    int puc_allocar = alloc_frame();

    if (puc_allocar != -1){  //Una pagina fisica !:D
      //set_ss_pag(taula_pagines_fill, PAG_LOG_INIT_DATA + i, puc_allocar);
      frames[i] = puc_allocar;
    }

    else {//no hi ha pagines fisiques disponibles ;_;

      //Anem a desfer el merder que hem fet abans de retornar l'error
      //int j;
      //for (j =0; j<i; j++){
        //free_frame(get_frame(taula_pagines_fill, PAG_LOG_INIT_DATA+j));
        //del_ss_pag(taula_pagines_fill, PAG_LOG_INIT_DATA +j);
      //}
      while ( i != 0){
        free_frame(frames[i-1]);
        --i;
      }
      list_add_tail(list_head_fill, &freequeue);
      return -51; //Sense pagines fisiques disponibles
    }

  }


  copy_data(current(), task_union_fill, sizeof(union task_union)); //b

  allocate_DIR((struct task_struct*) task_union_fill); //c
  page_table_entry *taula_pagines_fill = get_PT(&task_union_fill->task);
  page_table_entry *taula_pagines_pare = get_PT(current()); //pillem la taula de pagines del papi
  page_table_entry *directori_pare = get_DIR(current()); //pillem directori pare

  //Copiem CODE i DATA del pare al fill, usant lo nou que tenim //e1
  //CODI
  for ( i=0; i<NUM_PAG_CODE; i++){
    taula_pagines_fill[PAG_LOG_INIT_CODE+i].entry = taula_pagines_pare[PAG_LOG_INIT_CODE+i].entry;
  }
  //DADES 
  for (i=0; i<NUM_PAG_DATA; i++){
    set_ss_pag(taula_pagines_fill, PAG_LOG_INIT_DATA+i, frames[i]);
  }

  //El userdata i el stack no es poden fer igual, perque no es comparteixen
  //Buscarem pagines lliures al pare per fer el canvi

  int pagina_lliure = NUM_PAG_KERNEL+NUM_PAG_CODE+NUM_PAG_DATA;
  for (i=0; i<NUM_PAG_DATA;i++){
    while(taula_pagines_pare[pagina_lliure].entry != 0 && pagina_lliure<TOTAL_PAGES) pagina_lliure++;

    if(pagina_lliure == TOTAL_PAGES){
      if(i != 0){
        pagina_lliure = NUM_PAG_KERNEL+NUM_PAG_CODE+NUM_PAG_DATA;
        --i;
        set_cr3(directori_pare); //Flush TLB
      }
      else return -52;
    }
    else {
      //les pagines que hem buscat abans, les assignem al fill
      set_ss_pag(taula_pagines_pare, pagina_lliure, taula_pagines_fill[PAG_LOG_INIT_DATA+i].bits.pbase_addr);

      //Copiem dades del usuari al procés fill
      copy_data((void *)((PAG_LOG_INIT_DATA+i)<<12), (void *)(pagina_lliure<<12), PAGE_SIZE);

      //I finalment, les traiem del pare i les donem al fill
      del_ss_pag(taula_pagines_pare, pagina_lliure);
      pagina_lliure++;
    }
  }

  //Hem de flushejar la TLB
  set_cr3(directori_pare);

  //assignem PID
  PID = nouPID();
  list_head_to_task_struct(list_head_fill)->PID = PID; //f

  //Emulem el stack per al fill
  task_union_fill->stack[pos_ebp+8]=0; //PID de retorn del fill, matxaquem el eax perque sigui diferent
  task_union_fill->task.kernel_esp = (unsigned int)&task_union_fill->stack[pos_ebp]; //esp apunta al ebp desat
  task_union_fill->stack[pos_ebp+1] = (unsigned int)&ret_from_fork; //Modifiquem a quina funció retorna

  //posem el fill a ready
  list_add_tail(list_head_fill, &readyqueue);
  printk("FORKED!");
  return PID;
}

void sys_exit()
{  

  page_table_entry *process_PT = get_PT(current());
  int i;
  for (i=0; i<NUM_PAG_DATA; i++)
  {
    free_frame(get_frame(process_PT, PAG_LOG_INIT_DATA+i));
    del_ss_pag(process_PT, PAG_LOG_INIT_DATA+i);
  }
  
  list_add_tail(&(current()->list), &freequeue);
  
  current()->PID=-1;
  
  sched_next_rr();
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
