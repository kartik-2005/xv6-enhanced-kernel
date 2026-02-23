#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(void)
{
char *mem = sbrk(4096);
if(mem == (char*)-1){
printf("sbrk failed\n");
exit(1);
}

*mem = 'A';
char buf[2] = { *mem, '\0' };
printf("Parent: Wrote initial value 'A' at address %p\n", mem);
printf("Parent: Initial value = %s\n", buf);

int pid = fork();

if(pid < 0){
printf("fork failed\n");
exit(1);
}

if(pid == 0){
// Child process
char val_before = *mem;
char cbuf1[2] = { val_before, '\0' };
printf("Child: Reading from shared memory, should see 'A': %s\n", cbuf1);

printf("Child: Attempting to write 'B'...\n");
*mem = 'B';
printf("Child: Wrote 'B' successfully.\n");

char val_after = *mem;
char cbuf2[2] = { val_after, '\0' };
printf("Child: Reading again, should see 'B': %s\n", cbuf2);

exit(0);

} else {
// Parent process
wait(0);
printf("Parent: Child has finished.\n");

char parent_val = *mem;
char pbuf[2] = { parent_val, '\0' };
printf("Parent: Reading from memory, must see 'A': %s\n", pbuf);

if(parent_val == 'A'){
  printf("COW test PASSED!\n");
} else {
  printf("COW test FAILED! Parent saw '%s' instead of 'A'\n", pbuf);
}

}

exit(0);
}
