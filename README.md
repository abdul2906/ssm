## simple status monitor
Customisation:
```c
 int main(void)
 {
     ...
     sprintf(name, " [MEM:%.2f%%] [CPU:%.2f%%] [%s] ", 
            get_mem(), get_cpu(), get_time());
 }
```
Compilation: `cc -o ssm ssm.c -O3 -lX11
`
