/*
   +----------------------------------------------------------------------+
   | PHP version 4.0                                                      |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997, 1998, 1999, 2000 The PHP Group                   |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Sascha Schumann <sascha@schumann.cx>                        |
   |          Derick Rethans <d.rethans@jdimedia.nl>                      |
   +----------------------------------------------------------------------+
 */

#include "php.h"

#if HAVE_LIBMCRYPT

#include "php_mcrypt.h"
#include "fcntl.h"

#define NON_FREE
#define MCRYPT2
#include "mcrypt.h"
#include "php_ini.h"
#include "php_globals.h"
#include "ext/standard/info.h"


function_entry mcrypt_functions[] = {
	PHP_FE(mcrypt_ecb, NULL)
	PHP_FE(mcrypt_cbc, NULL)
	PHP_FE(mcrypt_cfb, NULL)
	PHP_FE(mcrypt_ofb, NULL)
	PHP_FE(mcrypt_get_key_size, NULL)
	PHP_FE(mcrypt_get_block_size, NULL)
	PHP_FE(mcrypt_get_cipher_name, NULL)
	PHP_FE(mcrypt_create_iv, NULL)
#if HAVE_LIBMCRYPT24
	PHP_FE(mcrypt_list_algorithms, NULL)
	PHP_FE(mcrypt_list_modes, NULL)
	PHP_FE(mcrypt_get_iv_size, NULL)
	PHP_FE(mcrypt_encrypt, NULL)
	PHP_FE(mcrypt_decrypt, NULL)

	PHP_FE(mcrypt_module_open, NULL)
	PHP_FE(mcrypt_generic_init, NULL)
	PHP_FE(mcrypt_generic, NULL)
	PHP_FE(mdecrypt_generic, NULL)
	PHP_FE(mcrypt_generic_end, NULL)
	PHP_FE(mcrypt_enc_self_test, NULL)
	PHP_FE(mcrypt_enc_is_block_algorithm_mode, NULL)
	PHP_FE(mcrypt_enc_is_block_algorithm, NULL)
	PHP_FE(mcrypt_enc_is_block_mode, NULL)
	PHP_FE(mcrypt_enc_get_block_size, NULL)
	PHP_FE(mcrypt_enc_get_key_size, NULL)
	PHP_FE(mcrypt_enc_get_supported_key_sizes, NULL)
	PHP_FE(mcrypt_enc_get_iv_size, NULL)
	PHP_FE(mcrypt_enc_get_algorithms_name, NULL)
	PHP_FE(mcrypt_enc_get_modes_name, NULL)
	PHP_FE(mcrypt_module_self_test, NULL)

	PHP_FE(mcrypt_module_is_block_algorithm_mode, NULL)
	PHP_FE(mcrypt_module_is_block_algorithm, NULL)
	PHP_FE(mcrypt_module_is_block_mode, NULL)
	PHP_FE(mcrypt_module_get_algo_block_size, NULL)
	PHP_FE(mcrypt_module_get_algo_key_size, NULL)
	PHP_FE(mcrypt_module_get_supported_key_sizes, NULL)

	PHP_FE(mcrypt_module_close, NULL)
#endif
	{0},
};

static PHP_MINFO_FUNCTION(mcrypt);
static PHP_MINIT_FUNCTION(mcrypt);
static PHP_MSHUTDOWN_FUNCTION(mcrypt);

zend_module_entry mcrypt_module_entry = {
	"mcrypt", 
	mcrypt_functions,
	PHP_MINIT(mcrypt), PHP_MSHUTDOWN(mcrypt),
	NULL, NULL,
	PHP_MINFO(mcrypt),
	STANDARD_MODULE_PROPERTIES,
};

#if HAVE_LIBMCRYPT24
ZEND_DECLARE_MODULE_GLOBALS(mcrypt)
#endif

#ifdef COMPILE_DL_MCRYPT
ZEND_GET_MODULE(mcrypt)
#endif

#define MCRYPT_ARGS2 											\
	zval **cipher, **data, **key, **mode; 						\
	int td; 													\
	char *ndata; 												\
	size_t bsize; 												\
	size_t nr; 													\
	size_t nsize

#define MCRYPT_ARGS 											\
	MCRYPT_ARGS2; 												\
	zval **iv

#if HAVE_LIBMCRYPT22
#define MCRYPT_CONVERT 											\
	convert_to_long_ex(cipher); 								\
	convert_to_long_ex(mode); 									\
	convert_to_string_ex(data); 								\
	convert_to_string_ex(key)
#else
#define MCRYPT_CONVERT 											\
	convert_to_string_ex(cipher); 								\
	convert_to_string_ex(mode); 								\
	convert_to_string_ex(data); 								\
	convert_to_string_ex(key)
#define MCRYPT_CONVERT_WO_MODE									\
	convert_to_string_ex(cipher); 								\
	convert_to_string_ex(data); 								\
	convert_to_string_ex(key)
#endif

#define MCRYPT_SIZE 											\
	bsize = mcrypt_get_block_size(Z_LVAL_PP(cipher)); 		\
	nr = (Z_STRLEN_PP(data) + bsize - 1) / bsize; 			\
	nsize = nr * bsize

#define MCRYPT_CHECK_TD_CPY 									\
	if(td < 0) { 												\
		php_error(E_WARNING, MCRYPT_FAILED); 					\
		RETURN_FALSE; 											\
	} 															\
	ndata = ecalloc(nr, bsize); 								\
	memcpy(ndata, Z_STRVAL_PP(data), Z_STRLEN_PP(data))

#define MCRYPT_CHECK_IV 										\
	convert_to_string_ex(iv);	 								\
	if(Z_STRLEN_PP(iv) != bsize) { 						\
		php_error(E_WARNING, MCRYPT_IV_WRONG_SIZE); 			\
		RETURN_FALSE; 											\
	}

#define MCRYPT_ACTION(x) 										\
	if(Z_LVAL_PP(mode) == 0) 								\
		mcrypt_##x(td, ndata, nsize); 							\
	else 														\
		mdecrypt_##x(td, ndata, nsize); 						\
	end_mcrypt_##x(td)

#define MCRYPT_IV_WRONG_SIZE "The IV parameter must be as long as the blocksize"

#if HAVE_LIBMCRYPT24
#define MCRYPT_ENCRYPT 0
#define MCRYPT_DECRYPT 1

#define MCRYPT_GET_INI											\
	cipher_dir_string = MCG(algorithms_dir); 					\
	module_dir_string = MCG(modes_dir);

#define MCRYPT_CHECK_PARAM_COUNT(a,b)							\
	if (argc < (a) || argc > (b)) {								\
		WRONG_PARAM_COUNT;										\
	}

#define MCRYPT_GET_CRYPT_ARGS									\
	switch (argc) {												\
		case 5:													\
			if (zend_get_parameters_ex(5, &cipher, &key, &data, &mode, &iv) == FAILURE) {	\
				WRONG_PARAM_COUNT;								\
			}													\
			convert_to_string_ex(iv);							\
			break;												\
		case 4:													\
			if (zend_get_parameters_ex(4, &cipher, &key, &data, &mode) == FAILURE) {		\
				WRONG_PARAM_COUNT;								\
			}													\
			iv = NULL;											\
			break;												\
		default:												\
			WRONG_PARAM_COUNT;									\
	}

