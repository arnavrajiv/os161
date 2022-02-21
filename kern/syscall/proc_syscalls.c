#include <types.h>
#include <kern/errno.h>
#include <kern/unistd.h>
#include <kern/wait.h>
#include <lib.h>
#include <syscall.h>
#include <current.h>
#include <proc.h>
#include <thread.h>
#include <addrspace.h>
#include <copyinout.h>
#include <opt-A1.h>
#include <clock.h>
#include <mips/trapframe.h>
#include <kern/syscall.h>

  /* this implementation of sys__exit does not do anything with the exit code */
  /* this needs to be fixed to get exit() and waitpid() working properly */

void sys__exit(int exitcode) {

  struct addrspace *as;
  struct proc *p = curproc;
  /* for now, just include this to keep the compiler from complaining about
     an unused variable */
  (void)exitcode;

  DEBUG(DB_SYSCALL,"Syscall: _exit(%d)\n",exitcode);

  KASSERT(curproc->p_addrspace != NULL);
  as_deactivate();
  /*
   * clear p_addrspace before calling as_destroy. Otherwise if
   * as_destroy sleeps (which is quite possible) when we
   * come back we'll be calling as_activate on a
   * half-destroyed address space. This tends to be
   * messily fatal.
   */
  as = curproc_setas(NULL);
  as_destroy(as);

#if OPT_A1
  int i;
  for(i = array_num(p->p_children) - 1; i >= 0; i--) {
    struct proc *temp_child = array_get(p->p_children, i);
    array_remove(p->p_children, i);
    spinlock_acquire(&temp_child->p_lock);
    if(temp_child->p_exitstatus == 0) 
    {
      spinlock_release(&temp_child->p_lock);
      proc_destroy(temp_child);  
    }
    else 
    {
      temp_child->p_parent = NULL;
      spinlock_release(&temp_child->p_lock);
      
    }
  }
#endif

  /* detach this thread from its process */
  /* note: curproc cannot be used after this call */
  proc_remthread(curthread);

  /* if this is the last user process in the system, proc_destroy()
     will wake up the kernel menu thread */
#if OPT_A1
  spinlock_acquire(&p->p_lock);
  if(p->p_parent == NULL) {
    spinlock_release(&p->p_lock);
    proc_destroy(p);
  }
  else {
    p->p_exitstatus = 0;
    p->p_exitcode = exitcode;
    spinlock_release(&p->p_lock);
  }
#else
  (void)exitcode;
  proc_destroy(p);
#endif

  
  thread_exit();
  /* thread_exit() does not return, so we should never get here */
  panic("return from thread_exit in sys_exit\n");
}


/* stub handler for getpid() system call                */
int
sys_getpid(pid_t *retval)
{
  /* for now, this is just a stub that always returns a PID of 1 */
  /* you need to fix this to make it work properly */
  #if OPT_A1
    *retval = curproc->p_pid;
  #else
    *retval = 1;
  #endif
  return(0);
}

#if OPT_A1
  int
  sys_fork(struct trapframe *tf, pid_t *retval) {
    struct proc *childproc = proc_create_runprogram("child");

    childproc->p_parent = curproc;
    array_add(curproc->p_children, childproc, NULL);

    struct addrspace *childaddrspace;
    int as_copy_result = as_copy(curproc_getas(), &(childaddrspace));
    if (as_copy_result) {}
    childproc->p_addrspace = childaddrspace;

    struct trapframe *child_tf = kmalloc(sizeof(struct trapframe));
    memcpy(child_tf, tf, sizeof(struct trapframe));

    int thread_fork_result = thread_fork("child_thread", childproc, enter_forked_process, (void *)child_tf, 0);
    if(thread_fork_result) {}

    *retval = childproc->p_pid;

    clocksleep(1);
    return 0;
  }
#endif

/* stub handler for waitpid() system call                */

int
sys_waitpid(pid_t pid,
	    userptr_t status,
	    int options,
	    pid_t *retval)
{
  int exitstatus;
  int result;

  /* this is just a stub implementation that always reports an
     exit status of 0, regardless of the actual exit status of
     the specified process.   
     In fact, this will return 0 even if the specified process
     is still running, and even if it never existed in the first place.

     Fix this!
  */

  if (options != 0) {
    return(EINVAL);
  }
#if OPT_A1
  int i;
  int flag = 0;
  struct proc *temp_child = NULL;
  for(i = array_num(curproc->p_children) - 1; i >= 0; i--) {
    struct proc *child = array_get(curproc->p_children, i);
    if(pid == child->p_pid) {
      temp_child = array_get(curproc->p_children, i);
      flag = 1;
      array_remove(curproc->p_children, i);
      break;
    }
  }
  if(!flag) {
    *retval = -1;
    return(ESRCH);
  }
  
  spinlock_acquire(&temp_child->p_lock);
  while(temp_child->p_exitstatus != 0) {
    spinlock_release(&temp_child->p_lock);
    clocksleep(1);
    spinlock_acquire(&temp_child->p_lock);
  }
  spinlock_release(&temp_child->p_lock);
  exitstatus = _MKWAIT_EXIT(temp_child->p_exitcode);
  proc_destroy(temp_child);
#else
  exitstatus = 0;
#endif
  /* for now, just pretend the exitstatus is 0 */
  
  result = copyout((void *)&exitstatus,status,sizeof(int));
  if (result) {
    return(result);
  }
  *retval = pid;
  return(0);
}
