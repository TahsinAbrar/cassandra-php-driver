/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: d33d758d771a2858949b5dcca6529fcb5b0d0632 */

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Cassandra_FutureClose_get, 0, 1, IS_MIXED, 0)
	ZEND_ARG_TYPE_MASK(0, timeout, MAY_BE_LONG|MAY_BE_DOUBLE|MAY_BE_NULL, NULL)
ZEND_END_ARG_INFO()


ZEND_METHOD(Cassandra_FutureClose, get);


static const zend_function_entry class_Cassandra_FutureClose_methods[] = {
	ZEND_ME(Cassandra_FutureClose, get, arginfo_class_Cassandra_FutureClose_get, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static zend_class_entry *register_class_Cassandra_FutureClose(zend_class_entry *class_entry_Cassandra_Future)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "Cassandra", "FutureClose", class_Cassandra_FutureClose_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	class_entry->ce_flags |= ZEND_ACC_FINAL|ZEND_ACC_NO_DYNAMIC_PROPERTIES;
	zend_class_implements(class_entry, 1, class_entry_Cassandra_Future);

	return class_entry;
}