#define MCRYPT_GET_TD_ARG										\
	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &mcryptind) == FAILURE) {			\
		WRONG_PARAM_COUNT																	\
	}																						\
	ZEND_FETCH_RESOURCE (td, MCRYPT, mcryptind, -1, "MCrypt", MCG(le_h));				

#define MCRYPT_GET_MODE_DIR_ARGS(DIRECTORY)								\
	switch (argc) {														\
		case 2:															\
			lib_dir_s = Z_STRVAL_PP(lib_dir);						\
			if (zend_get_parameters_ex(2, &arg1, &lib_dir) == FAILURE)	\
			{															\
				WRONG_PARAM_COUNT;										\
			}															\
			convert_to_string_ex (lib_dir);								\
			break;														\
		case 1:															\
			lib_dir_s = MCG(DIRECTORY);									\
			if (zend_get_parameters_ex(1, &arg1) == FAILURE)			\
			{															\
				WRONG_PARAM_COUNT;										\
			}															\
			break;														\
		default:														\
			WRONG_PARAM_COUNT;											\
	}																	\
	convert_to_string_ex(arg1);
	
#endif

#if HAVE_LIBMCRYPT22
#define MCRYPT_FAILED "mcrypt initialization failed"
#else
#define MCRYPT_OPEN_MODULE_FAILED "mcrypt module initialization failed"
#endif

#if HAVE_LIBMCRYPT22
#define MCRYPT_ENTRY_NAMED(a,b) REGISTER_LONG_CONSTANT("MCRYPT_" #a, b, CONST_PERSISTENT)
#define MCRYPT_ENTRY2(a) MCRYPT_ENTRY_NAMED(a, MCRYPT_##a)
#define MCRYPT_ENTRY(a) MCRYPT_ENTRY_NAMED(a, a)
#else /* MCRYPT_2_4 */
#define MCRYPT_ENTRY2_2_4(a,b) REGISTER_STRING_CONSTANT("MCRYPT_" #a, b, CONST_PERSISTENT)
#define MCRYPT_ENTRY2_4(a) MCRYPT_ENTRY_NAMED(a, a)
#endif

#if HAVE_LIBMCRYPT24
PHP_INI_BEGIN()
	STD_PHP_INI_ENTRY("mcrypt.algorithms_dir",	NULL, PHP_INI_ALL, OnUpdateString, algorithms_dir, zend_mcrypt_globals, mcrypt_globals)
	STD_PHP_INI_ENTRY("mcrypt.modes_dir",	NULL, PHP_INI_ALL, OnUpdateString, modes_dir, zend_mcrypt_globals, mcrypt_globals)
PHP_INI_END()
#endif

