#pragma once
#include <cmath>
#include <functional>
#include "plainobject.h"

namespace Lightning
{
	namespace Foundation
	{
		namespace Math
		{
			template<typename Derived, typename T>
			struct VectorBase : PlainObject<Derived>
			{
				static Derived& Zero()
				{
					static Derived d;
					return d;
				}
				T Dot(const Derived& other)const
				{
					T res{ 0 };
					const Derived *const pDerived = reinterpret_cast<const Derived* const>(this);
					for (int i = 0;i < Derived::Order;++i)
					{
						res += pDerived->operator[](i) * other[i];
					}
					return res;
				}
				Derived operator-()const
				{
					Derived res;
					const Derived *const pDerived = reinterpret_cast<const Derived*const>(this);
					for (int i = 0;i < Derived::Order;++i)
					{
						res[i] = -pDerived->operator[](i);
					}
					return res;
				}

				Derived operator-(const Derived& other)const
				{
					return ComponentwiseOperation(other, [](T comp0, T comp1) {return comp0 - comp1; });
				}

				Derived operator+(const Derived& other)const
				{
					return ComponentwiseOperation(other, [](T comp0, T comp1) {return comp0 + comp1; });
				}

				Derived ComponentwiseOperation(const Derived& other, std::function<T(T,T)> func)const
				{
					Derived res;
					const Derived *pDerived = reinterpret_cast<const Derived* const>(this);
					for (int i = 0; i < Derived::Order; ++i)
					{
						res[i] = func(pDerived->operator[](i), other[i]);
					}
					return res;
				}

				T Length()const
				{
					T length{ 0 };
					const Derived *pDerived = reinterpret_cast<const Derived* const>(this);
					for (int i = 0; i < Derived::Order; ++i)
					{
						length += pDerived->operator[](i) * pDerived->operator[](i);
					}

					return std::sqrt(length);
				}

				void Normalize()
				{
					auto length = Length();
					Derived* const pDerived = reinterpret_cast<Derived* const>(this);
					if (length > 1e-5)
					{
						for (int i = 0;i < Derived::Order;++i)
						{
							auto& val = pDerived->operator[](i);
							val /= length;
						}
					}
				}

				Derived Normalized()const
				{
					const Derived *const pDerived = reinterpret_cast<const Derived *const>(this);
					Derived res(*pDerived);
					res.Normalize();
					return res;
				}
			};

			template<typename T>
			struct Vector2 : VectorBase<Vector2<T>, T>
			{
				static constexpr unsigned Order = 2;
				Vector2() : x(0), y(0){}
				Vector2(T _x, T _y) : x(_x), y(_y){}
				T& operator[](int i) {
					switch (i) {
					case 1:
						return y;
					default:
						return x;
					}
				}

				T operator[](int i)const {
					switch (i) {
					case 1:
						return y;
					default:
						return x;
					}
				}

				T x, y;
			};

			template<typename T>
			struct Vector3 : VectorBase<Vector3<T>, T>
			{
				static constexpr unsigned Order = 3;
				Vector3() : x(0), y(0), z(0){}
				Vector3(T _x, T _y, T _z) : x(_x), y(_y), z(_z){}
				T& operator[](int i) {
					switch (i) {
					case 1:
						return y;
					case 2:
						return z;
					default:
						return x;
					}
				}

				T operator[](int i)const {
					switch (i) {
					case 1:
						return y;
					case 2:
						return z;
					default:
						return x;
					}
				}

				Vector3<T> Cross(const Vector3<T>& other)const
				{
					return Vector3<T>(y * other.z - z * other.y,
						z * other.x - x * other.z,
						x * other.y - y * other.x);
				}
				T x, y, z;
				static const Vector3<T> up;
				static const Vector3<T> down;
				static const Vector3<T> right;
				static const Vector3<T> left;
				static const Vector3<T> forward;
				static const Vector3<T> back;
			};

			template<typename T>
			const Vector3<T> Vector3<T>::up(0, 1, 0);

			template<typename T>
			const Vector3<T> Vector3<T>::down(0, -1, 0);

			template<typename T>
			const Vector3<T> Vector3<T>::right(1, 0, 0);

			template<typename T>
			const Vector3<T> Vector3<T>::left(-1, 0, 0);

			template<typename T>
			const Vector3<T> Vector3<T>::forward(0, 0, 1);

			template<typename T>
			const Vector3<T> Vector3<T>::back(0, 0, -1);

			template<typename T>
			struct Vector4 : VectorBase<Vector4<T>, T>
			{
				static constexpr unsigned Order = 4;
				Vector4() : x(0), y(0), z(0), w(0){}
				Vector4(T _x, T _y, T _z, T _w) : x(_x), y(_y), z(_z), w(_w){}
				Vector4(const Vector3<T>& v) : x(v.x), y(v.y), z(v.z), w(1){}
				T& operator[](int i) {
					switch (i) {
					case 1:
						return y;
					case 2:
						return z;
					case 3:
						return w;
					default:
						return x;
					}
				}

				T operator[](int i)const {
					switch (i) {
					case 1:
						return y;
					case 2:
						return z;
					case 3:
						return w;
					default:
						return x;
					}
				}
				T x, y, z, w;
			};

			template<typename V, typename T>
			std::enable_if_t<std::is_base_of<VectorBase<V, T>, V>::value, void> 
			VectorMulScalar(const V& v, const T& scalar, V& res)
			{
				for (int i = 0;i < V::Order;++i)
				{
					res[i] = v[i] * scalar;
				}
			}

			template<typename V, typename T>
			std::enable_if_t<std::is_base_of<VectorBase<V, T>, V>::value, V> operator*(const V& v, const T& scalar)
			{
				V res;
				VectorMulScalar(v, scalar, res);
				return res;
			}

			template<typename V, typename T>
			std::enable_if_t<std::is_base_of<VectorBase<V, T>, V>::value, V> operator*(const T& scalar, const V& v)
			{
				V res;
				VectorMulScalar(v, scalar, res);
				return res;
			}
			

			using Vector4f = Vector4<float>;
			using Vector3f = Vector3<float>;
			using Vector2f = Vector2<float>;
			using Vector4i = Vector4<int>;
			using Vector3i = Vector3<int>;
			using Vector2i = Vector2<int>;
		}
	}
}