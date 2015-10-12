/*
 * sched.c - initializes struct for task 0 anda task 1
 */

#include <sched.h>
#include <mm.h>
#include <io.h>

/**
 * Container for the Task array and 2 additional pages (the first and the last one)
 * to protect against out of bound accesses.
 */
union task_union protected_tasks[NR_TASKS+2]
  __attribute__((__section__(".data.task")));

union task_union *task = &protected_tasks[1]; /* == union task_union task[NR_TASKS] */

//#if 0
struct task_struct *list_head_to_task_struct(struct list_head *l)
{
  return list_entry( l, struct task_struct, list);
}
//#endif

extern struct list_head blocked;

//ENTREGA 2
struct list_head freequeue;
struct list_head readyqueue;

struct task_struct * idle_task;


/* get_DIR - Returns the Page Directory address for task 't' */
page_table_entry * get_DIR (struct task_struct *t) 
{
	return t->dir_pages_baseAddr;
}

/* get_PT - Returns the Page Table address for task 't' */
page_table_entry * get_PT (struct task_struct *t) 
{
	return (page_table_entry *)(((unsigned int)(t->dir_pages_baseAddr->bits.pbase_addr))<<12);
}


int allocate_DIR(struct task_struct *t) 
{
	int pos;

	pos = ((int)t-(int)task)/sizeof(union task_union);

	t->dir_pages_baseAddr = (page_table_entry*) &dir_pages[pos]; 

	return 1;
}

void cpu_idle(void)
{
	__asm__ __volatile__("sti": : :"memory");

	while(1)
	{
	;
	}
}
//ENTREGA 2, inicialitzem la feequeue posant moltes tasques buides
void init_freequeue (void){
    //Inicalitzem una llista buida, usem INIT_LIST_HEAD de list.h per assegurarho
    INIT_LIST_HEAD(&freequeue);

    int i;
    for ( i= 0; i < NR_TASKS; ++i){
        list_add_tail(&task[i].task.list, &freequeue);
    }
}

void init_readyqueue (void){
    INIT_LIST_HEAD(&readyqueue);
}


void init_idle (void)
//PREGUNTAR PERQUE ENCARA NO PILLO MASSA BE COM VA DIAGRAMA PAGINA 48
{
  //1.Obtenir task_union lliure
  //2. PID = 0
  //3. inicialitzar dir_pages_baseAaddr amb un nou directori per a desar l'espai d'adresses (Usar allocate_DIR) TO-DO 
  //4. Inicialitzar context d'execució per al procés, per restaurarlo quan se li doni CPU.
  //5. Definir variable global idle_task (struct task_struct * idle_task)
  //6. Inicialitza la variable global idle_task (per aconseguir rapid la task_struct del procés idle)


  //E1. Desar al stack del procés Idle la adreça del codi que executarà (@ funció cpu_idle)
  //E2. Desar al stack el valor inicial que volem per ebp al acabar el dynamic link (pot ser 0)
  //E3. En un camp de la seva task_struct hem de desar a quina posició del stack hem guardat el valor inicial del registre ebp (posarlo a esp)
  printk("INIT_IDLE\n");
  struct list_head *task_union_idle_hp = list_first(&freequeue); //1
  list_del(task_union_idle_hp); //traiem la llista de la freequeue
  idle_task =list_head_to_task_struct(task_union_idle_hp); //5&6
  idle_task->PID = 0; //2
  //4 ??????
  allocate_DIR(idle_task); //3
  union task_union *ta_un = (union task_union*)idle_task;
  ta_un->stack[KERNEL_STACK_SIZE-1] = (unsigned long)&cpu_idle; //E1
  ta_un->stack[KERNEL_STACK_SIZE-2] = 0; //E2
  idle_task->registre_esp = (int)&(ta_un->stack[KERNEL_STACK_SIZE-2]); //E3
  printk("ENDINIT IDLE\n");
}

void init_task1(void)
{
  //PROCËS D'USUARI
  //Pare de la resta de procesos del sistema el codi es posa a user.c (Perque es d'usuari)
  //1. PID = 1
  //2. Inicialitzar dir_pages_baseAaddr amb un mou directori per... (Igual que el 3. de ^)
  //3. Usar set_user_pages (mm.c) completar la inicialització del espai d'usuari.
  //4.Actualitzar la TSS per fer que apunti al stack de new_task (sistema)
  //5. Marca la pagina de directori actual com a pagina actual de directori al sistema, usant set_cr3 (mm.c)
printk("INIT TASK1\n");
struct list_head *task_union_task1_hp = list_first(&freequeue);
list_del(task_union_task1_hp);
struct task_struct * task1_task = list_head_to_task_struct(task_union_task1_hp);
task1_task -> PID = 1; //1
allocate_DIR(task1_task); //2
set_user_pages(task1_task); //3
//4?????
union task_union *ta_un = (union task_union*)task1_task;

tss.esp0 =(DWord)&(ta_un->stack[KERNEL_STACK_SIZE]); //4????
set_cr3(task1_task->dir_pages_baseAddr); //5
printk("ENDINIT TASK1\n");
}


void init_sched(){

}

struct task_struct* current()
{
  int ret_value;
  
  __asm__ __volatile__(
  	"movl %%esp, %0"
	: "=g" (ret_value)
  );
  return (struct task_struct*)(ret_value&0xfffff000);
}

