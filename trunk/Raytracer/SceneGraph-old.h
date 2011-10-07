#ifndef INCLUDED_SCENE_GRAPH_H
#define INCLUDED_SCENE_GRAPH_H

#include "Mesh.h"
#include "AABBox.h"
#include "Geometry.h"
#include "Clump.h"
#include "Frustum.h"
#include "TargetBuffer.h"

template<class _NumericType> class Light;

template<class _NumericType>
    class SceneGraph
    {
    public:
        typedef _NumericType                        NumericType;
        typedef GAL_imp::Point<NumericType,3>       PointType;
        typedef GAL_imp::Point<NumericType,4>       ColorType;
        typedef GAL_imp::Matrix<NumericType,3>      TransformType;
        typedef GAL_imp::Ray<NumericType,3>         RayType;
        typedef IntersectionPoint<NumericType,3>    IntersectionPointType;
        typedef Clump<NumericType>                  ClumpType;
        typedef std::shared_ptr<ClumpType>          ClumpPtr;
        typedef std::list<ClumpPtr>                 ListType;
        typedef Light<NumericType>                  LightType;
        typedef std::shared_ptr<LightType>          LightPtr;
        typedef std::list<LightPtr>                 ListLights;

        void addClump(const ClumpPtr &clump)
        {
            mList.push_back(clump);
        }

        void addLight(const LightPtr &light)
        {
            mLights.push_back(light);
        }

        bool intersectRay(RayType ray, IntersectionPointType &out)
        {
            return doIntersectRay(ray, out);
        }

        ColorType raytrace(RayType &ray, int recursions)
        {
            IntersectionPointType intersectionPoint;

            if (!intersectRay(ray, intersectionPoint))
            {
                return ColorType();
            }

            ray.direction /= GAL::Len(ray.direction);
            intersectionPoint.normal /= GAL::Len(intersectionPoint.normal);
            intersectionPoint.tangent /= GAL::Len(intersectionPoint.tangent);

            ColorType c1 = shade(ray, intersectionPoint);

            if (0 < recursions)
            {
                RayType reflectedRay;
                reflectedRay.start = intersectionPoint.position;
                reflectedRay.direction = GAL::Reflect(intersectionPoint.normal, ray.direction);
                reflectedRay.start += reflectedRay.direction * 0.5;

                ColorType c2 = raytrace(reflectedRay, recursions - 1);

                return c1 * 0.7 + c2 * 0.5;
            }
            else {
                return c1;
            }
        }

        ColorType shade(RayType &ray, IntersectionPointType &intersectionPoint)
        {
            ColorType c;
            ListLights::const_iterator it = mLights.begin();

            for (; it != mLights.end(); ++it)
            {
                c += (*it)->illuminate(ray, intersectionPoint, 1.0);
            }

            c[0] *= intersectionPoint.color[0];
            c[1] *= intersectionPoint.color[1];
            c[2] *= intersectionPoint.color[2];
            c[3] = intersectionPoint.color[3];

            return c;
        }

        bool shadow(const GAL::P3d &lightPosition, const IntersectionPointType &intersectionPoint)
        {
            RayType lightRay;
            lightRay.start = lightPosition;
            lightRay.direction = (intersectionPoint.position - lightPosition);

            IntersectionPointType lightIntersectionPoint;

            if (!intersectRay(lightRay, lightIntersectionPoint)) {
                return false;
            }

            GAL::P3d distVect = (lightIntersectionPoint.position - intersectionPoint.position);

            return (0.00001 < distVect.Sqr().Sum());
        }

        double shadowtrace(const GAL::P3d &lightPosition, const IntersectionPointType &intersectionPoint)
        {
            static double coeff[5][5] =
            {
                {0.10, 0.15, 0.10},
                {0.15, 0.20, 0.15},
                {0.10, 0.15, 0.10},
            };

            double k = 0.0;
            double lightSmoothWitdh = 0.05;

            GAL::P3d lightZ = intersectionPoint.position - lightPosition;
            lightZ /= GAL::Len(lightZ);

            GAL::P3d lightX = GAL::Orthogonal(lightZ);
            GAL::P3d lightY = GAL::Cross(lightZ, lightX);

            GAL::P3d tmpLightPosition;

            for (int iy = 0; iy < 3; ++iy)
            {
                for (int ix = 0; ix < 3; ++ix)
                {
                    double a = lightSmoothWitdh * double(ix - 1);
                    double b = lightSmoothWitdh * double(iy - 1);
                    
                    tmpLightPosition = lightPosition + lightX * a + lightY * b;

                    if (shadow(tmpLightPosition, intersectionPoint))
                    {
                        k += coeff[ix][iy];
                    }
                }
            }

            return 1.0 - k;
        }

    protected:
        bool doIntersectRay(const RayType &ray, IntersectionPointType &out)
        {
            typedef ListType::const_iterator IteratorType;

            IntersectionPointType tmp;
            IteratorType closest = mList.end();
            out.distance = -1;

            for (IteratorType it = mList.begin(); it != mList.end(); ++it)
            {
                if (!(*it)->intersectRay(ray, tmp))
                {
                    continue;
                }

                if (-1 == out.distance || tmp.distance < out.distance)
                {
                    out = tmp;
                    closest = it;
                }
            }

            if (closest == mList.end())
            {
                return false;
            }

            return true;
        }


    private:
        ListType    mList;
        ListLights  mLights;
    };

typedef SceneGraph<float> SceneGraph3f;
typedef SceneGraph<double> SceneGraph3d;

#endif
