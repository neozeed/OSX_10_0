/*
 * Copyright (c) 2000 Apple Computer, Inc. All rights reserved.
 *
 * @APPLE_LICENSE_HEADER_START@
 * 
 * The contents of this file constitute Original Code as defined in and
 * are subject to the Apple Public Source License Version 1.1 (the
 * "License").  You may not use this file except in compliance with the
 * License.  Please obtain a copy of the License at
 * http://www.apple.com/publicsource and read it before using this file.
 * 
 * This Original Code and all software distributed under the License are
 * distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY KIND, EITHER
 * EXPRESS OR IMPLIED, AND APPLE HEREBY DISCLAIMS ALL SUCH WARRANTIES,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE OR NON-INFRINGEMENT.  Please see the
 * License for the specific language governing rights and limitations
 * under the License.
 * 
 * @APPLE_LICENSE_HEADER_END@
 */
/*
 * Copyright (c) 1999 Apple Computer, Inc.  All rights reserved. 
 *
 * HISTORY
 *
 * 1999 Mar 29 rsulack created.
 */

#include <mach/mach_types.h>
#include <mach/vm_types.h>
#include <mach/kern_return.h>
#include <kern/kern_types.h>
#include <vm/vm_kern.h>
#include <kern/thread.h>

#include <mach_host.h>

kmod_info_t *kmod = 0;
static int kmod_index = 1;

decl_simple_lock_data(,kmod_lock)
decl_simple_lock_data(,kmod_queue_lock)

typedef struct cmd_queue_entry {
	queue_chain_t	links;
	vm_address_t 	data;
	vm_size_t	size;
} cmd_queue_entry_t;

queue_head_t	kmod_cmd_queue;

void
kmod_init()
{
	simple_lock_init(&kmod_lock, ETAP_MISC_Q);
	simple_lock_init(&kmod_queue_lock, ETAP_MISC_Q);
	queue_init(&kmod_cmd_queue);
}

kmod_info_t *
kmod_lookupbyid(kmod_t id)
{
	kmod_info_t *k = 0;

	k = kmod;
	while (k) {
		if (k->id == id) break;
		k = k->next;
	}

	return k;
}

kmod_info_t *
kmod_lookupbyname(char * name)
{
	kmod_info_t *k = 0;

	k = kmod;
	while (k) {
		if (!strcmp(k->name, name)) break;
		k = k->next;
	}

	return k;
}

// XXX add a nocopy flag??

kern_return_t
kmod_queue_cmd(vm_address_t data, vm_size_t size)
{
	kern_return_t rc;
	cmd_queue_entry_t *e = (cmd_queue_entry_t *)kalloc(sizeof(struct cmd_queue_entry));
	if (!e) return KERN_RESOURCE_SHORTAGE;

	rc = kmem_alloc(kernel_map, &e->data, size);
	if (rc != KERN_SUCCESS) {
		kfree((vm_offset_t)e, sizeof(struct cmd_queue_entry));
		return rc;
	}
	e->size = size;
	bcopy((void *)data, (void *)e->data, size);

	simple_lock(&kmod_queue_lock);
	enqueue_tail(&kmod_cmd_queue, (queue_entry_t)e);
	simple_unlock(&kmod_queue_lock);

	thread_wakeup_one((event_t)&kmod_cmd_queue);
	
	return KERN_SUCCESS;
}

kern_return_t
kmod_load_extension(char *name)
{
	kmod_load_extension_cmd_t	*data;
	vm_size_t			size;

	size = sizeof(kmod_load_extension_cmd_t);
	data = (kmod_load_extension_cmd_t *)kalloc(size);
	if (!data) return KERN_RESOURCE_SHORTAGE;

	data->type = KMOD_LOAD_EXTENSION_PACKET;
	strncpy(data->name, name, KMOD_MAX_NAME);

	return kmod_queue_cmd((vm_address_t)data, size);
}

kern_return_t
kmod_load_extension_with_dependencies(char *name, char **dependencies)
{
	kmod_load_with_dependencies_cmd_t *data;
	vm_size_t	size;
	char		**c;
	int 		i, count = 0;

	c = dependencies;
	if (c) {
		while (*c) {
			count++; c++;
		}
	}
	size = sizeof(int) + KMOD_MAX_NAME * (count + 1) + 1; 
	data = (kmod_load_with_dependencies_cmd_t *)kalloc(size);
	if (!data) return KERN_RESOURCE_SHORTAGE;

	data->type = KMOD_LOAD_WITH_DEPENDENCIES_PACKET;
	strncpy(data->name, name, KMOD_MAX_NAME);

	c = dependencies;
	for (i=0; i < count; i++) {
		strncpy(data->dependencies[i], *c, KMOD_MAX_NAME);
		c++;
	}
	data->dependencies[count][0] = 0;

	return kmod_queue_cmd((vm_address_t)data, size);
}
kern_return_t
kmod_send_generic(int type, void *generic_data, int size)
{
	kmod_generic_cmd_t	*data;

	data = (kmod_generic_cmd_t *)kalloc(size + sizeof(int));
	if (!data) return KERN_RESOURCE_SHORTAGE;

	data->type = type;
	bcopy(data->data, generic_data, size);

	return kmod_queue_cmd((vm_address_t)data, size + sizeof(int));
}

