/*
  Copyright (c) 2014 Dirk Willrodt <willrodt@zbh.uni-hamburg.de>
  Copyright (c) 2014-2016 Center for Bioinformatics, University of Hamburg

  Permission to use, copy, modify, and distribute this software for any
  purpose with or without fee is hereby granted, provided that the above
  copyright notice and this permission notice appear in all copies.

  THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
  WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
  MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
  ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
  WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
  ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
  OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/

/*
  THIS FILE IS GENERATED by
  scripts/gen-intsets.rb.
  DO NOT EDIT.
*/

#include <inttypes.h>
#include <limits.h>

#include "core/assert_api.h"
#include "core/divmodmul.h"
#include "core/ensure.h"
#include "core/intbits.h"
#include "core/ma.h"
#include "core/mathsupport.h"
#include "core/unused_api.h"
#include "extended/intset_8.h"
#include "extended/io_function_pointers.h"

#define gt_intset_8_cast(cvar) \
        gt_intset_cast(gt_intset_8_class(), cvar)

#define GT_ELEM2SECTION_M(X) GT_ELEM2SECTION(X, members->logsectionsize)

#define GT_INTSET_8_TYPE ((GtUword) 8)

#define gt_intset_8_io_one(element) \
        io_func(&element, sizeof (element), (size_t) 1, fp, err)

struct GtIntset8 {
  GtIntset parent_instance;
  uint8_t *elements;
};

GtIntset* gt_intset_8_new(GtUword maxelement, GtUword num_of_elems)
{
  GtIntset *intset;
  GtIntset8 *intset_8;
  GtIntsetMembers *members;
  GtUword idx;

  gt_assert(num_of_elems != 0);

  intset = gt_intset_create(gt_intset_8_class());
  intset_8 = gt_intset_8_cast(intset);
  members = intset->members;

  members->currentsectionnum = 0;
  members->maxelement = maxelement;
  members->nextfree = 0;
  members->num_of_elems = num_of_elems;
  members->previouselem = ULONG_MAX;
  members->refcount = 0;

  members->logsectionsize = GT_BITS_FOR_TYPE(uint8_t);
  members->numofsections = GT_ELEM2SECTION_M(maxelement) + 1;

  intset_8->elements =
    gt_malloc(sizeof (*intset_8->elements) * num_of_elems);

  members->sectionstart = gt_malloc(sizeof (*members->sectionstart) *
                                    (members->numofsections + 1));

  members->sectionstart[0] = 0;
  for (idx = (GtUword) 1; idx <= members->numofsections; idx++) {
    members->sectionstart[idx] = num_of_elems;
  }
  return intset;
}

static bool gt_intset_8_elems_is_valid(GtIntset *intset)
{
  GtUword idx, sec_idx = 0;
  GtIntset8 *intset_8;
  GtIntsetMembers *members;
  intset_8 = gt_intset_8_cast(intset);
  members = intset->members;

  for (idx = (GtUword) 1; idx < members->num_of_elems; idx++) {
    while (idx > members->sectionstart[sec_idx])
      sec_idx++;

    if (idx != members->sectionstart[sec_idx] &&
        intset_8->elements[idx] <= intset_8->elements[idx - 1])
      return false;
  }
  return true;
}

static bool gt_intset_8_secstart_is_valid(GtIntset *intset)
{
  GtUword idx;
  GtIntsetMembers *members;
  members = intset->members;

  for (idx = (GtUword) 1; idx <= members->numofsections; idx++) {
    if (members->sectionstart[idx] < members->sectionstart[idx - 1])
      return false;
  }
  return true;
}

