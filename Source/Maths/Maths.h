#ifndef MATHS_
#define MATHS_

#include "SDL.h"
#include <cassert>

#define ToRadian(x) ((x) * 3.14159 /180.0f)
#define ToDegree(x) ((x) * 180.0f / 3.14159)

typedef unsigned int uint;
template<uint nrows, uint ncols, typename T>
class mat;

//////////////////
// Vectors ///////
//////////////////

template <uint dim, typename T>
struct vec 
{
	vec() 
	{
		for (uint i = 0; i < dim; i++)
		{
			data[i] = T();
		}
	}

	T& operator[](const uint i) { assert(i < dim); return data[i]; }
	const T& operator[](const uint i) const { assert(i < dim); return data[i]; }

private:
	T data[dim];
};

template <typename T>
struct vec<2, T> 
{
	vec() : x(T()), y(T()) {}

	vec(T X, T Y) : x(X), y(Y) {}

	template <class U>
	vec<2, T>(const vec<2, U> &v);

	T& operator[](const uint i) { assert(i < 2); return i <= 0 ? x : y; }
	const T& operator[](const uint i) const { assert(i < 2); return i <= 0 ? x : y; }

	T x, y;
};

template<typename T> 
struct vec<3, T> 
{
	vec() : x(T()), y(T()), z(T()) {}

	vec(T X, T Y, T Z) : x(X), y(Y), z(Z) {}

	template <class U> vec<3, T>(const vec<3, U> &v);

	T& operator[](const uint i) { assert(i < 3); return i <= 0 ? x : (1 == i ? y : z); }
	const T& operator[](const uint i) const { assert(i < 3); return i <= 0 ? x : (1 == i ? y : z); }

	float mag() { return sqrt(x*x + y*y + z*z); }
	vec<3, T>& normalise() { *this = (*this)*(1 / mag()); return *this; }

	T x, y, z;
};

template<typename T>
struct vec<4, T>
{
	vec() : x(T()), y(T()), z(T()), w(T()) {}

	vec(T X, T Y, T Z, T W) : x(X), y(Y), z(Z), w(W) {}

	T& operator[](const uint i) { assert(i < 4); return i <= 0 ? x : (1 == i ? y : (2 == i ? z : w)); }
	const T& operator[](const uint i) const { assert(i < 4); return i <= 0 ? x : (1 == i ? y : (2 == i ? z : w)); }

	float mag() { return sqrt(x*x + y*y + z*z + w*w); }
	vec<4, T>& normalise() { *this = (*this)*(1 / mag()); return *this; }

	T x, y, z, w;
};

template<uint dim, typename T>
T operator*(const vec<dim, T>& lhs, const vec<dim, T>& rhs)
{
	T ret = T();
	for (uint i = dim; i--; ret += lhs[i] * rhs[i]);
	return ret;
}

template<uint dim, typename T>
vec<dim, T> operator+(vec<dim, T> lhs, const vec<dim, T>& rhs)
{
	for (uint i = dim; i--; lhs[i] += rhs[i]);
	return lhs;
}

template<uint dim, typename T>
vec<dim, T> operator-(vec<dim, T> lhs, const vec<dim, T>& rhs)
{
	for (uint i = dim; i--; lhs[i] -= rhs[i]);
	return lhs;
}

template<uint dim, typename T, typename U>
vec<dim, T> operator*(vec<dim, T> lhs, const U& rhs)
{
	for (uint i = dim; i--; lhs[i] *= rhs);
	return lhs;
}

template<uint dim, typename T, typename U>
vec<dim, T> operator/(vec<dim, T> lhs, const U& rhs)
{
	for (uint i = dim; i--; lhs[i] /= rhs);
	return lhs;
}

template<uint len, uint dim, typename T>
vec<len, T> embed(const vec<dim, T>& v, T fill = 1)
{
	vec<len, T> ret;
	for (uint i = len; i--; ret[i] = (i < dim ? v[i] : fill));
	return ret;
}

template<uint len, uint dim, typename T>
vec<len, T> proj(const vec<dim, T>& v)
{
	vec<len, T> ret;
	for (uint i = len; i--; ret[i] =  v[i]);
	return ret;
}

template <typename T>
vec<3, T> cross(vec<3, T> v1, vec<3, T> v2)
{
	return vec<3, T>(v1.y*v2.z - v1.z*v2.y, v1.z*v2.x - v1.x*v2.z, v1.x*v2.y - v1.y*v2.x);
}

//////////////////
// Matrices //////
//////////////////

