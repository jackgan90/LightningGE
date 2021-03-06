#include <cstdlib>
#include <iostream>
#include "catch.hpp"
#include "Math/Matrix.h"
#include "Math/Vector.h"
#include "Rect.h"
#include "Color.h"

namespace
{
	using Lightning::Foundation::Math::Matrix4f;
	//using Lightning::Foundation::Math::Matrix3f;
	using Lightning::Foundation::Math::Matrix4i;
	//using Lightning::Foundation::Math::Matrix3i;
	using Lightning::Foundation::Math::Vector4f;
	using Lightning::Foundation::Math::Vector4i;
	using Lightning::Foundation::Math::Vector3f;
	using Lightning::Foundation::Math::Vector2f;
	using Lightning::Render::RectF;
	using Lightning::Render::ColorF;

	TEST_CASE("Vector test", "[Vector test]")
	{
		Vector3f v1{ 1.0, 2.0, 3.0};
		const Vector3f v2{ 5.0, 6.0, 7.0};
		auto dotProduct = v1.Dot(v2);
		REQUIRE(dotProduct == Approx(38.0));
		auto v3 = v1.Cross(v2);
		REQUIRE(v3.x == Approx(-4.0));
		REQUIRE(v3.y == Approx(8.0));
		REQUIRE(v3.z == Approx(-4.0));

		Vector3f sum = v1 + v2;
		REQUIRE(sum.x == Approx(6.0f));
		REQUIRE(sum.y == Approx(8.0f));
		REQUIRE(sum.z == Approx(10.0f));

		//constructor
		Vector3f diff = v1 - v2;
		REQUIRE(diff.x == Approx(-4.0f));
		REQUIRE(diff.y == Approx(-4.0f));
		REQUIRE(diff.z == Approx(-4.0f));

		//operator=
		diff = v1 - v2;
		REQUIRE(diff.x == Approx(-4.0f));
		REQUIRE(diff.y == Approx(-4.0f));
		REQUIRE(diff.z == Approx(-4.0f));

		Vector3f zero = Vector3f::Zero();
		Vector3f zero1{ 0.0f, 0.0f, 0.0f };
		REQUIRE(zero == zero1);

		REQUIRE(zero.Length() == Approx(0.0f));

		Vector3f v4{0.0f, 3.0f, 4.0f};
		REQUIRE(v4.Length() == Approx(5.0f));

		v4.Normalize();
		REQUIRE(v4.Length() == Approx(1.0f));

		Vector3f v5{ 1.0f, 2.0f, 3.0f };
		auto v6 = v5.Normalized();
		REQUIRE(v6.Length() == Approx(1.0f));
		REQUIRE(v6.Cross(v5).Length() == Approx(0.0f));

		Vector3f v7(v5);
		REQUIRE(v7 == v5);


		//Vector3f v10;
		//REQUIRE(v10[0] == Approx(1.0f));
		//REQUIRE(v10[1] == Approx(2.0f));
		//REQUIRE(v10[2] == Approx(3.0f));



		Vector3f v13{ 3.0f, 4.0f, 5.0f };
		Vector4f v15{v13.x, v13.y, v13.z, 0};
		REQUIRE(v15.x == Approx(3.0f));
		REQUIRE(v15.y == Approx(4.0f));
		REQUIRE(v15.z == Approx(5.0f));

		Vector3f v16 = -v13;
		REQUIRE(v16.x == Approx(-3.0f));
		REQUIRE(v16.y == Approx(-4.0f));
		REQUIRE(v16.z == Approx(-5.0f));

		Vector3f v17 = -Vector3f{ 1.0f, 2.0f, 3.0f };
		REQUIRE(v17.x == Approx(-1.0f));
		REQUIRE(v17.y == Approx(-2.0f));
		REQUIRE(v17.z == Approx(-3.0f));
	}

	TEST_CASE("Matrix test", "[Matrix test]")
	{
		Matrix4f m;
		m.SetIdentity();
		REQUIRE(m.m[m.CELL_INDEX(0, 0)] == 1);
		REQUIRE(m.m[m.CELL_INDEX(1, 1)] == 1);
		REQUIRE(m.m[m.CELL_INDEX(2, 2)] == 1);
		REQUIRE(m.m[m.CELL_INDEX(3, 3)] == 1);
		for (std::size_t i = 0;i < 4;++i)
		{
			for (std::size_t j = 0; j < 4; ++j)
			{
				if (i == j)
					continue;
				REQUIRE(m.m[m.CELL_INDEX(i, j)] == 0);
			}
		}
		m.m[m.CELL_INDEX(3, 3)] = 10.0;
		REQUIRE(m.m[m.CELL_INDEX(3, 3)] == 10.0);

	}

}