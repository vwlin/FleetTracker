#ifndef _AES_H_
#define _AES_H_

/*
 * AES_Enc_Dec
 * Encrypt a 128-bit value using symmetric key
 *
 * Parameters:
 *  state - pointer to 128-bit value to be encrypted/decrypted
 *  dir - int indicating whether to encrypt or decrypt
 *        0: encrypt
 *		  1: decrypt
 * Returns nothing (but state is now encrypted)
 */
void AES_Enc_Dec(unsigned char *state,unsigned char dir);

#endif //_AES_H_
