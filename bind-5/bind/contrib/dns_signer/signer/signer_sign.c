/* $Header: /cvs/Darwin/Services/bind/bind/contrib/dns_signer/signer/signer_sign.c,v 1.1.1.3 2001/01/31 03:58:42 zarzycki Exp $ */

    
/* Portions Copyright (c) 1995,1996,1997,1998 by Trusted Information Systems, In
c.  
 *  
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *  
 * THE SOFTWARE IS PROVIDED "AS IS" AND TRUSTED INFORMATION SYSTEMS DISCLAIMS
 * ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL TRUSTED INFORMATION
 * SYSTEMS BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 * PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS
 * ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 *  
 *  
 * Trusted Information Systems, Inc. has received approval from the
 * United States Government for export and reexport of TIS/DNSSEC
 * software from the United States of America under the provisions of
 * the Export Administration Regulations (EAR) General Software Note
 * (GSN) license exception for mass market software.  Under the
 * provisions of this license, this software may be exported or
 * reexported to all destinations except for the embargoed countries of
 * Cuba, Iran, Iraq, Libya, North Korea, Sudan and Syria.  Any export
 * or reexport of TIS/DNSSEC software to the embargoed countries
 * requires additional, specific licensing approval from the United
 * States Government.
 */

#include "signer_sign.h"
#include "signer_ingest.h"
#include "signer_tree.h"
#include "signer_key_monitor.h"
#include "signer_write.h"
#include "signer_crypto.h"
#include "signer_debug.h"

/* Done */
int sign_cleanup (struct signing_options *options)
{
	if (options->so_zone_output != NULL)
	{
		fclose (options->so_zone_output);
		options->so_zone_output = NULL;
		unlink (options->so_output_zone_file);
	}

	if (options->so_glue_output != NULL)
	{
		fclose (options->so_glue_output);
		options->so_glue_output = NULL;
		unlink (options->so_output_glue_file);
	}

	if (options->so_parent_output != NULL)
	{
		fclose (options->so_parent_output);
		options->so_parent_output = NULL;
		unlink (options->so_output_parent_file);
	}

	options->so_return_code = SIGNER_ERROR;
	return FALSE;
}

/* Done */
int fill_types_field (struct signing_options *options, u_int8_t *bitfield)
{
	struct rrset_rec	*a_set;
	int					max_bit_set = 0;
	int					algorithm;

	SETON (bitfield, ns_t_nxt);
	max_bit_set = ns_t_nxt;

	for (	a_set = options->so_previous_name->nr_rrsets;
			a_set;
			a_set = a_set->rrs_next)
	{
		if (options->so_previous_name->nr_status == NR_STATUS_DELEGATION &&
				a_set->rrs_type_h != ns_t_ns &&
				a_set->rrs_type_h != ns_t_sig &&
				a_set->rrs_type_h != ns_t_key)
			continue;

		SETON (bitfield, a_set->rrs_type_h);
		if (max_bit_set < a_set->rrs_type_h) max_bit_set = a_set->rrs_type_h;

		if (a_set->rrs_sig != NULL)
		{
			SETON (bitfield, ns_t_sig);
			if (max_bit_set < ns_t_sig) max_bit_set = ns_t_sig;
		}
	}

	/* If mode is ZONE KEY, then the NXT will be signed */
	for (algorithm = VALUE_ALGORITHM_MINIMUM;
			algorithm <= options->so_max_algorithm;
				algorithm++)
	{
		if (options->so_signing_modes[algorithm] == SO_MODE_ZONE_KEY)
		{
			SETON (bitfield, ns_t_sig);
			if (max_bit_set < ns_t_sig) max_bit_set = ns_t_sig;
			break;
		}
	}

	if (max_bit_set == 0)
		return 0;
	else
		return (max_bit_set+7)/ 8;
}

/* Done */
int sign_handle_glue (	struct name_rec			*nr,
						struct rrset_rec		*rrs,
						struct signing_options	*options,
						int						dont_write_sigs)
{
	if (options->so_glue_output == NULL)
	{
		options->so_glue_output = fopen (options->so_output_glue_file, "w");
		if (options->so_glue_output == NULL)
		{
			sprintf (debug_message, "Can't open %s for writing",
											options->so_output_glue_file);
			ERROR (debug_message);
			options->so_return_code = SIGNER_ERROR;
			return SIGNER_ERROR;
		}
#ifdef __use_signer_dead_code__
		fprintf (options->so_glue_output, "; Generated by %s\n",
					software_version);
#endif
	}

	write_remove_duplicates (rrs, options);

	return write_rrset (options->so_glue_output, nr, rrs, options,
														dont_write_sigs);
}

void sign_remove_from_list (struct rr_rec **check_sigs,
							struct rr_rec *prior_sig)
{
	if (prior_sig == NULL)
		(*check_sigs) = (*check_sigs)->rr_next;
	else
		prior_sig->rr_next = prior_sig->rr_next->rr_next;
}

void sign_jump_to_list (	struct rr_rec **to_list,
							struct rr_rec **from_list,
							struct rr_rec *this,
							struct rr_rec *prior)
{
	sign_remove_from_list (from_list, prior);

	this->rr_next = *to_list;
	*to_list = this;
}

int will_try_to_recycle (struct rr_rec *sig, struct key_list *signers)
{
	u_int8_t		*this_signer;
	u_int16_t		short_n;
	u_int16_t		this_footprint_h;
	struct key_list	*a_signer;

	if (signers==NULL) return FALSE;

	this_signer = &sig->rr_rdata[SIG_RDATA_BY_NAME];
	memcpy (&short_n,&sig->rr_rdata[SIG_RDATA_BY_FOOTPRINT],sizeof(u_int16_t));
	this_footprint_h = ntohs (short_n);

	for (a_signer = signers; a_signer; a_signer = a_signer->kl_next)
	{
		if (a_signer->kl_key->km_any_key && a_signer->kl_key==NULL) continue;

		if (a_signer->kl_key->km_footprint_h == this_footprint_h &&
			namecmp (this_signer, a_signer->kl_key->km_name_n)==0)
		{
			return TRUE;
		}
	}

	return FALSE;
}

