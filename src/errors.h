#ifndef _ERRORS_H
#define _ERRORS_H

#define E_CONSTRAINT_VIOLATED -1  /* constraint violated */
#define E_FILE_CHILD          -2  /* can't add a child to a FILE node */
#define E_INVALID_SYNTAX      -3  /* command has invalid syntax */
#define E_CANNOT_PROCEED      -4  /* hit resource limits and can't continue */
#define E_OUT_OF_BOUNDS       -5  /* invalid range */
#define E_NO_ROOT             -6  /* no root node selected */
#define E_NO_DIR              -7  /* no directory node selected */
#define E_CMD_NOT_FOUND       -8  /* command not found */
#define E_DIR_NOT_FOUND       -9  /* directory not found */
#define E_FILE_NOT_FOUND      -10 /* file not found */
#define E_INVALID_TYPE        -11 /* operation on an invalid node type */

#endif /* _ERRORS_H */
