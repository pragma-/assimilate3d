typedef struct camera_data
{
  float move[4];
  float position[3];
  float view[3];
  float velocity[3];    
  float acceleration[3];
  float strafe[3];      
  float up[3];     
  ent_t *entity;
} camera_t;

void C_initCamera(void);
void C_bindCameraToEntity(camera_t *camera, ent_t *entity);
void C_moveCameraToEntity(camera_t *camera, ent_t *entity);
void C_positionCamera(camera_t *camera, float px, float py, float pz, 
                                        float vx, float vy, float vz, 
                                        float ux, float uy, float uz);
