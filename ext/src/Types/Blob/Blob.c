/**
 * Copyright 2015-2017 DataStax, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "php_driver.h"
#include "php_driver_types.h"
#include "util/bytes.h"
#include "util/types.h"

zend_class_entry *php_driver_blob_ce = NULL;

/* {{{ Blob::__construct(string) */
PHP_METHOD(Blob, __construct)
{
  php_driver_blob* self;
  char* blob;
  size_t blob_len;

  ZEND_PARSE_PARAMETERS_START(1, 1)
  Z_PARAM_STRING(blob, blob_len);
  ZEND_PARSE_PARAMETERS_END();

  if (getThis() && instanceof_function(Z_OBJCE_P(getThis()), php_driver_blob_ce)) {
    self = PHP_DRIVER_GET_BLOB(getThis());
  } else {
    object_init_ex(return_value, php_driver_blob_ce);
    self = PHP_DRIVER_GET_BLOB(return_value);
  }

  self->data = (cass_byte_t*) blob;
  self->size = blob_len;
}
/* }}} */

/* {{{ Blob::__toString() */
PHP_METHOD(Blob, __toString)
{
  php_driver_blob *self = PHP_DRIVER_GET_BLOB(getThis());
  char *hex;
  int hex_len;
  php_driver_bytes_to_hex((const char *) self->data, self->size, &hex, &hex_len);

  RETURN_STRINGL_FAST(hex, hex_len);
}
/* }}} */

/* {{{ Blob::type() */
PHP_METHOD(Blob, type)
{
  zval type = php_driver_type_scalar(CASS_VALUE_TYPE_BLOB);
  RETURN_ZVAL(PHP5TO7_ZVAL_MAYBE_P(type), 1, 1);
}
/* }}} */

/* {{{ Blob::bytes() */
PHP_METHOD(Blob, bytes)
{
  php_driver_blob *self = PHP_DRIVER_GET_BLOB(getThis());
  char *hex;
  int hex_len;
  php_driver_bytes_to_hex((const char *) self->data, self->size, &hex, &hex_len);

  PHP5TO7_RETVAL_STRINGL(hex, hex_len);
  efree(hex);
}
/* }}} */

/* {{{ Blob::toBinaryString() */
PHP_METHOD(Blob, toBinaryString)
{
  php_driver_blob *blob = PHP_DRIVER_GET_BLOB(getThis());

  PHP5TO7_RETVAL_STRINGL((const char *)blob->data, blob->size);
}
/* }}} */

ZEND_BEGIN_ARG_INFO_EX(arginfo__construct, 0, ZEND_RETURN_VALUE, 1)
  ZEND_ARG_INFO(0, bytes)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_none, 0, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()

static zend_function_entry php_driver_blob_methods[] = {
  PHP_ME(Blob, __construct, arginfo__construct, ZEND_ACC_CTOR|ZEND_ACC_PUBLIC)
  PHP_ME(Blob, __toString, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(Blob, bytes, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(Blob, toBinaryString, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_FE_END
};

static php_driver_value_handlers php_driver_blob_handlers;

static HashTable*
php_driver_blob_gc(zend_object* object, zval** table, int* n)
{
  *table = NULL;
  *n = 0;
  return zend_std_get_properties(object);
}

static HashTable*
php_driver_blob_properties(zend_object* object)
{
  char *hex;
  int hex_len;
  zval type;
  zval bytes;

  php_driver_blob* self = PHP5TO7_ZEND_OBJECT_GET(blob, object);
  HashTable* props      = zend_std_get_properties(object);

  type = php_driver_type_scalar(CASS_VALUE_TYPE_BLOB);
  PHP5TO7_ZEND_HASH_UPDATE(props, "type", sizeof("type"), PHP5TO7_ZVAL_MAYBE_P(type), sizeof(zval));

  php_driver_bytes_to_hex((const char *) self->data, self->size, &hex, &hex_len);
  PHP5TO7_ZVAL_MAYBE_MAKE(bytes);
  PHP5TO7_ZVAL_STRINGL(PHP5TO7_ZVAL_MAYBE_P(bytes), hex, hex_len);
  efree(hex);
  PHP5TO7_ZEND_HASH_UPDATE(props, "bytes", sizeof("bytes"), PHP5TO7_ZVAL_MAYBE_P(bytes), sizeof(zval));

  return props;
}

static int
php_driver_blob_compare(zval* obj1, zval* obj2)
{
  ZEND_COMPARE_OBJECTS_FALLBACK(obj1, obj2);
  php_driver_blob *blob1 = NULL;
  php_driver_blob *blob2 = NULL;

  if (Z_OBJCE_P(obj1) != Z_OBJCE_P(obj2)) {
    return 1; /* different classes */
  }

  blob1 = PHP_DRIVER_GET_BLOB(obj1);
  blob2 = PHP_DRIVER_GET_BLOB(obj2);

  if (blob1->size == blob2->size) {
    return memcmp((const char *) blob1->data, (const char *) blob2->data, blob1->size);
  } else if (blob1->size < blob2->size) {
    return -1;
  } else {
    return 1;
  }
}

static unsigned
php_driver_blob_hash_value(zval* obj)
{
  php_driver_blob *self = PHP_DRIVER_GET_BLOB(obj);
  return zend_inline_hash_func((const char *) self->data, self->size);
}

static void
php_driver_blob_free(zend_object* object)
{
  php_driver_blob *self = PHP5TO7_ZEND_OBJECT_GET(blob, object);

  if (self->data) {
    efree(self->data);
  }

  zend_object_std_dtor(&self->zval);
  }

static zend_object*
php_driver_blob_new(zend_class_entry* ce)
{
  php_driver_blob *self =
      PHP5TO7_ZEND_OBJECT_ECALLOC(blob, ce);

  PHP5TO7_ZEND_OBJECT_INIT(blob, self, ce);
}

void
php_driver_define_Blob()
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, PHP_DRIVER_NAMESPACE "\\Blob", php_driver_blob_methods);
  php_driver_blob_ce = zend_register_internal_class(&ce);
  zend_class_implements(php_driver_blob_ce, 1, php_driver_value_ce);
  memcpy(&php_driver_blob_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));

  php_driver_blob_handlers.std.get_properties  = php_driver_blob_properties;
  php_driver_blob_handlers.std.get_gc          = php_driver_blob_gc;
  php_driver_blob_handlers.std.compare         = php_driver_blob_compare;
  php_driver_blob_ce->ce_flags |= ZEND_ACC_FINAL;
  php_driver_blob_ce->create_object = php_driver_blob_new;

  php_driver_blob_handlers.hash_value = php_driver_blob_hash_value;
  php_driver_blob_handlers.std.clone_obj = NULL;
}