static PHP_MINIT_FUNCTION(mcrypt)
{
#if defined(ZTS) && defined(HAVE_LIBMCRYPT24)
    ZEND_INIT_MODULE_GLOBALS(mcrypt, NULL, NULL);
    Z_TYPE(mcrypt_module_entry) = type;
#endif
	/* modes for mcrypt_??? routines */
	REGISTER_LONG_CONSTANT("MCRYPT_ENCRYPT", 0, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("MCRYPT_DECRYPT", 1, CONST_PERSISTENT);
	
	/* sources for mcrypt_create_iv */
	REGISTER_LONG_CONSTANT("MCRYPT_DEV_RANDOM", 0, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("MCRYPT_DEV_URANDOM", 1, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("MCRYPT_RAND", 2, CONST_PERSISTENT);
	
	/* ciphers */
#if HAVE_LIBMCRYPT22
	MCRYPT_ENTRY2(BLOWFISH_448);
	MCRYPT_ENTRY2(DES);
	MCRYPT_ENTRY2(3DES);
	MCRYPT_ENTRY2(3WAY);
	MCRYPT_ENTRY2(GOST);
	MCRYPT_ENTRY2(SAFER_64);
	MCRYPT_ENTRY2(SAFER_128);
	MCRYPT_ENTRY2(CAST_128);
	MCRYPT_ENTRY2(XTEA);
	MCRYPT_ENTRY2(RC2_1024);
	MCRYPT_ENTRY2(TWOFISH_128);
	MCRYPT_ENTRY2(TWOFISH_192);
	MCRYPT_ENTRY2(TWOFISH_256);
	MCRYPT_ENTRY2(BLOWFISH_128);
	MCRYPT_ENTRY2(BLOWFISH_192);
	MCRYPT_ENTRY2(BLOWFISH_256);
	MCRYPT_ENTRY2(CAST_256);
	MCRYPT_ENTRY2(SAFERPLUS);
	MCRYPT_ENTRY2(LOKI97);
	MCRYPT_ENTRY2(SERPENT_128);
	MCRYPT_ENTRY2(SERPENT_192);
	MCRYPT_ENTRY2(SERPENT_256);
	MCRYPT_ENTRY2(RIJNDAEL_128);
	MCRYPT_ENTRY2(RIJNDAEL_192);
	MCRYPT_ENTRY2(RIJNDAEL_256);
	MCRYPT_ENTRY2(RC2_256);
	MCRYPT_ENTRY2(RC2_128);
	MCRYPT_ENTRY2(RC6_256);
	MCRYPT_ENTRY2(IDEA);
	MCRYPT_ENTRY2(RC6_128);
	MCRYPT_ENTRY2(RC6_192);
	MCRYPT_ENTRY2(RC4);
#endif
#if HAVE_LIBMCRYPT24
	MCRYPT_ENTRY2_2_4(ARCFOUR_IV, "arcfour-iv");
	MCRYPT_ENTRY2_2_4(ARCFOUR, "arcfour");
	MCRYPT_ENTRY2_2_4(BLOWFISH, "blowfish");
	MCRYPT_ENTRY2_2_4(CAST_128, "cast-128");
	MCRYPT_ENTRY2_2_4(CAST_256, "cast-256");
	MCRYPT_ENTRY2_2_4(CRYPT, "crypt");
	MCRYPT_ENTRY2_2_4(DES, "des");
	MCRYPT_ENTRY2_2_4(ENIGNA, "crypt");
	MCRYPT_ENTRY2_2_4(GOST, "gost");
	MCRYPT_ENTRY2_2_4(LOKI97, "loki97");
	MCRYPT_ENTRY2_2_4(PANAMA, "panama");
	MCRYPT_ENTRY2_2_4(RC2, "rc2");
	MCRYPT_ENTRY2_2_4(RIJNDAEL_128, "rijndael-128");
	MCRYPT_ENTRY2_2_4(RIJNDAEL_192, "rijndael-192");
	MCRYPT_ENTRY2_2_4(RIJNDAEL_256, "rijndael-256");
	MCRYPT_ENTRY2_2_4(SAFER64, "safer-sk64");
	MCRYPT_ENTRY2_2_4(SAFER128, "safer-sk128");
	MCRYPT_ENTRY2_2_4(SAFERPLUS, "saferplus");
	MCRYPT_ENTRY2_2_4(SERPENT, "serpent");
	MCRYPT_ENTRY2_2_4(THREEWAY, "threeway");
	MCRYPT_ENTRY2_2_4(TRIPLEDES, "tripledes");
	MCRYPT_ENTRY2_2_4(TWOFISH, "twofish");
	MCRYPT_ENTRY2_2_4(WAKE, "wake");
	MCRYPT_ENTRY2_2_4(XTEA, "xtea");

	MCRYPT_ENTRY2_2_4(IDEA, "idea");
	MCRYPT_ENTRY2_2_4(MARS, "mars");
	MCRYPT_ENTRY2_2_4(RC6, "rc6");
	MCRYPT_ENTRY2_2_4(SKIPJACK, "skipjack");
/* modes */
	MCRYPT_ENTRY2_2_4(MODE_CBC, "cbc");
	MCRYPT_ENTRY2_2_4(MODE_CFB, "cfb");
	MCRYPT_ENTRY2_2_4(MODE_ECB, "ecb");
	MCRYPT_ENTRY2_2_4(MODE_NOFB, "nofb");
	MCRYPT_ENTRY2_2_4(MODE_OFB, "ofb");
	MCRYPT_ENTRY2_2_4(MODE_STREAM, "stream");
	REGISTER_INI_ENTRIES();
#endif
	return SUCCESS;
}

static PHP_MSHUTDOWN_FUNCTION(mcrypt)
{
	UNREGISTER_INI_ENTRIES();
	return SUCCESS;
}

PHP_MINFO_FUNCTION(mcrypt)
{
#if HAVE_LIBMCRYPT24
	char **modules;
	int i, count;
	char *tmp, *tmp2;
	MCLS_FETCH();

	tmp = emalloc (2048);
	memset (tmp, 0, sizeof(tmp));
	modules = mcrypt_list_algorithms (MCG(algorithms_dir), &count);
	if (count == 0) {
		strcpy (tmp, "none");
	}
	for (i = 0; i < count; i++) {
		strcat (tmp, modules[i]);
		strcat (tmp, " ");
	}
	mcrypt_free_p (modules, count);

	tmp2 = emalloc (2048);
	memset (tmp2, 0, sizeof(tmp2));
	modules = mcrypt_list_modes (MCG(modes_dir), &count);
	if (count == 0) {
		strcpy (tmp2, "none");
	}
	for (i = 0; i < count; i++) {
		strcat (tmp2, modules[i]);
		strcat (tmp2, " ");
	}
	mcrypt_free_p (modules, count);
#endif

	php_info_print_table_start();
	php_info_print_table_header(2, "mcrypt support", "enabled");
#if HAVE_LIBMCRYPT22
	php_info_print_table_row(2, "version", "2.2.x");
#endif
#if HAVE_LIBMCRYPT24
	php_info_print_table_row(2, "version", "2.4.x");
	php_info_print_table_row(2, "Supported ciphers", tmp);
	php_info_print_table_row(2, "Supported modes", tmp2);
	efree (tmp2);
	efree (tmp);
#endif
	php_info_print_table_end();
	
	DISPLAY_INI_ENTRIES();
}

typedef enum {
	RANDOM = 0,
	URANDOM,
	RAND
} iv_source;

#if HAVE_LIBMCRYPT24

/* {{{ proto resource mcrypt_module_open(string cipher, string cipher_directory, string mode, string mode_directory)
   Opens the module of the algorithm and the mode to be used */
PHP_FUNCTION(mcrypt_module_open)
{
	zval **cipher, **cipher_directory, **mode, **mode_directory;
	MCRYPT td;
	int argc;
    MCLS_FETCH();
    
	argc = ZEND_NUM_ARGS();
	MCRYPT_CHECK_PARAM_COUNT (4,4)
	
	zend_get_parameters_ex(4, &cipher, &cipher_directory, &mode, &mode_directory);
	convert_to_string_ex(cipher);
	convert_to_string_ex(cipher_directory);
	convert_to_string_ex(mode);
	convert_to_string_ex(mode_directory);
	
	td = mcrypt_module_open (Z_STRVAL_PP(cipher),
		Z_STRLEN_PP(cipher_directory) > 0 ? Z_STRVAL_PP(cipher_directory) : MCG(algorithms_dir),
		Z_STRVAL_PP(mode), 
		Z_STRLEN_PP(mode_directory) > 0 ? Z_STRVAL_PP(mode_directory) : MCG(modes_dir));

	if (td == MCRYPT_FAILED) {
		php_error (E_WARNING, "could not open encryption module");
		RETURN_FALSE;
	} else {
		ZEND_REGISTER_RESOURCE (return_value, td, MCG(le_h));
	}
}
/* }}} */


/* {{{ proto int mcrypt_generic_init(resource td, string key, string iv)
   This function initializes all buffers for the specific module */
PHP_FUNCTION(mcrypt_generic_init)
{
	zval **key, **iv;
	zval **mcryptind;
	char *key_s, *iv_s;
	char dummy[256];
	int key_size, iv_size;
	MCRYPT td;
	int argc;
    MCLS_FETCH();
	
	argc = ZEND_NUM_ARGS();
	MCRYPT_CHECK_PARAM_COUNT (3,3)
	
	zend_get_parameters_ex(3, &mcryptind, &key, &iv);
	ZEND_FETCH_RESOURCE (td, MCRYPT, mcryptind, -1, "MCrypt", MCG(le_h));				
	convert_to_string_ex (key);
	convert_to_string_ex (iv);

	key_size = mcrypt_enc_get_key_size (td);
	key_s = emalloc (key_size + 1);
	memset (key_s, 0, key_size + 1);

	iv_size = mcrypt_enc_get_iv_size (td);
	iv_s = emalloc (iv_size + 1);
	memset (iv_s, 0, iv_size + 1);

	if (Z_STRLEN_PP(key) != key_size) {
		sprintf (dummy, "key size incorrect; supplied length: %d, needed: %d", 
			Z_STRLEN_PP(key), key_size);
		php_error (E_NOTICE, dummy);
	}
	strncpy (key_s, Z_STRVAL_PP(key), key_size);

	if (Z_STRLEN_PP(iv) != iv_size) {
		sprintf (dummy, "iv size incorrect; supplied length: %d, needed: %d", 
			Z_STRLEN_PP(iv), iv_size);
		php_error (E_WARNING, dummy);
	}
	strncpy (iv_s, Z_STRVAL_PP(iv), iv_size);

	RETVAL_LONG (mcrypt_generic_init (td, key_s, key_size, iv_s));
	efree (iv_s);
	efree (key_s);
}
/* }}} */


/* {{{ proto string mcrypt_generic(resource td, string data)
   This function encrypts the plaintext */
PHP_FUNCTION(mcrypt_generic)
{
	zval **data, **mcryptind;
	MCRYPT td;
	int argc;
	char* data_s;
	int block_size, data_size;
    MCLS_FETCH();
	
	argc = ZEND_NUM_ARGS();
	MCRYPT_CHECK_PARAM_COUNT (2,2)
	
	zend_get_parameters_ex(2, &mcryptind, &data);
	ZEND_FETCH_RESOURCE (td, MCRYPT, mcryptind, -1, "MCrypt", MCG(le_h));				
	convert_to_string_ex (data);

	/* Check blocksize */
	if (mcrypt_enc_is_block_algorithm (td) == 1) { /* It's a block algorithm */
		block_size = mcrypt_enc_get_block_size (td);
		data_size = (((Z_STRLEN_PP(data) - 1) / block_size) + 1) * block_size;
		data_s = emalloc (data_size);
		memset (data_s, 0, data_size);
		memcpy (data_s, Z_STRVAL_PP(data), Z_STRLEN_PP(data));
	}
	else { /* It's not a block algorithm */
		data_size = Z_STRLEN_PP(data);
		data_s = emalloc (data_size);
		memset (data_s, 0, data_size);
		memcpy (data_s, Z_STRVAL_PP(data), Z_STRLEN_PP(data));
	}
	
	mcrypt_generic (td, data_s, data_size);

	RETVAL_STRINGL (data_s, data_size, 1);
	efree (data_s);
}
/* }}} */


/* {{{ proto string mdecrypt_generic(resource td, string data)
   This function decrypts the plaintext */
PHP_FUNCTION(mdecrypt_generic)
{
	zval **data, **mcryptind;
	MCRYPT td;
	int argc;
	char* data_s;
	int block_size, data_size;
    MCLS_FETCH();
	
	argc = ZEND_NUM_ARGS();
	MCRYPT_CHECK_PARAM_COUNT (2,2)
	
	zend_get_parameters_ex(2, &mcryptind, &data);
	ZEND_FETCH_RESOURCE (td, MCRYPT, mcryptind, -1, "MCrypt", MCG(le_h));				
	convert_to_string_ex (data);

	/* Check blocksize */
	if (mcrypt_enc_is_block_algorithm (td) == 1) { /* It's a block algorithm */
		block_size = mcrypt_enc_get_block_size (td);
		data_size = (((Z_STRLEN_PP(data) - 1) / block_size) + 1) * block_size;
		data_s = emalloc (data_size);
		memset (data_s, 0, data_size);
		memcpy (data_s, Z_STRVAL_PP(data), Z_STRLEN_PP(data));
	}
	else { /* It's not a block algorithm */
		data_size = Z_STRLEN_PP(data);
		data_s = emalloc (data_size);
		memset (data_s, 0, data_size);
		memcpy (data_s, Z_STRVAL_PP(data), Z_STRLEN_PP(data));
	}
	
	mdecrypt_generic (td, data_s, data_size);

	RETVAL_STRINGL (data_s, data_size, 1);
	efree (data_s);
}
/* }}} */


/* {{{ proto int mcrypt_enc_get_supported_key_sizes(resource td)
   This function decrypts the crypttext */
PHP_FUNCTION(mcrypt_enc_get_supported_key_sizes)
{
	zval **mcryptind;
	MCRYPT td;
	int argc, i, count;
	int *key_sizes;
    MCLS_FETCH();
	
	argc = ZEND_NUM_ARGS();
	MCRYPT_GET_TD_ARG

	key_sizes = mcrypt_enc_get_supported_key_sizes (td, &count);

	if (array_init(return_value) == FAILURE) {
		php_error(E_ERROR, "Unable to initialize array");
		return;
	}
	if (count != 0) {
		for (i = 0; i < count; i++) {
			add_index_long(return_value, i, key_sizes[i]);
		}
	}
	mcrypt_free (key_sizes);
}
/* }}} */


/* {{{ proto int mcrypt_enc_self_test(resource td)
   This function runs the self test on the algorithm specified by the descriptor td */
PHP_FUNCTION(mcrypt_enc_self_test)
{
	zval **mcryptind;
	MCRYPT td;
    MCLS_FETCH();
	
	MCRYPT_GET_TD_ARG

	RETURN_LONG (mcrypt_enc_self_test (td));
}
/* }}} */


PHP_FUNCTION(mcrypt_module_close)
{
	zval **mcryptind;
	MCRYPT td;
    MCLS_FETCH();
	
	MCRYPT_GET_TD_ARG

	if (mcrypt_module_close (td) < 0) {
		php_error (E_WARNING, "could not close module");
		RETURN_FALSE
	} else {
		zend_list_delete (Z_LVAL_PP(mcryptind));
		RETURN_TRUE
	}
}


/* {{{ proto bool mcrypt_generic_end(resource td)
   This function terminates encrypt specified by the descriptor td */
PHP_FUNCTION(mcrypt_generic_end)
{
	zval **mcryptind;
	MCRYPT td;
    MCLS_FETCH();
	
	MCRYPT_GET_TD_ARG

	if (mcrypt_generic_end (td) < 0) {
		php_error (E_WARNING, "could not terminate encryption specifier");
		RETURN_FALSE
	}
	RETURN_TRUE
}
/* }}} */


/* {{{ proto bool mcrypt_enc_is_block_algorithm_mode(resource td)
   Returns TRUE if the mode is for use with block algorithms */
PHP_FUNCTION(mcrypt_enc_is_block_algorithm_mode)
{
	zval **mcryptind;
	MCRYPT td;
    MCLS_FETCH();
	
	MCRYPT_GET_TD_ARG

	if (mcrypt_enc_is_block_algorithm_mode (td) == 1)
		RETURN_TRUE
	else
		RETURN_FALSE
}
/* }}} */


/* {{{ proto bool mcrypt_enc_is_block_algorithm(resource td)
   Returns TRUE if the alrogithm is a block algorithms */
PHP_FUNCTION(mcrypt_enc_is_block_algorithm)
{
	zval **mcryptind;
	MCRYPT td;
    MCLS_FETCH();
	
	MCRYPT_GET_TD_ARG

	if (mcrypt_enc_is_block_algorithm (td) == 1)
		RETURN_TRUE
	else
		RETURN_FALSE
}
/* }}} */


/* {{{ proto bool mcrypt_enc_is_block_mode(resource td)
   Returns TRUE if the mode outputs blocks */
PHP_FUNCTION(mcrypt_enc_is_block_mode)
{
	zval **mcryptind;
	MCRYPT td;
    MCLS_FETCH();
	
	MCRYPT_GET_TD_ARG

	if (mcrypt_enc_is_block_mode (td) == 1)
		RETURN_TRUE
	else
		RETURN_FALSE
}
/* }}} */


/* {{{ proto int mcrypt_enc_get_block_size(resource td)
   Returns the block size of the cipher specified by the descriptor td */
PHP_FUNCTION(mcrypt_enc_get_block_size)
{
	zval **mcryptind;
	MCRYPT td;
    MCLS_FETCH();
	
	MCRYPT_GET_TD_ARG

	RETURN_LONG (mcrypt_enc_get_block_size (td));
}
/* }}} */


/* {{{ proto int mcrypt_enc_get_key_size(resource td)
   Returns the maximum supported key size in bytes of the algorithm specified by the descriptor td */
PHP_FUNCTION(mcrypt_enc_get_key_size)
{
	zval **mcryptind;
	MCRYPT td;
    MCLS_FETCH();
	
	MCRYPT_GET_TD_ARG

	RETURN_LONG (mcrypt_enc_get_key_size (td));
}
/* }}} */


/* {{{ proto int mcrypt_enc_get_iv_size(resource td)
   Returns the size of the IV in bytes of the algorithm specified by the descriptor td */
PHP_FUNCTION(mcrypt_enc_get_iv_size)
{
	zval **mcryptind;
	MCRYPT td;
    MCLS_FETCH();
	
	MCRYPT_GET_TD_ARG

	RETURN_LONG (mcrypt_enc_get_iv_size (td));
}
/* }}} */


/* {{{ proto string mcrypt_enc_get_algorithms_name(resource td)
   Returns the name of the algorithm specified by the descriptor td */
PHP_FUNCTION(mcrypt_enc_get_algorithms_name)
{
	zval **mcryptind;
	MCRYPT td;
	char *name;
    MCLS_FETCH();
	
	MCRYPT_GET_TD_ARG

	name = mcrypt_enc_get_algorithms_name (td);
	RETVAL_STRING (name, 1);
	mcrypt_free (name);
}
/* }}} */


/* {{{ proto string mcrypt_enc_get_modes_name(resource td)
   Returns the name of the mode specified by the descriptor td */
PHP_FUNCTION(mcrypt_enc_get_modes_name)
{
	zval **mcryptind;
	MCRYPT td;
	char *name;
    MCLS_FETCH();
	
	MCRYPT_GET_TD_ARG

	name = mcrypt_enc_get_modes_name (td);
	RETVAL_STRING (name, 1);
	mcrypt_free (name);
}
/* }}} */


/* {{{ proto bool mcrypt_module_self_test(string algorithm [, string lib_dir])
   Does a self test of the module "module" */
PHP_FUNCTION(mcrypt_module_self_test)
{
	zval **arg1, **lib_dir;
	char *lib_dir_s;
	int argc;
    MCLS_FETCH();
	
	argc = ZEND_NUM_ARGS();

	MCRYPT_GET_MODE_DIR_ARGS(algorithms_dir);
	
	if (mcrypt_module_self_test ((*arg1)->value.str.val, lib_dir_s) == 0) {
		RETURN_TRUE;
	}
	else {
		RETURN_FALSE;
	}
}
/* }}} */


/* {{{ proto bool mcrypt_module_is_block_algorithm_mode(string mode [, string lib_dir])
   Returns TRUE if the mode is for use with block algorithms */
PHP_FUNCTION(mcrypt_module_is_block_algorithm_mode)
{
	zval **arg1, **lib_dir;
	char *lib_dir_s;
	int argc;
    MCLS_FETCH();
	
	argc = ZEND_NUM_ARGS();

	MCRYPT_GET_MODE_DIR_ARGS(modes_dir)
	
	if (mcrypt_module_is_block_algorithm_mode ((*arg1)->value.str.val, lib_dir_s) == 0) {
		RETURN_TRUE;
	}
	else {
		RETURN_FALSE;
	}
}
/* }}} */


/* {{{ proto bool mcrypt_module_is_block_algorithm(string algorithm [, string lib_dir])
   Returns TRUE if the algorithm is a block algorithm */
PHP_FUNCTION(mcrypt_module_is_block_algorithm)
{
	zval **arg1, **lib_dir;
	char *lib_dir_s;
	int argc;
    MCLS_FETCH();
	
	argc = ZEND_NUM_ARGS();

	MCRYPT_GET_MODE_DIR_ARGS(algorithms_dir)
	
	if (mcrypt_module_is_block_algorithm ((*arg1)->value.str.val, lib_dir_s) == 0) {
		RETURN_TRUE;
	}
	else {
		RETURN_FALSE;
	}
}
/* }}} */


/* {{{ proto bool mcrypt_module_is_block_mode(string mode [, string lib_dir])
   Returns TRUE if the mode outputs blocks of bytes */
PHP_FUNCTION(mcrypt_module_is_block_mode)
{
	zval **arg1, **lib_dir;
	char *lib_dir_s;
	int argc;
    MCLS_FETCH();
	
	argc = ZEND_NUM_ARGS();

	MCRYPT_GET_MODE_DIR_ARGS(modes_dir)
	
	if (mcrypt_module_is_block_mode ((*arg1)->value.str.val, lib_dir_s) == 0) {
		RETURN_TRUE;
	}
	else {
		RETURN_FALSE;
	}
}
/* }}} */


/* {{{ proto int mcrypt_module_get_algo_block_size(string algorithm [, string lib_dir])
   Returns the block size of the algorithm */
PHP_FUNCTION(mcrypt_module_get_algo_block_size)
{
	zval **arg1, **lib_dir;
	char *lib_dir_s;
	int argc;
    MCLS_FETCH();
	
	argc = ZEND_NUM_ARGS();

	MCRYPT_GET_MODE_DIR_ARGS(algorithms_dir)
	
	RETURN_LONG(mcrypt_module_get_algo_block_size ((*arg1)->value.str.val, lib_dir_s))
}
/* }}} */


/* {{{ proto int mcrypt_module_get_algo_key_size(string algorithm [, string lib_dir])
   Returns the maximum supported key size of the algorithm */
PHP_FUNCTION(mcrypt_module_get_algo_key_size)
{
	zval **arg1, **lib_dir;
	char *lib_dir_s;
	int argc;
    MCLS_FETCH();
	
	argc = ZEND_NUM_ARGS();

	MCRYPT_GET_MODE_DIR_ARGS(algorithms_dir);
	
	RETURN_LONG(mcrypt_module_get_algo_key_size ((*arg1)->value.str.val, lib_dir_s))
}
/* }}} */


/* {{{ proto int mcrypt_module_get_supported_key_sizes(string algorithm [, string lib_dir])
   This function decrypts the crypttext */
PHP_FUNCTION(mcrypt_module_get_supported_key_sizes)
{
	zval **arg1, **lib_dir;
	char *lib_dir_s;
	int argc, i, count;
	int *key_sizes;
    MCLS_FETCH();
	
	argc = ZEND_NUM_ARGS();

	MCRYPT_GET_MODE_DIR_ARGS(algorithms_dir)

	key_sizes = mcrypt_module_get_algo_supported_key_sizes ((*arg1)->value.str.val, lib_dir_s, &count);

	if (array_init(return_value) == FAILURE) {
		php_error(E_ERROR, "Unable to initialize array");
		return;
	}
	if (count != 0) {
		for (i = 0; i < count; i++) {
			add_index_long(return_value, i, key_sizes[i]);
		}
	}
	mcrypt_free (key_sizes);
}
/* }}} */


/* {{{ proto array mcrypt_list_algorithms([string lib_dir])
   List all algorithms in "module_dir" */
PHP_FUNCTION(mcrypt_list_algorithms)
{
	zval **lib_dir;
	char **modules;
	char *lib_dir_s;
	int i, count, argc;

	argc = ZEND_NUM_ARGS();
	MCRYPT_CHECK_PARAM_COUNT (0,1)
	
	switch (argc) {
		case 1:
			if (zend_get_parameters_ex (1, &lib_dir) == FAILURE) {
				WRONG_PARAM_COUNT
			}
			convert_to_string_ex (lib_dir);
			lib_dir_s = Z_STRVAL_PP(lib_dir);
			break;
		case 0:
			lib_dir_s = INI_STR("mcrypt.algorithms_dir");
			break;
		default:
			WRONG_PARAM_COUNT
	}	

	modules = mcrypt_list_algorithms (lib_dir_s, &count);

	if (array_init(return_value) == FAILURE) {
		php_error(E_ERROR, "Unable to initialize array");
		return;
	}
	if (count == 0) {
		php_error (E_WARNING, "No algorithms found in module dir");
	}
	for (i = 0; i < count; i++) {
		add_index_string(return_value, i, modules[i], 1);
	}
	mcrypt_free_p (modules, count);
}
/* }}} */


/* {{{ proto array mcrypt_list_modes([string lib_dir])
   List all modes "module_dir" */
PHP_FUNCTION(mcrypt_list_modes)
{
	zval **lib_dir;
	char **modules;
	char *lib_dir_s;
	int i, count, argc;
    MCLS_FETCH();

	argc = ZEND_NUM_ARGS();
	MCRYPT_CHECK_PARAM_COUNT (0,1)
	
	switch (argc) {
		case 1:
			if (zend_get_parameters_ex (1, &lib_dir) == FAILURE) {
				WRONG_PARAM_COUNT
			}
			convert_to_string_ex (lib_dir);
			lib_dir_s = Z_STRVAL_PP(lib_dir);
			break;
		case 0:
			lib_dir_s = MCG(modes_dir);
			break;
		default:
			WRONG_PARAM_COUNT
	}	

	modules = mcrypt_list_modes (lib_dir_s, &count);

	if (array_init(return_value) == FAILURE) {
		php_error(E_ERROR, "Unable to initialize array");
		return;
	}
	if (count == 0) {
		php_error (E_WARNING, "No modes found in module dir");
	}
	for (i = 0; i < count; i++) {
		add_index_string(return_value, i, modules[i], 1);
	}
	mcrypt_free_p (modules, count);
}
/* }}} */


/* {{{ proto int mcrypt_get_key_size(string cipher, string module)
   Get the key size of cipher */
PHP_FUNCTION(mcrypt_get_key_size)
{
	zval **cipher;
	zval **module;
	char *cipher_dir_string;
	char *module_dir_string;
	long key_size;
	MCRYPT td;
    MCLS_FETCH();

	MCRYPT_GET_INI

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &cipher, &module) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_string_ex(cipher);
	convert_to_string_ex(module);
	
	td = mcrypt_module_open(
		Z_STRVAL_PP(cipher), 
		cipher_dir_string,
		Z_STRVAL_PP(module),
		module_dir_string);
	if (td != MCRYPT_FAILED)
	{
		key_size = mcrypt_enc_get_key_size(td);
		mcrypt_module_close(td);
		RETVAL_LONG(key_size);
	}
	else
	{
		php_error (E_WARNING, MCRYPT_OPEN_MODULE_FAILED);
		RETVAL_LONG(0);
	}
}
/* }}} */


/* {{{ proto int mcrypt_get_block_size(string cipher, string module)
   Get the key size of cipher */
PHP_FUNCTION(mcrypt_get_block_size)
{
	zval **cipher;
	zval **module;
	char *cipher_dir_string;
	char *module_dir_string;
	long key_size;
	MCRYPT td;
    MCLS_FETCH();

	MCRYPT_GET_INI

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &cipher, &module) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_string_ex(cipher);
	convert_to_string_ex(module);
	
	td = mcrypt_module_open(
		Z_STRVAL_PP(cipher), 
		cipher_dir_string,
		Z_STRVAL_PP(module),
		module_dir_string);
	if (td != MCRYPT_FAILED)
	{
		key_size = mcrypt_enc_get_block_size(td);
		mcrypt_module_close(td);
		RETVAL_LONG(key_size);
	}
	else
	{
		php_error (E_WARNING, MCRYPT_OPEN_MODULE_FAILED);
		RETVAL_LONG(0);
	}
}
/* }}} */


/* {{{ proto int mcrypt_get_iv_size(string cipher, string module)
   Get the IV size of cipher (Usually the same as the blocksize) */
PHP_FUNCTION(mcrypt_get_iv_size)
{
	zval **cipher;
	zval **module;
	char *cipher_dir_string;
	char *module_dir_string;
	long key_size;
	MCRYPT td;
    MCLS_FETCH();

	MCRYPT_GET_INI

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &cipher, &module) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_string_ex(cipher);
	convert_to_string_ex(module);
	
	td = mcrypt_module_open(
		Z_STRVAL_PP(cipher), 
		cipher_dir_string,
		Z_STRVAL_PP(module),
		module_dir_string);
	if (td != MCRYPT_FAILED)
	{
		key_size = mcrypt_enc_get_iv_size(td);
		mcrypt_module_close(td);
		RETURN_LONG(key_size);
	}
	else
	{
		php_error (E_WARNING, MCRYPT_OPEN_MODULE_FAILED);
		RETURN_FALSE;
	}
}
/* }}} */


