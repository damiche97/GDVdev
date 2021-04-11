// ========================================================================= //
// Authors: Roman Getto, Matthias Bein                                       //
// mailto:roman.getto@gris.informatik.tu-darmstadt.de                        //
//                                                                           //
// GRIS - Graphisch Interaktive Systeme                                      //
// Technische Universität Darmstadt                                          //
// Fraunhoferstrasse 5                                                       //
// D-64283 Darmstadt, Germany                                                //
//                                                                           //
// ========================================================================== //

#include "Vec3.h"

template<class T>
class Ray {

  typedef Vec3<T> Vec3_;

 public:
	 	 
  Vec3_ d,o;

  Ray(const Vec3_ origin, const Vec3_ normalized_direction)
    : d {normalized_direction}, o{origin} {}

  Ray(const T origin[3], const T p[3])
    {
      o[0] = origin[0];
      o[1] = origin[1];
      o[2] = origin[2];

      d[0] = p[0] - o[0];
      d[1] = p[1] - o[1];
      d[2] = p[2] - o[2];

      d = d.normalized();
    }

  /**
   * @param p0 corner of a triangle
   * @param p1 corner of a triangle
   * @param p2 corner of a triangle
   * @param u output parameter where the barycentric coordinate where this ray
   *   hits the triangle is stored
   * @param v output parameter for other barycentric coordinate
   * @param rayt output parameter, point on raym where the triangle was hit
   *
   * @returns boolean which indicates, whether the triangle given by p0,p1,p2
   *   was hit by the ray
   **/
  bool  triangleIntersect(const T p0[3],const T p1[3],const T p2[3],T& u, T& v, T &rayt) const
   {
     // alternative algorithm based on https://en.wikipedia.org/wiki/M%C3%B6ller%E2%80%93Trumbore_intersection_algorithm
     //const float EPSILON = 0.0000001;

     //const Vec3_ rayOrigin = o;
     //const Vec3_ rayVector = d;

     //const Vec3_ vertex0 {
     //  p0[0], p0[1], p0[2]
     //};
     //const Vec3_ vertex1 {
     //  p1[0], p1[1], p1[2]
     //};
     //const Vec3_ vertex2 {
     //  p2[0], p2[1], p2[2]
     //};

     //// Find edge vectors for vertex 0
     //const Vec3_ edge1 = vertex1 - vertex0;
     //const Vec3_ edge2 = vertex2 - vertex0;

     //// calculate determinant
     //const Vec3_ pvec = rayVector ^ edge2;
     //const float det = edge1 * pvec;

     //// if determinant is near zero, ray is in plane of triangle
     //if (det > -EPSILON && det < EPSILON)
     //  return false;    // This ray is parallel to this triangle.

     //// Vector from vertex 0 to ray origin
     //const Vec3_ toOrigin = rayOrigin - vertex0;

     //// we will normalize the barycentric coordinates over the determinant by factor f
     //const float invDet = 1.0/det;

     //// calculate barycentric coordinate u
     //u = invDet * (toOrigin * pvec);

     //// u coordinate out of bounds of triangle?
     //if (u < 0.0 || u > 1.0)
     //  return false;

     //// calculate barycentric coordinate v
     //const Vec3f qvec = toOrigin ^ edge1;
     //v = invDet * (rayVector * qvec);

     //// v coordinate out of bounds of triangle?
     //if (v < 0.0 || u + v > 1.0)
     //  return false;

     //// ray intersects, intersection point can now be computed
     //rayt = invDet * (edge2 * qvec);
     //return true;
     const Vec3_ e1(
         p1[0]-p0[0],
	       p1[1]-p0[1],
	       p1[2]-p0[2]
     );

     const Vec3_ e2(
         p2[0]-p0[0],
         p2[1]-p0[1],
         p2[2]-p0[2]
     );

     const Vec3_ t(
         o[0]-p0[0],
	       o[1]-p0[1],
	       o[2]-p0[2]
     );

     const Vec3_ p = d^e2;
     const Vec3_ q = t^e1;

     const T d1 = p*e1;
     if (fabs(d1) < 10e-7)
       return false;

     const T f = 1.0f/d1;

     u = f*(p*t);

     if (u < 0 || u > 1.0)
       return false;

     v = f*(q*d);

     if (v < 0.0 || v > 1.0 || (u+v) > 1.0)
       return false;

     rayt = f*(q*e2);

     return true;
   }


};
