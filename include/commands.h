/** These are in command.c **/

COMMAND(CMD_echo);       COMMAND(CMD_qui);
COMMAND(CMD_quit);       COMMAND(CMD_help);
COMMAND(CMD_gl_info);    COMMAND(CMD_load);
COMMAND(CMD_list);       COMMAND(CMD_gamemenu);

/** These are in cvar.c **/

COMMAND(CMD_set);     COMMAND(CMD_alias);
COMMAND(CMD_unset);   COMMAND(CMD_unalias);
COMMAND(CMD_cvars);   COMMAND(CMD_aliaslist);

/** These are in entity.c **/

COMMAND(CMD_listEntities);
COMMAND(CMD_setEntityModel);
COMMAND(CMD_loadEntity);
COMMAND(CMD_positionEntity);
COMMAND(CMD_selectEntity);

/** These are in input.c **/

COMMAND(CMD_keynames);
COMMAND(CMD_bind);
COMMAND(CMD_unbind);
COMMAND(CMD_bindlist);

/** These are in config.c **/

COMMAND(CMD_exec);

/** These are in camera.c **/

COMMAND(CMD_camera);

/** These are in bsp.c **/

COMMAND(CMD_map);

/** These are in lights.c **/

COMMAND(CMD_addLight);
COMMAND(CMD_positionLight);
COMMAND(CMD_setLightProperty);

/** In server.c **/

COMMAND(CMD_sv_start);
COMMAND(CMD_sv_shutdown);

/** In client.c **/

COMMAND(CMD_connect);
COMMAND(CMD_disconnect);
COMMAND(CMD_say);

/** In console.c **/

COMMAND(CMD_clear);

/** In skybox.c **/

COMMAND(CMD_skybox);
