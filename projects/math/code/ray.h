#pragma once

#pragma once

#include "vector3.h"
#include "plane.h"

#define EPSILON 0.00001

namespace efiilj
{
	class ray
	{
		private:


		public:

			ray(const vector3& origin, const vector3& direction)
			{
				this->origin = origin;
				this->direction = direction;
			}

			vector3 origin, direction;

			bool intersect(const plane& plane, vector3& result)
			{
				float denom = vector3::dot(direction, plane.normal);

				if (denom > EPSILON)
				{
					vector3 pl = plane.offset - origin;
					vector3 t = vector3::dot(pl, plane.normal) / denom;
					result = origin + direction * t;

					return true;
				}

				return false;
			}
	};
}