u_int8_t sign_name_status (	u_int8_t *name_n,
							struct name_rec **name_rec,
							struct signing_options *options)
{
	int				index = 0;
	struct name_rec	*nr = NULL;

	(*name_rec) = NULL;

	if (in_domain (options->so_zone_name_n, name_n) == FALSE)
		return NR_STATUS_GLUE;

	if (((*name_rec) = tree_find_name (name_n)) != NULL)
		return (*name_rec)->nr_status;

	while (nr==NULL)
	{
		index += (int) name_n[index] + 1;
	
		if (name_n[index]=='\0')
			return NR_STATUS_GLUE;

		nr = tree_find_name (&name_n[index]);
	}

	switch (nr->nr_status)
	{
		case NR_STATUS_IN:
		case NR_STATUS_APEX:
		case NR_STATUS_PARENT:
			return NR_STATUS_IN;	
		case NR_STATUS_GLUE:
		case NR_STATUS_SUBZONE:
		case NR_STATUS_DELEGATION:
			return NR_STATUS_GLUE;
		default:
			return NR_STATUS_ERROR;
	}
}

int sign_inspect_signature (	struct rr_rec			**check_sigs,
								struct rrset_rec		*rrs,
								struct signing_options	*options)
{
	struct rr_rec		*this_sig;
	struct rr_rec		*prior_sig;
	u_int8_t			*this_name_n;
	u_int8_t			this_name_status;
	struct name_rec		*this_name;
	u_int8_t			this_algorithm;
	u_int16_t			short_n;
	u_int16_t			this_footprint_h;
	struct key_matter	*km;
	struct rrset_rec	*key_set;
	struct rr_rec		*key_rr;

	this_sig = *check_sigs;
	prior_sig = NULL;

	if (this_sig == NULL) return SIGNER_OK;

	while (this_sig != NULL)
	{
		this_name_n = &this_sig->rr_rdata[SIG_RDATA_BY_NAME];
		this_algorithm = this_sig->rr_rdata[SIG_RDATA_ALGORITHM];
		this_name_status = sign_name_status(this_name_n,&this_name,options);

		memcpy (&short_n, &this_sig->rr_rdata[SIG_RDATA_BY_FOOTPRINT],
														sizeof(u_int16_t));
		this_footprint_h = ntohs (short_n);

		/* If the name,footprint is in the signing list */
			/* Move the signature to the recyclers */

		if ((km = key_find_public(this_name_n, this_algorithm,
										this_footprint_h)) != NULL)
		{
			
			sign_jump_to_list (&rrs->rrs_recyclers, check_sigs, this_sig,
				prior_sig);

			if (prior_sig)
				this_sig = prior_sig->rr_next;
			else
				this_sig = *check_sigs;

			continue;
		}

		/* If the name does not exist */
		/* OR The name exists but the name does not have keys */
		/* OR The key set exists but the footprint does not exist */
			/* If the name is not in the zone */
				/* Return signature to the sig list */
			/* Else */
				/* Delete the signature */
				/* Hmm, if the key set exists - add $SIGNER name any */

		if (this_name == NULL 
			|| ((key_set = tree_find_set (this_name, ns_t_key)) == NULL)
			|| ((key_rr = key_find_key (key_set, this_algorithm,
											this_footprint_h)) == NULL))
		{
			if (this_name_status != NR_STATUS_IN
				&& this_name_status != NR_STATUS_APEX
				&& this_name_status != NR_STATUS_DELEGATION)
			{
				/* Name is outside our zone */
				sign_jump_to_list (&rrs->rrs_sig, check_sigs, this_sig,
					prior_sig);
				options->so_stats.ss_signatures_cant_confirm++;
			}
			else
			{
				/* The name is in our zone */
				sign_remove_from_list (check_sigs, prior_sig);
				this_sig->rr_next = NULL;
				free_rr_recs (&this_sig);
				options->so_stats.ss_signatures_retired++;
			}

			if (prior_sig)
				this_sig = prior_sig->rr_next;
			else
				this_sig = *check_sigs;
			continue;
		}

		/* Make a DST public key from the found key */
		/* Add it to the key list */
		/* Move the sig to the recyclers */

		if ((km = key_add_public (this_name_n, this_algorithm,
					this_footprint_h, key_set, key_rr, options)) == NULL)
		{
			wire_to_ascii_name (debug_name1_h, this_name_n, MAXPNAME);
			sprintf (debug_message, "Couldn't add public key for %s/%d",
										debug_name1_h, this_footprint_h);
			ERROR (debug_message);
			options->so_return_code = SIGNER_ERROR;
			return SIGNER_ERROR;
		}

		sign_jump_to_list (&rrs->rrs_recyclers,check_sigs,this_sig,prior_sig);

		if (prior_sig)
			this_sig = prior_sig->rr_next;
		else
			this_sig = *check_sigs;
	}

	return SIGNER_OK;
}

void sign_collect_dubious_sigs (struct rr_rec			**check_sigs,
								struct rrset_rec		*rrs, 
								struct signing_options	*options)
{
	struct rr_rec	*prior_sig;
	struct rr_rec	*this_sig;

