/*
 * crypto.c 
 *
 * Copyright (C) 2008 Samsung Electronics 
 *          SungMin Lee  <sung.min.lee@samsung.com>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public version 2 of License as published by
 * the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "crypto/crypto.h"
#include <xen/lib.h>
#include <security/ssm-xen/sra_func.h>

#undef CHECK_RETURN
#define CHECK_RETURN(cond, msg)	{ \
	if (!(cond))	\
{	\
	printf("%s\n", msg);	\
	return -1;	\
} }


#undef CHECK_DATA
#define CHECK_DATA(cond, msg)	{\
		int _ret = cond;	\
		if (_ret < 0 && _ret != -ENODATA) {	\
			printf("%s\n", msg);	\
			return -1;	\
		} }

/***********************************
 * local data
 ***********************************/


extern unsigned long sra_info_ptr;

/* master symmetric key */
static sym_key_t* sym_key;

static int block_sym_crypt(unsigned char *src, int src_len, unsigned char **dest_ptr, int *dest_len, int crypt_type);


/* Initialize public key, private key, and symmetric key */
int crypto_init(void)
{
	default_struct_t* part = NULL;

	/* get symmetric key */
	init_master_key();

	/* get public key (certificate) */
	part = sra_get_image(PART_SP1, CERTM_IMG);

	if (part == NULL)
	{
		printf("crypto_init(): can't get CERTM_IMG");
	}

	return 0;
}


/* if success, return 0, otherwise return negative number */
int crypto_sign_data(unsigned char* src, int src_len, unsigned char** sig, int* sig_len)
{
        printf("crypto_sign_data(): This function is not implemented!\n");

		return -1;
}


/* if success, return 0, otherwise return negative number */
int crypto_verify_data(unsigned char* src, int src_len, unsigned char* sig, int sig_len)
{
	
        printf("crypto_verify_data(): This function is not implemented!\n");

        return 0;
}


/* if success, return 0, otherwise return negative number */
int crypto_verify_data_with_certm(unsigned char* src, int src_len, unsigned char* sig, int sig_len)
{
        printf("crypto_verify_data_with_certm(): This function is not implemented!\n");
   
        return 0;
}


/* if success, return 0, otherwise return negative number */
int crypto_hash_data(unsigned char* src, int src_len, unsigned char** hash, int* hash_len)
{
        printf("crypto_hash_data(): This function is not implemented!\n");

		return -1;
}


/* if success, return 0, otherwise return negative number */
int crypto_encrypt_data(unsigned char *src, int src_len, unsigned char **enc, int *enc_len)
{
	int max_len = 0x7FFFFFFF;
	int err = 0;

	if (src == NULL || src_len <= 0)
		return -1;

	if (*enc != NULL)
		max_len = *enc_len;

	err = block_sym_crypt(src, src_len, enc, enc_len, SYMMETRIC_ENCRYPTION);

	if (max_len < *enc_len || err < 0)
		return -1;

	return err;
}


/* if success, return 0, otherwise return negative number */
int crypto_decrypt_data(unsigned char *src, int src_len, unsigned char **dec, int *dec_len)
{
	int max_len = 0x7FFFFFFF;
	int err = 0;

	if (src == NULL || src_len <= 0)
		return -1;

	if (*dec != NULL)
		max_len = *dec_len;

	err = block_sym_crypt(src, src_len, dec, dec_len, SYMMETRIC_DECRYPTION);

	if (max_len < *dec_len || err < 0)
		return -1;

	return err;
}


/* if success, return 0, otherwise return negative number */
int block_sym_crypt(unsigned char *src, int src_len, unsigned char **dest_ptr, int *dest_len, int crypt_type)
{
        printf("block_sym_crypt(): This function is not implemented!\n");

        if (src == NULL || dest_ptr == NULL || dest_len == NULL || *dest_ptr == NULL || *dest_len < src_len) {
          	printf("block_sym_crypt(): input parameter is not available!\n");
          	return -1;
	}

        memcpy(*dest_ptr, src, src_len);
        *dest_len = src_len;

	return 0;
}


/* if success, return 0, otherwise return negative number */
int init_master_key(void)
{
	ssb_transfer_container_t *tc = (ssb_transfer_container_t *)sra_info_ptr;
	transfer_struct_t *ts = _find_transfer_image(tc, TRANSFER_MASTER_KEY);

	if (ts == NULL) {
		printf("init_master_key(): no symmetric key\n");
	}

        /* symmetric key initialization code must be followed: */ 
        sym_key = NULL;

	return 0;
}
