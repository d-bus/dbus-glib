#include <dbus/dbus-glib.h>
#include <gio/gio.h>

/**
 * test_g_variant_equivalent:
 *
 * The function g_variant_equal() cannot be used for dictionaries because it
 * cares about the ordering of dictionaries, which breaks our tests.
 */
static gboolean
test_g_variant_equivalent (GVariant *one,
    GVariant *two)
{
  if (!g_variant_type_equal (
        g_variant_get_type (one),
        g_variant_get_type (two)))
    {
      return FALSE;
    }
  else if (g_variant_is_of_type (one, G_VARIANT_TYPE_DICTIONARY) &&
           g_variant_is_of_type (two, G_VARIANT_TYPE_DICTIONARY))
    {
      GHashTable *hash;
      GVariantIter iter;
      GVariant *child;
      gboolean equal = TRUE;

      if (g_variant_n_children (one) != g_variant_n_children (two))
        return FALSE;

      /* pack @one into a hash table */
      hash = g_hash_table_new_full (g_variant_hash, g_variant_equal,
          (GDestroyNotify) g_variant_unref, (GDestroyNotify) g_variant_unref);

      g_variant_iter_init (&iter, one);
      while ((child = g_variant_iter_next_value (&iter)))
        {
          g_hash_table_insert (hash,
              g_variant_get_child_value (child, 0),
              g_variant_get_child_value (child, 1));
          g_variant_unref (child);
        }

      /* now iterate @two to check for the keys in @hash */
      g_variant_iter_init (&iter, two);
      while (equal && (child = g_variant_iter_next_value (&iter)))
        {
          GVariant *k, *v1, *v2;

          k = g_variant_get_child_value (child, 0);
          v1 = g_variant_get_child_value (child, 1);

          v2 = g_hash_table_lookup (hash, k);

          if (v2 == NULL || !test_g_variant_equivalent (v1, v2))
            equal = FALSE;
          else
            g_hash_table_remove (hash, k);

          g_variant_unref (k);
          g_variant_unref (v1);
          g_variant_unref (child);
        }

      if (g_hash_table_size (hash) > 0)
        equal = FALSE;

      g_hash_table_destroy (hash);

      return equal;
    }
  else if (g_variant_is_container (one) &&
           g_variant_is_container (two))
    {
      guint i, size;
      gboolean equal = TRUE;

      if (g_variant_n_children (one) != g_variant_n_children (two))
        return FALSE;

      size = g_variant_n_children (one);
      for (i = 0; equal && i < size; i++)
        {
          GVariant *child1, *child2;

          child1 = g_variant_get_child_value (one, i);
          child2 = g_variant_get_child_value (two, i);

          equal = test_g_variant_equivalent (child1, child2);

          g_variant_unref (child1);
          g_variant_unref (child2);
        }

      return equal;
    }
  else
    {
      return g_variant_equal (one, two);
    }
}

/* test_g_variant_equivalent tests */
static void
test_simple_equiv (void)
{
  GVariant *v1, *v2;

  v1 = g_variant_new_int32 (1984);
  v2 = g_variant_new_int32 (1984);

  g_assert (test_g_variant_equivalent (v1, v2));

  g_variant_unref (v1);
  g_variant_unref (v2);
}

static void
test_simple_not_equiv (void)
{
  GVariant *v1, *v2;

  v1 = g_variant_new_int32 (1982);
  v2 = g_variant_new_int32 (1984);

  g_assert (!test_g_variant_equivalent (v1, v2));

  g_variant_unref (v1);
  g_variant_unref (v2);
}

static void
test_array_not_equiv (void)
{
  GVariantBuilder b;
  GVariant *v1, *v2;

  g_variant_builder_init (&b, G_VARIANT_TYPE ("av"));
  g_variant_builder_add (&b, "v", g_variant_new_int32 (1984));
  g_variant_builder_add (&b, "v", g_variant_new_string ("Orwell"));
  g_variant_builder_add (&b, "v", g_variant_new_object_path ("/cats/escher"));
  v1 = g_variant_builder_end (&b);

  g_variant_builder_init (&b, G_VARIANT_TYPE ("av"));
  /* note the order has changed */
  g_variant_builder_add (&b, "v", g_variant_new_string ("Orwell"));
  g_variant_builder_add (&b, "v", g_variant_new_int32 (1984));
  g_variant_builder_add (&b, "v", g_variant_new_object_path ("/cats/escher"));
  v2 = g_variant_builder_end (&b);

  g_assert (!test_g_variant_equivalent (v1, v2));

  g_variant_unref (v1);
  g_variant_unref (v2);
}

static void
test_map_equiv (void)
{
  GVariantBuilder b;
  GVariant *v1, *v2;

  g_variant_builder_init (&b, G_VARIANT_TYPE ("a{os}"));
  g_variant_builder_add (&b, "{os}", "/cats/escher", "Escher Moonbeam");
  g_variant_builder_add (&b, "{os}", "/cats/harvey", "Harvey Nomcat");
  g_variant_builder_add (&b, "{os}", "/cats/josh", "Josh Smith");
  v1 = g_variant_builder_end (&b);

  g_variant_builder_init (&b, G_VARIANT_TYPE ("a{os}"));
  /* note the order has changed */
  g_variant_builder_add (&b, "{os}", "/cats/harvey", "Harvey Nomcat");
  g_variant_builder_add (&b, "{os}", "/cats/escher", "Escher Moonbeam");
  g_variant_builder_add (&b, "{os}", "/cats/josh", "Josh Smith");
  v2 = g_variant_builder_end (&b);

  g_assert (test_g_variant_equivalent (v1, v2));

  g_variant_unref (v1);
  g_variant_unref (v2);
}

