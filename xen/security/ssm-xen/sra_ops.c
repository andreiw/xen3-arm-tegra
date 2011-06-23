/*
 * sra_ops.c 
 *
 * Copyright (C) 2008 Samsung Electronics 
 *          SangDok Mo  <sd.mo@samsung.com>
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

#include <xen/sched.h>
#include <xen/event.h>
#include <xen/errno.h>
#include <xen/iocap.h>
#include <xen/irq.h>
#include <asm/uaccess.h>
#include <asm/current.h>
#include <security/acm/acm_hooks.h>
#include <security/acm/aci_generator.h>

#include <public/security/sra_ops.h>
#include <security/ssm-xen/sra_func.h>
#include <security/crypto/crypto.h>

long do_sra_ops(struct sra_op *uop)
{
	long rc = 0;
#ifdef CONFIG_VMM_SECURITY
        struct sra_op op;
 
        if ( copy_from_user(&op, uop, sizeof(op)) != 0 )
        	return -EFAULT;
 
        switch ( op.cmd )
        {
        	case SRA_OP_READ:
            		rc = sra_func_read(&op.u.read_image);

            		if ( (rc == 0) && (copy_to_user(uop, &op, sizeof(op)) != 0) )
                		rc = -EFAULT; /* Cleaning up here would be a mess! */
            		break;
 
        	case SRA_OP_READ_PART_INFO:
            		rc = sra_func_read_part_info(&op.u.read_part);

            		if ( (rc == 0) && (copy_to_user(uop, &op, sizeof(op)) != 0) )
                		rc = -EFAULT; /* Cleaning up here would be a mess! */
            		break;
 
        	case SRA_OP_READ_ENC_PART:
            		rc = sra_func_read_enc_part(&op.u.read_enc_image);

            		if ( (rc == 0) && (copy_to_user(uop, &op, sizeof(op)) != 0) )
                		rc = -EFAULT; /* Cleaning up here would be a mess! */
            		break;
 
        	case SRA_OP_WRITE:
            		rc = sra_func_write(&op.u.write_image);

            		if ( (rc == 0) && (copy_to_user(uop, &op, sizeof(op)) != 0) )
                		rc = -EFAULT; /* Cleaning up here would be a mess! */
            		break;
 
        	case SRA_OP_UPDATE:
            		rc = sra_func_update(&op.u.update_image);

              		if ( (rc == 0) && (copy_to_user(uop, &op, sizeof(op)) != 0) )
                  		rc = -EFAULT; /* Cleaning up here would be a mess! */
            		break;
 
        	case SRA_OP_VERIFY:
        		rc = sra_func_verify(&op.u.verify_image);

             	 	if ( (rc == 0) && (copy_to_user(uop, &op, sizeof(op)) != 0) )
                  		rc = -EFAULT; /* Cleaning up here would be a mess! */
            		break;
 
        	case SRA_OP_COMPARE:
            		rc = sra_func_compare(&op.u.comp_image);

              		if ( (rc == 0) && (copy_to_user(uop, &op, sizeof(op)) != 0) )
                  		rc = -EFAULT; /* Cleaning up here would be a mess! */
            		break;
 
        	default:
            		rc = -ENOSYS;
            		break;
        }

#endif
    	return rc;
}

int sra_op_init(struct domain *d)
{
	return 0;
}

