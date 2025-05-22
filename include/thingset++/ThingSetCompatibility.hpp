/*
 * Copyright (c) 2024 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "ThingSet.hpp"

#define TS_ID_ROOT 0x00

#define THINGSET_ROLE_USR (1U << 0) /**< Normal user */
#define THINGSET_ROLE_EXP (1U << 1) /**< Expert user */
#define THINGSET_ROLE_MFR (1U << 2) /**< Manufacturer */
#define THINGSET_ROLE_ANY (THINGSET_USR_R | THINGSET_EXP_R | THINGSET_MFR_R)

#define THINGSET_READ_MASK  0x0F /**< Read flags stored in 4 least-significant bits */
#define THINGSET_WRITE_MASK 0xF0 /**< Write flags stored in 4 most-significant bits */

/* Masks for different roles */
#define THINGSET_USR_MASK (THINGSET_ROLE_USR << 4 | THINGSET_ROLE_USR)
#define THINGSET_EXP_MASK (THINGSET_ROLE_EXP << 4 | THINGSET_ROLE_EXP)
#define THINGSET_MFR_MASK (THINGSET_ROLE_MFR << 4 | THINGSET_ROLE_MFR)

#define THINGSET_READ(roles)       ((roles) & THINGSET_READ_MASK)
#define THINGSET_WRITE(roles)      (((roles) << 4) & THINGSET_WRITE_MASK)
#define THINGSET_READ_WRITE(roles) (THINGSET_READ(roles) | THINGSET_WRITE(roles))

/** @endcond */

#define THINGSET_USR_R THINGSET_READ(THINGSET_ROLE_USR) /**< Read-only access for normal user */
#define THINGSET_EXP_R THINGSET_READ(THINGSET_ROLE_EXP) /**< Read-only access for expert user */
#define THINGSET_MFR_R THINGSET_READ(THINGSET_ROLE_MFR) /**< Read-only access for manufacturer */
#define THINGSET_ANY_R THINGSET_READ(THINGSET_ROLE_ANY) /**< Read-only access for any user */

#define THINGSET_USR_W THINGSET_WRITE(THINGSET_ROLE_USR) /**< Write-only access for normal user */
#define THINGSET_EXP_W THINGSET_WRITE(THINGSET_ROLE_EXP) /**< Write-only access for expert user */
#define THINGSET_MFR_W THINGSET_WRITE(THINGSET_ROLE_MFR) /**< Write-only access for manuf. */
#define THINGSET_ANY_W THINGSET_WRITE(THINGSET_ROLE_ANY) /**< Write-only access for any user */

#define THINGSET_USR_RW THINGSET_READ_WRITE(THINGSET_ROLE_USR) /**< Read/write access for normal user */
#define THINGSET_EXP_RW THINGSET_READ_WRITE(THINGSET_ROLE_EXP) /**< Read/write access for expert user */
#define THINGSET_MFR_RW THINGSET_READ_WRITE(THINGSET_ROLE_MFR) /**< Read/write access for manufacturer */
#define THINGSET_ANY_RW THINGSET_READ_WRITE(THINGSET_ROLE_ANY) /**< Read/write access for any user */

namespace ThingSet {

template <unsigned T> struct convertAccess
{
    static const ThingSetAccess value;
};

template <> struct convertAccess<THINGSET_ANY_R>
{
    static const ThingSetAccess value = ThingSetAccess::anyRead;
};

template <> struct convertAccess<THINGSET_ANY_RW>
{
    static const ThingSetAccess value = ThingSetAccess::anyReadWrite;
};

template <> struct convertAccess<THINGSET_ANY_R | THINGSET_MFR_W>
{
    static const ThingSetAccess value = ThingSetAccess::anyRead | ThingSetAccess::manufacturerWrite;
};

template <> struct convertAccess<THINGSET_ANY_R | THINGSET_EXP_W | THINGSET_MFR_W>
{
    static const ThingSetAccess value = ThingSetAccess::anyRead | ThingSetAccess::expertWrite | ThingSetAccess::manufacturerWrite;
};

/// @brief Shim class to facilitate compatibility with two-stage array property
/// declarations in C ThingSet.
/// @tparam T The type of the elements in the array.
template <typename T> struct __ArrayHolder {
    T* array;
    size_t maxElements;
    size_t numElements;
};

/// @brief Facilitates the construction of properties where the type is to be inferred.
/// @tparam Id The unique ID of the property.
/// @tparam ParentId The ID of the parent container.
/// @tparam Name The name of the property.
/// @tparam Access The access permissions for this property.
template <unsigned Id, unsigned ParentId, StringLiteral Name, ThingSetAccess Access> struct __PropertyBuilder
{
    template <typename T>
    static ThingSetProperty<Id, ParentId, Name, Access, T*> build(T** value) {
        return ThingSetProperty<Id, ParentId, Name, Access, T*>(*value);
    }
};

} // namespace ThingSet

