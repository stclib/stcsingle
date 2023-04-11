# stcsingle
Single headers for STC. Useful for inclusion with Compiler Explorer https://godbolt.org

E.g:
```c
#define i_type Vec
#define i_val float
#include <https://raw.githubusercontent.com/stclib/stcsingle/main/stc/cvec.h>
#include <https://raw.githubusercontent.com/stclib/stcsingle/main/c11/print.h>

int main()
{
    Vec vec = {0};
    c_forrange (i, 10) Vec_push(&vec, i*i);
    c_foreach (i, Vec, vec) print("{} ", *i.ref);
}
```