long sra_func_read(sra_op_read_t *op_read)
{
	long           rc = 0;
#ifdef CONFIG_VMM_SECURITY
        default_struct_t *pds = NULL;
 
        partition_type_t ptype = PART_END;
        static spinlock_t sra_read_lock = SPIN_LOCK_UNLOCKED;
 
        spin_lock(&sra_read_lock);
 
        if (op_read == NULL)
        	ERROR_EXIT(-EINVAL);
 
        op_read->rc = 0;
 
        if (VERIFY_IMAGES(op_read->image.type) && VERIFY_KERNEL_AND_SIG(op_read->image.type)) {
 
            	/* read kernel location info */
        	ptype = PART_OS_IMAGE;
 
        } else {
        	/* read image in SP3 partition */
        	ptype = PART_SP3;
        }
 
        if (!acm_sra_read(ptype)) {
         op_read->rc = -EACCES;
         ERROR_EXIT(-EACCES);
        }

        pds = sra_get_image(ptype, op_read->image.type);
 
        if (pds == NULL) {
        	op_read->rc = -ENODATA;
        	ERROR_EXIT(0);
        }
 
        if ((ptype != PART_OS_IMAGE) && (op_read->image.size < pds->size)) {
        	op_read->rc = -EMSGSIZE;
        	ERROR_EXIT(0);
        }
 
        op_read->image.size = pds->size;
 
        if (ptype == PART_OS_IMAGE) {
        	/* for kernel image information */
        	op_read->image.u.pos = pds->u.pos;
 
        } else {
        	/* for SP3's image information */
        	if (copy_to_user(op_read->image.u.ptr, pds->u.ptr, pds->size) != 0) 
            	rc = -ENOMEM;
        }

 out:
 	spin_unlock(&sra_read_lock);
#endif
    return rc;
}


long sra_func_read_part_info(sra_op_read_part_info_t *op_read_part)
{
	long           rc = 0;
#ifdef CONFIG_VMM_SECURITY
        static spinlock_t sra_read_part_lock = SPIN_LOCK_UNLOCKED;
 
        if (op_read_part == NULL) ERROR_EXIT(-EINVAL);
	if (op_read_part->part.ptype < PART_MBB || 
	    op_read_part->part.ptype >= PART_END) {
	  op_read_part->rc = -EACCES;
	  return rc;
	}

	/*
		  if(!acm_sra_read(op_read_part->part.ptype)){
			  op_read_part->rc = -EACCES;
			  return rc;
		  }
	*/
 
        spin_lock(&sra_read_part_lock);
        op_read_part->rc = 0;
        rc = _get_mtd_partition(&(op_read_part->part));
 
        if (rc != 0) {
            op_read_part->rc = -ENODATA;
            ERROR_EXIT(0);
        }

 out:
 	spin_unlock(&sra_read_part_lock);
#endif
    	return rc;
}

long sra_func_read_enc_part(sra_op_read_enc_part_t *op_read_enc)
{
	long rc = 0;
#ifdef CONFIG_VMM_SECURITY
	int i, j;

	transfer_struct_t ts, *part = NULL;
	ssb_master_boot_block_t *mbb = NULL;
        ssb_master_boot_block_t lmbb;
        static spinlock_t sra_read_enc_lock = SPIN_LOCK_UNLOCKED;
        
        spin_lock(&sra_read_enc_lock);
 
        if (op_read_enc == NULL) ERROR_EXIT(-EINVAL);
 
        part = op_read_enc->part;
        mbb = &(op_read_enc->part_pos);
        op_read_enc->rc = 0;
 
        lmbb = *sra_get_mbb();
 
        for (i=0; i<op_read_enc->size; i++) {
        	if (part[i].type != PART_MBB && part[i].type != PART_SP1 &&
            	part[i].type != PART_SP2 && part[i].type != PART_SP3 &&
            	part[i].type != PART_OS_IMAGE) {
            		op_read_enc->rc = -EINVAL;
            		ERROR_EXIT(0);
          	}

          	if (!acm_sra_read_enc_part(part[i].type)) {
            		op_read_enc->rc = -EACCES;
            		ERROR_EXIT(0);
          	}
 
         	mbb->mtd_part[i].ptype = PART_END;
          	ts = sra_get_bin(part[i].type);
 
          	if (ts.type != PART_END) {
            		/* request partition encryption */
            		if (access_ok_type(VERIFY_WRITE, part[i].ptr, part[i].size))
              			rc = sra_make_enc_part(part[i].ptr, part[i].size, ts.ptr, ts.size, ts.type);

            		if (rc <= 0) {
              			if (rc == -EMSGSIZE) op_read_enc->rc = rc;
              			else op_read_enc->rc = -ENOEXEC;

              			ERROR_EXIT(0);
            		}
 
            		op_read_enc->part[i].size = rc;
 
            		/* get partition position */
            		for (j=0; j<lmbb.psize; j++) {
            			if (lmbb.mtd_part[j].ptype == ts.type) break;
            		}
 
            		if (j == lmbb.psize) {
            			op_read_enc->rc = -ENODEV;
            			ERROR_EXIT(0);
            		} else {
            			mbb->mtd_part[i] = lmbb.mtd_part[j];
            		}
          	}	
        }
 
        mbb->psize = i;
        rc = 0;

 out:
	spin_unlock(&sra_read_enc_lock);
#endif
	return rc;
}