/* {{{ proto string mcrypt_get_cipher_name(string cipher)
   Get the key size of cipher */
PHP_FUNCTION(mcrypt_get_cipher_name)
{
	zval **cipher;
	char *cipher_dir_string;
	char *module_dir_string;
	char *cipher_name;
	MCRYPT td;
    MCLS_FETCH();

	MCRYPT_GET_INI

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &cipher) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_string_ex(cipher);

	/* The code below is actually not very nice, but I didn see a better method */
	td = mcrypt_module_open(
		Z_STRVAL_PP(cipher), 
		cipher_dir_string,
		"ecb",
		module_dir_string);
	if (td != MCRYPT_FAILED)
	{
		cipher_name = mcrypt_enc_get_algorithms_name(td);
		mcrypt_module_close(td);
		RETVAL_STRING(cipher_name,1);
		mcrypt_free (cipher_name);
	}
	else
	{
		td = mcrypt_module_open(
			Z_STRVAL_PP(cipher), 
			cipher_dir_string,
			"stream",
			module_dir_string);
		if (td != MCRYPT_FAILED)
		{
			cipher_name = mcrypt_enc_get_algorithms_name(td);
			mcrypt_module_close(td);
			RETVAL_STRING(cipher_name,1);
			mcrypt_free (cipher_name);
		}
		else
		{
			php_error (E_WARNING, MCRYPT_OPEN_MODULE_FAILED);
			RETURN_FALSE;
		}
	}
}
/* }}} */


