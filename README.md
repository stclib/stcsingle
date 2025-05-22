# stcsingle
Single headers for [STC](https://github.com/stclib/STC). For use with [Compiler Explorer](https://godbolt.org/z/bGzzGjh86)

```c++
#define T Vec, float
#include <https://raw.githubusercontent.com/stclib/stcsingle/main/stc/vec.h>
#include <stdio.h>

int main()
{
    Vec vec = {0};
    
    for (c_range(i, 10))
        Vec_push(&vec, i*i);
    
    for (c_each(i, Vec, vec))
        printf("%f\n", *i.ref);

    Vec_drop(&vec);
}
```