/* for SP3 */
long sra_func_write(sra_op_write_t *op_write)
{
	long rc = 0;
#ifdef CONFIG_VMM_SECURITY
	int result = 0;

        default_struct_t *pds=NULL;
        default_struct_t *psig=NULL;
        static spinlock_t sra_write_lock = SPIN_LOCK_UNLOCKED;
 
        if (op_write == NULL) ERROR_EXIT(-EINVAL);
 
        spin_lock(&sra_write_lock);
        op_write->rc = 0;
        pds = &(op_write->image);
        psig = &(op_write->sig);
 
        if (!VERIFY_SP3(pds->type))
        {
        	op_write->rc = -EACCES;
        	ERROR_EXIT(0);
        }

		  if (!acm_sra_write(PART_SP3, 0))
        {
        	op_write->rc = -EACCES;
        	ERROR_EXIT(0);
        }

        /* memory verification */
        if (!access_ok_type(VERIFY_READ, pds->u.ptr, pds->size)) {
        	op_write->rc = -EACCES;
          	ERROR_EXIT(0);
        }
 
        if (psig->type != IMAGE_END) {
        	if (pds->type == psig->type) {
            		if (!access_ok_type(VERIFY_READ, psig->u.ptr, psig->size)) {
              			op_write->rc = -EACCES;
             	 		ERROR_EXIT(0);
            		}

            		/* verifying */
            		result = crypto_verify_data((unsigned char*)pds->u.ptr, pds->size, 
            			 (unsigned char*)psig->u.ptr, psig->size);

            		if (result != noError) {
              			op_write->rc = -EACCES;
              			ERROR_EXIT(0);
            		}
          	} else {
            		op_write->rc = -EINVAL;
            		ERROR_EXIT(0);
          	}
        }
 
        /* writing */
        result = sra_set_image(pds, PARTITION_INFO(pds->type));
        if (result != 0) {
        	op_write->rc = -ENOEXEC;
        	ERROR_EXIT(0);
        }

 out:
	spin_unlock(&sra_write_lock);
#endif
	return rc;
}

