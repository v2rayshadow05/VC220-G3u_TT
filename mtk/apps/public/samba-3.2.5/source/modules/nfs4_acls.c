/*
 * NFS4 ACL handling
 *
 * Copyright (C) Jim McDonough, 2006
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/>.
 */

#include "includes.h"
#include "nfs4_acls.h"

#undef DBGC_CLASS
#define DBGC_CLASS DBGC_ACLS

#define SMBACL4_PARAM_TYPE_NAME "nfs4"

extern const struct generic_mapping file_generic_mapping;

#define SMB_ACE4_INT_MAGIC 0x76F8A967
typedef struct _SMB_ACE4_INT_T
{
	uint32	magic;
	SMB_ACE4PROP_T	prop;
	void	*next;
} SMB_ACE4_INT_T;

#define SMB_ACL4_INT_MAGIC 0x29A3E792
typedef struct _SMB_ACL4_INT_T
{
	uint32	magic;
	uint32	naces;
	SMB_ACE4_INT_T	*first;
	SMB_ACE4_INT_T	*last;
} SMB_ACL4_INT_T;

extern struct current_user current_user;
extern int try_chown(connection_struct *conn, const char *fname, uid_t uid, gid_t gid);
extern NTSTATUS unpack_nt_owners(int snum, uid_t *puser, gid_t *pgrp,
	uint32 security_info_sent, SEC_DESC *psd);

static SMB_ACL4_INT_T *get_validated_aclint(SMB4ACL_T *acl)
{
	SMB_ACL4_INT_T *aclint = (SMB_ACL4_INT_T *)acl;
	if (acl==NULL)
	{
		DEBUG(2, ("acl is NULL\n"));
		errno = EINVAL;
		return NULL;
	}
	if (aclint->magic!=SMB_ACL4_INT_MAGIC)
	{
		DEBUG(2, ("aclint bad magic 0x%x\n", aclint->magic));
		errno = EINVAL;
		return NULL;
	}
	return aclint;
}

static SMB_ACE4_INT_T *get_validated_aceint(SMB4ACE_T *ace)
{
	SMB_ACE4_INT_T *aceint = (SMB_ACE4_INT_T *)ace;
	if (ace==NULL)
	{
		DEBUG(2, ("ace is NULL\n"));
		errno = EINVAL;
		return NULL;
	}
	if (aceint->magic!=SMB_ACE4_INT_MAGIC)
	{
		DEBUG(2, ("aceint bad magic 0x%x\n", aceint->magic));
		errno = EINVAL;
		return NULL;
	}
	return aceint;
}

SMB4ACL_T *smb_create_smb4acl(void)
{
	TALLOC_CTX *mem_ctx = talloc_tos();
	SMB_ACL4_INT_T	*acl = (SMB_ACL4_INT_T *)TALLOC_ZERO_SIZE(mem_ctx, sizeof(SMB_ACL4_INT_T));
	if (acl==NULL)
	{
		DEBUG(0, ("TALLOC_SIZE failed\n"));
		errno = ENOMEM;
		return NULL;
	}
	acl->magic = SMB_ACL4_INT_MAGIC;
	/* acl->first, last = NULL not needed */
	return (SMB4ACL_T *)acl;
}

SMB4ACE_T *smb_add_ace4(SMB4ACL_T *acl, SMB_ACE4PROP_T *prop)
{
	SMB_ACL4_INT_T *aclint = get_validated_aclint(acl);
	TALLOC_CTX *mem_ctx = talloc_tos();
	SMB_ACE4_INT_T *ace;

	ace = (SMB_ACE4_INT_T *)TALLOC_ZERO_SIZE(mem_ctx, sizeof(SMB_ACE4_INT_T));
	if (ace==NULL)
	{
		DEBUG(0, ("TALLOC_SIZE failed\n"));
		errno = ENOMEM;
		return NULL;
	}
	ace->magic = SMB_ACE4_INT_MAGIC;
	/* ace->next = NULL not needed */
	memcpy(&ace->prop, prop, sizeof(SMB_ACE4PROP_T));

	if (aclint->first==NULL)
	{
		aclint->first = ace;
		aclint->last = ace;
	} else {
		aclint->last->next = (void *)ace;
		aclint->last = ace;
	}
	aclint->naces++;

	return (SMB4ACE_T *)ace;
}

