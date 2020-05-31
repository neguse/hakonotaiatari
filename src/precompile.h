
#pragma once

#if _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

// windows
#include <Windows.h>
#include <mmsystem.h>

// directx
#include <XAudio2.h>

// cdbpp
#include <cdbpp.h>

// sqlite3
#include <sqlite3/sqlite3.h>

// standard library
#include <vector>
#include <list>
#include <string>
#include <map>
#include <fstream>
#include <cmath>
#include <float.h>
#include <stdlib.h>
#include <stdio.h>

// GLM
#define GLM_FORCE_INLINE
#define GLM_SWIZZLE_XYZW
#include <glm/glm.hpp>
#include <glm/gtx/projection.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_precision.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/quaternion.hpp>

using glm::f32;
using glm::f64;
using glm::i8;
using glm::i16;
using glm::i32;
using glm::i64;
using glm::u8;
using glm::u16;
using glm::u32;
using glm::u64;
using glm::vec2;
using glm::vec3;
using glm::vec4;
using glm::mat3;
using glm::mat4;
using glm::quat;

// glog
#define GLOG_NO_ABBREVIATED_SEVERITIES
#define GOOGLE_GLOG_DLL_DECL
#include <glog/logging.h>

// 
#include "const.h"
#include "variable.h"
#include "return_code.h"
#include "util.h"

