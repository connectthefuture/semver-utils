/*
 * Copyright (c) 2016 Enrico M. Crisostomo
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation; either version 3, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 */
/**
 * @file
 * @brief Main `libsemver` source file.
 * @copyright Copyright (c) 2016 Enrico M. Crisostomo
 * @license GNU General Public License v. 3.0
 * @author Enrico M. Crisostomo
 * @version 1.0.0
 */

#include "libsemver.h"
#include "errors.h"
#include "../c++/version.hpp"
#include "../gettext_defs.h"
#include <exception>
#include <cstdlib>

#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"

#ifdef HAVE_CXX_THREAD_LOCAL
  #define THREAD_LOCAL thread_local
#else
  #define THREAD_LOCAL
#endif

static THREAD_LOCAL int last_error;

static void semver_set_last_error(int err);
static void semver_reset_last_error();

int semver_last_error()
{
  return last_error;
}

void semver_set_last_error(int err)
{
  last_error = err;
}

void semver_reset_last_error()
{
  last_error = SEMVER_EXIT_OK;
}

semver_t *semver_from_string(char *v)
{
  semver_reset_last_error();

  try
  {
    std::unique_ptr<semver::version> version(
      new semver::version(semver::version::from_string(v)));
    semver_t *s = new semver_t();
    s->ptr = version.release();

    return s;
  }
  catch (std::invalid_argument& ex)
  {
    semver_set_last_error(SEMVER_EXIT_INVALID_VERSION);
    return nullptr;
  }
  catch (std::bad_alloc& ex)
  {
    semver_set_last_error(SEMVER_EXIT_BAD_ALLOC);
    return nullptr;
  }
}

semver_t *semver_create(const unsigned int *v,
                        const unsigned long v_num,
                        const char *prerelease,
                        const char *metadata)
{
  semver_reset_last_error();

  try
  {
    std::unique_ptr<semver::version> p(
      new semver::version(std::vector<unsigned int>(v, v + v_num),
                          prerelease ? prerelease : "",
                          metadata ? metadata : ""));
    semver_t *s = new semver_t();
    s->ptr = p.release();

    return s;
  }
  catch (std::bad_alloc& ex)
  {
    semver_set_last_error(SEMVER_EXIT_BAD_ALLOC);
    return nullptr;
  }

}

void semver_free(semver_t *ver)
{
  delete static_cast<semver::version *>(ver->ptr);
  delete ver;
}

char *semver_str(semver_t *ver)
{
  semver_reset_last_error();

  try
  {
    semver::version *p = static_cast<semver::version *>(ver->ptr);
    std::string s = p->str();

    char *ptr_s = (char *) malloc(sizeof(char) * (s.size() + 1));

    if (!ptr_s) throw std::bad_alloc();

    std::copy(s.begin(), s.end(), ptr_s);
    ptr_s[s.size()] = '\0';

    return ptr_s;
  }
  catch (std::bad_alloc& ex)
  {
    semver_set_last_error(SEMVER_EXIT_BAD_ALLOC);
    return nullptr;
  }
}

unsigned int *semver_get_versions(semver_t *ver)
{
  semver_reset_last_error();

  try
  {
    semver::version *p = static_cast<semver::version *>(ver->ptr);
    std::vector<unsigned int> versions = p->get_version();

    unsigned int *v =
      (unsigned int *) malloc(sizeof(unsigned int) * versions.size());

    if (!v) throw std::bad_alloc();

    std::copy(versions.begin(), versions.end(), v);

    return v;
  }
  catch (std::bad_alloc& ex)
  {
    semver_set_last_error(SEMVER_EXIT_BAD_ALLOC);
    return nullptr;
  }
}

unsigned int semver_get_version(semver_t *ver, unsigned int index)
{
  semver_reset_last_error();

  try
  {
    semver::version *p = static_cast<semver::version *>(ver->ptr);

    return p->get_version(index);
  }
  catch (std::bad_alloc& ex)
  {
    semver_set_last_error(SEMVER_EXIT_BAD_ALLOC);
    return 0;
  }
}

char *semver_get_prerelease(semver_t *ver)
{
  semver_reset_last_error();

  try
  {
    semver::version *p = static_cast<semver::version *>(ver->ptr);

    std::string prerelease = p->get_prerelease();

    char *ptr_s = (char *) malloc(sizeof(char) * (prerelease.size() + 1));

    if (!ptr_s) return nullptr;

    std::copy(prerelease.begin(), prerelease.end(), ptr_s);
    ptr_s[prerelease.size()] = '\0';

    return ptr_s;
  }
  catch (std::bad_alloc& ex)
  {
    semver_set_last_error(SEMVER_EXIT_BAD_ALLOC);
    return nullptr;
  }

}

