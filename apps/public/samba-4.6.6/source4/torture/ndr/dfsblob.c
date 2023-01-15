/*
   Unix SMB/CIFS implementation.

   Test DFS blobs.

   Copyright (C) Matthieu Patou <mat@matws.net> 2009-2010

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "includes.h"
#include "torture/ndr/ndr.h"
#include "librpc/gen_ndr/ndr_dfsblobs.h"
#include "torture/ndr/proto.h"
#include "librpc/gen_ndr/dfsblobs.h"

DATA_BLOB blob;
static const uint8_t dfs_get_ref_in[] = {
	0x03, 0x00, 0x5c, 0x00, 0x57, 0x00, 0x32, 0x00,
	0x4b, 0x00, 0x33, 0x00, 0x00, 0x00 };

static const uint8_t dfs_get_ref_out[] = {
	0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x03, 0x00, 0x22, 0x00, 0x00, 0x00, 0x02, 0x00,
	0x58, 0x02, 0x00, 0x00, 0x22, 0x00, 0x01, 0x00,
	0x3a, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x5c, 0x00, 0x6d, 0x00, 0x73, 0x00,
	0x77, 0x00, 0x32, 0x00, 0x6b, 0x00, 0x33, 0x00,
	0x2e, 0x00, 0x74, 0x00, 0x73, 0x00, 0x74, 0x00,
	0x00, 0x00, 0x5c, 0x00, 0x77, 0x00, 0x32, 0x00,
	0x6b, 0x00, 0x33, 0x00, 0x61, 0x00, 0x64, 0x00,
	0x76, 0x00, 0x7a, 0x00, 0x30, 0x00, 0x31, 0x00,
	0x2e, 0x00, 0x6d, 0x00, 0x73, 0x00, 0x77, 0x00,
	0x32, 0x00, 0x6b, 0x00, 0x33, 0x00, 0x2e, 0x00,
	0x74, 0x00, 0x73, 0x00, 0x74, 0x00, 0x00, 0x00};

static const uint8_t dfs_get_ref_out2[] = {
	0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x03, 0x00, 0x12, 0x00, 0x00, 0x00, 0x02, 0x00,
	0x58, 0x02, 0x00, 0x00, 0x24, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x03, 0x00, 0x12, 0x00, 0x00, 0x00,
	0x02, 0x00, 0x58, 0x02, 0x00, 0x00, 0x22, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x5c, 0x00, 0x57, 0x00,
	0x32, 0x00, 0x4b, 0x00, 0x38, 0x00, 0x52, 0x00,
	0x32, 0x00, 0x00, 0x00, 0x5c, 0x00, 0x77, 0x00,
	0x32, 0x00, 0x6b, 0x00, 0x38, 0x00, 0x72, 0x00,
	0x32, 0x00, 0x2e, 0x00, 0x6d, 0x00, 0x61, 0x00,
	0x74, 0x00, 0x77, 0x00, 0x73, 0x00, 0x2e, 0x00,
	0x6e, 0x00, 0x65, 0x00, 0x74, 0x00, 0x00, 0x00
};
static bool dfs_referral_out_check(struct torture_context *tctx, struct dfs_referral_resp *r)
{
	torture_assert_str_equal(tctx,
		r->referral_entries[0].referral.v3.referrals.r2.special_name,
		"\\msw2k3.tst", "Special name");
	ndr_push_struct_blob(&blob, tctx, r, (ndr_push_flags_fn_t)ndr_push_dfs_referral_resp);
	torture_assert_int_equal(tctx, blob.data[blob.length-2], 0, "expanded names not null terminated");
	torture_assert_int_equal(tctx, blob.data[blob.length-1], 0, "expanded names not null terminated");
	return true;
}

struct torture_suite *ndr_dfsblob_suite(TALLOC_CTX *ctx)
{
	struct torture_suite *suite = torture_suite_create(ctx, "dfsblob");

	torture_suite_add_ndr_pull_test(suite, dfs_GetDFSReferral_in, dfs_get_ref_in, NULL);

	torture_suite_add_ndr_pull_test(suite, dfs_referral_resp, dfs_get_ref_out2, NULL);

	torture_suite_add_ndr_pull_test(suite, dfs_referral_resp, dfs_get_ref_out,dfs_referral_out_check);

	return suite;
}