// This little helper function will recursively calculate the determinant of any square matrix with dimension "dim"
template<uint dim, typename T>
struct dt
{
	static T det(const mat<dim, dim, T>& src)
	{
		T ret = 0;
		for (uint i=dim; i--; ret += src[0][i]*src.cofactor(0, i));
		return ret;
	}
};

// This is required to make sure the recursion above stops at some point
template<typename T>
struct dt<1, T> {
	static T det(const mat<1, 1, T>& src) {
		return src[0][0];
	}
};

template<uint nrows, uint ncols, typename T>
class mat 
{
	vec<ncols, T> rows[nrows];
public:
	mat() {}

	vec<ncols, T>& operator[] (const uint id)
	{
		assert(id < nrows);
		return rows[id];
	}

	const vec<ncols, T>& operator[] (const uint id) const 
	{
		assert(id < nrows);
		return rows[id];
	}

	vec<nrows, T> col(const uint id) const 
	{
		assert(id < ncols);
		vec<nrows, T> ret;
		for (uint i = nrows; i--; ret[i] = rows[i][id]);
		return ret;
	}

	void setcol(const uint id, vec<nrows, T> v)
	{
		assert(id < ncols);
		for (uint i = nrows; i--; rows[i][id] = v[i]);
	}

	static mat<nrows, ncols, T> identity()
	{
		mat<nrows, ncols, T> ret;
		for (uint i = nrows; i--;)
			for (uint j = ncols; j--; ret[i][j] = (i == j));
		return ret;
	}

	T det() const
	{
		// Determinants only work for square matrices
		assert(nrows == ncols);
		return dt<ncols, T>::det(*this);
	}

	mat<nrows - 1, ncols - 1, T> get_minor(uint row, uint col) const 
	{
		mat<nrows - 1, ncols - 1, T> ret;
		for (uint i = nrows - 1; i--;)
			for (uint j = ncols - 1; j--; ret[i][j] = rows[i < row ? i : i + 1][j < col ? j : j + 1]);
		return ret;
	}

	// Recursive
	T cofactor(uint row, uint col) const
	{
		return get_minor(row, col).det()*((row + col) % 2 ? -1 : 1);
	}

	mat<nrows, ncols, T> adjugate() const 
	{
		mat<nrows, ncols, T> ret;
		for (uint i = nrows; i--;)
			for (uint j = ncols; j--; ret[i][j] = cofactor(i, j));
		return ret;
	}

	mat<nrows, ncols, T> invert_transpose()
	{
		mat<nrows, ncols, T> ret = adjugate();
		T temp = ret[0] * rows[0];
		return ret / temp;
	}

	mat<nrows, ncols, T> invert()
	{
		return invert_transpose().transpose();
	}

	mat<ncols, nrows, T> transpose()
	{
		mat<ncols, nrows, T> ret;
		for (uint i = ncols; i--; ret[i] = this->col(i));
		return ret;
	}
};

template<uint nrows, uint ncols, typename T> 
vec<nrows, T> operator*(const mat<nrows, ncols, T>& lhs, const vec<ncols, T>& rhs)
{
	vec<nrows, T> ret;
	// note that lhs[i] is a vector and this is a dot product operation
	for (uint i = nrows; i--; ret[i] = lhs[i] * rhs);
	return ret;
}

template<uint R1, uint C1, uint C2, typename T>
mat<R1, C2, T> operator*(const mat<R1, C1, T>& lhs, const mat<C1, C2, T>& rhs)
{
	mat<R1, C2, T> ret;
	for (uint i = R1; i--;)
		for (uint j = C2; j--; ret[i][j] = lhs[i] * rhs.col(j));
	return ret;
}

template<uint nrows, uint ncols, typename T>
mat<ncols, nrows, T> operator/(mat<nrows, ncols, T> lhs, const T& rhs)
{
	for (size_t i = nrows; i--; lhs[i] = lhs[i] / rhs);
	return lhs;
}

typedef vec<2, float> vec2;
typedef vec<2, int>   vec2i;
typedef vec<3, float> vec3;
typedef vec<3, int>   vec3i;
typedef vec<4, float> vec4;
typedef mat<4, 4, float> mat4;
typedef mat<3, 3, float> mat3;

mat4 MakeScale(vec3 Scaling);

mat4 MakeTranslate(vec3 translate);

mat4 MakeRotate(vec3 rotation);

mat4 MakePerspective(float ScreenWidth, float ScreenHeight, float Near, float Far, float FOV);

mat4 MakeViewport(float width, float height);

mat4 MakeLookAt(vec3 Forward, vec3 Up);

void GetAxesFromRotation(vec3 Rotation, vec3 &Forward, vec3 &Right, vec3 & Up);

#endif
