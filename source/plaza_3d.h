#ifndef SPDS_PLAZA_3D_H
#define SPDS_PLAZA_3D_H

/* Nitro Engine uses DSMA for skeletal DSM/DSA model animation.
 * Assets are supplied by the user under:
 *   streetpass-plaza/models/visitor.dsm
 *   streetpass-plaza/models/idle.dsa
 */
int spds_plaza_3d_enter(const char *save_dir);
void spds_plaza_3d_render(void);
void spds_plaza_3d_leave(void);
int spds_plaza_3d_active(void);

#endif