char *semver_get_metadata(semver_t *ver)
{
  semver_reset_last_error();

  try
  {
    semver::version *p = static_cast<semver::version *>(ver->ptr);

    std::string metadata = p->get_metadata();

    char *ptr_s = (char *) malloc(sizeof(char) * (metadata.size() + 1));

    if (!ptr_s) return nullptr;

    std::copy(metadata.begin(), metadata.end(), ptr_s);
    ptr_s[metadata.size()] = '\0';

    return ptr_s;
  }
  catch (std::bad_alloc& ex)
  {
    semver_set_last_error(SEMVER_EXIT_BAD_ALLOC);
    return nullptr;
  }
}

semver_t *semver_bump_major(semver_t *ver)
{
  semver_reset_last_error();

  try
  {
    semver::version *p = static_cast<semver::version *>(ver->ptr);

    std::unique_ptr<semver::version> n(new semver::version(p->bump_major()));
    semver_t *s = new semver_t();
    s->ptr = n.release();

    return s;
  }
  catch (std::bad_alloc& ex)
  {
    semver_set_last_error(SEMVER_EXIT_BAD_ALLOC);
    return nullptr;
  }
}

semver_t *semver_bump_minor(semver_t *ver)
{
  semver_reset_last_error();

  try
  {
    semver::version *p = static_cast<semver::version *>(ver->ptr);

    std::unique_ptr<semver::version> n(new semver::version(p->bump_minor()));
    semver_t *s = new semver_t();
    s->ptr = n.release();

    return s;
  }
  catch (std::bad_alloc& ex)
  {
    semver_set_last_error(SEMVER_EXIT_BAD_ALLOC);
    return nullptr;
  }
}

semver_t *semver_bump_patch(semver_t *ver)
{
  semver_reset_last_error();

  try
  {
    semver::version *p = static_cast<semver::version *>(ver->ptr);

    std::unique_ptr<semver::version> n(new semver::version(p->bump_patch()));
    semver_t *s = new semver_t();
    s->ptr = n.release();

    return s;
  }
  catch (std::bad_alloc& ex)
  {
    semver_set_last_error(SEMVER_EXIT_BAD_ALLOC);
    return nullptr;
  }
}

semver_t *semver_bump(semver_t *ver, unsigned int index)
{
  semver_reset_last_error();

  try
  {
    semver::version *p = static_cast<semver::version *>(ver->ptr);

    std::unique_ptr<semver::version> n(new semver::version(p->bump(index)));
    semver_t *s = new semver_t();
    s->ptr = n.release();

    return s;
  }
  catch (std::bad_alloc& ex)
  {
    semver_set_last_error(SEMVER_EXIT_BAD_ALLOC);
    return nullptr;
  }
}

semver_t *semver_strip_prerelease(semver_t *ver)
{
  semver_reset_last_error();

  try
  {
    semver::version *p = static_cast<semver::version *>(ver->ptr);

    std::unique_ptr<semver::version> n(
      new semver::version(p->strip_prerelease()));
    semver_t *s = new semver_t();
    s->ptr = n.release();

    return s;
  }
  catch (std::bad_alloc& ex)
  {
    semver_set_last_error(SEMVER_EXIT_BAD_ALLOC);
    return nullptr;
  }
}

semver_t *semver_strip_metadata(semver_t *ver)
{
  semver_reset_last_error();

  try
  {
    semver::version *p = static_cast<semver::version *>(ver->ptr);

    std::unique_ptr<semver::version> n(
      new semver::version(p->strip_metadata()));
    semver_t *s = new semver_t();
    s->ptr = n.release();

    return s;
  }
  catch (std::bad_alloc& ex)
  {
    semver_set_last_error(SEMVER_EXIT_BAD_ALLOC);
    return nullptr;
  }
}

bool semver_is_release(semver_t *ver)
{
  semver_reset_last_error();

  semver::version *p = static_cast<semver::version *>(ver->ptr);

  return p->is_release();
}

bool semver_equals(semver_t *lh, semver_t *rh)
{
  semver_reset_last_error();

  semver::version *lhs = static_cast<semver::version *>(lh->ptr);
  semver::version *rhs = static_cast<semver::version *>(rh->ptr);

  return *lhs == *rhs;
}

bool semver_is_less(semver_t *lh, semver_t *rh)
{
  semver_reset_last_error();

  semver::version *lhs = static_cast<semver::version *>(lh->ptr);
  semver::version *rhs = static_cast<semver::version *>(rh->ptr);

  return *lhs < *rhs;
}

bool semver_is_greater(semver_t *lh, semver_t *rh)
{
  semver_reset_last_error();

  semver::version *lhs = static_cast<semver::version *>(lh->ptr);
  semver::version *rhs = static_cast<semver::version *>(rh->ptr);

  return *lhs > *rhs;
}

#pragma clang diagnostic pop