SMB_ACE4PROP_T *smb_get_ace4(SMB4ACE_T *ace)
{
	SMB_ACE4_INT_T *aceint = get_validated_aceint(ace);
	if (aceint==NULL)
		return NULL;

	return &aceint->prop;
}

SMB4ACE_T *smb_next_ace4(SMB4ACE_T *ace)
{
	SMB_ACE4_INT_T *aceint = get_validated_aceint(ace);
	if (aceint==NULL)
		return NULL;

	return (SMB4ACE_T *)aceint->next;
}

SMB4ACE_T *smb_first_ace4(SMB4ACL_T *acl)
{
	SMB_ACL4_INT_T *aclint = get_validated_aclint(acl);
	if (aclint==NULL)
		return NULL;

	return (SMB4ACE_T *)aclint->first;
}

uint32 smb_get_naces(SMB4ACL_T *acl)
{
	SMB_ACL4_INT_T *aclint = get_validated_aclint(acl);
	if (aclint==NULL)
		return 0;

	return aclint->naces;
}

static int smbacl4_GetFileOwner(struct connection_struct *conn,
				const char *filename,
				SMB_STRUCT_STAT *psbuf)
{
	memset(psbuf, 0, sizeof(SMB_STRUCT_STAT));

	/* Get the stat struct for the owner info. */
	if (SMB_VFS_STAT(conn, filename, psbuf) != 0)
	{
		DEBUG(8, ("SMB_VFS_STAT failed with error %s\n",
			strerror(errno)));
		return -1;
	}

	return 0;
}

static int smbacl4_fGetFileOwner(files_struct *fsp, SMB_STRUCT_STAT *psbuf)
{
	memset(psbuf, 0, sizeof(SMB_STRUCT_STAT));

	if (fsp->is_directory || fsp->fh->fd == -1) {
		return smbacl4_GetFileOwner(fsp->conn, fsp->fsp_name, psbuf);
	}
	if (SMB_VFS_FSTAT(fsp, psbuf) != 0)
	{
		DEBUG(8, ("SMB_VFS_FSTAT failed with error %s\n",
			strerror(errno)));
		return -1;
	}

	return 0;
}

static bool smbacl4_nfs42win(TALLOC_CTX *mem_ctx, SMB4ACL_T *acl, /* in */
	DOM_SID *psid_owner, /* in */
	DOM_SID *psid_group, /* in */
	SEC_ACE **ppnt_ace_list, /* out */
	int *pgood_aces /* out */
)
{
	SMB_ACL4_INT_T *aclint = (SMB_ACL4_INT_T *)acl;
	SMB_ACE4_INT_T *aceint;
	SEC_ACE *nt_ace_list = NULL;
	int good_aces = 0;

	DEBUG(10, ("smbacl_nfs42win entered"));

	aclint = get_validated_aclint(acl);
	/* We do not check for naces being 0 or acl being NULL here because it is done upstream */
	/* in smb_get_nt_acl_nfs4(). */
	nt_ace_list = (SEC_ACE *)TALLOC_ZERO_SIZE(mem_ctx, aclint->naces * sizeof(SEC_ACE));
	if (nt_ace_list==NULL)
	{
		DEBUG(10, ("talloc error"));
		errno = ENOMEM;
		return False;
	}

	for (aceint=aclint->first; aceint!=NULL; aceint=(SMB_ACE4_INT_T *)aceint->next) {
		SEC_ACCESS mask;
		DOM_SID sid;
		SMB_ACE4PROP_T	*ace = &aceint->prop;

		DEBUG(10, ("magic: 0x%x, type: %d, iflags: %x, flags: %x, mask: %x, "
			"who: %d\n", aceint->magic, ace->aceType, ace->flags,
			ace->aceFlags, ace->aceMask, ace->who.id));

		SMB_ASSERT(aceint->magic==SMB_ACE4_INT_MAGIC);

		if (ace->flags & SMB_ACE4_ID_SPECIAL) {
			switch (ace->who.special_id) {
			case SMB_ACE4_WHO_OWNER:
				sid_copy(&sid, psid_owner);
				break;
			case SMB_ACE4_WHO_GROUP:
				sid_copy(&sid, psid_group);
				break;
			case SMB_ACE4_WHO_EVERYONE:
				sid_copy(&sid, &global_sid_World);
				break;
			default:
				DEBUG(8, ("invalid special who id %d "
					"ignored\n", ace->who.special_id));
			}
		} else {
			if (ace->aceFlags & SMB_ACE4_IDENTIFIER_GROUP) {
				gid_to_sid(&sid, ace->who.gid);
			} else {
				uid_to_sid(&sid, ace->who.uid);
			}
		}
		DEBUG(10, ("mapped %d to %s\n", ace->who.id,
			   sid_string_dbg(&sid)));

		init_sec_access(&mask, ace->aceMask);
		init_sec_ace(&nt_ace_list[good_aces++], &sid,
			ace->aceType, mask,
			ace->aceFlags & 0xf);
	}

	*ppnt_ace_list = nt_ace_list;
	*pgood_aces = good_aces;

	return True;
}