	this_sig = rrs->rrs_sig;
	prior_sig = NULL;
	while (this_sig != NULL)
	{
		/* Did we just sign it? */
		if (memcmp(&options->so_time_signed_n,
			&this_sig->rr_rdata[SIG_RDATA_TIME_SIGNED], sizeof (u_int32_t))==0)
		{
			prior_sig = this_sig;
			this_sig = this_sig->rr_next;
		}

		/* Will we try to recycle it? */
		else if (will_try_to_recycle (this_sig, rrs->rrs_signers))
		{
			prior_sig = this_sig;
			this_sig = this_sig->rr_next;
		}

		/* This sig is potentially dubious, add it to the list */
		else
		{
			sign_jump_to_list(check_sigs,&rrs->rrs_sig,this_sig,prior_sig);

			if (prior_sig)
				this_sig = prior_sig->rr_next;
			else
				this_sig = rrs->rrs_sig;
		}
	}
}

int sign_remove_dubious_sigs (	struct rrset_rec *rrs,
								struct signing_options *options)
{
	struct rr_rec	*check_sigs=NULL;

	/* Collect the potentially dubious sigs */

	sign_collect_dubious_sigs (&check_sigs, rrs, options);

	/* For the dubious ones */
		/* If they belong to existing keys and will recycle */
			/* Remove from dubious list */
		/* If !(the key is absent from an APEX/IN/DELEG's KEY set) */
			/* Remove from dubious list */

	if (sign_inspect_signature (&check_sigs, rrs, options) == SIGNER_ERROR)
		return SIGNER_ERROR;

	return SIGNER_OK;
}

/* Done */
int sign_a_set (		struct name_rec			*nr,
						struct rrset_rec		*rrs,
						struct signing_options	*options)
{
	/* Before even recycling, try throwing out signatures */

	if (sign_remove_dubious_sigs (rrs, options) == SIGNER_ERROR)
	{
		options->so_return_code = SIGNER_ERROR;
		return SIGNER_ERROR;
	}

	/* Do the crypto operations */

	if (crypto_make_signatures (nr, rrs, options) == SIGNER_ERROR)
	{
		options->so_return_code = SIGNER_ERROR;
		return SIGNER_ERROR;
	}

	/* Write the results */

	if (nr->nr_status == NR_STATUS_PARENT)
		return sign_handle_glue (nr, rrs, options, FALSE);
	else
	{
		write_remove_duplicates (rrs, options);

		if (options->so_zone_output == NULL)
		{
			char	name_h[MAXPNAME];

			options->so_zone_output = fopen (options->so_output_zone_file, "w");
			if (options->so_zone_output == NULL)
			{
				sprintf (debug_message, "Can't open %s for writing",
											options->so_output_zone_file);
				ERROR (debug_message);
				return SIGNER_ERROR;
			}

			fprintf (options->so_zone_output, "; Generated by %s\n",
						software_version);

			if (options->so_parent_name_n)
			{
				wire_to_ascii_name(name_h, options->so_parent_name_n, MAXPNAME);
				fprintf (options->so_zone_output, "$PARENT %s\n", name_h);
			}
			wire_to_ascii_name (name_h, options->so_zone_name_n, MAXPNAME);
			fprintf (options->so_zone_output, "$ORIGIN %s\n", name_h);
		}

		return write_rrset (options->so_zone_output, nr, rrs, options, FALSE);
	}
}

/* Done */
int sign_make_nxt (struct signing_options *options, u_int8_t *next_name_n)
{
	/* Make the NXT */

	int					next_name_length = wire_name_length(next_name_n);
	u_int8_t			*new_rdata = NULL;
	u_int16_t			new_rdata_length;
	u_int8_t			bitfield [(((ns_t_max+1)+4)/8)];
	u_int16_t			bitfield_length;
	struct rrset_rec	*nxt_set;
	

	/* Prepare the data set */

	SETCLEAR(bitfield);

	bitfield_length = fill_types_field (options, bitfield);

	new_rdata = (u_int8_t *) MALLOC (next_name_length + bitfield_length);

	if (new_rdata==NULL)
	{
		ERROR ("Out of memory - exiting");
		tree_delete_parent_files (options);
		sign_cleanup (options);
		exit (1);
	}

	new_rdata_length = next_name_length + bitfield_length;

	memcpy (new_rdata, next_name_n, next_name_length);
	memcpy (&new_rdata[next_name_length], bitfield, bitfield_length);

	/* Add the set to the name */

	if ((nxt_set = tree_add_set (options->so_previous_name, ns_t_nxt,
		options->so_soa_min_ttl_n, new_rdata, options)) == NULL)
	{
		options->so_return_code = SIGNER_ERROR;
		return SIGNER_ERROR;
	}

	if (tree_add_as_data (nxt_set, new_rdata_length, new_rdata, options)
															== SIGNER_ERROR)
	{
		options->so_return_code = SIGNER_ERROR;
		return SIGNER_ERROR;
	}

	/*
		Don't need the new_rdata anymore (add_as_data copies it,
		'cause of the other way it is called - after ingesting a
		printed RR)
	*/

	FREE (new_rdata);

	/* Assign keys to sign the NXT */

	if (key_list_copy (&nxt_set->rrs_signers, options->so_soa_keys, options)
														== SIGNER_ERROR)
		return SIGNER_ERROR;

	/* Count the NXT */

	options->so_stats.ss_nxts_added++;

	/* Now, sign the set & maybe recycle signatures */

	return sign_a_set (options->so_previous_name, nxt_set, options);
}

int sign_delete_parent_file(struct name_rec *nr,struct signing_options *options)
{
	char	name_h[PATH_MAX];
	char	filename[PATH_MAX];

	if (nr->nr_status != NR_STATUS_DELEGATION) return 1;

	wire_to_ascii_name (name_h, nr->nr_name_n, PATH_MAX);

	sprintf (filename, "%s/%s.PARENT", options->so_output_directory, name_h);

	unlink (filename);

	return 1;
}