static void php_mcrypt_do_crypt (char* cipher, zval **key, zval **data, char *mode, zval **iv, int argc, int dencrypt, zval* return_value)
{
	char *cipher_dir_string;
	char *module_dir_string;
	int block_size, max_key_length, use_key_length, i, count, iv_size;
	unsigned long int data_size;
	int *key_length_sizes;
	char *key_s, *iv_s;
	char *data_s;
	MCRYPT td;
    MCLS_FETCH();

	MCRYPT_GET_INI

	td = mcrypt_module_open (
		cipher, 
		cipher_dir_string, 
		mode, 
		module_dir_string);
	if (td == MCRYPT_FAILED) {
		php_error (E_WARNING, MCRYPT_OPEN_MODULE_FAILED);
		RETURN_FALSE;
	}
	/* Checking for key-length */
	max_key_length = mcrypt_enc_get_key_size (td);
	if (Z_STRLEN_PP(key) > max_key_length) {
		php_error (E_WARNING, "size of key is too large for this algorithm");
	}
	key_length_sizes = mcrypt_enc_get_supported_key_sizes (td, &count);
	if (count == 0 && key_length_sizes == NULL) { /* all lengths 1 - k_l_s = OK */
		key_s = estrdup (Z_STRVAL_PP(key));
		use_key_length = Z_STRLEN_PP(key);
	}
	else if (count == 1) {  /* only m_k_l = OK */
		key_s = emalloc (key_length_sizes[0]);
		memset (key_s, 0, key_length_sizes[0]);
		strcpy (key_s, Z_STRVAL_PP(key));
		use_key_length = key_length_sizes[0];
	}
	else { /* derterminating smallest supported key > length of requested key */
		use_key_length = max_key_length; /* start with max key length */
		for (i = 0; i < count; i++) {
			if (key_length_sizes[i] >= Z_STRLEN_PP(key) && 
				key_length_sizes[i] < use_key_length)
			{
				use_key_length = key_length_sizes[i];
			}
		}
		key_s = emalloc (use_key_length);
		memset (key_s, 0, use_key_length);
		strcpy (key_s, Z_STRVAL_PP(key));
	}
	mcrypt_free (key_length_sizes);
	
	/* Check IV */
	iv_s = NULL;
	iv_size = mcrypt_enc_get_iv_size (td);
	if (argc == 5) {
		if (iv_size != Z_STRLEN_PP(iv)) {
			php_error (E_WARNING, MCRYPT_IV_WRONG_SIZE);
			
		}
		else {
			iv_s = emalloc (iv_size + 1);
			memcpy (iv_s, Z_STRVAL_PP(iv), iv_size);
		}
	}
	else if (argc == 4)
	{
		if (iv_size != 0) {
			php_error (E_WARNING, "attempt to use an empty IV, which is NOT recommend");
			iv_s = emalloc (iv_size + 1);
			memset (iv_s, 0, iv_size + 1);
		}
	}

	/* Check blocksize */
	if (mcrypt_enc_is_block_algorithm (td) == 1) { /* It's a block algorithm */
		block_size = mcrypt_enc_get_block_size (td);
		data_size = (((Z_STRLEN_PP(data) - 1) / block_size) + 1) * block_size;
		data_s = emalloc (data_size);
		memset (data_s, 0, data_size);
		memcpy (data_s, Z_STRVAL_PP(data), Z_STRLEN_PP(data));
	}
	else { /* It's not a block algorithm */
		data_size = Z_STRLEN_PP(data);
		data_s = emalloc (data_size);
		memset (data_s, 0, data_size);
		memcpy (data_s, Z_STRVAL_PP(data), Z_STRLEN_PP(data));
	}
	
	if (mcrypt_generic_init (td, key_s, use_key_length, iv_s) < 0) {
		php_error (E_ERROR, "generic_init failed");
	}
	if (dencrypt == MCRYPT_ENCRYPT)
		mcrypt_generic (td, data_s, data_size);
	else
		mdecrypt_generic (td, data_s, data_size);
	
	RETVAL_STRINGL (data_s, data_size, 1);

/* freeing vars */
	mcrypt_generic_end (td);
	if (iv_s != NULL)
		efree (iv_s);
	efree (data_s);
	mcrypt_module_close (td);
}

