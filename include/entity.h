typedef struct ent_s
{
  char *name;                 // identification string
  int id;                     // identification number

  m_model_t *model;           // pointer to model structure (3rd person, rendering)

  int health;                 // health points
  
  float position[3],          // position point
        strafe[3],            // strafe vector for moving sideways
        view[3],              // direction entity is facing/looking
        move[3],              // move desired, altered by collision
        up[3],                // entity's up vector
        velocity[3],          // final movement speed
        acceleration[3];      // movement speed adjusted by player

  float boundingSphereRadius; // bounding sphere radius for collision

  int special_flags; // PSF_um...
  int action_flags;  // PAF_JUMPING, etc

  int jump_time;     // time jump occured, used for collision and effects

  int groundPlaneIndex; // Is the entity touching a ground plane? (-1 == no)

  struct ent_s *next;
} ent_t;

ent_t *E_createEntity(char *name);
void E_destroyEntity(ent_t **entity);
ent_t *E_findEntityById(int id);
void E_bindModelToEntity(m_model_t *model, ent_t *entity);

void E_destroyAllEntities(void);
void E_strafeEntity(ent_t *entity, float speed);
void E_moveEntity(ent_t *entity, float speed);
void E_moveEntityTo(ent_t *entity, float *position);
void E_rotateEntity(ent_t *entity, float angle, float x, float y, float z);
void E_lookHoriz(ent_t *entity, float speed);
void E_lookVert(ent_t *entity, float speed);
void E_updateEntity(ent_t *entity, int msec);
void E_updateEntityPhysics(ent_t *entity, float time);
void E_positionEntity(ent_t *entity, float px, float py, float pz, 
                                     float vx, float vy, float vz, 
                                     float ux, float uy, float uz);
void E_moveEntityCollisionBSP(ent_t *entity, BSPLevel_t *bsplevel);
void E_moveEntityNewPositionBSP(ent_t *entity, BSPLevel_t *bsplevel);
int E_initPlayerEntity(void);
void E_updatePlayerEntity(ent_t *entity, int msec);
