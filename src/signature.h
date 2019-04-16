#ifndef __SIGNATURE_H__
#define __SIGNATURE_H__

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Initialize this module.
 *
 * @param cert_path the path to the certificate to be used to check signatures.
 * @param crl       a certificate revocation list to be used to check if the
 *                  certificate is revoked. If null, the certificate is
 *                  considered valid.
 *
 * @return true if no error.
 */
bool signature_init(const char *certificate_path, const char *crl);

/*
 * Terminate usage of this module, free resources.
 */
void signature_deinit(void);

/*
 * The given data file has already been downloaded from the given URL.
 * Download the corresponding signature file, and verify the data against the signature.
 * Delete the signature file if and only if the verification fails.
 *
 * @param data_url      the URL from which the data came
 * @param data_filename the file containing the data
 *
 * @return true if no error
 */
bool signature_verify(const char *data_url, const char *data_filename, int version, bool mix_exists);

#ifdef __cplusplus
}
#endif

#endif