kern_return_t
kmod_create_internal(kmod_info_t *info, kmod_t *id)
{
	kern_return_t rc;

	if (!info) return KERN_INVALID_ADDRESS;

	// double check for page alignment
	if ((info->address | info->hdr_size) & (PAGE_SIZE - 1)) {
		return KERN_INVALID_ADDRESS;
	}

	rc = vm_map_wire(kernel_map, info->address + info->hdr_size, 
			 info->address + info->size, VM_PROT_DEFAULT, FALSE);
	if (rc != KERN_SUCCESS) {
		return rc;
	}

	simple_lock(&kmod_lock);

	// check to see if already loaded
	if (kmod_lookupbyname(info->name)) {
		simple_unlock(&kmod_lock);
		rc = vm_map_unwire(kernel_map, info->address + info->hdr_size, 
				   info->address + info->size, FALSE);
		assert(rc == KERN_SUCCESS);
		return KERN_INVALID_ARGUMENT;
	}

	info->id = kmod_index++;
	info->reference_count = 0;

	info->next = kmod;
	kmod = info;

	*id = info->id;

	simple_unlock(&kmod_lock);

	printf("kmod_create: %s (id %d), %d pages loaded at 0x%x, header size 0x%x\n", 
	       info->name, info->id, info->size / PAGE_SIZE, info->address, info->hdr_size);

	return KERN_SUCCESS;
}


kern_return_t
kmod_create(host_priv_t host_priv,
	    kmod_info_t *info,
	    kmod_t *id)
{
	if (host_priv == HOST_PRIV_NULL) return KERN_INVALID_HOST;
	return kmod_create_internal(info, id);
}

kern_return_t
kmod_create_fake(char *name, char *version)
{
	kmod_info_t *info;

	info = (kmod_info_t *)kalloc(sizeof(kmod_info_t));
	if (!info) {
		return KERN_RESOURCE_SHORTAGE;
	}

	// make de fake
	info->info_version = KMOD_INFO_VERSION;
	bcopy(name, info->name, KMOD_MAX_NAME);
	bcopy(version, info->version, KMOD_MAX_NAME);
	info->reference_count = 1;	// keep it from unloading, starting, stopping
	info->reference_list = 0;
	info->address = info->size = info->hdr_size = 0;
	info->start = info->stop = 0;

	simple_lock(&kmod_lock);

	// check to see if already "loaded"
	if (kmod_lookupbyname(info->name)) {
		simple_unlock(&kmod_lock);
		return KERN_INVALID_ARGUMENT;
	}

	info->id = kmod_index++;

	info->next = kmod;
	kmod = info;

	simple_unlock(&kmod_lock);

	return KERN_SUCCESS;
}

kern_return_t
kmod_destroy_internal(kmod_t id)
{
	kern_return_t rc;
	kmod_info_t *k;
	kmod_info_t *p;

	simple_lock(&kmod_lock);

	k = p = kmod;
	while (k) {
		if (k->id == id) {
			kmod_reference_t *r, *t;

			if (k->reference_count != 0) {
				simple_unlock(&kmod_lock);
				return KERN_INVALID_ARGUMENT;
			}
				
			if (k == p) {	// first element
				kmod = k->next;
			} else {
				p->next = k->next;
			}
			simple_unlock(&kmod_lock);

			r = k->reference_list;
			while (r) {
				r->info->reference_count--;
				t = r;
				r = r->next;
				kfree((vm_offset_t)t, sizeof(struct kmod_reference));
			}

			printf("kmod_destroy: %s (id %d), deallocating %d pages starting at 0x%x\n", 
			       k->name, k->id, k->size / PAGE_SIZE, k->address);

			rc = vm_map_unwire(kernel_map, k->address + k->hdr_size, 
					   k->address + k->size, FALSE);
			assert(rc == KERN_SUCCESS);

			rc = vm_deallocate(kernel_map, k->address, k->size);
			assert(rc == KERN_SUCCESS);

			return KERN_SUCCESS;
		}
		p = k;
		k = k->next;
	}

	simple_unlock(&kmod_lock);

	return KERN_INVALID_ARGUMENT;
}


