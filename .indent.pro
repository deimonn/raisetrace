-linux /* linux kernel coding style */
-nut   /* spaces instead of tabs */
-i4    /* 4-space indentation */
-il0   /* no space before label */

/* if GNU indent is doing something weird, its probably because it thinks a
 * typedef'd type is a value; in that case update the following list with the
 * typedef */

-T FILE

/* properly tagged structs inside function pointers also have this issue, track
 * them here */

-T backtrace_state
