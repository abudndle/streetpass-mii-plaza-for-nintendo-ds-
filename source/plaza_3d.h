#ifndef SPDS_PLAZA_3D_H
#define SPDS_PLAZA_3D_H

/* Nitro Engine / DSMA-backed 3D room renderer.
 * Assets are user-supplied converted DSM/DSA files under:
 *   streetpass-plaza/models/visitor.dsm
 *   streetpass-plaza/models/idle.dsa
 */
int spds_plaza_3d_enter(const char *save_dir);
void spds_plaza_3d_render(void);
void spds_plaza_3d_leave(void);
int spds_plaza_3d_active(void);

#endif