/* Needs adjustin' */
int sign_time_to_make_parent (struct signing_options *options)
{
	struct name_rec		*this = options->so_previous_name;
	struct name_rec		*apex = options->so_apex_record;
	struct rrset_rec	*check_set;
	int					algorithm;

	/* Time to make parent occurs when:
		Apex-key -> (UP) present
		Apex-ns -> (UP) no more signers to go
		Apex-glue -> (UP) don't matter
		This-key -> if experimental or null or no more signers
		This-nxt -> if no more signers (or if not to be done)
	*/

	if (this==NULL) return FALSE;

	if (!options->so_generate_parents && !options->so_generate_one_parent)
		return FALSE;

	if (options->so_up_policy)
	{
		if (apex == NULL) return FALSE;

		if ((check_set=tree_find_set (apex, ns_t_key))==NULL) return FALSE;

		if ((check_set=tree_find_set (apex, ns_t_ns))==NULL) return FALSE;
		if (check_set->rrs_signers != NULL) return FALSE;
	}

	if ((check_set=tree_find_set (this, ns_t_key))==NULL)
		for (algorithm = VALUE_ALGORITHM_MINIMUM;
				algorithm <= options->so_max_algorithm;
					algorithm++)
		{
			if (!(options->so_signing_modes[algorithm] == SO_MODE_BOTH_KEY ||
					options->so_signing_modes[algorithm] == SO_MODE_NULL_KEY))
			{
					return FALSE;
			}
		}

	if (check_set != NULL && check_set->rrs_signers != NULL) return FALSE;

	if (options->so_do_nxts == FALSE) return TRUE;
	if ((check_set = tree_find_set (this, ns_t_nxt))==NULL) return FALSE;
	if (check_set->rrs_signers != NULL) return FALSE;

	return TRUE;
}

void sign_restore_stats (	struct signing_statistics *optstats,
							struct signing_statistics *hold)
{
	memcpy (optstats, hold, sizeof(struct signing_statistics));
}

#define INDIV_WRITE_ERROR_BLOCK(type) \
		/* Couldn't write set to individual parent file */ \
		wire_to_ascii_name (debug_name1_h, nr->nr_name_n, MAXPNAME); \
		if (type==ns_t_any) \
		sprintf (debug_message, "Error writing %s/%s to %s", debug_name1_h, \
					"GLUE", options->so_output_parent_file);\
		else \
		sprintf (debug_message, "Error writing %s/%s to %s", debug_name1_h, \
					local_sym_ntop(__p_type_syms, type, &debug_success),\
					ind_fname);\
		ERROR (debug_message); \
		sign_restore_stats (&options->so_stats, &hold_stats); \
		if (ind_file != NULL) fclose (ind_file); \
		options->so_return_code = SIGNER_ERROR; \
		return SIGNER_ERROR

#define WHOLE_WRITE_ERROR_BLOCK(type) \
		/* Couldn't write set to single parent file */ \
		wire_to_ascii_name (debug_name1_h, nr->nr_name_n, MAXPNAME); \
		if (type==ns_t_any) \
		sprintf (debug_message, "Error writing %s/%s to %s", debug_name1_h, \
					"GLUE", options->so_output_parent_file);\
		else \
		sprintf (debug_message, "Error writing %s/%s to %s", debug_name1_h, \
					local_sym_ntop(__p_type_syms, type, &debug_success),\
					options->so_output_parent_file);\
		ERROR (debug_message); \
		sign_restore_stats (&options->so_stats, &hold_stats); \
		if (ind_file != NULL) fclose (ind_file); \
		options->so_return_code = SIGNER_ERROR; \
		return SIGNER_ERROR

/* Done */
int sign_make_parent (struct signing_options *options)
{
	struct name_rec				*nr = options->so_previous_name;
	char						name_h[PATH_MAX];
	char						ind_fname[PATH_MAX];
	FILE						*ind_file;
	struct signing_statistics	hold_stats;

	if (!sign_time_to_make_parent (options))
		return SIGNER_OK;

	wire_to_ascii_name (name_h, nr->nr_name_n, PATH_MAX);

	if (options->so_generate_parents)
	{
		sprintf (ind_fname,"%s/%s.PARENT",options->so_output_directory,name_h);

		if ((ind_file = fopen (ind_fname, "w")) == NULL)
		{
			sprintf (debug_message, "Can't open %s for writing", ind_fname);
			ERROR (debug_message);
			options->so_return_code = SIGNER_ERROR;
			return SIGNER_ERROR;
		}

		fprintf (ind_file, "; Generated by %s\n", software_version);

	}
	else
		ind_file = NULL;

	if (options->so_generate_one_parent)
	{
		if (options->so_parent_output == NULL)
			options->so_parent_output=fopen(options->so_output_parent_file,"w");

		if (options->so_parent_output == NULL)
		{
			sprintf (debug_message, "Can't open %s for writing",
												options->so_output_parent_file);
			ERROR (debug_message);
			options->so_return_code = SIGNER_ERROR;
			return SIGNER_ERROR;
		}

		fprintf (options->so_parent_output, "; Generated by %s\n",
						software_version);

		fprintf (options->so_parent_output, "#start %s.PARENT\n", name_h);
	}

	/* Store stats so we can mangle them later */

	memcpy (&hold_stats, &options->so_stats, sizeof(struct signing_statistics));

	/* Dump KEY set to file */

	if (ind_file && write_set (ind_file, nr, ns_t_key, options, FALSE)
														== SIGNER_ERROR)
	{
		INDIV_WRITE_ERROR_BLOCK (ns_t_key);
	}

	if (options->so_generate_one_parent && 
			write_set (options->so_parent_output, nr, ns_t_key, options, FALSE)
																== SIGNER_ERROR)
	{
		WHOLE_WRITE_ERROR_BLOCK (ns_t_key);
	}

	/* Dump NXT set to file */

	if (options->so_do_nxts && ind_file &&
			write_set (ind_file, nr, ns_t_nxt, options, FALSE) == SIGNER_ERROR)
	{
		INDIV_WRITE_ERROR_BLOCK (ns_t_nxt);
	}

	if (options->so_do_nxts && options->so_generate_one_parent && 
			write_set (options->so_parent_output, nr, ns_t_nxt, options, FALSE)
																== SIGNER_ERROR)
	{
		WHOLE_WRITE_ERROR_BLOCK (ns_t_nxt);
	}

	/* Up policy support */

	if (options->so_up_policy)
	{
		/* For UP: dump apex KEYS (w/o signature) */

		if (ind_file && write_set (ind_file, options->so_apex_record,
									ns_t_key, options, TRUE) == SIGNER_ERROR)
		{
			INDIV_WRITE_ERROR_BLOCK (ns_t_key);
		}

		if (options->so_generate_one_parent && 
				write_set (options->so_parent_output, options->so_apex_record,
									ns_t_key, options, TRUE) == SIGNER_ERROR)
		{
			WHOLE_WRITE_ERROR_BLOCK (ns_t_key);
		}

		/* For UP: dump apex NS (ditto) */

		if (ind_file && write_set (ind_file, options->so_apex_record,
									ns_t_ns, options, TRUE) == SIGNER_ERROR)
		{
			INDIV_WRITE_ERROR_BLOCK (ns_t_ns);
		}

		if (options->so_generate_one_parent && 
				write_set (options->so_parent_output, options->so_apex_record,
									ns_t_ns, options, TRUE) == SIGNER_ERROR)
		{
			WHOLE_WRITE_ERROR_BLOCK (ns_t_ns);
		}

		/* For UP: dump apex glue records */

		if (ind_file && write_glue (ind_file, options->so_apex_record,
													options) == SIGNER_ERROR)
		{
			INDIV_WRITE_ERROR_BLOCK (ns_t_any);
		}

		if (options->so_generate_one_parent && 
				write_glue (options->so_parent_output, options->so_apex_record,
													options) == SIGNER_ERROR)
		{
			WHOLE_WRITE_ERROR_BLOCK (ns_t_any);
		}
	}

	if (ind_file != NULL) fclose (ind_file);

	if (options->so_generate_one_parent)
		fprintf (options->so_parent_output, "#end %s.PARENT\n", name_h);

	sign_restore_stats (&options->so_stats, &hold_stats);

	return SIGNER_OK;
}

