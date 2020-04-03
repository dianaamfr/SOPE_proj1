
`int currentDirSize(int flags_B, int flags_b, struct stat * stat_buf);`
flags_B is inutile 

doubt: sprintf and other string functions really need malloc when dealing only with temporary variables??
sometimes you make malloc, other not...

there is still a TODO in searchFiles

is this

#include <sys/stat.h>
#include "types.h"

needed in simpledu.h??

doxygen author group

makefile really needs -lm ? we could copy libm.a to a lib folder... question the professor

Check output format when: simpledu ./ 

Check all includes if needed

Check for untraceable printfs

ERROR and ERRORARGS same thing ... could be only one