GtIntset *gt_intset_8_io_fp(GtIntset *intset, FILE *fp, GtError *err,
                                    GtIOFunc io_func)
{
  int had_err = 0;
  GtUword type = (GtUword) GT_INTSET_8_TYPE;
  GtIntset8 *intset_8;
  GtIntsetMembers *members;

  gt_error_check(err);

  intset_8 = gt_intset_8_cast(intset);

  had_err = gt_intset_8_io_one(type);
  if (!had_err && type != GT_INTSET_8_TYPE) {
    /* only applies to reading */
    had_err = 1;
    gt_error_set(err, "Trying to read GtIntset8 from file,"
                 " type does not match!");
  }
  if (!had_err) {
    members = intset->members;
    had_err = gt_intset_8_io_one(members->currentsectionnum);
    if (!had_err)
      had_err = gt_intset_8_io_one(members->maxelement);
    if (!had_err)
      had_err = gt_intset_8_io_one(members->nextfree);
    if (!had_err)
      had_err = gt_intset_8_io_one(members->num_of_elems);
    if (!had_err)
      had_err = gt_intset_8_io_one(members->previouselem);
    if (!had_err) {
      members->logsectionsize = GT_BITS_FOR_TYPE(uint8_t);
      members->numofsections = GT_ELEM2SECTION_M(members->maxelement) + 1;
      members->sectionstart = gt_realloc(members->sectionstart,
                sizeof (*members->sectionstart) * (members->numofsections + 1));
    }
    had_err = io_func(members->sectionstart, sizeof (*members->sectionstart),
                      (size_t) (members->numofsections + 1), fp, err);
    if (!had_err && members->sectionstart[0] != 0) {
      had_err = 1;
      gt_error_set(err, "Unexpected value in sectionstart[0]: "
                   GT_WU " expected 0!", members->sectionstart[0]);
    }
  }
  if (!had_err) {
    intset_8->elements = gt_realloc(intset_8->elements,
                  sizeof (*intset_8->elements) * members->num_of_elems);
    had_err = io_func(intset_8->elements,
                      sizeof (*intset_8->elements),
                      (size_t) members->num_of_elems, fp, err);
  }
  if (had_err) {
    gt_intset_8_delete(intset);
    intset = NULL;
  }
  return intset;

}

GtIntset *gt_intset_8_io(GtIntset *intset, FILE *fp, GtError *err)
{
  GtIntset8 *intset_8;
  if (intset == NULL) {
    intset = gt_intset_create(gt_intset_8_class());
    intset->members->sectionstart = NULL;
    intset->members->refcount = 0;
    intset_8 = gt_intset_8_cast(intset);
    intset_8->elements = NULL;
    intset = gt_intset_8_io_fp(intset, fp, err, gt_io_error_fread);
  }
  else {
    intset = gt_intset_8_io_fp(intset, fp, err, gt_io_error_fwrite);
  }
  return intset;
}

GtIntset *gt_intset_8_new_from_file(FILE *fp, GtError *err)
{
  gt_assert(fp != NULL);
  gt_error_check(err);
  return gt_intset_8_io(NULL, fp, err);
}

GtIntset *gt_intset_8_write(GtIntset *intset, FILE *fp, GtError *err)
{
  gt_assert(intset != NULL);
  gt_assert(fp != NULL);
  gt_error_check(err);
  return gt_intset_8_io(intset, fp, err);
}

void gt_intset_8_add(GtIntset *intset, GtUword elem)
{
  GtIntset8 *intset_8 = gt_intset_8_cast(intset);
  GtIntsetMembers *members = intset->members;
  GtUword *sectionstart = members->sectionstart;
  gt_assert(members->nextfree < members->num_of_elems);
  gt_assert(elem <= members->maxelement);
  gt_assert(members->previouselem == ULONG_MAX || members->previouselem < elem);
  while (elem >= GT_SECTIONMINELEM(members->currentsectionnum + 1)) {
    gt_assert(members->currentsectionnum < members->numofsections);
    sectionstart[members->currentsectionnum + 1] = members->nextfree;
    members->currentsectionnum++;
  }
  gt_assert(GT_SECTIONMINELEM(members->currentsectionnum) <= elem &&
            elem < GT_SECTIONMINELEM(members->currentsectionnum+1) &&
            GT_ELEM2SECTION_M(elem) == members->currentsectionnum);
  intset_8->elements[members->nextfree++] = (uint8_t) elem;
  members->previouselem = elem;
}