/* Done */
int add_NULL_key (struct rrset_rec *set, u_int16_t algorithm,
											struct signing_options *options)
{
	u_int8_t		null_key_rdata[4];
	u_int16_t		null_key_rdata_len=sizeof(u_int16_t) + 2*sizeof(u_int8_t);
	u_int16_t		null_zone_key_flags_n;

	null_zone_key_flags_n = ntohs(	VALUE_FLAGS_CANT_AUTHENTICATE |
									VALUE_FLAGS_CANT_CONFIDE |
									VALUE_FLAGS_ZONE_KEY);
	memcpy (&null_key_rdata[KEY_RDATA_FLAGS], &null_zone_key_flags_n,
														sizeof (u_int16_t));
	null_key_rdata[KEY_RDATA_PROTOCOL] = PROTOCOL_DNSSEC;
	null_key_rdata[KEY_RDATA_ALGORITHM] = (u_int8_t) algorithm;

	if (tree_add_as_data (set, null_key_rdata_len, null_key_rdata, options)
																== SIGNER_ERROR)
	{
		options->so_return_code = SIGNER_ERROR;
		return SIGNER_ERROR;
	}

	options->so_stats.ss_null_keys_added++;

	return SIGNER_OK;
}

/* Done */
int sign_check_delegation_point (struct name_rec *nm,
									struct signing_options *options)
{
	/* nm->nr_status is DELEGATION upon entry */

	struct rrset_rec	*key_set;
	struct rr_rec		*a_key;
	int					algorithm;

	/* If there is no key but a non-null soa key exists & no null ones */
		/* Make a null zone key entry for the name */

	/* Find the set of keys for this name first and hold onto them. */
	key_set = tree_find_set (nm, ns_t_key);

	for (algorithm = VALUE_ALGORITHM_MINIMUM;
			algorithm <= options->so_max_algorithm;
				algorithm++)
	{
		if (options->so_signing_modes[algorithm] == SO_MODE_ZONE_KEY)
		{
			if (key_set == NULL)
			{
				/* Make a warning available */
		
				wire_to_ascii_name (debug_name1_h, nm->nr_name_n, MAXPNAME);
				sprintf (debug_message,"Adding NULL key for %s, algorithm #%d",
								debug_name1_h, algorithm);

				WARN (debug_message);
		
				/* Add a NULL key set */
		
				if ((key_set=tree_add_set (nm, ns_t_key,
					options->so_soa_min_ttl_n, NULL, options)) == NULL)
				{
					options->so_return_code = SIGNER_ERROR;
					return SIGNER_ERROR;
				}
		
				/* Add NULL key for this algorithm to the zone keys */
		
				if (add_NULL_key (key_set, algorithm, options)==SIGNER_ERROR)
					return SIGNER_ERROR;
			}
			else
			{
				int found_a_non_zone_key = FALSE;
				int need_a_null_key = TRUE;
		
				for (a_key = key_set->rrs_data; a_key; a_key = a_key->rr_next)
				{
					if (a_key->rr_rdata[KEY_RDATA_ALGORITHM]==algorithm)
					{
						if(key_is_a_zone_key (a_key->rr_rdata))
							need_a_null_key = FALSE;
						else
							found_a_non_zone_key = TRUE;
					}
				}

				if (found_a_non_zone_key && need_a_null_key)
				{
					wire_to_ascii_name (debug_name1_h, nm->nr_name_n, MAXPNAME);
					sprintf (debug_message,
							"No zone key (of algorithm %d) in key set for %s",
											algorithm, debug_name1_h);
					ERROR (debug_message);
					return SIGNER_ERROR;
				}

				/* Add NULL key for this algorithm to the zone keys */
		
				if (need_a_null_key)
				{
					/* Make a warning available */
		
					wire_to_ascii_name (debug_name1_h, nm->nr_name_n, MAXPNAME);
					sprintf (debug_message,
									"Adding NULL key for %s, algorithm #%d",
									debug_name1_h, algorithm);

					WARN (debug_message);

					if (add_NULL_key(key_set,algorithm,options)==SIGNER_ERROR)
						return SIGNER_ERROR;
				}
			}
		}
	}
	
