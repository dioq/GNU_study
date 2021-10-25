#include <linux/audit.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/ptrace.h>
#include <linux/seccomp.h>
#include <linux/signal.h>
#include <linux/perf_event.h>
#include <linux/hw_breakpoint.h>
#include <linux/elf.h>
#include <stdlib.h>
#include <sys/uio.h>

int main(int argc, char** argv)
{
    pid_t child_pid;

    if(argc < 2)
    {
        printf("Expected a program name as argument\n");
        return -1;
    }

    child_pid = fork(); 

    if (child_pid == 0)
        run_target(argv[1]);
    else if (child_pid > 0)
        run_debugger(child_pid);
    else 
    {
        perror("fork");
        return -1;
    }

    return 0;
}


void run_target(const char* programname)
{
    printf("target started. will run '%s'\n", programname);

    /* Allow tracing of this process */
    if (ptrace(PTRACE_TRACEME, 0, 0, 0) < 0)
    {
        perror("ptrace");
        return;
    }

    /* Replace this process's image with the given program */
    execl(programname, programname, 0);
}

void run_debugger(pid_t child_pid)
{
    int wait_status;
    unsigned icounter = 0;
    printf("debugger started\n");

    /* Wait for child to stop on its first instruction */
    wait(&wait_status);

    while (WIFSTOPPED(wait_status)) 
    {
        icounter++;

        struct user_pt_regs regs;
        struct iovec io;
        io.iov_base = &regs;
        io.iov_len = sizeof(regs);


        if (ptrace(PTRACE_GETREGSET, child_pid, (void*)NT_PRSTATUS, (void*)&io) == -1)
            printf("BAD REGISTER REQUEST\n");

        unsigned instr = ptrace(PTRACE_PEEKTEXT, child_pid, regs.pc, 0);

        printf("icounter = %u.  PCP = 0x%08x.  instr = 0x%08x\n", icounter, regs.pc, instr);

        /* Make the child execute another instruction */
        if (ptrace(PTRACE_SINGLESTEP, child_pid, 0, 0) < 0) 
        {
            perror("ptrace");
            return;
        }

        /* Wait for child to stop on its next instruction */
        wait(&wait_status);
    }

    printf("the child executed %u instructions\n", icounter);
}