static GtUword gt_intset_8_sec_idx_largest_seq(GtUword *sectionstart,
                                               GtUword idx)
{
  GtUword result = 0;
  while (sectionstart[result] <= idx)
    result++;
  return result - 1;
}

static GtUword
gt_intset_8_binarysearch_sec_idx_largest_seq(GtUword *secstart_begin,
                                             GtUword *secstart_end,
                                             GtUword idx)
{
  GtUword *midptr = NULL, *found = NULL,
          *startorig = secstart_begin;
  if (*secstart_begin <= idx)
    found = secstart_begin;
  while (secstart_begin < secstart_end) {
    midptr = secstart_begin + (GtUword) GT_DIV2(secstart_end - secstart_begin);
    if (*midptr < idx) {
      found = midptr;
      if (*midptr == idx) {
        break;
      }
      secstart_begin = midptr + 1;
    }
    else {
      secstart_end = midptr - 1;
    }
  }
  gt_assert(found != NULL);
  while (found[1] <= idx)
    found++;
  return (GtUword) (found - startorig);
}

static GtUword gt_intset_8_get_test(GtIntset *intset, GtUword idx)
{
  GtIntset8 *intset_8 = gt_intset_8_cast(intset);
  GtIntsetMembers *members = intset->members;
  GtUword *sectionstart = members->sectionstart;
  gt_assert(idx < members->nextfree);

  return (gt_intset_8_sec_idx_largest_seq(sectionstart, idx) <<
         members->logsectionsize) + intset_8->elements[idx];
}

GtUword gt_intset_8_get(GtIntset *intset, GtUword idx)
{
  GtUword quotient;
  GtIntset8 *intset_8 = gt_intset_8_cast(intset);
  GtIntsetMembers *members = intset->members;
  GtUword *sectionstart = members->sectionstart;
  gt_assert(idx < members->nextfree);

  quotient = gt_intset_8_binarysearch_sec_idx_largest_seq(
                                      sectionstart,
                                      sectionstart + members->numofsections - 1,
                                      idx);
  return (quotient << members->logsectionsize) +
         intset_8->elements[idx];
}

GtUword gt_intset_8_size(GtIntset *intset)
{
  GT_UNUSED GtIntset8 *intset_8 = gt_intset_8_cast(intset);
  GtIntsetMembers *members = intset->members;
  return members->nextfree;
}

static bool gt_intset_8_binarysearch_is_member(const uint8_t *leftptr,
                                               const uint8_t *rightptr,
                                               uint8_t elem)
{
  const uint8_t *midptr;
    while (leftptr <= rightptr) {
      midptr = leftptr + (GtUword) GT_DIV2(rightptr - leftptr);
      if (elem < *midptr) {
        rightptr = midptr - 1;
      }
      else {
        if (elem > *midptr)
          leftptr = midptr + 1;
        else
          return true;
      }
    }
  return false;
}

bool gt_intset_8_is_member(GtIntset *intset, GtUword elem)
{
  GtIntset8 *intset_8 = gt_intset_8_cast(intset);
  GtIntsetMembers *members = intset->members;
  GtUword *sectionstart = members->sectionstart;
  if (elem <= members->maxelement)
  {
    const GtUword sectionnum = GT_ELEM2SECTION_M(elem);

    if (sectionstart[sectionnum] < sectionstart[sectionnum+1]) {
      return gt_intset_8_binarysearch_is_member(
                            intset_8->elements + sectionstart[sectionnum],
                            intset_8->elements + sectionstart[sectionnum+1] - 1,
                            (uint64_t) elem);
    }
  }
  return false;
}

