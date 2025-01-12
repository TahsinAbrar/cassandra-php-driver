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

#include "src/Types/Container/Tuple/Tuple.h"
#include "php_driver.h"
#include "php_driver_types.h"
#include "util/collections.h"
#include "util/types.h"
#if PHP_MAJOR_VERSION >= 7
#include <zend_smart_str.h>
#else
#include <ext/standard/php_smart_str.h>
#endif

zend_class_entry *php_driver_type_tuple_ce = NULL;

int
php_driver_type_tuple_add(php_driver_type* type,
                          zval* zsub_type)
{
  php_driver_type *sub_type = PHP_DRIVER_GET_TYPE(zsub_type);
  if (cass_data_type_add_sub_type(type->data_type,
                                  sub_type->data_type) != CASS_OK) {
    return 0;
  }
  PHP5TO7_ZEND_HASH_NEXT_INDEX_INSERT(&type->data.tuple.types,
                                      zsub_type, sizeof(zval *));
  return 1;
}

PHP_METHOD(TypeTuple, __construct)
{
  zend_throw_exception_ex(php_driver_logic_exception_ce, 0,
                          "Instantiation of a " PHP_DRIVER_NAMESPACE "\\Type\\Tuple type is not supported.");
}

PHP_METHOD(TypeTuple, name)
{
  if (zend_parse_parameters_none() == FAILURE) {
    return;
  }

  RETVAL_STRING("tuple");
}

PHP_METHOD(TypeTuple, types)
{
  php_driver_type *self;

  if (zend_parse_parameters_none() == FAILURE) {
    return;
  }

  self = PHP_DRIVER_GET_TYPE(getThis());

  array_init(return_value);
  PHP5TO7_ZEND_HASH_ZVAL_COPY(Z_ARRVAL_P(return_value), &self->data.tuple.types);
}

PHP_METHOD(TypeTuple, __toString)
{
  php_driver_type *self;
  smart_str string = PHP5TO7_SMART_STR_INIT;

  if (zend_parse_parameters_none() == FAILURE) {
    return;
  }

  self = PHP_DRIVER_GET_TYPE(getThis());

  php_driver_type_string(self, &string);
  smart_str_0(&string);

  RETVAL_STRING(PHP5TO7_SMART_STR_VAL(string));
  smart_str_free(&string);
}

PHP_METHOD(TypeTuple, create)
{
  php_driver_type *self;
  php_driver_tuple *tuple;
  zval* args = NULL;
  int argc = 0, i, num_types;

  if (zend_parse_parameters(ZEND_NUM_ARGS(), "*",
                            &args, &argc)
      == FAILURE) {
    return;
  }

  self = PHP_DRIVER_GET_TYPE(getThis());

  object_init_ex(return_value, php_driver_tuple_ce);
  tuple = PHP_DRIVER_GET_TUPLE(return_value);

  PHP5TO7_ZVAL_COPY(PHP5TO7_ZVAL_MAYBE_P(tuple->type), getThis());

  num_types = zend_hash_num_elements(&self->data.tuple.types);

  if (argc > 0) {
    if (argc != num_types) {
      zend_throw_exception_ex(php_driver_invalid_argument_exception_ce,
                              0,
                              "Invalid number of elements given. Expected %d arguments.",
                              zend_hash_num_elements(&self->data.tuple.types));
            return;
    }

    for (i = 0; i < argc; i++) {
      zval* sub_type;

      if (!PHP5TO7_ZEND_HASH_INDEX_FIND(&self->data.tuple.types, i, sub_type) || !php_driver_validate_object(PHP5TO7_ZVAL_ARG(args[i]), PHP5TO7_ZVAL_MAYBE_DEREF(sub_type))) {
                return;
      }

      php_driver_tuple_set(tuple, i, PHP5TO7_ZVAL_ARG(args[i]));
    }

      }
}

ZEND_BEGIN_ARG_INFO_EX(arginfo_none, 0, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_values, 0, ZEND_RETURN_VALUE, 0)
#if PHP_MAJOR_VERSION >= 8
  ZEND_ARG_VARIADIC_INFO(0, values)