kern_return_t
kmod_destroy(host_priv_t host_priv,
	     kmod_t id)
{
	if (host_priv == HOST_PRIV_NULL) return KERN_INVALID_HOST;
	return kmod_destroy_internal(id);
}


kern_return_t
kmod_start_or_stop(
    kmod_t id,
    int start,
    kmod_args_t *data,
    mach_msg_type_number_t *dataCount)
{
    kern_return_t rc = KERN_SUCCESS;
    void * user_data = 0;
    kern_return_t (*func)();
    kmod_info_t *k;

    simple_lock(&kmod_lock);

    k = kmod_lookupbyid(id);
    if (!k || k->reference_count) {
        simple_unlock(&kmod_lock);
        rc = KERN_INVALID_ARGUMENT;
        goto finish;
    }

    if (start) {
        func = (void *)k->start;
    } else {
        func = (void *)k->stop;
    }

    simple_unlock(&kmod_lock);

    //
    // call kmod entry point
    //
    if (data && dataCount && *data && *dataCount) {
        vm_map_copyout(kernel_map, (vm_offset_t *)&user_data, (vm_map_copy_t)*data);
    }

    rc = (*func)(k, user_data);

finish:

    if (user_data) {
        (void) vm_deallocate(kernel_map, (vm_offset_t)user_data, *dataCount);
    }
    if (data) *data = 0;
    if (dataCount) *dataCount = 0;

    return rc;
}


/*
 * The retain and release calls take no user data, but the caller
 * may have sent some in error (the MIG definition allows it).
 * If this is the case, they will just return that same data
 * right back to the caller (since they never touch the *data and
 * *dataCount fields).
 */
kern_return_t
kmod_retain(kmod_t id)
{
    kern_return_t rc = KERN_SUCCESS;

    kmod_info_t *t;    // reference to
    kmod_info_t *f;    // reference from
    kmod_reference_t *r = 0;

    r = (kmod_reference_t *)kalloc(sizeof(struct kmod_reference));
    if (!r) {
        rc = KERN_RESOURCE_SHORTAGE;
        goto finish;
    }

    simple_lock(&kmod_lock);

    t = kmod_lookupbyid(KMOD_UNPACK_TO_ID(id));
    f = kmod_lookupbyid(KMOD_UNPACK_FROM_ID(id));
    if (!t || !f) {
        simple_unlock(&kmod_lock);
        if (r) kfree((vm_offset_t)r, sizeof(struct kmod_reference));
        rc = KERN_INVALID_ARGUMENT;
        goto finish;
    }

    r->next = f->reference_list;
    r->info = t;
    f->reference_list = r;
    t->reference_count++;

    simple_unlock(&kmod_lock);

finish:

    return rc;
}


kern_return_t
kmod_release(kmod_t id)
{
    kern_return_t rc = KERN_INVALID_ARGUMENT;

    kmod_info_t *t;    // reference to
    kmod_info_t *f;    // reference from
    kmod_reference_t *r = 0;
    kmod_reference_t * p;

    simple_lock(&kmod_lock);

    t = kmod_lookupbyid(KMOD_UNPACK_TO_ID(id));
    f = kmod_lookupbyid(KMOD_UNPACK_FROM_ID(id));
    if (!t || !f) {
        rc = KERN_INVALID_ARGUMENT;
        goto finish;
    }

    p = r = f->reference_list;
    while (r) {
        if (r->info == t) {
            if (p == r) {	// first element
                f->reference_list = r->next;
            } else {
                p->next = r->next;
            }
            r->info->reference_count--;

	    simple_unlock(&kmod_lock);
            kfree((vm_offset_t)r, sizeof(struct kmod_reference));
	    rc = KERN_SUCCESS;
            goto finish;
        }
        p = r;
        r = r->next;
    }

    simple_unlock(&kmod_lock);

finish:

    return rc;
}