/* for SP1 */
long sra_func_update(sra_op_update_t *op_update)
{
	long rc = 0;
#ifdef CONFIG_VMM_SECURITY
        int result = 0;
 
        unsigned char *enc_ac = NULL;
        u_int32_t lcrc32;
        default_struct_t img;
        default_struct_t sig;
        default_struct_t *pss=NULL;
 
        unsigned char pad[0x10] = {0, };
        static spinlock_t sra_update_lock = SPIN_LOCK_UNLOCKED;
 
        spin_lock(&sra_update_lock);

        if (op_update == NULL)
        	ERROR_EXIT(-EINVAL);
 
        if (op_update->image.u.ptr == NULL || op_update->sig.u.ptr == NULL) {
        	op_update->rc = -EINVAL;
        	ERROR_EXIT(0);
        }
 
        op_update->rc = 0;
        img = op_update->image;
        sig = op_update->sig;
 
        if (VERIFY_RESERVED(img.type) || VERIFY_SP2(img.type) || VERIFY_SP3(img.type))
        {
        	op_update->rc = -EACCES;
        	ERROR_EXIT(0);
        }
 
        if (!VERIFY_IMAGES(img.type) || 
            (img.type & 0x00000fff) != (sig.type & 0x00000fff))
        {
        	op_update->rc = -EINVAL;
        	ERROR_EXIT(0);
        }
     
		  if (!acm_sra_write(PART_SP1, sig.type))
        {
        	op_update->rc = -EACCES;
        	ERROR_EXIT(0);
        }

        /* kernel memory access check */
        if (!access_ok_type(VERIFY_READ, img.u.ptr, img.size)) {
        	op_update->rc = -EACCES;
        	ERROR_EXIT(0);
        }
 
        if (!access_ok_type(VERIFY_READ, sig.u.ptr, sig.size)) {
        	op_update->rc = -EACCES;
        	ERROR_EXIT(0);
        }
 
        if (img.type == XEN_ARM_IMG || VERIFY_KERNEL_AND_SIG(img.type)) {
        	if (op_update->access_code.u.ptr != NULL) {
        		if (!access_ok_type(VERIFY_WRITE, op_update->access_code.u.ptr, op_update->access_code.size)) {
              			op_update->rc = -EACCES;
              			ERROR_EXIT(0);
            		}
          	} else 
		{
	        	op_update->rc = -EINVAL;
            		ERROR_EXIT(0);
          	}

        	if (img.type == XEN_ARM_IMG) {
        		if (op_update->vmm.size < (((img.size/0x100)+1)*0x100)) {
              			op_update->rc = -EMSGSIZE;
              			ERROR_EXIT(0);
            		}

            		if (op_update->vmm.u.ptr != NULL) {
              			if (!access_ok_type(VERIFY_WRITE, op_update->vmm.u.ptr, op_update->vmm.size)) {
                			op_update->rc = -EACCES;
                			ERROR_EXIT(0);
              			}
            		} else 
			{
              			op_update->rc = -EINVAL;
              			ERROR_EXIT(0);
            		}
        	}
        }
 
        /* verifying: use the guest domain address (no conversion) */
        result = crypto_verify_data((unsigned char*)img.u.ptr, img.size, 
            			(unsigned char*)sig.u.ptr, sig.size);

        if (result != noError) {
        	op_update->rc = -EACCES;
          	ERROR_EXIT(0);
        }
 
        /* VMM encryption: use the guest domain address (no conversion) */
        if (img.type == XEN_ARM_IMG) {
        	result = sra_make_enc_part(op_update->vmm.u.ptr, op_update->vmm.size, 
            	   			   img.u.ptr, img.size, PART_SUB_VMM_IMAGE);
        	if (result <= 0) {
        		op_update->rc = -ENOEXEC;
            		ERROR_EXIT(0);
        	}

        	op_update->vmm.size = result;
        	op_update->vmm.type = img.type;

        }
 
        /* update image */
        if (img.type == CERTM_IMG) { /* image update */
        	pss = &img;
        } else 
	{ 
		/* signature update */
        	pss = &sig;
        }
 
        result = sra_set_image(pss, PART_SP1);

        if (result != 0) {
        	op_update->rc = -ENOEXEC;
          	ERROR_EXIT(0);
        }
 
        /* make access code */
        if (img.type == XEN_ARM_IMG || VERIFY_KERNEL_AND_SIG(img.type)) {
        	if (op_update->access_code.u.ptr == NULL) {
            		op_update->rc = -EINVAL;
            		ERROR_EXIT(0);
          	}

          	if (img.type == XEN_ARM_IMG) {
            		pss = &(op_update->vmm);
          	} else 
		{
            		pss = &img;
          	}
 
        	op_update->access_code.type = pss->type;
        	lcrc32 = crc32(0, (unsigned char*)pss->u.ptr, pss->size);
        	memcpy(pad, &lcrc32, sizeof(lcrc32));
        	enc_ac = _page_alloc(NULL, 256);
        	result = crypto_encrypt_data(pad, sizeof(lcrc32), 
             				    (unsigned char**)&enc_ac, 
            				    (int*)&(op_update->access_code.size));
        	if (result < 0) {
        		op_update->rc = -EFAULT;
            		ERROR_EXIT(0);
        	}

        	copy_to_user(op_update->access_code.u.ptr, enc_ac, op_update->access_code.size);
        }

 out:
	if (enc_ac != NULL)
      		_page_alloc(enc_ac, 0);

    	spin_unlock(&sra_update_lock);
#endif
	return rc;
}