static GtUword gt_intset_8_idx_sm_geq(const uint8_t *leftptr,
                                      const uint8_t *rightptr,
                                      uint8_t value)
{
  const uint8_t *leftorig = leftptr;
  if (value < *leftptr)
    return 0;
  if (value > *rightptr)
    return 1UL + (GtUword) (rightptr - leftptr);
  gt_assert(value <= *rightptr);
  while (*leftptr < value)
    leftptr++;
  return (GtUword) (leftptr - leftorig);
}

static GtUword gt_intset_8_binarysearch_idx_sm_geq(const uint8_t *leftptr,
                                                   const uint8_t *rightptr,
                                                   uint8_t value)
{
  const uint8_t *midptr = NULL,
        *leftorig = leftptr;

  gt_assert(leftptr <= rightptr);
  if (value <= *leftptr)
    return 0;
  if (value > *rightptr)
    return 1UL + (GtUword) (rightptr - leftptr);
  while (leftptr < rightptr) {
    midptr = leftptr + (GtUword) GT_DIV2(rightptr - leftptr);
    if (value <= *midptr)
      rightptr = midptr;
    else {
      leftptr = midptr + 1;
    }
  }
  return (GtUword) (leftptr - leftorig);
}

static GtUword gt_intset_8_get_idx_smallest_geq_test(GtIntset *intset,
                                                      GtUword value)
{
  GtIntset8 *intset_8 = gt_intset_8_cast(intset);
  GtIntsetMembers *members = intset->members;

  GtUword sectionnum = GT_ELEM2SECTION_M(value);

  if (value > members->previouselem)
    return members->num_of_elems;

  gt_assert(value <= members->maxelement);
  if (members->sectionstart[sectionnum] < members->sectionstart[sectionnum+1]) {
    return members->sectionstart[sectionnum] +
           gt_intset_8_idx_sm_geq(
                   intset_8->elements + members->sectionstart[sectionnum],
                   intset_8->elements + members->sectionstart[sectionnum+1] - 1,
                   (uint8_t) value);
  }
  return members->sectionstart[sectionnum];
}

GtUword gt_intset_8_get_idx_smallest_geq(GtIntset *intset, GtUword value)
{
  GtIntset8 *intset_8 = gt_intset_8_cast(intset);
  GtIntsetMembers *members = intset->members;

  GtUword sectionnum = GT_ELEM2SECTION_M(value);

  if (value > members->previouselem)
    return members->num_of_elems;

  gt_assert(value <= members->maxelement);

  if (members->sectionstart[sectionnum] < members->sectionstart[sectionnum+1]) {
    return members->sectionstart[sectionnum] +
           gt_intset_8_binarysearch_idx_sm_geq(
                   intset_8->elements + members->sectionstart[sectionnum],
                   intset_8->elements + members->sectionstart[sectionnum+1] - 1,
                   (uint8_t) value);
  }
  return members->sectionstart[sectionnum];
}

void gt_intset_8_delete(GtIntset *intset)
{
  GtIntset8 *intset_8 = gt_intset_8_cast(intset);
  if (intset_8 != NULL) {
    gt_free(intset_8->elements);
  }
}

size_t gt_intset_8_size_of_rep(GtUword maxelement, GtUword num_of_elems)
{
  size_t logsectionsize = GT_BITS_FOR_TYPE(uint8_t);
  gt_assert(GT_BITS_FOR_TYPE(GtUword) > logsectionsize);
  return sizeof (uint8_t) * num_of_elems +
    sizeof (GtUword) * (GT_ELEM2SECTION(maxelement, logsectionsize) + 1);
}

size_t gt_intset_8_size_of_struct(void)
{
  return sizeof (GtIntset8) +
         sizeof (struct GtIntsetClass) +
         sizeof (struct GtIntsetMembers);
}

bool gt_intset_8_file_is_type(GtUword type)
{
  return type == GT_INTSET_8_TYPE;
}