	/* No keys were found at the delegation point */
	if (key_set==NULL)
		return SIGNER_OK;

	/* Sign the keys */
	return key_list_copy (&key_set->rrs_signers, options->so_soa_keys, options);

}

int sign_check_suitable_signatures (struct name_rec *nr,
									struct rrset_rec *a_set,
									struct signing_options *options)
{
	/*
		For each algorithm that is in signing mode, make sure
		there is a signature for that algorithm.
	*/
	int	alg_number;

	if (nr->nr_status==NR_STATUS_APEX)
	{
		if (a_set->rrs_type_h == ns_t_key) return SIGNER_OK;
		if (a_set->rrs_type_h == ns_t_nxt) return SIGNER_OK;
	}

	else if (nr->nr_status==NR_STATUS_DELEGATION)
	{
		if (a_set->rrs_type_h != ns_t_key) return SIGNER_OK;
	}

	else if (nr->nr_status != NR_STATUS_IN)
	{
		return SIGNER_OK;
	}

	for (alg_number = VALUE_ALGORITHM_MINIMUM;
			alg_number <= options->so_max_algorithm;
				alg_number++)
	{
		struct rr_rec	*a_sig;
		int				found_one;

		if (options->so_signing_modes[alg_number] != SO_MODE_ZONE_KEY)
			continue;

		found_one = FALSE;

		for (a_sig = a_set->rrs_sig; a_sig; a_sig = a_sig->rr_next)
		{
			/*
				If there is a signature of the right algorithm,
				for procotol 3 or 255, and it is signed by either a
				zone key, wildcard key, or key of the same name,
				we're covered.
			*/

			/*
				For now, the if statement just looks at the flags,
				proto, and alg fields.
			*/

			/* Aack - of the key, not the signature! */

			if (a_sig->rr_rdata[SIG_RDATA_ALGORITHM] == (u_int8_t) alg_number)
			{
				u_int8_t			*signer_n;
				u_int8_t			algorithm;
				u_int16_t			footprint_n;
				struct name_rec		*signer = NULL;
				struct rrset_rec	*signers_keys = NULL;
				struct rr_rec		*the_key = NULL;

				/* Identify and retrieve the key */

				crypto_identify_key (a_sig,&signer_n,&algorithm,&footprint_n);

				if (((signer = tree_find_name (signer_n))==NULL)
						||
					((signers_keys=tree_find_set (signer, ns_t_key))==NULL)
						||
					((the_key = key_find_key (signers_keys, algorithm,
												ntohs(footprint_n))) ==NULL)
					)
				{
					/*
						There is no key material available to check this sig.
						Therefore, we can't check this signature so we can't
						count on it being good enough.
					*/

					/* See if the signer is outside the zone. */

					if (in_domain (options->so_zone_name_n, signer_n)==FALSE
							||
						found_ns (options->so_zone_name_n, signer_n)==TRUE)
					{
						continue;
					}
					else
					{
						/*
							The signer doesn't exist, the signature
							should have been already dropped
						*/
						wire_to_ascii_name (debug_name1_h, signer_n, MAXPNAME);
						sprintf (debug_message,
									"Signature by %s/%d somehow slipped by", 
									debug_name1_h, ntohs(footprint_n));
						WARN (debug_message);
						continue;
					}
				}
				/* Check its flags and protocol */

				if (the_key->rr_rdata[KEY_RDATA_PROTOCOL]==PROTOCOL_DNSSEC ||
						the_key->rr_rdata[KEY_RDATA_PROTOCOL]==PROTOCOL_ALL)
				{
					u_int16_t	flags_n;

					memcpy (&flags_n, the_key->rr_rdata, sizeof (u_int16_t));

					if (!(ntohs (flags_n) & VALUE_FLAGS_CANT_AUTHENTICATE))
					{
						found_one = TRUE;
						break;
					}
				}
			}

		}

		if (!found_one)
		{
			int dbgs;

			/* The set lacks a needed signature */

			wire_to_ascii_name (debug_name1_h, nr->nr_name_n, MAXPNAME);

			/* Maybe one will still come... */

			if (a_set->rrs_signers != NULL)
			{
				sprintf (debug_message,
					"%s/%s still needs a signature for algorithm number %d, %s",
					debug_name1_h, 
					local_sym_ntop(__p_type_syms,a_set->rrs_type_h,&dbgs),
					alg_number,
					"\n\tbut more signing passes are needed anyway.");
				WARN (debug_message);
			}
			else
			{
				sprintf (debug_message,
					"%s/%s needs a signature for algorithm number %d",
					debug_name1_h, 
					local_sym_ntop(__p_type_syms,a_set->rrs_type_h,&dbgs),
					alg_number);
				ERROR (debug_message);
				sign_cleanup (options);
				return SIGNER_ERROR;
			}
		}
	}

	return SIGNER_OK;
}

