#ifndef NTRU_H
#define NTRU_H

#include "poly.h"

int ntru_keygen(poly *hhat, poly *fhat, const unsigned char *coins);
void ntru_encrypt(poly *chat,
                  poly *m,
                  const poly *hhat,
                  const unsigned char *coins);
void ntru_decrypt(poly *m,
                  const poly *chat,
                  const poly *fhat);

#endif