/* map static local methods to interface */
const GtIntsetClass* gt_intset_8_class(void)
{
  static const GtIntsetClass *this_c = NULL;
  if (this_c == NULL) {
    this_c = gt_intset_class_new(sizeof (GtIntset8),
                                 gt_intset_8_add,
                                 gt_intset_8_file_is_type,
                                 gt_intset_8_get,
                                 gt_intset_8_io,
                                 gt_intset_8_get_idx_smallest_geq,
                                 gt_intset_8_is_member,
                                 gt_intset_8_size_of_rep,
                                 gt_intset_8_size,
                                 gt_intset_8_size_of_struct,
                                 gt_intset_8_write,
                                 gt_intset_8_delete);
  }
  return this_c;
}

int gt_intset_8_unit_test(GtError *err)
{
  int had_err = 0;
  GtIntset *is;
  GtUword num_of_elems = gt_rand_max(((GtUword) 1) << 10) + 1,
          *arr = gt_malloc(sizeof (*arr) * num_of_elems),
          stepsize = GT_DIV2(num_of_elems <<4 / num_of_elems),
          idx;
  size_t is_size;

  gt_error_check(err);

  arr[0] = gt_rand_max(stepsize) + 1;
  for (idx = (GtUword) 1; idx < num_of_elems; ++idx) {
    arr[idx] = arr[idx - 1] + gt_rand_max(stepsize) + 1;
  }

  is_size =     gt_intset_8_size_of_rep(arr[num_of_elems - 1], num_of_elems);

  if (!had_err) {
    if (is_size < (size_t) UINT_MAX) {
      is = gt_intset_8_new(arr[num_of_elems - 1], num_of_elems);
      for (idx = 0; idx < num_of_elems; idx++) {
        gt_intset_8_add(is, arr[idx]);
        gt_ensure(idx + 1 == gt_intset_8_size(is));
        if (idx < num_of_elems - 1)
          gt_ensure(gt_intset_8_get_idx_smallest_geq(is,
                                                     arr[idx] + 1) ==
                    num_of_elems);
      }

      gt_ensure(gt_intset_8_elems_is_valid(is));
      gt_ensure(gt_intset_8_secstart_is_valid(is));

      for (idx = 0; !had_err && idx < num_of_elems; idx++) {
        if (arr[idx] != 0 && arr[idx - 1] != (arr[idx] - 1)) {
          gt_ensure(
            gt_intset_8_get_idx_smallest_geq_test(is, arr[idx] - 1) ==
            idx);
          gt_ensure(
            gt_intset_8_get_idx_smallest_geq(is, arr[idx] - 1) ==
            idx);
        }
        gt_ensure(gt_intset_8_get_test(is, idx) == arr[idx]);
        gt_ensure(gt_intset_8_get(is, idx) == arr[idx]);
        gt_ensure(
          gt_intset_8_get_idx_smallest_geq_test(is, arr[idx] + 1) ==
          idx + 1);
        gt_ensure(
          gt_intset_8_get_idx_smallest_geq(is, arr[idx] + 1) ==
          idx + 1);
      }
      if (!had_err)
        had_err = gt_intset_unit_test_notinset(is, 0, arr[0] - 1, err);
      if (!had_err)
        had_err = gt_intset_unit_test_check_seqnum(is, 0, arr[0] - 1, 0, err);
      for (idx = (GtUword) 1; !had_err && idx < num_of_elems; idx++) {
        had_err = gt_intset_unit_test_notinset(is, arr[idx - 1] + 1,
                                               arr[idx] - 1, err);
        if (!had_err)
          had_err = gt_intset_unit_test_check_seqnum(is, arr[idx - 1] + 1,
                                                     arr[idx] - 1, idx, err);
      }
      gt_intset_delete(is);
    }
  }
  gt_free(arr);
  return had_err;
}
