#include <cassert>
#include "logger.h"
#include "transform.h"

namespace Lightning
{
	namespace Render
	{
		using Foundation::Math::Vector4f;
		using Foundation::Math::EulerAnglef;
		using Foundation::Math::PI;
		Transform::Transform():
			mPosition(0.0f, 0.0f, 0.0f)
			,mRotation(0, 0, 0, 1)
			,mScale(1.0f, 1.0f, 1.0f)
			,mMatrixDirty(true)
		{
		}

		Transform::Transform(const Vector3f& pos, const Vector3f& scale, const Quaternionf& rot) :
			mPosition(pos), mScale(scale), mRotation(rot), mMatrixDirty(true)
		{
		}

		void Transform::SetPosition(const Vector3f& position)
		{
			mPosition = position;
			mMatrixDirty = true;
		}

		void Transform::SetRotation(const Quaternionf& rotation)
		{
			assert(rotation.Length() >= 0.999 && rotation.Length() <= 1.0001);
			mRotation = rotation;
			mMatrixDirty = true;
		}

		void Transform::SetScale(const Vector3f& scale)
		{
			mScale = scale;
			mMatrixDirty = true;
		}

		void Transform::UpdateMatrix()
		{
			if (!mMatrixDirty)
				return;
			mMatrixDirty = false;

			Matrix4f matTrans;
			matTrans.SetIdentity();
			matTrans.SetColumn(3, Vector4f(mPosition));

			Matrix4f matRotation;
			mRotation.ToMatrix(matRotation);

			Matrix4f matScale;
			matScale.SetIdentity();
			matScale.SetCell(0, 0, mScale.x);
			matScale.SetCell(1, 1, mScale.y);
			matScale.SetCell(2, 2, mScale.z);

			mMatrix = matTrans * matRotation * matScale;

			matScale.SetIdentity();
			matScale.SetCell(0, 0, float(1.0 / mScale.x));
			matScale.SetCell(1, 1, float(1.0 / mScale.y));
			matScale.SetCell(2, 2, float(1.0 / mScale.z));

			mRotation.Inversed().ToMatrix(matRotation);

			matTrans.SetIdentity();
			matTrans.SetColumn(3, Vector4f(-mPosition));

			mInvMatrix = matScale * matRotation * matTrans;
		}

		void Transform::LookAt(const Vector3f& position, const Vector3f& up)
		{
			auto direction = position - mPosition;
			if (direction.IsZero())
				return;
			OrientTo(direction, up);
		}

		void Transform::OrientTo(Vector3f direction, const Vector3f& up)
		{
			assert(!direction.IsZero());
			assert(up.IsUnitVector());
			direction.Normalize();
			auto right = direction.Cross(up);
			if (right.IsZero())
			{
				right = Right();
			}
			auto desiredUp = right.Cross(direction);

			auto rot1 = Quaternionf::MakeRotation(Vector3f::back(), direction);

			Vector3f newUp = rot1.RotateVector(Vector3f::up());

			auto rot2 = Quaternionf::MakeRotation(newUp, desiredUp);

			mRotation = rot2 * rot1;

			mMatrixDirty = true;
		}

		Matrix4f Transform::LocalToGlobalMatrix4()
		{
			UpdateMatrix();
			return mMatrix;
		}

		Matrix4f Transform::GlobalToLocalMatrix4()
		{
			UpdateMatrix();
			return mInvMatrix;
		}
	}
}