/* {{{ proto string mcrypt_encrypt(string cipher, string key, string data, string mode, string iv)
   OFB crypt/decrypt data using key key with cipher cipher starting with iv */
PHP_FUNCTION(mcrypt_encrypt)
{
	zval **cipher, **key, **data, **mode, **iv;
	int argc;
	
	argc = ZEND_NUM_ARGS();
	
	MCRYPT_CHECK_PARAM_COUNT (4, 5)
	MCRYPT_GET_CRYPT_ARGS

	MCRYPT_CONVERT;

	php_mcrypt_do_crypt (Z_STRVAL_PP(cipher), key, data, Z_STRVAL_PP(mode), iv, argc, MCRYPT_ENCRYPT, return_value);
}


/* {{{ proto string mcrypt_decrypt(string cipher, string key, string data, string mode, string iv)
   OFB crypt/decrypt data using key key with cipher cipher starting with iv */
PHP_FUNCTION(mcrypt_decrypt)
{
	zval **cipher, **key, **data, **mode, **iv;
	int argc;
	
	argc = ZEND_NUM_ARGS();

	MCRYPT_CHECK_PARAM_COUNT (4, 5)
	MCRYPT_GET_CRYPT_ARGS

	MCRYPT_CONVERT;

	php_mcrypt_do_crypt (Z_STRVAL_PP(cipher), key, data, Z_STRVAL_PP(mode), iv, argc, MCRYPT_DECRYPT, return_value);
}