long sra_func_verify(sra_op_verify_t *op_verify)
{
	long rc = 0;
#ifdef CONFIG_VMM_SECURITY
    	int result = 0, outlen=4;
    	u_int32_t lcrc32, ac_crc32;
    	default_struct_t *pac = NULL, *pss = NULL;
    	ssb_mtd_partition_t ssb_mp;
    	unsigned char *crc32_buf=NULL;

    	static spinlock_t sra_verify_lock = SPIN_LOCK_UNLOCKED;

    	memset(&ssb_mp, 0, sizeof(ssb_mp));

        spin_lock(&sra_verify_lock);
 
        if (op_verify == NULL) ERROR_EXIT(-EINVAL);
 
        op_verify->rc = 0;

        if (op_verify->image.u.ptr == NULL || op_verify->access_code.u.ptr == NULL) {
        	op_verify->rc = -EINVAL;
          	ERROR_EXIT(0);
        }
 
        pss = &(op_verify->image);
        pac = &(op_verify->access_code);
 
        if (pac->type != pss->type) {
        	op_verify->rc = -EINVAL;
          	ERROR_EXIT(0);
        }
 
        /* kernel memory access check */
        if (!access_ok_type(VERIFY_READ, pss->u.ptr, pss->size)) {
        	op_verify->rc = -EACCES;
            	ERROR_EXIT(0);
        }
        if (!access_ok_type(VERIFY_READ, pac->u.ptr, pac->size)) {
        	op_verify->rc = -EACCES;
            	ERROR_EXIT(0);
        }
 
        if (VERIFY_IMAGES(pac->type) && 
            (pac->type == XEN_ARM_IMG || VERIFY_KERNEL_AND_SIG(pac->type))) {
        	if (pac->size > 32) {
            		op_verify->rc = -EFAULT;
            		ERROR_EXIT(0);
          	}
 
                /* Access Code decryption */
                crc32_buf = _page_alloc(NULL, 256);
                outlen = pac->size;
                result = crypto_decrypt_data((unsigned char*)pac->u.ptr, pac->size, (unsigned char **)&crc32_buf, &outlen);
                memcpy(&ac_crc32, crc32_buf, 4);
 
                if (result < 0) {
                	op_verify->rc = -EFAULT;
                  	ERROR_EXIT(0);
                }
  
                /* verify access code */
                lcrc32 = crc32(0, (unsigned char*)pss->u.ptr, pss->size);
                if (lcrc32 != ac_crc32) {
                	op_verify->rc = -EACCES;
                	ERROR_EXIT(0);
                }
  
                /* find location */
                if (VERIFY_IMAGES(pss->type) && VERIFY_KERNEL_AND_SIG(pss->type)) {
                	pac = sra_get_image(PART_OS_IMAGE, pss->type);
 
                	if (pac == NULL) {
                        	op_verify->rc = -ENODATA;
                     		ERROR_EXIT(0);
                    	}
 
                    	pac->size = pss->size;
                    	ssb_mp.ptype = PART_OS_IMAGE;
                    	_get_mtd_partition(&ssb_mp);
                    	ssb_mp.data_offset = pac->u.pos;
                    	ssb_mp.size = pac->size;
  
                    	/* OS image information update */
                    	result = sra_set_image(pac, PART_OS_IMAGE);
 
                    	if (result != 0) {
                      		op_verify->rc = -ENOMEM;
                      		ERROR_EXIT(0);
                    	}
  
                } else if (pss->type == XEN_ARM_IMG) {
 	       		ssb_mp.ptype = PART_SUB_VMM_IMAGE;
 	       		_get_mtd_partition(&ssb_mp);
       	     	}
 
       	     	op_verify->pos = ssb_mp;

    	} else if (pac->type == IMAGE_END) { 

		/* unknown image verification with signature */
      		result = crypto_verify_data((unsigned char*)pss->u.ptr, pss->size,
					    (unsigned char*)pac->u.ptr, pac->size);
      	if (result != noError) {
		op_verify->rc = -EACCES;
		ERROR_EXIT(0);
      	}

    	} else 
	{
      		ERROR_EXIT(-EINVAL);
    	}

 out:
	_page_alloc(crc32_buf, 0);
	spin_unlock(&sra_verify_lock);
#endif
	return rc;
}

#define SUCCESS 1
#define FAIL    0