static NTSTATUS smb_get_nt_acl_nfs4_common(const SMB_STRUCT_STAT *sbuf,
	uint32 security_info,
	SEC_DESC **ppdesc, SMB4ACL_T *acl)
{
	int	good_aces = 0;
	DOM_SID sid_owner, sid_group;
	size_t sd_size = 0;
	SEC_ACE *nt_ace_list = NULL;
	SEC_ACL *psa = NULL;
	TALLOC_CTX *mem_ctx = talloc_tos();

	if (acl==NULL || smb_get_naces(acl)==0)
		return NT_STATUS_ACCESS_DENIED; /* special because we
						 * shouldn't alloc 0 for
						 * win */

	uid_to_sid(&sid_owner, sbuf->st_uid);
	gid_to_sid(&sid_group, sbuf->st_gid);

	if (smbacl4_nfs42win(mem_ctx, acl, &sid_owner, &sid_group, &nt_ace_list, &good_aces)==False) {
		DEBUG(8,("smbacl4_nfs42win failed\n"));
		return map_nt_error_from_unix(errno);
	}

	psa = make_sec_acl(mem_ctx, NT4_ACL_REVISION, good_aces, nt_ace_list);
	if (psa == NULL) {
		DEBUG(2,("make_sec_acl failed\n"));
		return NT_STATUS_NO_MEMORY;
	}

	DEBUG(10,("after make sec_acl\n"));
	*ppdesc = make_sec_desc(mem_ctx, SEC_DESC_REVISION, SEC_DESC_SELF_RELATIVE,
	                        (security_info & OWNER_SECURITY_INFORMATION) ? &sid_owner : NULL,
	                        (security_info & GROUP_SECURITY_INFORMATION) ? &sid_group : NULL,
	                        NULL, psa, &sd_size);
	if (*ppdesc==NULL) {
		DEBUG(2,("make_sec_desc failed\n"));
		return NT_STATUS_NO_MEMORY;
	}

	DEBUG(10, ("smb_get_nt_acl_nfs4_common successfully exited with sd_size %d\n",
		   ndr_size_security_descriptor(*ppdesc, 0)));

	return NT_STATUS_OK;
}

NTSTATUS smb_fget_nt_acl_nfs4(files_struct *fsp,
			       uint32 security_info,
			       SEC_DESC **ppdesc, SMB4ACL_T *acl)
{
	SMB_STRUCT_STAT sbuf;

	DEBUG(10, ("smb_fget_nt_acl_nfs4 invoked for %s\n", fsp->fsp_name));

	if (smbacl4_fGetFileOwner(fsp, &sbuf)) {
		return map_nt_error_from_unix(errno);
	}

	return smb_get_nt_acl_nfs4_common(&sbuf, security_info, ppdesc, acl);
}

NTSTATUS smb_get_nt_acl_nfs4(struct connection_struct *conn,
			      const char *name,
			      uint32 security_info,
			      SEC_DESC **ppdesc, SMB4ACL_T *acl)
{
	SMB_STRUCT_STAT sbuf;

	DEBUG(10, ("smb_get_nt_acl_nfs4 invoked for %s\n", name));

	if (smbacl4_GetFileOwner(conn, name, &sbuf)) {
		return map_nt_error_from_unix(errno);
	}

	return smb_get_nt_acl_nfs4_common(&sbuf, security_info, ppdesc, acl);
}

