/* e_aeswrap.c */
/* Written by Dr Stephen N Henson (steve@openssl.org) for the OpenSSL
 * project.
 */
/* ====================================================================
 * Copyright (c) 2012 The OpenSSL Project.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer. 
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * 3. All advertising materials mentioning features or use of this
 *    software must display the following acknowledgment:
 *    "This product includes software developed by the OpenSSL Project
 *    for use in the OpenSSL Toolkit. (http://www.OpenSSL.org/)"
 *
 * 4. The names "OpenSSL Toolkit" and "OpenSSL Project" must not be used to
 *    endorse or promote products derived from this software without
 *    prior written permission. For written permission, please contact
 *    licensing@OpenSSL.org.
 *
 * 5. Products derived from this software may not be called "OpenSSL"
 *    nor may "OpenSSL" appear in their names without prior written
 *    permission of the OpenSSL Project.
 *
 * 6. Redistributions of any form whatsoever must retain the following
 *    acknowledgment:
 *    "This product includes software developed by the OpenSSL Project
 *    for use in the OpenSSL Toolkit (http://www.OpenSSL.org/)"
 *
 * THIS SOFTWARE IS PROVIDED BY THE OpenSSL PROJECT ``AS IS'' AND ANY
 * EXPRESSED OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE OpenSSL PROJECT OR
 * ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 * ====================================================================
 */
#include <stdio.h>
#include <string.h>
#include <stdint.h>


#include <openssl/evp.h>
#include "../src/libwagocrypt.h"

#if 0
static const unsigned char default_iv[] = {
  0xA6, 0xA6, 0xA6, 0xA6, 0xA6, 0xA6, 0xA6, 0xA6,
};
#endif


static int EVP_aes_wrap_unwrap_test(const unsigned char *kek, int keybits,
       const unsigned char *iv,
       const unsigned char *eout,
       const unsigned char *key, int keylen)
  {
  unsigned char *otmp = NULL, *ptmp = NULL;
  int r, ret = 0;
  EVP_CIPHER_CTX *ctx = WCRYPT_EVP_CIPHER_CTX_new();
  WCRYPT_EVP_CIPHER_CTX_init(ctx);

    const EVP_CIPHER *cipher;
  if (keybits == 128)
    cipher = EVP_aes_128_ecb();
  else if (keybits == 192)
    cipher = EVP_aes_192_ecb();
  else if (keybits == 256)
    cipher = EVP_aes_256_ecb();
  else
    return 0;
  otmp = WCRYPT_OPENSSL_malloc(keylen + 8);
  ptmp = WCRYPT_OPENSSL_malloc(keylen);
  if (!otmp || !ptmp)
    return 0;
  if (!WCRYPT_EVP_CipherInit_ex(ctx, cipher, NULL, kek, NULL, 1))
    goto err;
  r = WCRYPT_EVP_aes_wrap_key(ctx, iv, otmp, key, keylen);
  if (r <= 0)
    goto err;

  if (eout && memcmp(eout, otmp, keylen))
    goto err;

  if (!WCRYPT_EVP_CipherInit_ex(ctx, cipher, NULL, kek, NULL, 0))
    goto err;
  r = WCRYPT_EVP_aes_unwrap_key(ctx, iv, ptmp, otmp, r);

  if (memcmp(key, ptmp, keylen))
    goto err;

  ret = 1;

  err:
  WCRYPT_EVP_CIPHER_CTX_cleanup(ctx);
  WCRYPT_EVP_CIPHER_CTX_free(ctx);
  if (otmp)
    WCRYPT_OPENSSL_free(otmp);
  if (ptmp)
    WCRYPT_OPENSSL_free(ptmp);

  return ret;

  }