#define MATCH_POSITIVE	TRUE
#define MATCH_NEGATIVE	FALSE
void sign_move_rrset (struct rrset_rec **dest, struct rrset_rec **src,
						u_int16_t type_h, int match_flag)
{
	struct rrset_rec	*target;
	struct rrset_rec	*trailer = NULL;
	struct rrset_rec	*tail;

	target = *src;
	tail = *dest;
	while (tail && tail->rrs_next)
			tail = tail->rrs_next;

	/* Locate the target set */
	while (target)
	{
		if ((match_flag == MATCH_POSITIVE && target->rrs_type_h == type_h) ||
			(match_flag == MATCH_NEGATIVE && target->rrs_type_h != type_h))
		{
			/* Trim the target out of the list */
			if (trailer == NULL)
				*src = target->rrs_next;
			else
				trailer->rrs_next = target->rrs_next;

			if (*dest == NULL)
				*dest = target;
			else
				tail->rrs_next = target;

			tail = target;
			target->rrs_next = NULL;

			if (trailer)
				target = trailer->rrs_next;
			else
				target = *src;
		}
		else
		{
			trailer = target;
			target = target->rrs_next;
		}
	}

}

void sign_sort_rr_sets (struct name_rec *nm)
{
	/*
		Place the data in the following sequence :
					SOA (if present)
					KEY (if present)
					anything else other than NXT
					Upper NXT (if present)
					Lower NXT (if present)
	*/

	struct rrset_rec	*unsorted;

	unsorted = nm->nr_rrsets;
	nm->nr_rrsets = NULL;

	sign_move_rrset (&nm->nr_rrsets, &unsorted, ns_t_soa, MATCH_POSITIVE);
	sign_move_rrset (&nm->nr_rrsets, &unsorted, ns_t_key, MATCH_POSITIVE);
	sign_move_rrset (&nm->nr_rrsets, &unsorted, ns_t_nxt, MATCH_NEGATIVE);
	sign_move_rrset (&nm->nr_rrsets, &unsorted, ns_t_nxt, MATCH_POSITIVE);
}

/* Done */
int sign_a_name (struct name_rec *nm, struct signing_options *options)
{
	/* For the data sets in this name */
		/* If the name is in */
			/* Sign the data */ /* -- if signing key set, use soa/zone keys */
		/* Else if the name is apex */
			/* Treat the keys as "special" glue */
			/* Sign the data */
		/* Else if the name is parent */
			/* ... */
		/* Else if the name is delegation */
			/* If the set is key */
				/* Sign the data */
				/* If generating parent files is wanted */
					/* Make a parent file for the name if it isn't open */
					/* Add this to the file */
			/* Else */
				/* If the data is ns */
					/* Pass it unsigned */
					/* If generating parent files is wanted */
						/* Make a parent file for the name if it isn't open */
						/* Add this to the file */
				/* Else */
					/* Treat it as glue */

	struct rrset_rec	*a_set;

	if (nm->nr_status == NR_STATUS_APEX || nm->nr_status == NR_STATUS_IN ||
		nm->nr_status == NR_STATUS_DELEGATION)
	{
		options->so_stats.ss_names_in_zone++;
	}

	/*
		Place the data in the following sequence :
					SOA (if present)
					KEY (if present)
					anything else other than NXT
					Upper NXT (if present)
					Lower NXT (if present)
	*/

	sign_sort_rr_sets (nm);

	for (a_set = nm->nr_rrsets; a_set; a_set = a_set->rrs_next)
	{
		/*
			All NXT's are done as they are made
			- except the one made by the PARENT
		*/

		if (a_set->rrs_type_h == ns_t_nxt)
		{
			if (nm->nr_status != NR_STATUS_APEX) continue;

			if (a_set->rrs_data != NULL && a_set->rrs_data->rr_rdata != NULL)
			{
					int i = wire_name_length(a_set->rrs_data->rr_rdata);
	
					if (ISSET ((&a_set->rrs_data->rr_rdata[i]), ns_t_soa))
						continue;
			}
			else
				continue;
		}

		switch (nm->nr_status)
		{
			case NR_STATUS_APEX:
			case NR_STATUS_IN:
				if (sign_a_set (nm, a_set, options) == SIGNER_ERROR)
					return SIGNER_ERROR;
				break;

			case NR_STATUS_PARENT: /* UP policy */ /* DOWN: treat as glue */
				if (options->so_up_policy == FALSE)
				{
					if (sign_handle_glue (nm, a_set, options, TRUE)
														== SIGNER_ERROR)
						return SIGNER_ERROR;
				}
				else if (a_set->rrs_type_h == ns_t_key)
				{
					if (sign_a_set (nm, a_set, options) == SIGNER_ERROR)
						return SIGNER_ERROR;
				}
				else
				{
					if (sign_handle_glue (nm, a_set, options, TRUE)
														== SIGNER_ERROR)
						return SIGNER_ERROR;
				}
				break;

			case NR_STATUS_DELEGATION:
				if (a_set->rrs_type_h == ns_t_key)
				{
					if (sign_a_set (nm, a_set, options) == SIGNER_ERROR)
						return SIGNER_ERROR;
				}
				else if (a_set->rrs_type_h == ns_t_ns)
				{
					if (sign_a_set (nm, a_set, options) == SIGNER_ERROR)
						return SIGNER_ERROR;
				}
				else
				{
					if (sign_handle_glue (nm, a_set, options, TRUE)
														== SIGNER_ERROR)
						return SIGNER_ERROR;
				}
				break;

			default:
				if (sign_handle_glue (nm, a_set, options, TRUE)
														== SIGNER_ERROR)
					return SIGNER_ERROR;
				break;
		}

		/* Make sure the set is duly processed */

		if (sign_check_suitable_signatures (nm, a_set, options)==SIGNER_ERROR)
			return SIGNER_ERROR;
	}

	return SIGNER_OK;
}
	
