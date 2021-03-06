
/* Apache License 2.0 */

/*
	文件：yd_sha1.c
	作者：wzh
	邮箱：wangzhihai_138@163.com
	简介：SHA1算法实现，详情参考《FIPS PUB 180-4》
	版本：README.md定义
*/

#include "yd_sha1.h"


/*
 *	数据填充
 *	返回值：false=数据没有填充完；true=数据填充完成
 */
static bool padding_bits(uint8_t *msg,
						 uint32_t msg_length,
						 uint32_t *msg_length_remain,
						 uint8_t *m_8bit)
{
	uint8_t i;
	
	if(*msg_length_remain >= 64)
	{
		for(i=0; i<64; i++)
		{
			m_8bit[i] = msg[i];
		}
		
		*msg_length_remain -= 64;
	}
	else //小于64字节.
	{
		if(*msg_length_remain >= 56) //56-63字节之间，一个块填充不完，还需要填充1次.
		{
			for(i=0; i<*msg_length_remain; i++)
			{
				m_8bit[i] = msg[i];
			}
			m_8bit[i++] = 0x80;
			while(i < 64)
			{
				m_8bit[i++] = 0;
			}
			
			*msg_length_remain = 0;
		}
		else //小于等于56字节.
		{
			for(i=0; i<*msg_length_remain; i++)
			{
				m_8bit[i] = msg[i];
			}
			
			/* 消息小于56字节时或者消息是64的倍数时，‘1’没有填充 */
			if(*msg_length_remain != 0 || msg_length % 64 == 0)
			{
				m_8bit[i++] = 0x80;
			}
			
			while(i < 56)
			{
				m_8bit[i++] = 0;
			}
			
			while(i < 60) //最大计算数据长度限制到32位，所以这4字节填0.
			{
				m_8bit[i++] = 0;
			}
			
			msg_length <<= 3; //乘8转为位长度.
			m_8bit[60] = msg_length >> 24;
			m_8bit[61] = msg_length >> 16;
			m_8bit[62] = msg_length >> 8;
			m_8bit[63] = msg_length;
			
			return true;
		}
	}
	
	return false;
}

/* 准备Wt */
static void prepare_wt(uint8_t *m, uint32_t *w)
{
	uint8_t t, t_tmp;
	uint32_t tmp;
	
	for(t=0; t<16; t++)
	{
		t_tmp = t << 2; //t<<2=t*4.
		w[t] = m[t_tmp]<<24 | m[t_tmp+1]<<16 | m[t_tmp+2]<<8 | m[t_tmp+3]; //大端.
	}
	
	while(t<80) //异或，并循环左移1位.
	{
		w[t] = w[t-3] ^ w[t-8] ^ w[t-14] ^ w[t-16];
		tmp = w[t] >> 31;
		w[t] <<= 1;
		w[t] |= tmp;
		
		t++;
	}
}

/* 计算值 */
static void compute_hash_value(uint32_t *wt, uint32_t *H)
{
	uint8_t t;
	uint32_t a, b, c, d, e;
	uint32_t T, ft, kt, tmp;
	
	/* 2步------------------------------------------------------------ */
	a = H[0];
	b = H[1];
	c = H[2];
	d = H[3];
	e = H[4];
	
	/* 3步------------------------------------------------------------ */
	for(t=0; t<80; t++)
	{
		if(t < 20)
		{
			kt = 0x5a827999;
			ft = (b & c) ^ (~b & d);
		}
		else if(t < 40)
		{
			kt = 0x6ed9eba1;
			ft = b ^ c ^ d;
		}
		else if(t < 60)
		{
			kt = 0x8f1bbcdc;
			ft = (b & c) ^ (b & d) ^ (c & d);
		}
		else //t<80.
		{
			kt = 0xca62c1d6;
			ft = b ^ c ^ d;
		}
		
		tmp = a << 5;
		tmp |= a >> 27; //a循环左移5位.
		T = tmp + ft + e + kt + wt[t];
		e = d;
		d = c;
		tmp = b << 30;
		tmp |= b >> 2; //b循环左移30位.
		c = tmp;
		b = a;
		a = T;
	}
	
	/* 4步------------------------------------------------------------ */
	H[0] += a;
	H[1] += b;
	H[2] += c;
	H[3] += d;
	H[4] += e;
}

/*
 *	产生安全散列值SHA1
 *	msg：		参与计算的数据
 *	msg_length	参与计算的数据长度
 *	sha1：	 	计算得到的散列值(160位)
 */
bool yd_sha1(uint8_t *msg, uint32_t msg_length, uint32_t *sha1)
{
	uint8_t flag, m_8bit[64];
	uint32_t i, msg_length_remain;
	uint32_t wt[80], H[5];
	
	/* 限制：最大计算0xffffffff / 8 = 536870911字节 */
	if(msg_length > 536870911)
	{
		return false;
	}
	msg_length_remain = msg_length;
	
	H[0] = 0x67452301;
	H[1] = 0xefcdab89;
	H[2] = 0x98badcfe;
	H[3] = 0x10325476;
	H[4] = 0xc3d2e1f0;
	
	flag = 1;
	do
	{
		/* 0步------------------------------------------------------------ */
		i = msg_length - msg_length_remain; //定位要计算的消息.
		if(true == padding_bits(&msg[i],
								msg_length,
								&msg_length_remain,
								m_8bit))
		{
			flag = 0;
		}
		
		/* 1步------------------------------------------------------------ */
		prepare_wt(m_8bit, wt);
		
		/* 2-4步---------------------------------------------------------- */
		compute_hash_value(wt, H);
	}while(flag == 1);
	
	sha1[0] = H[0];
	sha1[1] = H[1];
	sha1[2] = H[2];
	sha1[3] = H[3];
	sha1[4] = H[4];
	
	return true;
}