enum smbacl4_mode_enum {e_simple=0, e_special=1};
enum smbacl4_acedup_enum {e_dontcare=0, e_reject=1, e_ignore=2, e_merge=3};

typedef struct _smbacl4_vfs_params {
	enum smbacl4_mode_enum mode;
	bool do_chown;
	enum smbacl4_acedup_enum acedup;
	struct db_context *sid_mapping_table;
} smbacl4_vfs_params;

/*
 * Gather special parameters for NFS4 ACL handling
 */
static int smbacl4_get_vfs_params(
	const char *type_name,
	files_struct *fsp,
	smbacl4_vfs_params *params
)
{
	static const struct enum_list enum_smbacl4_modes[] = {
		{ e_simple, "simple" },
		{ e_special, "special" }
	};
	static const struct enum_list enum_smbacl4_acedups[] = {
		{ e_dontcare, "dontcare" },
		{ e_reject, "reject" },
		{ e_ignore, "ignore" },
		{ e_merge, "merge" },
	};

	memset(params, 0, sizeof(smbacl4_vfs_params));
	params->mode = (enum smbacl4_mode_enum)lp_parm_enum(
		SNUM(fsp->conn), type_name,
		"mode", enum_smbacl4_modes, e_simple);
	params->do_chown = lp_parm_bool(SNUM(fsp->conn), type_name,
		"chown", True);
	params->acedup = (enum smbacl4_acedup_enum)lp_parm_enum(
		SNUM(fsp->conn), type_name,
		"acedup", enum_smbacl4_acedups, e_dontcare);

	DEBUG(10, ("mode:%s, do_chown:%s, acedup: %s\n",
		enum_smbacl4_modes[params->mode].name,
		params->do_chown ? "true" : "false",
		enum_smbacl4_acedups[params->acedup].name));

	return 0;
}

static void smbacl4_dump_nfs4acl(int level, SMB4ACL_T *acl)
{
	SMB_ACL4_INT_T *aclint = get_validated_aclint(acl);
	SMB_ACE4_INT_T *aceint;

	DEBUG(level, ("NFS4ACL: size=%d\n", aclint->naces));

	for(aceint = aclint->first; aceint!=NULL; aceint=(SMB_ACE4_INT_T *)aceint->next) {
		SMB_ACE4PROP_T *ace = &aceint->prop;

		DEBUG(level, ("\tACE: type=%d, flags=0x%x, fflags=0x%x, mask=0x%x, id=%d\n",
			ace->aceType,
			ace->aceFlags, ace->flags,
			ace->aceMask,
			ace->who.id));
	}
}

/* 
 * Find 2 NFS4 who-special ACE property (non-copy!!!)
 * match nonzero if "special" and who is equal
 * return ace if found matching; otherwise NULL
 */
static SMB_ACE4PROP_T *smbacl4_find_equal_special(
	SMB4ACL_T *acl,
	SMB_ACE4PROP_T *aceNew)
{
	SMB_ACL4_INT_T *aclint = get_validated_aclint(acl);
	SMB_ACE4_INT_T *aceint;

	for(aceint = aclint->first; aceint!=NULL; aceint=(SMB_ACE4_INT_T *)aceint->next) {
		SMB_ACE4PROP_T *ace = &aceint->prop;

		if (ace->flags == aceNew->flags &&
			ace->aceType==aceNew->aceType &&
			(ace->aceFlags&SMB_ACE4_IDENTIFIER_GROUP)==
			(aceNew->aceFlags&SMB_ACE4_IDENTIFIER_GROUP)
		) {
			/* keep type safety; e.g. gid is an u.short */
			if (ace->flags & SMB_ACE4_ID_SPECIAL)
			{
				if (ace->who.special_id==aceNew->who.special_id)
					return ace;
			} else {
				if (ace->aceFlags & SMB_ACE4_IDENTIFIER_GROUP)
				{
					if (ace->who.gid==aceNew->who.gid)
						return ace;
				} else {
					if (ace->who.uid==aceNew->who.uid)
						return ace;
				}
			}
		}
	}

	return NULL;
}

