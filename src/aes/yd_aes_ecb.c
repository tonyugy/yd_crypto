
/* Apache License 2.0 */

/*
	文件：yd_aes_ecb.c
	作者：wzh
	邮箱：wangzhihai_138@163.com
	简介：AES算法模式ECB(Electronic Codebook)实现，详情参考《SP800-38A》
	版本：README.md定义
*/

#include "yd_aes_ecb.h"
#include "yd_aes_base.h"


/*
 *	ECB模式，加密(注：按块加密，16字节的倍数)
 *	in： 待加密数据
 *	out：加密后数据
 *	key：密钥(4*AES_NK字节)
 *	blk：要加密的块数
 */
void yd_aes_ecb_encrypt(uint8_t *in, uint8_t *out, uint8_t *key, uint32_t blk)
{
	uint8_t i;
	
	while(blk > 0)
	{
		for(i=0; i<16; i++)
		{
			out[i] = in[i];
		}
		yd_aes_encrypt(out, key);
		
		in += 16;
		out += 16;
		blk--;
	}
}

/*
 *	ECB模式，解密(注：按块解密，16字节的倍数)
 *	in： 待解密数据
 *	out：解密后数据
 *	key：密钥(4*AES_NK字节)
 *	blk：要解密的块数
 */
void yd_aes_ecb_decrypt(uint8_t *in, uint8_t *out, uint8_t *key, uint32_t blk)
{
	uint8_t i;
	
	while(blk > 0)
	{
		for(i=0; i<16; i++)
		{
			out[i] = in[i];
		}
		yd_aes_decrypt(out, key);
		
		in += 16;
		out += 16;
		blk--;
	}
}
