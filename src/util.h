
#pragma once

#define SAFE_DELETE(p)			{ if(p) { delete (p);     (p)=NULL; } }
#define SAFE_DELETE_ARRAY(p)	{ if(p) { delete[] (p);     (p)=NULL; } }

//! 変数の中身いれかえ
template <typename T>
void Swap(T& a, T& b) {
	T tmp(a);
	a = b;
	b = tmp;
}

//! 変数の中身ゼロクリア
template <typename T>
void Clear(T& a)
{
	memset(&a, 0, sizeof(a));
}

//! インスタンスの解放
template<typename T>
void Release(T& a)
{
	delete a;
	a = NULL;
}

static unsigned long xor128() {     // 周期は 2^128-1
	static unsigned long x=123456789,y=362436069,z=521288629,w=timeGetTime();
	unsigned long t;
	t=(x^(x<<11));
	x=y;y=z;z=w;
	return( w=(w^(w>>19))^(t^(t>>8)) );
}

template <typename T>
static T RandRange(T minimum, T maximum)
{	
	double rand_0_1 = (double)xor128() / (double)0xffffffff;
	return rand_0_1 * (maximum - minimum) + minimum;
}

template <>
static u32 RandRange<u32>(u32 minimum, u32 maximum)
{
	CHECK(minimum <= maximum);
	if (minimum == maximum) {
		return minimum;
	}

	u32 d = maximum - minimum;
	u32 ret= (xor128() % (d + 1)) + minimum;


	return ret;
}

template <typename T>
static T RandRangeMid(T center, T range)
{
	return RandRange(center - range / (T)2, center + range / (T)2);
}

template <typename T>
static T RandVector(const std::vector<T> source, T default = T())
{
	T value = default;
	if (!source.empty()) {
		value = source[RandRange<size_t>(0, source.size() - 1)];
	}
	return value;
}

//! 角度の引き算()
static f32 SubRad(f32 a, f32 b)
{
	f32 f = a - b;
	while (f >= PI) f -= PI * 2;
	while (f < -PI) f += PI * 2;
	return f;
}

template <typename T>
static T Clamp(T x, T min, T max)
{
	return (x < min) ? min : (x > max ? max : x);
}

template <typename T>
static T Min(T x, T min)
{
	return (x < min) ? x : min;
}

template <typename T>
static T Max(T x, T max)
{
	return (max < x) ? x : max;
}

template <typename T>
static T IsInRange(T x, T min, T max)
{
	return (min <= x) && (x <= max);
}

// create new Vec2 in Polar Coordinate System
static vec2 Pcs(f32 radius, f32 angle)
{
	return vec2(cosf(angle), sinf(angle)) * radius;
}

static u32 Color(u8 a, u8 r, u8 g, u8 b)
{
	return ((u32)a << 24) | ((u32)r << 16) | ((u32)g << 8) | (u32)b;
}

extern bool IsCollideSquare(vec2 c1, f32 l1, f32 a1, vec2 c2, f32 l2, f32 a2);

extern void EmitConeParticle(
	u32 amount, vec3 center,
	f32 y_angle_center, f32 y_angle_range,
	f32 xz_angle_center, f32 xz_angle_range,
	f32 velo_center, f32 velo_range,
	u32 tick_center, u32 tick_range,
	vec3 acc_center, vec3 acc_range,
	u32 color );

static void TraceWin32Error()
{
	LPVOID lpMessageBuffer;
	DWORD err = GetLastError();

	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM,
		NULL,
		err,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // デフォルト ユーザー言語 
		(LPTSTR) &lpMessageBuffer,
		0,
		NULL );

	LOG(ERROR) << "Win32API fail code:" << err << ", message:" << lpMessageBuffer;

	// システムによって確保されたバッファを開放します。
	LocalFree( lpMessageBuffer );
}