#define THINGSET_ADD_GROUP(parentId, id, name, callback) \
    ThingSet::ThingSetGroup<id, parentId, name> thingset_##id;

#define THINGSET_ADD_ITEM(parentId, id, name, pointer, access, subsets, type)                                          \
    ThingSet::ThingSetProperty<id, parentId, name, ThingSet::convertAccess<access>::value, type *> thingset_##id(pointer);

#define THINGSET_ADD_ITEM_BOOL(parentId, id, name, pointer, access, subsets)                                           \
    THINGSET_ADD_ITEM(parentId, id, name, pointer, access, subsets, bool)

#define THINGSET_ADD_ITEM_UINT8(parentId, id, name, pointer, access, subsets)                                          \
    THINGSET_ADD_ITEM(parentId, id, name, pointer, access, subsets, uint8_t)

#define THINGSET_ADD_ITEM_UINT16(parentId, id, name, pointer, access, subsets)                                         \
    THINGSET_ADD_ITEM(parentId, id, name, pointer, access, subsets, uint16_t)

#define THINGSET_ADD_ITEM_UINT32(parentId, id, name, pointer, access, subsets)                                         \
    THINGSET_ADD_ITEM(parentId, id, name, pointer, access, subsets, uint32_t)

#define THINGSET_ADD_ITEM_UINT64(parentId, id, name, pointer, access, subsets)                                         \
    THINGSET_ADD_ITEM(parentId, id, name, pointer, access, subsets, uint64_t)

#define THINGSET_ADD_ITEM_INT8(parentId, id, name, pointer, access, subsets)                                           \
    THINGSET_ADD_ITEM(parentId, id, name, pointer, access, subsets, int8_t)

#define THINGSET_ADD_ITEM_INT16(parentId, id, name, pointer, access, subsets)                                          \
    THINGSET_ADD_ITEM(parentId, id, name, pointer, access, subsets, int16_t)

#define THINGSET_ADD_ITEM_INT32(parentId, id, name, pointer, access, subsets)                                          \
    THINGSET_ADD_ITEM(parentId, id, name, pointer, access, subsets, int32_t)

#define THINGSET_ADD_ITEM_INT64(parentId, id, name, pointer, access, subsets)                                          \
    THINGSET_ADD_ITEM(parentId, id, name, pointer, access, subsets, int64_t)

#define THINGSET_ADD_ITEM_FLOAT(parentId, id, name, pointer, ignored, access, subsets)                                 \
    THINGSET_ADD_ITEM(parentId, id, name, pointer, access, subsets, float)

#define THINGSET_ADD_ITEM_STRING(parentId, id, name, pointer, length, access, subsets)                                 \
    THINGSET_ADD_ITEM(parentId, id, name, pointer, access, subsets, char)

#define THINGSET_DEFINE_FLOAT_ARRAY(variableName, numDecimals, arr, usedElements) \
    ThingSet::__ArrayHolder<float> variableName = { .array = arr, .maxElements = sizeof(arr), .numElements = usedElements };

#define THINGSET_ADD_ITEM_ARRAY(parentId, id, name, arrayHolder, access, subsets) \
    auto thingset_##id = ThingSet::__PropertyBuilder<parentId, id, name, ThingSet::convertAccess<access>::value>::build(arrayHolder.array);

#define ARRAY_SIZE(array) (sizeof(array) / sizeof((array)[0]))