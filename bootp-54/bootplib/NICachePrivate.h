
/*
 * NICachePrivate.h
 */
PLCacheEntry_t *PLCacheEntry_create(ni_id dir, ni_proplist pl);
void		PLCacheEntry_free(PLCacheEntry_t * ent);

void		PLCache_init(PLCache_t * cache);
void		PLCache_free(PLCache_t * cache);
int		PLCache_count(PLCache_t * c);
boolean_t	PLCache_read(PLCache_t * cache, u_char * filename);
boolean_t	PLCache_write(PLCache_t * cache, u_char * filename);
void		PLCache_add(PLCache_t * cache, PLCacheEntry_t * entry);
void		PLCache_remove(PLCache_t * cache, PLCacheEntry_t * entry);
void		PLCache_set_max(PLCache_t * c, int m);
PLCacheEntry_t *PLCache_lookup_prop(PLCache_t * PLCache, 
				    char * prop, char * value);
PLCacheEntry_t *PLCache_lookup_hw(PLCache_t * PLCache, 
				  u_char hwtype, void * hwaddr, int hwlen,
				  NICacheFunc_t * func, void * arg,
				  struct in_addr * client_ip,
				  boolean_t * has_binding);
PLCacheEntry_t *PLCache_lookup_identifier(PLCache_t * PLCache, 
					  char * idstr, 
					  NICacheFunc_t * func, void * arg,
					  struct in_addr * client_ip,
					  boolean_t * has_binding);
PLCacheEntry_t *PLCache_lookup_ip(PLCache_t * PLCache, struct in_addr iaddr);
void		PLCache_make_head(PLCache_t * cache, PLCacheEntry_t * entry);
void		PLCache_print(PLCache_t * cache);