#else
  ZEND_ARG_INFO(0, values)
#endif
ZEND_END_ARG_INFO()

static zend_function_entry php_driver_type_tuple_methods[] = {
  PHP_ME(TypeTuple, __construct, arginfo_none,   ZEND_ACC_PRIVATE)
  PHP_ME(TypeTuple, name,        arginfo_none,   ZEND_ACC_PUBLIC)
  PHP_ME(TypeTuple, __toString,  arginfo_none,   ZEND_ACC_PUBLIC)
  PHP_ME(TypeTuple, types,       arginfo_none,   ZEND_ACC_PUBLIC)
  PHP_ME(TypeTuple, create,      arginfo_values, ZEND_ACC_PUBLIC)
  PHP_FE_END
};

static zend_object_handlers php_driver_type_tuple_handlers;

static HashTable*
php_driver_type_tuple_gc(
#if PHP_MAJOR_VERSION >= 8
  zend_object* object,
#else
  zval* object,
#endif
  zval** table,
  int* n)
{
  *table = NULL;
  *n = 0;
  return zend_std_get_properties(object);
}

static HashTable *
php_driver_type_tuple_properties(
#if PHP_MAJOR_VERSION >= 8
        zend_object *object
#else
  zval* object
#endif
)
{
  zval types;
#if PHP_MAJOR_VERSION >= 8
  php_driver_type *self  = PHP5TO7_ZEND_OBJECT_GET(type, object);
#else
  php_driver_type *self  = PHP_DRIVER_GET_TYPE(object);
#endif
  HashTable* props = zend_std_get_properties(object);


  array_init(PHP5TO7_ZVAL_MAYBE_P(types));
  PHP5TO7_ZEND_HASH_ZVAL_COPY(PHP5TO7_Z_ARRVAL_MAYBE_P(types), &self->data.tuple.types);
  PHP5TO7_ZEND_HASH_UPDATE(props,
                           "types", sizeof("types"),
                           PHP5TO7_ZVAL_MAYBE_P(types), sizeof(zval));

  return props;
}

static int
php_driver_type_tuple_compare(zval* obj1, zval* obj2)
{
#if PHP_MAJOR_VERSION >= 8
  ZEND_COMPARE_OBJECTS_FALLBACK(obj1, obj2);
#endif
  php_driver_type* type1 = PHP_DRIVER_GET_TYPE(obj1);
  php_driver_type* type2 = PHP_DRIVER_GET_TYPE(obj2);

  return php_driver_type_compare(type1, type2);
}

static void
php_driver_type_tuple_free(zend_object* object)
{
  php_driver_type *self = PHP5TO7_ZEND_OBJECT_GET(type, object);

  if (self->data_type) cass_data_type_free(self->data_type);
  zend_hash_destroy(&self->data.tuple.types);

  zend_object_std_dtor(&self->zval);
  }

static zend_object*
php_driver_type_tuple_new(zend_class_entry* ce)
{
  php_driver_type *self = PHP5TO7_ZEND_OBJECT_ECALLOC(type, ce);

  self->type = CASS_VALUE_TYPE_TUPLE;
  self->data_type = cass_data_type_new(self->type);
  zend_hash_init(&self->data.tuple.types, 0, NULL, ZVAL_PTR_DTOR, 0);

  PHP5TO7_ZEND_OBJECT_INIT_EX(type, type_tuple, self, ce);
}

void
php_driver_define_TypeTuple()
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, PHP_DRIVER_NAMESPACE "\\Type\\Tuple", php_driver_type_tuple_methods);
  php_driver_type_tuple_ce = php5to7_zend_register_internal_class_ex(&ce, php_driver_type_ce);
  memcpy(&php_driver_type_tuple_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
  php_driver_type_tuple_handlers.get_properties  = php_driver_type_tuple_properties;
  php_driver_type_tuple_handlers.get_gc          = php_driver_type_tuple_gc;
  php_driver_type_tuple_handlers.compare = php_driver_type_tuple_compare;
  php_driver_type_tuple_ce->ce_flags     |= ZEND_ACC_FINAL;
  php_driver_type_tuple_ce->create_object = php_driver_type_tuple_new;
}