int main()
{

static const unsigned char kek[] = {
  0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
  0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
  0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
  0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f
};

static const unsigned char key[] = {
  0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
  0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff,
  0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
  0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
};

static const unsigned char e1[] = {
  0x1f, 0xa6, 0x8b, 0x0a, 0x81, 0x12, 0xb4, 0x47,
  0xae, 0xf3, 0x4b, 0xd8, 0xfb, 0x5a, 0x7b, 0x82,
  0x9d, 0x3e, 0x86, 0x23, 0x71, 0xd2, 0xcf, 0xe5
};

static const unsigned char e2[] = {
  0x96, 0x77, 0x8b, 0x25, 0xae, 0x6c, 0xa4, 0x35,
  0xf9, 0x2b, 0x5b, 0x97, 0xc0, 0x50, 0xae, 0xd2,
  0x46, 0x8a, 0xb8, 0xa1, 0x7a, 0xd8, 0x4e, 0x5d
};

static const unsigned char e3[] = {
  0x64, 0xe8, 0xc3, 0xf9, 0xce, 0x0f, 0x5b, 0xa2,
  0x63, 0xe9, 0x77, 0x79, 0x05, 0x81, 0x8a, 0x2a,
  0x93, 0xc8, 0x19, 0x1e, 0x7d, 0x6e, 0x8a, 0xe7
};

static const unsigned char e4[] = {
  0x03, 0x1d, 0x33, 0x26, 0x4e, 0x15, 0xd3, 0x32,
  0x68, 0xf2, 0x4e, 0xc2, 0x60, 0x74, 0x3e, 0xdc,
  0xe1, 0xc6, 0xc7, 0xdd, 0xee, 0x72, 0x5a, 0x93,
  0x6b, 0xa8, 0x14, 0x91, 0x5c, 0x67, 0x62, 0xd2
};

static const unsigned char e5[] = {
  0xa8, 0xf9, 0xbc, 0x16, 0x12, 0xc6, 0x8b, 0x3f,
  0xf6, 0xe6, 0xf4, 0xfb, 0xe3, 0x0e, 0x71, 0xe4,
  0x76, 0x9c, 0x8b, 0x80, 0xa3, 0x2c, 0xb8, 0x95,
  0x8c, 0xd5, 0xd1, 0x7d, 0x6b, 0x25, 0x4d, 0xa1
};

static const unsigned char e6[] = {
  0x28, 0xc9, 0xf4, 0x04, 0xc4, 0xb8, 0x10, 0xf4,
  0xcb, 0xcc, 0xb3, 0x5c, 0xfb, 0x87, 0xf8, 0x26,
  0x3f, 0x57, 0x86, 0xe2, 0xd8, 0x0e, 0xd3, 0x26,
  0xcb, 0xc7, 0xf0, 0xe7, 0x1a, 0x99, 0xf4, 0x3b,
  0xfb, 0x98, 0x8b, 0x9b, 0x7a, 0x02, 0xdd, 0x21
};

    //OpenSSL_add_all_algorithms();
    const EVP_CIPHER *c; 
    c = WCRYPT_EVP_get_cipherbyname("aes-128-cbc"); 
    fprintf(stderr, "EVP_get_cipherbyname returns: 0x%lx\n", (unsigned long)c);

  int ret;
  ret = EVP_aes_wrap_unwrap_test(kek, 128, NULL, e1, key, 16);
  fprintf(stderr, "Key test result %d\n", ret);
  ret = EVP_aes_wrap_unwrap_test(kek, 192, NULL, e2, key, 16);
  fprintf(stderr, "Key test result %d\n", ret);
  ret = EVP_aes_wrap_unwrap_test(kek, 256, NULL, e3, key, 16);
  fprintf(stderr, "Key test result %d\n", ret);
  ret = EVP_aes_wrap_unwrap_test(kek, 192, NULL, e4, key, 24);
  fprintf(stderr, "Key test result %d\n", ret);
  ret = EVP_aes_wrap_unwrap_test(kek, 256, NULL, e5, key, 24);
  fprintf(stderr, "Key test result %d\n", ret);
  ret = EVP_aes_wrap_unwrap_test(kek, 256, NULL, e6, key, 32);
  fprintf(stderr, "Key test result %d\n", ret);

    return 1;
}