static bool nfs4_map_sid(smbacl4_vfs_params *params, const DOM_SID *src,
			 DOM_SID *dst)
{
	static struct db_context *mapping_db = NULL;
	TDB_DATA data;
	
	if (mapping_db == NULL) {
		const char *dbname = lp_parm_const_string(
			-1, SMBACL4_PARAM_TYPE_NAME, "sidmap", NULL);
		
		if (dbname == NULL) {
			DEBUG(10, ("%s:sidmap not defined\n",
				   SMBACL4_PARAM_TYPE_NAME));
			return False;
		}
		
		become_root();
		mapping_db = db_open(NULL, dbname, 0, TDB_DEFAULT,
				     O_RDONLY, 0600);
		unbecome_root();
		
		if (mapping_db == NULL) {
			DEBUG(1, ("could not open sidmap: %s\n",
				  strerror(errno)));
			return False;
		}
	}
	
	if (mapping_db->fetch(mapping_db, NULL,
			      string_term_tdb_data(sid_string_tos(src)),
			      &data) == -1) {
		DEBUG(10, ("could not find mapping for SID %s\n",
			   sid_string_dbg(src)));
		return False;
	}
	
	if ((data.dptr == NULL) || (data.dsize <= 0)
	    || (data.dptr[data.dsize-1] != '\0')) {
		DEBUG(5, ("invalid mapping for SID %s\n",
			  sid_string_dbg(src)));
		TALLOC_FREE(data.dptr);
		return False;
	}
	
	if (!string_to_sid(dst, (char *)data.dptr)) {
		DEBUG(1, ("invalid mapping %s for SID %s\n",
			  (char *)data.dptr, sid_string_dbg(src)));
		TALLOC_FREE(data.dptr);
		return False;
	}

	TALLOC_FREE(data.dptr);
	
	return True;
}

static bool smbacl4_fill_ace4(
	TALLOC_CTX *mem_ctx,
	const char *filename,
	smbacl4_vfs_params *params,
	uid_t ownerUID,
	gid_t ownerGID,
	SEC_ACE *ace_nt, /* input */
	SMB_ACE4PROP_T *ace_v4 /* output */
)
{
	DEBUG(10, ("got ace for %s\n", sid_string_dbg(&ace_nt->trustee)));

	memset(ace_v4, 0, sizeof(SMB_ACE4PROP_T));
	ace_v4->aceType = ace_nt->type; /* only ACCESS|DENY supported right now */
	ace_v4->aceFlags = ace_nt->flags & SEC_ACE_FLAG_VALID_INHERIT;
	ace_v4->aceMask = ace_nt->access_mask &
		(STD_RIGHT_ALL_ACCESS | SA_RIGHT_FILE_ALL_ACCESS);

	se_map_generic(&ace_v4->aceMask, &file_generic_mapping);

	if (ace_v4->aceFlags!=ace_nt->flags)
		DEBUG(9, ("ace_v4->aceFlags(0x%x)!=ace_nt->flags(0x%x)\n",
			ace_v4->aceFlags, ace_nt->flags));

	if (ace_v4->aceMask!=ace_nt->access_mask)
		DEBUG(9, ("ace_v4->aceMask(0x%x)!=ace_nt->access_mask(0x%x)\n",
			ace_v4->aceMask, ace_nt->access_mask));

	if (sid_equal(&ace_nt->trustee, &global_sid_World)) {
		ace_v4->who.special_id = SMB_ACE4_WHO_EVERYONE;
		ace_v4->flags |= SMB_ACE4_ID_SPECIAL;
	} else {
		const char *dom, *name;
		enum lsa_SidType type;
		uid_t uid;
		gid_t gid;
		DOM_SID sid;
		
		sid_copy(&sid, &ace_nt->trustee);
		
		if (!lookup_sid(mem_ctx, &sid, &dom, &name, &type)) {
			
			DOM_SID mapped;
			
			if (!nfs4_map_sid(params, &sid, &mapped)) {
				DEBUG(1, ("nfs4_acls.c: file [%s]: SID %s "
					  "unknown\n", filename, sid_string_dbg(&sid)));
				errno = EINVAL;
				return False;
			}
			
			DEBUG(2, ("nfs4_acls.c: file [%s]: mapped SID %s "
				  "to %s\n", filename, sid_string_dbg(&sid), sid_string_dbg(&mapped)));
			
			if (!lookup_sid(mem_ctx, &mapped, &dom,
					&name, &type)) {
				DEBUG(1, ("nfs4_acls.c: file [%s]: SID %s "
					  "mapped from %s is unknown\n",
					  filename, sid_string_dbg(&mapped), sid_string_dbg(&sid)));
				errno = EINVAL;
				return False;
			}
			
			sid_copy(&sid, &mapped);
		}
		
		if (type == SID_NAME_USER) {
			if (!sid_to_uid(&sid, &uid)) {
				DEBUG(1, ("nfs4_acls.c: file [%s]: could not "
					  "convert %s to uid\n", filename,
					  sid_string_dbg(&sid)));
				return False;
			}

			if (params->mode==e_special && uid==ownerUID) {
				ace_v4->flags |= SMB_ACE4_ID_SPECIAL;
				ace_v4->who.special_id = SMB_ACE4_WHO_OWNER;
			} else {
				ace_v4->who.uid = uid;
			}
		} else { /* else group? - TODO check it... */
			if (!sid_to_gid(&sid, &gid)) {
				DEBUG(1, ("nfs4_acls.c: file [%s]: could not "
					  "convert %s to gid\n", filename,
					  sid_string_dbg(&sid)));
				return False;
			}
				
			ace_v4->aceFlags |= SMB_ACE4_IDENTIFIER_GROUP;

			if (params->mode==e_special && gid==ownerGID) {
				ace_v4->flags |= SMB_ACE4_ID_SPECIAL;
				ace_v4->who.special_id = SMB_ACE4_WHO_GROUP;
			} else {
				ace_v4->who.gid = gid;
			}
		}
	}

	return True; /* OK */
}