/* {{{ proto string mcrypt_ecb(int cipher, string key, string data, int mode, string iv)
   ECB crypt/decrypt data using key key with cipher cipher starting with iv */
PHP_FUNCTION(mcrypt_ecb)
{
	zval **cipher, **key, **data, **mode, **iv;
	int argc;
	
	argc = ZEND_NUM_ARGS();

	MCRYPT_CHECK_PARAM_COUNT (4, 5)
	MCRYPT_GET_CRYPT_ARGS

	MCRYPT_CONVERT_WO_MODE;
	convert_to_long_ex(mode);

	php_mcrypt_do_crypt (Z_STRVAL_PP(cipher), key, data, "ecb", iv, argc, Z_LVAL_PP(mode), return_value);
}


/* {{{ proto string mcrypt_cbc(int cipher, string key, string data, int mode, string iv)
   CBC crypt/decrypt data using key key with cipher cipher starting with iv */
PHP_FUNCTION(mcrypt_cbc)
{
	zval **cipher, **key, **data, **mode, **iv;
	int argc;
	
	argc = ZEND_NUM_ARGS();

	MCRYPT_CHECK_PARAM_COUNT (4, 5)
	MCRYPT_GET_CRYPT_ARGS

	MCRYPT_CONVERT_WO_MODE;
	convert_to_long_ex(mode);

	php_mcrypt_do_crypt (Z_STRVAL_PP(cipher), key, data, "cbc", iv, argc, Z_LVAL_PP(mode), return_value);
}


/* {{{ proto string mcrypt_cfb(int cipher, string key, string data, int mode, string iv)
   CFB crypt/decrypt data using key key with cipher cipher starting with iv */
PHP_FUNCTION(mcrypt_cfb)
{
	zval **cipher, **key, **data, **mode, **iv;
	int argc;
	
	argc = ZEND_NUM_ARGS();

	MCRYPT_CHECK_PARAM_COUNT (4, 5)
	MCRYPT_GET_CRYPT_ARGS

	MCRYPT_CONVERT_WO_MODE;
	convert_to_long_ex(mode);

	php_mcrypt_do_crypt (Z_STRVAL_PP(cipher), key, data, "cfb", iv, argc, Z_LVAL_PP(mode), return_value);
}