int _equal_mbb(ssb_master_boot_block_t *mbb1, ssb_master_boot_block_t *mbb2);
int _equal_sp (ssb_image_container_t *sp1, ssb_image_container_t *sp2, partition_type_t ptype);

long sra_func_compare(sra_op_compare_t *op_compare)
{
  	long rc = 0;
#ifdef CONFIG_VMM_SECURITY
        int i, dsize;
        ssb_image_container_t sp1, *psp2=NULL;
        ssb_master_boot_block_t mbb1, *pmbb2=NULL;
        partition_type_t ptype;
        image_type_t itype;
        default_struct_t *pds=NULL, *pimg=NULL;
        ssb_mtd_partition_t ssb_mp, *pmp=NULL;
        ssb_transfer_container_t ltc;
        ssb_master_boot_block_t lmbb;
        unsigned char *ptemp = NULL;
   
        static spinlock_t sra_compare_lock = SPIN_LOCK_UNLOCKED;
   
        if (op_compare == NULL) ERROR_EXIT(-EINVAL);

        spin_lock(&sra_compare_lock);
        op_compare->rc = 0;
   
        ptype = op_compare->ptype;
        itype = op_compare->itype;
   
        switch (op_compare->cmd) {

        	case SRA_OP_COMPARE_IMAGE:
          	{
            		pds = &(op_compare->image);

            		if ((ptype != PART_SP1 && ptype != PART_SP2 && 
                 	     ptype != PART_SP3 && ptype != PART_OS_IMAGE) ||
                   	    (ptype != PARTITION_INFO(pds->type))) {
                  		op_compare->rc = -EACCES;
                  		ERROR_EXIT(0);
            		}
   
            		/* memory verification */
            		if (!access_ok_type(VERIFY_READ, pds->u.ptr, pds->size)) {
              			op_compare->rc = -EACCES;
              			ERROR_EXIT(0);
            		}
   
            		pimg = sra_get_image(ptype, pds->type);
            		if (pimg == NULL) {
              			op_compare->rc = -ENODATA;
              			ERROR_EXIT(0);
            		}
   
            		if (pds->size != pimg->size) {
              			op_compare->rc = -EINVAL;
              			ERROR_EXIT(0);
            		}
   
            		if (ptype == PART_OS_IMAGE) {
              			if (pds->u.pos != pimg->u.pos) {
                			op_compare->rc = -EINVAL;
                			ERROR_EXIT(0);
              			}
            		} else {
              			for (i=0; i<pds->size; i++) {
                			if (pds->u.ptr[i] != pimg->u.ptr[i]) break;
              			}
   
              			if (i != pds->size) {
                			op_compare->rc = -EINVAL;
                			ERROR_EXIT(0);
              			}
            		}
          	}

        	break;
   
                case SRA_OP_COMPARE_POS:
                {
                	pmp = &(op_compare->pos);
           
                    	if (VERIFY_IMAGES(itype) && VERIFY_KERNEL_AND_SIG(itype)) {
                      		pds = sra_get_image(PART_OS_IMAGE, itype);
 
                      		if (pds == NULL) {
                        		op_compare->rc = -ENODATA;
                        		ERROR_EXIT(0);
                      		}
 
                      		ssb_mp.ptype = PART_OS_IMAGE;
                      		_get_mtd_partition(&ssb_mp);
                      		ssb_mp.data_offset = pds->u.pos;
                      		ssb_mp.size = pds->size;
           
                    	} else if (itype == XEN_ARM_IMG) {
 	       			ssb_mp.ptype = PART_SUB_VMM_IMAGE;
 	       			_get_mtd_partition(&ssb_mp);
       	     		} else {
 	       			op_compare->rc = -ENODATA;
 	       			ERROR_EXIT(0);
       	     		}
 
       	     			if (ssb_mp.ptype != pmp->ptype || ssb_mp.mtd_tb_num != pmp->mtd_tb_num ||
 	         		    ssb_mp.part_offset != pmp->part_offset || ssb_mp.data_offset != pmp->data_offset ||ssb_mp.size != pmp->size ||
 	         		    ssb_mp.max_size != pmp->max_size || ssb_mp.mask_flags != pmp->mask_flags) {
 	       				op_compare->rc = -EINVAL;
 	       				ERROR_EXIT(0);
       	     			}
     	   	}
 
     	   	break;
 
   	 	case SRA_OP_COMPARE_PART_INFO:
     	   	{
       	     		pmp = &(op_compare->pos);
       	     		ssb_mp.ptype = pmp->ptype;
 
       	     		if (_get_mtd_partition(&ssb_mp) != 0) {
 	       			op_compare->rc = -ENODATA;
 	       			ERROR_EXIT(0);
 
       	     		}
 
       	     		if (ssb_mp.ptype != pmp->ptype || ssb_mp.mtd_tb_num != pmp->mtd_tb_num ||
 	         	    ssb_mp.part_offset != pmp->part_offset || ssb_mp.data_offset != pmp->data_offset || ssb_mp.size != pmp->size ||
 	         	    ssb_mp.max_size != pmp->max_size || ssb_mp.mask_flags != pmp->mask_flags) {
 	       			op_compare->rc = -EINVAL;
 	       			ERROR_EXIT(0);
       	     		}
     	   	}
 
     	   	break;
 
   	 	case SRA_OP_COMPARE_PARTS:
     	   	{
       	     		ltc = op_compare->parts;
       	     		lmbb = op_compare->parts_pos;
 
       	     		/* for loop start */
       	     		for (i=0; i<ltc.images_size; i++) {
 	       			ptemp = _page_alloc(ptemp, ltc.image[i].size);
 
 	       			if (ptemp == NULL) {
 	        			 op_compare->rc = -ENOMEM;
 	        			 ERROR_EXIT(0);
 	       			}
 
 	       			dsize = ltc.image[i].size;
 
 	       			if (!access_ok_type(VERIFY_READ, ltc.image[i].ptr, dsize)) {
 	         			op_compare->rc = -EACCES;
 	         			ERROR_EXIT(0);
 	       			}
 
 	       			if (crypto_decrypt_data((unsigned char*)ltc.image[i].ptr, 
 	       				ltc.image[i].size, &ptemp, &dsize) < 0) {
 	        			op_compare->rc = -EINVAL;
 	         			ERROR_EXIT(0);
 	       			}
 
 	       			switch(ltc.image[i].type) {
 	       				case PART_MBB:
 	         				if (_sra_unserialize(&mbb1, sizeof(mbb1), ptemp, dsize, 
 	       		            	    	ltc.image[i].type, NOCOPY) < 0) {
 	           					op_compare->rc = -EINVAL;
 	           					ERROR_EXIT(0);
 	         				}
 
 	         				pmbb2 = sra_get_mbb();
 
 	         				if (pmbb2 == NULL) {
 	           					op_compare->rc = -EINVAL;
 	           					ERROR_EXIT(0);
 	         				}
 
 	         				if(!_equal_mbb(&mbb1, pmbb2)) {
 	       	    				op_compare->rc = -EINVAL;
 	           					ERROR_EXIT(0);
 	         				}
 
 	         				ssb_mp.ptype = ltc.image[i].type;
 
 	         				if (_get_mtd_partition(&ssb_mp)<0) {
 	           					op_compare->rc = -EINVAL;
 	           					ERROR_EXIT(0);
 	         				}
 
 	         				if (ssb_mp.ptype != lmbb.mtd_part[i].ptype || 
 	             				    ssb_mp.mtd_tb_num != lmbb.mtd_part[i].mtd_tb_num ||
 	             			            ssb_mp.part_offset != lmbb.mtd_part[i].part_offset || 
 	             			            ssb_mp.data_offset != lmbb.mtd_part[i].data_offset || 
 	             			            ssb_mp.max_size != lmbb.mtd_part[i].max_size ||
 	             				    ssb_mp.mask_flags != lmbb.mtd_part[i].mask_flags) {
 	           					op_compare->rc = -EINVAL;
 	           					ERROR_EXIT(0);
 	         				}
 	         			break;
 
 	       				case PART_SP1:
 	       				case PART_SP2:
 	       				case PART_SP3:
 	       				case PART_OS_IMAGE:
 	       					if (_sra_unserialize(&sp1, sizeof(sp1), ptemp, dsize, 
 	       			            	    ltc.image[i].type, NOCOPY) < 0) {
 	       			    			op_compare->rc = -EINVAL;
 	       			   			ERROR_EXIT(0);
 	       			  		}
 
 	       			  		psp2 = sra_get_sp(ltc.image[i].type);
 
 	       			  		if (psp2 == NULL) {
 	       			    			op_compare->rc = -EINVAL;
 	       			   	 		ERROR_EXIT(0);
 	       			  		}
 
 	       			  		if(!_equal_sp(&sp1, psp2, ltc.image[i].type)) {
 	       			    			op_compare->rc = -EINVAL;
 	       			    			ERROR_EXIT(0);
 	       			  		}
                                
 	       			  		ssb_mp.ptype = ltc.image[i].type;
 
 	       			  		if (_get_mtd_partition(&ssb_mp)<0) {
 	       			    			op_compare->rc = -EINVAL;
 	       			    			ERROR_EXIT(0);
 	       			  		}
 
 	       			  		if (ssb_mp.ptype != lmbb.mtd_part[i].ptype || 
 	       			      	    	    ssb_mp.mtd_tb_num != lmbb.mtd_part[i].mtd_tb_num ||
 	       			      	    	    ssb_mp.part_offset != lmbb.mtd_part[i].part_offset || 
 	       			      	    	    ssb_mp.data_offset != lmbb.mtd_part[i].data_offset || 
 	       			      	    	    ssb_mp.max_size != lmbb.mtd_part[i].max_size || 
 	       			      	    	    ssb_mp.mask_flags != lmbb.mtd_part[i].mask_flags) {
 	       			    			op_compare->rc = -EINVAL;
 	       			    			ERROR_EXIT(0);
 	       			  		}
 
 	       			  		break;
                                
 	       				default:
 	       					op_compare->rc = -EINVAL;
 	       			  		ERROR_EXIT(0);

						break;
 	       			}
       	     		} /* for loop start */
     		}

    		break;

  		default:
    			op_compare->rc = -EINVAL;
    			ERROR_EXIT(0);
  	}
    
 out:
	_page_alloc(ptemp, 0);
	spin_unlock(&sra_compare_lock);
#endif
	return rc;
}