/* Done */
int sign_name (struct name_rec *nm, struct signing_options *options)
{
	/* If this name is either unset, proto-*, or error */
		/* Close and unlink the output file */
		/* Place a return code in options.so_return_code */
		/* Return -1 */

	if (nm->nr_status==NR_STATUS_UNSET || nm->nr_status==NR_STATUS_PROTO_DELEG
	|| nm->nr_status==NR_STATUS_ERROR || nm->nr_status==NR_STATUS_PROTO_APEX)
	{
		wire_to_ascii_name (debug_name1_h, nm->nr_name_n, MAXPNAME);
		sprintf (debug_message,
			"%s is not properly configured - it is likely missing an NS set",
			debug_name1_h);
		ERROR (debug_message);
		return sign_cleanup (options);
	}

	/* If this name is not a subzone */
		/* If there is a previous name */
			/* If the current is not glue */
				/* Make the NXT for it with next name = this one */
			/* Else */
				/* Make the NXT for it with next name = zone name */
			/* Set the previous name to NULL */
	/* Else */
		/* Return 0; */

	if (nm->nr_status != NR_STATUS_SUBZONE)
	{
		if (options->so_previous_name != NULL)
		{
			if (options->so_do_nxts==TRUE)
			{
				if (nm->nr_status != NR_STATUS_GLUE &&
							nm->nr_status != NR_STATUS_PARENT)
				{
					if (sign_make_nxt (options, nm->nr_name_n)==SIGNER_ERROR)
						return sign_cleanup(options);
				}
				else
				{
					if(sign_make_nxt (options, options->so_zone_name_n)
															==SIGNER_ERROR)
						return sign_cleanup(options);
				}
			}

			/*
				If the previous name is a delegation point we may need
				to make a parent file
			*/

			if (options->so_previous_name->nr_status == NR_STATUS_DELEGATION &&
									sign_make_parent (options)==SIGNER_ERROR)
			{
				/* Couldn't make the parent */
				sign_cleanup(options);
				return SIGNER_ERROR;
			}
		}

		options->so_previous_name = NULL;
	}
	else
	{
		struct rrset_rec	*a_set;

		for (a_set = nm->nr_rrsets; a_set;a_set = a_set->rrs_next)
			if (sign_handle_glue (nm, a_set, options, TRUE) == SIGNER_ERROR)
					return FALSE;

		return TRUE;
	}

	/* If this name is glue */
		/* Pass it unsigned */
		/* Return 1 */

	if (nm->nr_status == NR_STATUS_GLUE)
	{
		struct rrset_rec	*a_set;

		for (a_set = nm->nr_rrsets; a_set;a_set = a_set->rrs_next)
			if (sign_handle_glue (nm, a_set, options, TRUE) == SIGNER_ERROR)
				return FALSE;

		return TRUE;
	}

	/* If the name is delegation - check for a key set */

	if (nm->nr_status == NR_STATUS_DELEGATION)
	{
		if (sign_check_delegation_point (nm, options)==SIGNER_ERROR)
		{
			return sign_cleanup(options);
		}
	}
	
	/* In the function ... for the data sets in this name do */

	if (sign_a_name (nm, options) == SIGNER_ERROR)
	{
		return sign_cleanup(options);
	}

	/* If the name is apex */

	if (nm->nr_status == NR_STATUS_APEX)
	{
		options->so_apex_record = nm;
	}

	/* Save this as the previous name */

	if (nm->nr_status != NR_STATUS_PARENT)
		options->so_previous_name = nm;

	return TRUE;
}

void sign_cat_in_glue (FILE **zonefile, char *gluefilename, char *zonefilename)
{
	/*
		If zone file is not open, open it.
		Open glue, read it and write it into zone.
	*/

	FILE *gluefile;
	char buffer[1024];
	int  buffer_size = 1024;
	int  chars_read;
	int  chars_written;
	int  next_to_write;

	if (*zonefile==NULL)
	{
		if (((*zonefile) = fopen (zonefilename, "w")) == NULL)
		{
			/* Can't open the zonefile */
			/* Test this - what if all data is glue and the file can't open? */
			sprintf (debug_message, "Can't open %s for writing",zonefilename);
			ERROR (debug_message);
			return;
		}
	}

	if ((gluefile = fopen (gluefilename, "r")) == NULL)
	{
		/*
			If this can't happen, something is really wrong - this file
			was just closed (for writing) a moment ago */
			sprintf (debug_message, "Can't open %s for reading",gluefilename);
			ERROR (debug_message);
			return;
	}


	while (!feof (gluefile))
	{
		chars_read = fread (buffer, sizeof(char), buffer_size, gluefile);

		if (chars_read == -1)
		{
				sprintf (debug_message, "Failed reading from %s",gluefilename);
				ERROR (debug_message);
				break;
		}

		chars_written = 0;
		next_to_write = 0;

		while (next_to_write < chars_read)
		{
			chars_written = fwrite (&buffer[next_to_write], sizeof(char),
				chars_read-next_to_write, *zonefile);
			if (chars_read == -1)
			{
					sprintf (debug_message,"Failed writing to %s",zonefilename);
					ERROR (debug_message);
					break;
			}

			next_to_write += chars_written;
		}
	}

	fclose (gluefile);
	unlink (gluefilename);
}

/* Done */
int sign_finish(struct signing_options *options)
{
	if (options->so_previous_name != NULL)
	{
		if (options->so_do_nxts &&
			sign_make_nxt (options, options->so_zone_name_n)==SIGNER_ERROR)
		{
			sign_cleanup(options);
			return SIGNER_ERROR;
		}

		if (options->so_previous_name->nr_status == NR_STATUS_DELEGATION &&
			sign_make_parent (options)==SIGNER_ERROR)
		{
			sign_cleanup(options);
			return SIGNER_ERROR;
		}
	}

	if (options->so_glue_output!=NULL)
	{
		fclose (options->so_glue_output);

		/* Move the contents of the glue to the end of the zone file */
		sign_cat_in_glue (&options->so_zone_output,
			options->so_output_glue_file,
			options->so_output_zone_file);
	}

	if (options->so_zone_output!=NULL) fclose (options->so_zone_output);
	if (options->so_parent_output!=NULL) fclose (options->so_parent_output);

	return SIGNER_OK;
}