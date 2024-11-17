/*
 * Copyright (c) 2024 Axoflow
 * Copyright (c) 2024 Attila Szakacs <attila.szakacs@axoflow.com>
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

#include <criterion/criterion.h>
#include "libtest/filterx-lib.h"

#include "filterx/filterx-metrics-labels.h"
#include "filterx/expr-literal.h"
#include "filterx/expr-literal-generator.h"
#include "filterx/object-string.h"
#include "stats/stats-cluster.h"
#include "metrics/dyn-metrics-cache.h"
#include "scratch-buffers.h"
#include "apphook.h"

static GList *
_add_label_expr(GList *label_exprs, FilterXExpr *key, FilterXExpr *value)
{
  return g_list_append(label_exprs, filterx_literal_generator_elem_new(key, value, TRUE));
}

Test(filterx_metrics_labels, null_labels)
{
  FilterXMetricsLabels *metrics_labels = filterx_metrics_labels_new(NULL);
  cr_assert(metrics_labels);

  cr_assert(filterx_metrics_labels_is_const(metrics_labels));

  DynMetricsStore *store = dyn_metrics_cache();

  StatsClusterLabel *sc_labels;
  gsize len;
  cr_assert(filterx_metrics_labels_format(metrics_labels, store, &sc_labels, &len));
  cr_assert_eq(len, 0);

  filterx_metrics_labels_free(metrics_labels);
}

Test(filterx_metrics_labels, const_literal_generator_empty_labels)
{
  FilterXExpr *labels_expr = filterx_literal_dict_generator_new();
  FilterXMetricsLabels *metrics_labels = filterx_metrics_labels_new(labels_expr);
  filterx_expr_unref(labels_expr);
  cr_assert(metrics_labels);

  cr_assert(filterx_metrics_labels_is_const(metrics_labels));

  DynMetricsStore *store = dyn_metrics_cache();

  StatsClusterLabel *sc_labels;
  gsize len;
  cr_assert(filterx_metrics_labels_format(metrics_labels, store, &sc_labels, &len));
  cr_assert_eq(len, 0);

  filterx_metrics_labels_free(metrics_labels);
}

Test(filterx_metrics_labels, non_literal_empty_labels)
{
  FilterXExpr *labels_expr = filterx_non_literal_new(filterx_test_dict_new());
  FilterXMetricsLabels *metrics_labels = filterx_metrics_labels_new(labels_expr);
  filterx_expr_unref(labels_expr);
  cr_assert(metrics_labels);

  cr_assert_not(filterx_metrics_labels_is_const(metrics_labels));

  DynMetricsStore *store = dyn_metrics_cache();

  StatsClusterLabel *sc_labels;
  gsize len;
  cr_assert(filterx_metrics_labels_format(metrics_labels, store, &sc_labels, &len));
  cr_assert_eq(len, 0);

  filterx_metrics_labels_free(metrics_labels);
}

Test(filterx_metrics_labels, const_literal_generator_labels)
{
  GList *label_exprs = NULL;
  label_exprs = _add_label_expr(label_exprs,
                                filterx_literal_new(filterx_string_new("foo", -1)),
                                filterx_literal_new(filterx_string_new("foovalue", -1)));
  label_exprs = _add_label_expr(label_exprs,
                                filterx_literal_new(filterx_string_new("bar", -1)),
                                filterx_literal_new(filterx_string_new("barvalue", -1)));

  FilterXExpr *labels_expr = filterx_literal_dict_generator_new();
  filterx_literal_generator_set_elements(labels_expr, label_exprs);

  FilterXMetricsLabels *metrics_labels = filterx_metrics_labels_new(labels_expr);
  filterx_expr_unref(labels_expr);
  cr_assert(metrics_labels);

  cr_assert(filterx_metrics_labels_is_const(metrics_labels));

  DynMetricsStore *store = dyn_metrics_cache();

  StatsClusterLabel *sc_labels;
  gsize len;
  cr_assert(filterx_metrics_labels_format(metrics_labels, store, &sc_labels, &len));
  cr_assert_eq(len, 2);

  cr_assert_str_eq(sc_labels[0].name, "bar");
  cr_assert_str_eq(sc_labels[0].value, "barvalue");
  cr_assert_str_eq(sc_labels[1].name, "foo");
  cr_assert_str_eq(sc_labels[1].value, "foovalue");

  filterx_metrics_labels_free(metrics_labels);
}

Test(filterx_metrics_labels, non_const_literal_generator_labels)
{
  GList *label_exprs = NULL;
  label_exprs = _add_label_expr(label_exprs,
                                filterx_literal_new(filterx_string_new("foo", -1)),
                                filterx_non_literal_new(filterx_string_new("foovalue", -1)));
  label_exprs = _add_label_expr(label_exprs,
                                filterx_literal_new(filterx_string_new("bar", -1)),
                                filterx_literal_new(filterx_string_new("barvalue", -1)));

  FilterXExpr *labels_expr = filterx_literal_dict_generator_new();
  filterx_literal_generator_set_elements(labels_expr, label_exprs);

  FilterXMetricsLabels *metrics_labels = filterx_metrics_labels_new(labels_expr);
  filterx_expr_unref(labels_expr);
  cr_assert(metrics_labels);

  cr_assert_not(filterx_metrics_labels_is_const(metrics_labels));

  DynMetricsStore *store = dyn_metrics_cache();

  StatsClusterLabel *sc_labels;
  gsize len;
  cr_assert(filterx_metrics_labels_format(metrics_labels, store, &sc_labels, &len));
  cr_assert_eq(len, 2);

  cr_assert_str_eq(sc_labels[0].name, "bar");
  cr_assert_str_eq(sc_labels[0].value, "barvalue");
  cr_assert_str_eq(sc_labels[1].name, "foo");
  cr_assert_str_eq(sc_labels[1].value, "foovalue");

  filterx_metrics_labels_free(metrics_labels);
}

Test(filterx_metrics_labels, non_literal_key_in_literal_generator_labels)
{
  GList *label_exprs = NULL;
  label_exprs = _add_label_expr(label_exprs,
                                filterx_non_literal_new(filterx_string_new("foo", -1)),
                                filterx_literal_new(filterx_string_new("foovalue", -1)));

  FilterXExpr *labels_expr = filterx_literal_dict_generator_new();
  filterx_literal_generator_set_elements(labels_expr, label_exprs);

  FilterXMetricsLabels *metrics_labels = filterx_metrics_labels_new(labels_expr);
  filterx_expr_unref(labels_expr);
  cr_assert_not(metrics_labels);
}

static void
setup(void)
{
  app_startup();
  init_libtest_filterx();
}

static void
teardown(void)
{
  deinit_libtest_filterx();
  scratch_buffers_explicit_gc();
  app_shutdown();
}

TestSuite(filterx_metrics_labels, .init = setup, .fini = teardown);
