#ifndef __included_id_h
#define __included_id_h


/*
 * There are various types of id numbers used, namely client ids,
 * object ids, and also object collision tags.  They are different,
 * but out of convenience they are often the same.
 *
 * Each object has a unique id number.  All machines playing the same
 * game have the same id numbers for objects.  Objects representing
 * clients have the client's id number as their object id number.
 *
 * Collision tags classes which consist only of a single client and
 * it's bullets (etc.) have the client id as their tag.
 *
 * The number zero is often given special meaning.
 *
 * Otherwise these are distinct types.  We keep these typedefs here so
 * server header files don't have to include client.h, etc.
 *
 * There is more information in object.h.
 */

typedef uint32_t client_id_t;
typedef uint32_t objid_t;
typedef uint32_t objtag_t;


#endif