int _equal_mbb(ssb_master_boot_block_t *mbb1, ssb_master_boot_block_t *mbb2)
{
#ifdef CONFIG_VMM_SECURITY
	int i;

	if (mbb1->psize != mbb2->psize) return FAIL;

  	for (i=0; i<mbb1->psize; i++) {
    		if (mbb1->mtd_part[i].ptype != mbb2->mtd_part[i].ptype) return FAIL;
    		if (mbb1->mtd_part[i].part_offset != mbb2->mtd_part[i].part_offset) return FAIL;
    		if (mbb1->mtd_part[i].data_offset != mbb2->mtd_part[i].data_offset) return FAIL;
    		if (mbb1->mtd_part[i].mtd_tb_num != mbb2->mtd_part[i].mtd_tb_num) return FAIL;
    		if (mbb1->mtd_part[i].max_size != mbb2->mtd_part[i].max_size) return FAIL;
    		if (mbb1->mtd_part[i].mask_flags != mbb2->mtd_part[i].mask_flags) return FAIL;
  	}
#endif
  	return SUCCESS;
}

int _equal_sp (ssb_image_container_t *sp1, ssb_image_container_t *sp2, partition_type_t ptype)
{
#ifdef CONFIG_VMM_SECURITY
	int i;

  	if (sp1->images_size != sp2->images_size) return FAIL;

  	for (i=0; i<sp1->images_size; i++) {
    		if(sp1->image[i].type != sp2->image[i].type) return FAIL;
    		if(sp1->image[i].size != sp2->image[i].size) return FAIL;

    		if (ptype == PART_OS_IMAGE) {
      			if (sp1->image[i].u.pos != sp2->image[i].u.pos) return FAIL;
    		} else 
		{
      			if (memcmp(sp1->image[i].u.ptr, sp2->image[i].u.ptr,
		 	sp1->image[i].size) != 0) return FAIL;
    		}
  	}
#endif
  	return SUCCESS;
}