kern_return_t
kmod_control(host_priv_t host_priv,
	     kmod_t id,
	     kmod_control_flavor_t flavor,
	     kmod_args_t *data,
	     mach_msg_type_number_t *dataCount)
{
	kern_return_t rc = KERN_SUCCESS;

	if (host_priv == HOST_PRIV_NULL) return KERN_INVALID_HOST;

	switch (flavor) {

	case KMOD_CNTL_START:
	case KMOD_CNTL_STOP: 
	{
            rc = kmod_start_or_stop(id, (flavor == KMOD_CNTL_START),
                data, dataCount);
            break;
	}

	case KMOD_CNTL_RETAIN:
        {
            rc = kmod_retain(id);
            break;
        }

	case KMOD_CNTL_RELEASE:
        {
            rc = kmod_release(id);
            break;
        }

	case KMOD_CNTL_GET_CMD: {

		cmd_queue_entry_t *e;

		/*
		 * Throw away any data the user may have sent in error.
		 * We must do this, because we are likely to return to
		 * some data for these commands (thus causing a leak of
		 * whatever data the user sent us in error).
		 */
		if (*data && *dataCount) {
			vm_map_copy_discard(*data);
			*data = 0;
			*dataCount = 0;
		}
			
		simple_lock(&kmod_queue_lock);

		if (queue_empty(&kmod_cmd_queue)) {
			assert_wait((event_t)&kmod_cmd_queue, THREAD_ABORTSAFE);
			simple_unlock(&kmod_queue_lock);
			thread_block((void(*)(void))0);
			simple_lock(&kmod_queue_lock);
			if (queue_empty(&kmod_cmd_queue)) {
				// we must have been interrupted!
				simple_unlock(&kmod_queue_lock);
				return KERN_ABORTED;
			}
		}
		e = (cmd_queue_entry_t *)dequeue_head(&kmod_cmd_queue);

		simple_unlock(&kmod_queue_lock);

		rc = vm_map_copyin(kernel_map, e->data, e->size, TRUE, (vm_map_copy_t *)data);
		if (rc) {
			simple_lock(&kmod_queue_lock);
			enqueue_head(&kmod_cmd_queue, (queue_entry_t)e);
			simple_unlock(&kmod_queue_lock);
			*data = 0;
			*dataCount = 0;
			return rc;
		}
		*dataCount = e->size;

		kfree((vm_offset_t)e, sizeof(struct cmd_queue_entry));
		
		break;
	}

	default:
		rc = KERN_INVALID_ARGUMENT;
	}

	return rc;
};


kern_return_t
kmod_get_info(host_t host,
	      kmod_info_array_t *kmods,
	      mach_msg_type_number_t *kmodCount)
{
	vm_offset_t data;
	kmod_info_t *k, *p1;
	kmod_reference_t *r, *p2;
	int ref_count;
	unsigned size = 0;
	kern_return_t rc = KERN_SUCCESS;

	*kmods = (void *)0;
	*kmodCount = 0;

retry:
	simple_lock(&kmod_lock);
	size = 0;
	k = kmod;
	while (k) {
		size += sizeof(kmod_info_t);
		r = k->reference_list;
		while (r) {
			size +=sizeof(kmod_reference_t);
			r = r->next;
		}
		k = k->next;
	}
	simple_unlock(&kmod_lock);
	if (!size) return KERN_SUCCESS;

	rc = kmem_alloc(kernel_map, &data, size);
	if (rc) return rc;

	// copy kmod into data, retry if kmod's size has changed (grown)
	// the copied out data is tweeked to figure what's what at user level
	// change the copied out k->next pointers to point to themselves
	// change the k->reference into a count, tack the references on
	// the end of the data packet in the order they are found

	simple_lock(&kmod_lock);
	k = kmod; p1 = (kmod_info_t *)data;
	while (k) {
		if ((p1 + 1) > (kmod_info_t *)(data + size)) {
			simple_unlock(&kmod_lock);
			kmem_free(kernel_map, data, size);
			goto retry;
		}

		*p1 = *k;
		if (k->next) p1->next = k;
		p1++; k = k->next;
	}

	p2 = (kmod_reference_t *)p1;
	k = kmod; p1 = (kmod_info_t *)data;
	while (k) {
		r = k->reference_list; ref_count = 0;
		while (r) {
			if ((p2 + 1) > (kmod_reference_t *)(data + size)) {
				simple_unlock(&kmod_lock);
				kmem_free(kernel_map, data, size);
				goto retry;
			}
			// note the last 'k' in the chain has its next == 0
			// since there can only be one like that, 
			// this case is handled by the caller
			*p2 = *r;
			p2++; r = r->next; ref_count++;
		}
		p1->reference_list = (kmod_reference_t *)ref_count;
		p1++; k = k->next;
	}
	simple_unlock(&kmod_lock);
	
	rc = vm_map_copyin(kernel_map, data, size, TRUE, (vm_map_copy_t *)kmods);
	if (rc) {
		kmem_free(kernel_map, data, size);
		*kmods = 0;
		*kmodCount = 0;
		return rc;
	}
	*kmodCount = size;

	return KERN_SUCCESS;
}

#include <mach-o/loader.h>

