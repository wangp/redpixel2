#ifndef __included_svgame_h
#define __included_svgame_h


struct svclient;
struct object;


void svgame_process_cs_gameinfo_packet (struct svclient *c, const char *buf, size_t size);
void svgame_process_cs_ping (struct svclient *c);
void svgame_process_cs_boing (struct svclient *c);


/* begin Lua bindings */
struct object *svgame_spawn_object (const char *typename, float x, float y);
struct object *svgame_spawn_projectile (const char *typename, struct object *owner, float speed, float delta_angle);
void svgame_spawn_blood (float x, float y, long nparticles, float spread);
void svgame_spawn_sparks (float x, float y, long nparticles, float spread);
void svgame_spawn_respawn_particles (float x, float y, long nparticles, float spread);
void svgame_spawn_blod (float x, float y, long nparticles);
void svgame_spawn_explosion (const char *name, float x, float y);
void svgame_spawn_blast (float x, float y, float radius, int damage, int owner);
void svgame_call_method_on_clients (struct object *obj, const char *method, const char *arg);
int svgame_object_would_collide_with_objects (struct object *obj);
int svgame_tell_health (struct object *obj, int health); /* internal */
int svgame_tell_ammo (struct object *obj, int ammo); /* internal */
void svgame_set_score (int client_id, const char *score);
void svgame_play_sound_on_clients (struct object *obj, const char *sound);
/* end Lua bindings */


extern struct server_state_procs *svgame_procs;


#endif