static void
test_map_not_equiv1 (void)
{
  GVariantBuilder b;
  GVariant *v1, *v2;

  g_variant_builder_init (&b, G_VARIANT_TYPE ("a{os}"));
  g_variant_builder_add (&b, "{os}", "/cats/escher", "Escher Moonbeam");
  g_variant_builder_add (&b, "{os}", "/cats/harvey", "Harvey Nomcat");
  g_variant_builder_add (&b, "{os}", "/cats/josh", "Josh Smith");
  v1 = g_variant_builder_end (&b);

  g_variant_builder_init (&b, G_VARIANT_TYPE ("a{os}"));
  g_variant_builder_add (&b, "{os}", "/cats/escher", "Escher Moonbeam");
  g_variant_builder_add (&b, "{os}", "/cats/harvey", "Harvey Nomcat");
  g_variant_builder_add (&b, "{os}", "/cats/josh", "Josh Smith");
  g_variant_builder_add (&b, "{os}", "/cats/rory", "Rory Cat");
  v2 = g_variant_builder_end (&b);

  g_assert (!test_g_variant_equivalent (v1, v2));

  g_variant_unref (v1);
  g_variant_unref (v2);
}

static void
test_map_not_equiv2 (void)
{
  GVariantBuilder b;
  GVariant *v1, *v2;

  g_variant_builder_init (&b, G_VARIANT_TYPE ("a{os}"));
  g_variant_builder_add (&b, "{os}", "/cats/escher", "Escher Moonbeam");
  g_variant_builder_add (&b, "{os}", "/cats/harvey", "Harvey Nomcat");
  g_variant_builder_add (&b, "{os}", "/cats/josh", "Josh Smith");
  v1 = g_variant_builder_end (&b);

  g_variant_builder_init (&b, G_VARIANT_TYPE ("a{os}"));
  g_variant_builder_add (&b, "{os}", "/cats/escher", "Escher Moonbeam");
  g_variant_builder_add (&b, "{os}", "/cats/harvey", "Harvey Nomcat");
  g_variant_builder_add (&b, "{os}", "/cats/josh", "Josh Cat");
  v2 = g_variant_builder_end (&b);

  g_assert (!test_g_variant_equivalent (v1, v2));

  g_variant_unref (v1);
  g_variant_unref (v2);
}

/* dbus_g_value_build_g_variant tests */
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

  g_assert (test_g_variant_equivalent (var, varc));

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

  g_assert (test_g_variant_equivalent (var, varc));

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

  g_assert (test_g_variant_equivalent (var, varc));

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

  g_assert (test_g_variant_equivalent (var, varc));

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
  g_variant_builder_add (&b, "{os}", "/cats/escher", "Escher Moonbeam");
  g_variant_builder_add (&b, "{os}", "/cats/harvey", "Harvey Nomcat");
  g_variant_builder_add (&b, "{os}", "/cats/josh", "Josh Smith");
  varc = g_variant_builder_end (&b);

  g_assert (test_g_variant_equivalent (var, varc));

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

  g_assert (test_g_variant_equivalent (var, varc));

  g_variant_unref (var);
  g_variant_unref (varc);
}

static void
test_g (void)
{
  GValue v = { 0, };
  GVariant *var, *varc;

  g_value_init (&v, DBUS_TYPE_G_SIGNATURE);
  g_value_set_boxed (&v, "a{u(ua{sa{sv}})}");

  var = dbus_g_value_build_g_variant (&v);
  g_value_unset (&v);

  varc = g_variant_new_signature ("a{u(ua{sa{sv}})}");

  g_assert (test_g_variant_equivalent (var, varc));

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

  /* test_g_variant_equivalent tests */
  g_test_add_func ("/test_g_variant_equivalent/test_simple_equiv",
      test_simple_equiv);
  g_test_add_func ("/test_g_variant_equivalent/test_simple_not_equiv",
      test_simple_not_equiv);
  g_test_add_func ("/test_g_variant_equivalent/test_array_not_equiv",
      test_array_not_equiv);
  g_test_add_func ("/test_g_variant_equivalent/test_map_equiv",
      test_map_equiv);
  g_test_add_func ("/test_g_variant_equivalent/test_map_not_equiv1",
      test_map_not_equiv1);
  g_test_add_func ("/test_g_variant_equivalent/test_map_not_equiv2",
      test_map_not_equiv2);

  /* dbus_g_value_build_g_variant tests */
  g_test_add_func ("/gvalue-to-gvariant/i", test_i);
  g_test_add_func ("/gvalue-to-gvariant/s", test_s);
  g_test_add_func ("/gvalue-to-gvariant/o", test_o);
  g_test_add_func ("/gvalue-to-gvariant/us", test_us);
  g_test_add_func ("/gvalue-to-gvariant/a{os}", test_a_os);
  g_test_add_func ("/gvalue-to-gvariant/av", test_av);
  g_test_add_func ("/gvalue-to-gvariant/g", test_g);

  return g_test_run ();
}
