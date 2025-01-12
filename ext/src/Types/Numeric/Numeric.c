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

#include <php.h>

#include "src/Types/Numeric/Bigint/Bigint.h"
#include "src/Types/Numeric/Decimal/Decimal.h"
#include "src/Types/Numeric/Float/Float.h"
#include "src/Types/Numeric/Smallint/Smallint.h"
#include "src/Types/Numeric/Tinyint/Tinyint.h"
#include "src/Types/Numeric/Varint/Varint.h"

#include "Numeric_arginfo.h"


zend_class_entry* php_driver_numeric_ce = NULL;

void
php_driver_define_Numeric(zend_class_entry* value_interface)
{
  php_driver_numeric_ce = register_class_Cassandra_Numeric();

  php_driver_define_Bigint(value_interface, php_driver_numeric_ce);
  php_driver_define_Decimal(value_interface, php_driver_numeric_ce);
  php_driver_define_Float(value_interface, php_driver_numeric_ce);
  php_driver_define_Smallint(value_interface, php_driver_numeric_ce);
  php_driver_define_Tinyint(value_interface, php_driver_numeric_ce);
  php_driver_define_Varint(value_interface, php_driver_numeric_ce);
}
