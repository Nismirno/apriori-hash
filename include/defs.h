/* ===========================================================================
   $File: defs.h $
   $Date: 11 September 2018. $
   $Revision: $
   $Author: Nikita Smirnov $
   =========================================================================== */

#ifndef DEFS_H
#define DEFS_H

#include <cstdint>
#include <string>
#include <vector>

#define int8  int8_t
#define int16 int16_t
#define int32 int32_t
#define int64 int64_t

#define uint8  uint8_t
#define uint16 uint16_t
#define uint32 uint32_t
#define uint64 uint64_t

struct matrixDimension {
	uint32 rows;
	uint32 columns;
};

enum Order {
	NONE = 0,
	ASC,
	DESC
};

std::string vecToStr(std::vector<uint32> vec);
#endif
