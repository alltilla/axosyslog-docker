/*
 * Copyright (c) 2002-2013 Balabit
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * As an additional exemption you are allowed to compile & link against the
 * OpenSSL libraries as published by the OpenSSL project. See the file
 * COPYING for details.
 *
 */
#include "cache.h"
#include "testutils.h"

gint fetch_count;
gint free_fn_count;

void *
fetch(CacheResolver *self, const gchar *key)
{
  fetch_count++;
  return g_strdup_printf("almafa_%s", key);
}

void
free_fn(CacheResolver *self)
{
  free_fn_count++;
  return;
}

CacheResolver *
dummy_cache_resolver(void)
{
  CacheResolver *self = g_new0(CacheResolver, 1);

  self->resolve_elem = fetch;
  self->free_elem = g_free;
  self->free_fn = free_fn;
  return self;
}

static void
assert_cache_lookup(Cache *c, const gchar *key)
{
  gchar *value;
  gchar *expected_value = g_strdup_printf("almafa_%s", key);

  value = cache_lookup(c, key);
  assert_string(value, expected_value, "Value error for \"%s\" key", key);

  g_free(expected_value);
}

static void
assert_cache_lookup_uncached(Cache *c, const gchar *key)
{
  fetch_count = 0;
  assert_cache_lookup(c, key);
  assert_gint(fetch_count, 1, "Cache lookup expected when looking up uncached elements, but one didn't arrive key=\"%s\"",
              key);
}

static void
assert_cache_lookup_cached(Cache *c, const gchar *key)
{
  fetch_count = 0;
  assert_cache_lookup(c, key);
  assert_gint(fetch_count, 0, "Cache lookup unexpected when looking up cached elements, but one did arrive key=\"%s\"",
              key);
}

void
test_cache_write_and_read(void)
{
  Cache *c;

  c = cache_new(dummy_cache_resolver());

  assert_cache_lookup_uncached(c, "key");
  assert_cache_lookup_cached(c, "key");

  assert_cache_lookup_uncached(c, "key2");
  assert_cache_lookup_cached(c, "key2");

  assert_cache_lookup_cached(c, "key");
  assert_cache_lookup_cached(c, "key2");
  cache_free(c);
}

void
test_cache_free_calls_resolver_free_fn(void)
{
  Cache *c;

  free_fn_count = 0;
  c = cache_new(dummy_cache_resolver());
  cache_free(c);
  assert_gint(free_fn_count, 1, "cache_free call the free_fn %d times", free_fn_count);
}

int
main(int argc G_GNUC_UNUSED, char *argv[] G_GNUC_UNUSED)
{
  test_cache_write_and_read();
  test_cache_free_calls_resolver_free_fn();
  return 0;
}