static int smbacl4_MergeIgnoreReject(
	enum smbacl4_acedup_enum acedup,
	SMB4ACL_T *acl, /* may modify it */
	SMB_ACE4PROP_T *ace, /* the "new" ACE */
	bool	*paddNewACE,
	int	i
)
{
	int	result = 0;
	SMB_ACE4PROP_T *ace4found = smbacl4_find_equal_special(acl, ace);
	if (ace4found)
	{
		switch(acedup)
		{
		case e_merge: /* "merge" flags */
			*paddNewACE = False;
			ace4found->aceFlags |= ace->aceFlags;
			ace4found->aceMask |= ace->aceMask;
			break;
		case e_ignore: /* leave out this record */
			*paddNewACE = False;
			break;
		case e_reject: /* do an error */
			DEBUG(8, ("ACL rejected by duplicate nt ace#%d\n", i));
			errno = EINVAL; /* SHOULD be set on any _real_ error */
			result = -1;
			break;
		default:
			break;
		}
	}
	return result;
}

static SMB4ACL_T *smbacl4_win2nfs4(
	const char *filename,
	SEC_ACL *dacl,
	smbacl4_vfs_params *pparams,
	uid_t ownerUID,
	gid_t ownerGID
)
{
	SMB4ACL_T *acl;
	uint32	i;
	TALLOC_CTX *mem_ctx = talloc_tos();

	DEBUG(10, ("smbacl4_win2nfs4 invoked\n"));

	acl = smb_create_smb4acl();
	if (acl==NULL)
		return NULL;

	for(i=0; i<dacl->num_aces; i++) {
		SMB_ACE4PROP_T	ace_v4;
		bool	addNewACE = True;

		if (!smbacl4_fill_ace4(mem_ctx, filename, pparams,
				       ownerUID, ownerGID,
				       dacl->aces + i, &ace_v4)) {
			DEBUG(3, ("Could not fill ace for file %s, SID %s\n",
				  filename,
				  sid_string_dbg(&((dacl->aces+i)->trustee))));
			continue;
		}

		if (pparams->acedup!=e_dontcare) {
			if (smbacl4_MergeIgnoreReject(pparams->acedup, acl,
				&ace_v4, &addNewACE, i))
				return NULL;
		}

		if (addNewACE)
			smb_add_ace4(acl, &ace_v4);
	}

	return acl;
}