extern void *getsectdatafromheader(struct mach_header *mhp,
                                   const char *segname,
                                   const char *sectname,
                                   int *size);

static kern_return_t
kmod_call_funcs_in_section(struct mach_header *header, const char *sectName)
{
	typedef void 	(*Routine)(void);
	Routine *	routines;
	int		size, i;

	if (header->magic != MH_MAGIC) {
		return KERN_INVALID_ARGUMENT;
	}

	routines = (Routine *) getsectdatafromheader(header, SEG_TEXT, sectName, &size);
	if (!routines) return KERN_SUCCESS;

	size /= sizeof(Routine);
	for (i = 0; i < size; i++) {
		(*routines[i])();
	}

	return KERN_SUCCESS;
}

kern_return_t
kmod_initialize_cpp(kmod_info_t *info)
{
	return kmod_call_funcs_in_section((struct mach_header *)info->address, "__constructor");
}

kern_return_t
kmod_finalize_cpp(kmod_info_t *info)
{
	return kmod_call_funcs_in_section((struct mach_header *)info->address, "__destructor");
}

kern_return_t
kmod_default_start(struct kmod_info *ki, void *data)
{
	return KMOD_RETURN_SUCCESS;
}

kern_return_t
kmod_default_stop(struct kmod_info *ki, void *data)
{
	return KMOD_RETURN_SUCCESS;
}

#define IS_IN_BACKTRACE 0xdeadbeef
#define IS_A_DEPENDENCY 0xbeefdead

void
kmod_dump(vm_offset_t *addr, unsigned int cnt)
{
    kmod_info_t *k;
    kmod_reference_t *r;
    int i, found_one = 0;

    // find backtrace addresses that are inside a kmod
    for (i=0; i < cnt; i++, addr++) {
	k = kmod;
	while (k) {
	    // XXX - validate page(s) that k points to
		if(pmap_extract(kernel_pmap, (vm_offset_t)k) == 0) { 	/* Exit loop if page not mapped */
			printf("kmod scan stopped due to missing page: %08X\n", k);
			break;
		}
	    if ((*addr >= k->address) && (*addr < (k->address + k->size))) {
		// got one, blast info_version, we don't need it at this point
		k->info_version = IS_IN_BACKTRACE;
		found_one++;
		break;
	    }
	    k = k->next;
	}
    }
    if (!found_one) return;

    printf("kernel modules in backtrace: ");
    k = kmod;
    while (k) {
		if(pmap_extract(kernel_pmap, (vm_offset_t)k) == 0) { 	/* Exit loop if page not mapped */
			printf("kmod scan stopped due to missing page: %08X\n", k);
			break;
		}
		if (k->info_version == IS_IN_BACKTRACE) {
		    printf("%s(%s)@0x%x ", k->name, k->version, k->address);
		}
		k = k->next;
    }
    printf("\n");

    // look for dependencies
    k = kmod; found_one = 0;
    while (k) {
		if(pmap_extract(kernel_pmap, (vm_offset_t)k) == 0) { 	/* Exit loop if page not mapped */
			printf("kmod dependency scan stopped due to missing page: %08X\n", k);
			break;
		}
		if (k->info_version == IS_IN_BACKTRACE) {
		    r = k->reference_list;
		    while (r) {
				// XXX - validate page(s) that r and r->info point to
				if(pmap_extract(kernel_pmap, (vm_offset_t)r) == 0) { 	/* Exit loop if page not mapped */
					printf("kmod validation scan stopped due to missing page: %08X\n", r);
					break;
				}
				if (r->info->info_version != IS_IN_BACKTRACE) {
				    r->info->info_version = IS_A_DEPENDENCY;
				    found_one++;
				}
				r = r->next;
		    }
		}
		k = k->next;
    }
    if (!found_one) goto cleanup;

    printf("kernel module dependencies: ");
    k = kmod;
    while (k) {
		if(pmap_extract(kernel_pmap, (vm_offset_t)k) == 0) { 	/* Exit loop if page not mapped */
			printf("kmod dependency print stopped due to missing page: %08X\n", k);
			break;
		}
		if (k->info_version == IS_A_DEPENDENCY) {
		    printf("%s(%s)@0x%x ", k->name, k->version, k->address);
		}
		k = k->next;
    }
    printf("\n");

 cleanup:
    // in case we double panic
    k = kmod;
    while (k) {
		if(pmap_extract(kernel_pmap, (vm_offset_t)k) == 0) { 	/* Exit loop if page not mapped */
			printf("kmod dump cleanup stopped due to missing page: %08X\n", k);
			break;
		}
		k->info_version = KMOD_INFO_VERSION;
		k = k->next;
    }
}