/* {{{ proto string mcrypt_ofb(int cipher, string key, string data, int mode, string iv)
   OFB crypt/decrypt data using key key with cipher cipher starting with iv */
PHP_FUNCTION(mcrypt_ofb)
{
	zval **cipher, **key, **data, **mode, **iv;
	int argc;
	
	argc = ZEND_NUM_ARGS();

	MCRYPT_CHECK_PARAM_COUNT (4, 5)
	MCRYPT_GET_CRYPT_ARGS

	MCRYPT_CONVERT_WO_MODE;
	convert_to_long_ex(mode);

	php_mcrypt_do_crypt (Z_STRVAL_PP(cipher), key, data, "ofb", iv, argc, Z_LVAL_PP(mode), return_value);
}

#endif

/* {{{ proto string mcrypt_create_iv(int size, int source)
   Create an initializing vector (IV) */
PHP_FUNCTION(mcrypt_create_iv)
{
	zval **size, **psource;
	char *iv;
	iv_source source;
	int i;
	int n = 0;

	if(ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &size, &psource) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long_ex(size);
	convert_to_long_ex(psource);
	
	source = Z_LVAL_PP(psource);
	i = Z_LVAL_PP(size);
	
	if(i <= 0) {
		php_error(E_WARNING, "can not create an IV with size 0 or smaller");
		RETURN_FALSE;
	}
	
	iv = ecalloc(i, 1);
	
	if(source == RANDOM || source == URANDOM) {
		int fd;
		size_t read_bytes = 0;

		fd = open(source == RANDOM ? "/dev/random" : "/dev/urandom",
				O_RDONLY);
		if(fd < 0) {
			efree(iv);
			php_error(E_WARNING, "cannot open source device");
			RETURN_FALSE;
		}
		while (read_bytes < i) {
			n = read(fd, iv + read_bytes, i - read_bytes);
			if (n < 0)
				break;
			read_bytes += n;
		}
		n = read_bytes;
		close(fd);
	} else {
		while(i) {
			iv[--i] = 255.0 * rand() / RAND_MAX;
		}
		n = Z_LVAL_PP(size);
	}
	RETURN_STRINGL(iv, n, 0);
}
/* }}} */


#if HAVE_LIBMCRYPT22

/* {{{ proto string mcrypt_get_cipher_name(int cipher)
   Get the name of cipher */
PHP_FUNCTION(mcrypt_get_cipher_name)
{
	zval **cipher;
	char *str, *nstr;

	if(ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &cipher) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long_ex(cipher);

	str = mcrypt_get_algorithms_name(Z_LVAL_PP(cipher));
	if (str) {
		nstr = estrdup(str);
		free(str);
		RETURN_STRING(nstr, 0);
	}
	
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto int mcrypt_get_key_size(int cipher)
   Get the key size of cipher */
PHP_FUNCTION(mcrypt_get_key_size)
{
	zval **cipher;

	if(ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &cipher) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long_ex(cipher);

	RETURN_LONG(mcrypt_get_key_size(Z_LVAL_PP(cipher)));
}
/* }}} */

/* {{{ proto int mcrypt_get_block_size(int cipher)
   Get the block size of cipher */
PHP_FUNCTION(mcrypt_get_block_size)
{
	zval **cipher;

	if(ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &cipher) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long_ex(cipher);

	RETURN_LONG(mcrypt_get_block_size(Z_LVAL_PP(cipher)));
}
/* }}} */

/* {{{ proto string mcrypt_ofb(int cipher, string key, string data, int mode, string iv)
   OFB crypt/decrypt data using key key with cipher cipher starting with iv */
PHP_FUNCTION(mcrypt_ofb)
{
	MCRYPT_ARGS;
	
	if(ZEND_NUM_ARGS() != 5 || 
			zend_get_parameters_ex(5, &cipher, &key, &data, &mode, &iv) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	MCRYPT_CONVERT;
	MCRYPT_SIZE;
	MCRYPT_CHECK_IV;

	td = init_mcrypt_ofb(Z_LVAL_PP(cipher), Z_STRVAL_PP(key), Z_STRLEN_PP(key), Z_STRVAL_PP(iv));
	MCRYPT_CHECK_TD_CPY;
	MCRYPT_ACTION(ofb);

	RETURN_STRINGL(ndata, nsize, 0);
}
/* }}} */

/* {{{ proto string mcrypt_cfb(int cipher, string key, string data, int mode, string iv)
   CFB crypt/decrypt data using key key with cipher cipher starting with iv */
PHP_FUNCTION(mcrypt_cfb)
{
	MCRYPT_ARGS;

	if(ZEND_NUM_ARGS() != 5 || 
			zend_get_parameters_ex(5, &cipher, &key, &data, &mode, &iv) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	MCRYPT_CONVERT;
	MCRYPT_SIZE;
	MCRYPT_CHECK_IV;

	td = init_mcrypt_cfb(Z_LVAL_PP(cipher), Z_STRVAL_PP(key), Z_STRLEN_PP(key), Z_STRVAL_PP(iv));
	MCRYPT_CHECK_TD_CPY;
	MCRYPT_ACTION(cfb);

	RETURN_STRINGL(ndata, nsize, 0);
}
/* }}} */


/* {{{ proto string mcrypt_cbc(int cipher, string key, string data, int mode [, string iv])
   CBC crypt/decrypt data using key key with cipher cipher using optional iv */
PHP_FUNCTION(mcrypt_cbc)
{
	MCRYPT_ARGS;
	int ac = ZEND_NUM_ARGS();

	if(ac < 4 || ac > 5 || 
			zend_get_parameters_ex(ac, &cipher, &key, &data, &mode, &iv) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	MCRYPT_CONVERT;
	MCRYPT_SIZE;
	if(ac > 4) {
		MCRYPT_CHECK_IV;
	}
	
	td = init_mcrypt_cbc(Z_LVAL_PP(cipher), Z_STRVAL_PP(key), Z_STRLEN_PP(key));
	MCRYPT_CHECK_TD_CPY;
	
	if(ac > 4) {
		mcrypt(td, Z_STRVAL_PP(iv));
	}
	
	MCRYPT_ACTION(cbc);

	RETURN_STRINGL(ndata, nsize, 0);
}
/* }}} */

/* {{{ proto string mcrypt_ecb(int cipher, string key, string data, int mode)
   ECB crypt/decrypt data using key key with cipher cipher */
PHP_FUNCTION(mcrypt_ecb)
{
	MCRYPT_ARGS2;

	if(ZEND_NUM_ARGS() != 4 || 
			zend_get_parameters_ex(4, &cipher, &key, &data, &mode) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	MCRYPT_CONVERT;
	MCRYPT_SIZE;
	
	td = init_mcrypt_ecb(Z_LVAL_PP(cipher), Z_STRVAL_PP(key), Z_STRLEN_PP(key));
	MCRYPT_CHECK_TD_CPY;
	MCRYPT_ACTION(ecb);

	RETURN_STRINGL(ndata, nsize, 0);
}
/* }}} */

#endif /* MCRYPT_2_2 */

#endif