NTSTATUS smb_set_nt_acl_nfs4(files_struct *fsp,
	uint32 security_info_sent,
	SEC_DESC *psd,
	set_nfs4acl_native_fn_t set_nfs4_native)
{
	smbacl4_vfs_params params;
	SMB4ACL_T *acl = NULL;
	bool	result;

	SMB_STRUCT_STAT sbuf;
	bool need_chown = False;
	uid_t newUID = (uid_t)-1;
	gid_t newGID = (gid_t)-1;

	DEBUG(10, ("smb_set_nt_acl_nfs4 invoked for %s\n", fsp->fsp_name));

	if ((security_info_sent & (DACL_SECURITY_INFORMATION |
		GROUP_SECURITY_INFORMATION | OWNER_SECURITY_INFORMATION)) == 0)
	{
		DEBUG(9, ("security_info_sent (0x%x) ignored\n",
			security_info_sent));
		return NT_STATUS_OK; /* won't show error - later to be refined... */
	}

	/* Special behaviours */
	if (smbacl4_get_vfs_params(SMBACL4_PARAM_TYPE_NAME, fsp, &params))
		return NT_STATUS_NO_MEMORY;

	if (smbacl4_fGetFileOwner(fsp, &sbuf))
		return map_nt_error_from_unix(errno);

	if (params.do_chown) {
		/* chown logic is a copy/paste from posix_acl.c:set_nt_acl */
		NTSTATUS status = unpack_nt_owners(SNUM(fsp->conn), &newUID, &newGID, security_info_sent, psd);
		if (!NT_STATUS_IS_OK(status)) {
			DEBUG(8, ("unpack_nt_owners failed"));
			return status;
		}
		if (((newUID != (uid_t)-1) && (sbuf.st_uid != newUID)) ||
		    ((newGID != (gid_t)-1) && (sbuf.st_gid != newGID))) {
			need_chown = True;
		}
		if (need_chown) {
			if ((newUID == (uid_t)-1 || newUID == current_user.ut.uid)) {
				if(try_chown(fsp->conn, fsp->fsp_name, newUID, newGID)) {
					DEBUG(3,("chown %s, %u, %u failed. Error = %s.\n",
						 fsp->fsp_name, (unsigned int)newUID, (unsigned int)newGID, 
						 strerror(errno)));
					return map_nt_error_from_unix(errno);
				}

				DEBUG(10,("chown %s, %u, %u succeeded.\n",
					  fsp->fsp_name, (unsigned int)newUID, (unsigned int)newGID));
				if (smbacl4_GetFileOwner(fsp->conn, fsp->fsp_name, &sbuf))
					return map_nt_error_from_unix(errno);
				need_chown = False;
			} else { /* chown is needed, but _after_ changing acl */
				sbuf.st_uid = newUID; /* OWNER@ in case of e_special */
				sbuf.st_gid = newGID; /* GROUP@ in case of e_special */
			}
		}
	}

	if ((security_info_sent & DACL_SECURITY_INFORMATION)!=0 && psd->dacl!=NULL)
	{
		acl = smbacl4_win2nfs4(fsp->fsp_name, psd->dacl, &params, sbuf.st_uid, sbuf.st_gid);
		if (!acl)
			return map_nt_error_from_unix(errno);

		smbacl4_dump_nfs4acl(10, acl);

		result = set_nfs4_native(fsp, acl);
		if (result!=True)
		{
			DEBUG(10, ("set_nfs4_native failed with %s\n", strerror(errno)));
			return map_nt_error_from_unix(errno);
		}
	} else
		DEBUG(10, ("no dacl found; security_info_sent = 0x%x\n", security_info_sent));

	/* Any chown pending? */
	if (need_chown) {
		DEBUG(3,("chown#2 %s. uid = %u, gid = %u.\n",
			 fsp->fsp_name, (unsigned int)newUID, (unsigned int)newGID));
		if (try_chown(fsp->conn, fsp->fsp_name, newUID, newGID)) {
			DEBUG(2,("chown#2 %s, %u, %u failed. Error = %s.\n",
				 fsp->fsp_name, (unsigned int)newUID, (unsigned int)newGID,
				 strerror(errno)));
			return map_nt_error_from_unix(errno);
		}
		DEBUG(10,("chown#2 %s, %u, %u succeeded.\n",
			  fsp->fsp_name, (unsigned int)newUID, (unsigned int)newGID));
	}

	DEBUG(10, ("smb_set_nt_acl_nfs4 succeeded\n"));
	return NT_STATUS_OK;
}
