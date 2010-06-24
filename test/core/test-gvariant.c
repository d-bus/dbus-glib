#include <dbus/dbus-glib.h>
#include <gio/gio.h>

static void
test_i (void)
{
  GValue v = { 0, };
  GVariant *var, *varc;

  g_value_init (&v, G_TYPE_INT);
  g_value_set_int (&v, 1984);

  var = dbus_g_value_build_g_variant (&v);
  g_value_unset (&v);

  varc = g_variant_new_int32 (1984);

  g_assert (g_variant_equal (var, varc));

  g_variant_unref (var);
  g_variant_unref (varc);
}

static void
test_s (void)
{
  GValue v = { 0, };
  GVariant *var, *varc;

  g_value_init (&v, G_TYPE_STRING);
  g_value_set_static_string (&v, "Orwell");

  var = dbus_g_value_build_g_variant (&v);
  g_value_unset (&v);

  varc = g_variant_new_string ("Orwell");

  g_assert (g_variant_equal (var, varc));

  g_variant_unref (var);
  g_variant_unref (varc);
}

static void
test_o (void)
{
  GValue v = { 0, };
  GVariant *var, *varc;

  g_value_init (&v, DBUS_TYPE_G_OBJECT_PATH);
  g_value_set_boxed (&v, "/cats/escher");

  var = dbus_g_value_build_g_variant (&v);
  g_value_unset (&v);

  varc = g_variant_new_object_path ("/cats/escher");

  g_assert (g_variant_equal (var, varc));

  g_variant_unref (var);
  g_variant_unref (varc);
}

static void
test_us (void)
{
  GValue v = { 0, };
  GVariant *var, *varc;
  GType us = dbus_g_type_get_struct ("GValueArray",
      G_TYPE_UINT,
      G_TYPE_STRING,
      G_TYPE_INVALID);

  g_value_init (&v, us);
  g_value_take_boxed (&v, dbus_g_type_specialized_construct (us));
  dbus_g_type_struct_set (&v,
      0, 1984,
      1, "Orwell",
      G_MAXUINT);

  var = dbus_g_value_build_g_variant (&v);
  g_value_unset (&v);

  varc = g_variant_new ("(us)", 1984, "Orwell");

  g_assert (g_variant_equal (var, varc));

  g_variant_unref (var);
  g_variant_unref (varc);
}

static void
test_a_os (void)
{
  GValue v = { 0, };
  GHashTable *map;
  GVariantBuilder b;
  GVariant *var, *varc;
  GType a_os = dbus_g_type_get_map ("GHashTable",
      DBUS_TYPE_G_OBJECT_PATH,
      G_TYPE_STRING);

  g_value_init (&v, a_os);
  map = dbus_g_type_specialized_construct (a_os);

  g_hash_table_insert (map,
      g_strdup ("/cats/escher"), g_strdup ("Escher Moonbeam"));
  g_hash_table_insert (map,
      g_strdup ("/cats/harvey"), g_strdup ("Harvey Nomcat"));
  g_hash_table_insert (map,
      g_strdup ("/cats/josh"), g_strdup ("Josh Smith"));
  g_value_take_boxed (&v, map);

  var = dbus_g_value_build_g_variant (&v);
  g_value_unset (&v);

  g_variant_builder_init (&b, G_VARIANT_TYPE ("a{os}"));
  /* WARNING: ordering must match hash table for g_variant_equal to work */
  g_variant_builder_add (&b, "{os}", "/cats/escher", "Escher Moonbeam");
  g_variant_builder_add (&b, "{os}", "/cats/josh", "Josh Smith");
  g_variant_builder_add (&b, "{os}", "/cats/harvey", "Harvey Nomcat");
  varc = g_variant_builder_end (&b);

  g_assert (g_variant_equal (var, varc));

  g_variant_unref (var);
  g_variant_unref (varc);
}

static void
test_av (void)
{
  GValue v = { 0, }, *v2;
  GVariantBuilder b;
  GVariant *var, *varc;
  GType av = dbus_g_type_get_collection ("GPtrArray", G_TYPE_VALUE);
  GPtrArray *array;

  g_value_init (&v, av);
  array = dbus_g_type_specialized_construct (av);

  v2 = g_new0 (GValue, 1);
  g_value_init (v2, G_TYPE_INT);
  g_value_set_int (v2, 1984);
  g_ptr_array_add (array, v2);

  v2 = g_new0 (GValue, 1);
  g_value_init (v2, G_TYPE_STRING);
  g_value_set_static_string (v2, "Orwell");
  g_ptr_array_add (array, v2);

  v2 = g_new0 (GValue, 1);
  g_value_init (v2, DBUS_TYPE_G_OBJECT_PATH);
  g_value_set_boxed (v2, "/cats/escher");
  g_ptr_array_add (array, v2);

  g_value_take_boxed (&v, array);

  var = dbus_g_value_build_g_variant (&v);
  g_value_unset (&v);

  g_variant_builder_init (&b, G_VARIANT_TYPE ("av"));
  g_variant_builder_add (&b, "v", g_variant_new_int32 (1984));
  g_variant_builder_add (&b, "v", g_variant_new_string ("Orwell"));
  g_variant_builder_add (&b, "v", g_variant_new_object_path ("/cats/escher"));
  varc = g_variant_builder_end (&b);

  g_assert (g_variant_equal (var, varc));

  g_variant_unref (var);
  g_variant_unref (varc);
}

int
main (int argc,
    char **argv)
{
  g_type_init ();
  dbus_g_type_specialized_init ();

  g_test_init (&argc, &argv, NULL);

  g_test_add_func ("/gvalue-to-gvariant/i", test_i);
  g_test_add_func ("/gvalue-to-gvariant/s", test_s);
  g_test_add_func ("/gvalue-to-gvariant/o", test_o);
  g_test_add_func ("/gvalue-to-gvariant/us", test_us);
  g_test_add_func ("/gvalue-to-gvariant/a{os}", test_a_os);
  g_test_add_func ("/gvalue-to-gvariant/av", test_av);

  return g_test_run ();
}
