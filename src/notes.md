
<strike> 
`int currentDirSize(int flags_B, int flags_b, struct stat * stat_buf);`
flags_B is inutile  
</strike>

doubt: sprintf and other string functions really need malloc when dealing only with temporary variables??
sometimes you make malloc, other not...

<strike>
there is still a TODO in searchFiles
</strike>

<strike>
is this
```
#include <sys/stat.h>
#include "types.h"
``` 

needed in simpledu.h??
</strike> 

doxygen author group

makefile really needs -lm ? we could copy libm.a to a lib folder... question the professor

Check output format when: simpledu / 
Check output on linux init folder /

Check all includes if needed

Check for untraceable printfs

ERROR and ERRORARGS same thing ... could be only one