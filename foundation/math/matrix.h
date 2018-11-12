#pragma once
#include <type_traits>
#include "vector.h"

namespace Lightning
{
	namespace Foundation
	{
		namespace Math
		{
			//column major matrix
			template<typename Derived, typename T>
			struct MatrixBase : PlainObject<Derived>
			{
				void SetZero()
				{
					std::memset(this, 0, sizeof(Derived));
				}

				void SetIdentity()
				{
					std::memset(this, 0, sizeof(Derived));
					Derived* const pDerived = reinterpret_cast<Derived* const>(this);
					for (int i = 0; i < Derived::Order; ++i)
						pDerived->m[CELL_INDEX(i, i)] = 1;
				}

				void SetCell(unsigned row, unsigned col, const T& value)
				{
					Derived* const pDerived = reinterpret_cast<Derived* const>(this);
					pDerived->m[CELL_INDEX(row, col)] = value;
				}

				void MultMatrix(const Derived& other, Derived& mat)const
				{
					const Derived *const pDerived = reinterpret_cast<const Derived* const>(this);
					for (int i = 0;i < Derived::Order;++i)
					{
						for (int j = 0; j < Derived::Order; ++j)
						{
							auto idx = CELL_INDEX(i, j);
							mat.m[idx] = 0;
							for (int k = 0; k < Derived::Order; ++k)
								mat.m[idx] += pDerived->m[CELL_INDEX(i, k)] * other.m[CELL_INDEX(k, j)];
						}
					}
				}

				Derived operator*(const Derived& other)const
				{
					Derived mat;
					MultMatrix(other, mat);
					return mat;
				}

				Derived& operator*=(const Derived& other)
				{
					auto pDerived = reinterpret_cast<Derived*>(this);
					*pDerived = *pDerived * other;
					return *pDerived;
				}

				template<typename V>
				std::enable_if_t<std::is_base_of<VectorBase<V, T>, V>::value, V> operator*(const V& v)const
				{
					static_assert(Derived::Order == V::Order, "matrix multiply vector requires the same dimension!");
					V res;
					const Derived* const pDerived = reinterpret_cast<const Derived* const>(this);
					auto& m = pDerived->m;
					for (int i = 0;i < Derived::Order;++i)
					{
						T s{ 0 };
						for (int j = 0; j < Derived::Order;++j)
						{
							s += m[CELL_INDEX(i, j)] * v[j];
						}
						res[i] = s;
					}
					return res;
				}
				
				static inline std::size_t CELL_INDEX(std::size_t row, std::size_t col) { return col * Derived::Order + row; }
			};

			template<typename T>
			struct Matrix3 : MatrixBase<Matrix3<T>, T>
			{
				static constexpr unsigned Order = 3;
				void SetRow(unsigned row, const Vector3<T>& v) { SetMatrixRow(*this, row, v); }
				void SetColumn(unsigned col, const Vector3<T>& v) { SetMatrixColumn(*this, col, v); }
				T m[Order * Order];
			};

			template<typename T>
			struct Matrix4 : MatrixBase<Matrix4<T>, T>
			{
				static constexpr unsigned Order = 4;
				void SetRow(unsigned row, const Vector4<T>& v) { SetMatrixRow(*this, row, v); }
				void SetColumn(unsigned col, const Vector4<T>& v) { SetMatrixColumn(*this, col, v); }
				T m[Order * Order];
			};

			template<typename MatrixType, typename VectorType>
			void SetMatrixColumn(MatrixType& mat, unsigned col, const VectorType& v)
			{
				static_assert(MatrixType::Order == VectorType::Order, "vector order must match matrix order!");
				if (col >= 0 && col < MatrixType::Order)
				{
					for (int i = 0; i < MatrixType::Order; ++i)
					{
						mat.m[MatrixType::CELL_INDEX(i, col)] = v[i];
					}
				}
			}

			template<typename MatrixType, typename VectorType>
			void SetMatrixRow(MatrixType& mat, unsigned row, const VectorType& v)
			{
				static_assert(MatrixType::Order == VectorType::Order, "vector order must match matrix order!");
				if (row >= 0 && row < MatrixType::Order)
				{
					for (int i = 0; i < MatrixType::Order; ++i)
					{
						mat.m[MatrixType::CELL_INDEX(row, i)] = v[i];
					}
				}
			}

			using Matrix4f = Matrix4<float>;
			using Matrix4i = Matrix4<int>;
			using Matrix3f = Matrix3<float>;
			using Matrix3i = Matrix3<int>;
		}

	}
}
