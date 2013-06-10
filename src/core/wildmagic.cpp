#include <math.h>
#include "demutil.h"

bool Solve3 (const double A[3][3], const double B[3], double X[3])
{
    double invA[3][3];
    invA[0][0] = A[1][1]*A[2][2]-A[1][2]*A[2][1];
    invA[0][1] = A[0][2]*A[2][1]-A[0][1]*A[2][2];
    invA[0][2] = A[0][1]*A[1][2]-A[0][2]*A[1][1];
    invA[1][0] = A[1][2]*A[2][0]-A[1][0]*A[2][2];
    invA[1][1] = A[0][0]*A[2][2]-A[0][2]*A[2][0];
    invA[1][2] = A[0][2]*A[1][0]-A[0][0]*A[1][2];
    invA[2][0] = A[1][0]*A[2][1]-A[1][1]*A[2][0];
    invA[2][1] = A[0][1]*A[2][0]-A[0][0]*A[2][1];
    invA[2][2] = A[0][0]*A[1][1]-A[0][1]*A[1][0];
    double det = A[0][0]*invA[0][0] + A[0][1]*invA[1][0] + A[0][2]*invA[2][0];

    if (fabs(det) < dTolerance)
    {
        return false;
    }

    double invDet = ((double)1)/det;
    for (int row = 0; row < 3; ++row)
    {
        for (int col = 0; col < 3; ++col)
        {
            invA[row][col] *= invDet;
        }
    }

    X[0] = invA[0][0]*B[0] + invA[0][1]*B[1] + invA[0][2]*B[2];
    X[1] = invA[1][0]*B[0] + invA[1][1]*B[1] + invA[1][2]*B[2];
    X[2] = invA[2][0]*B[0] + invA[2][1]*B[1] + invA[2][2]*B[2];
    return true;
}

bool HeightPlaneFit3 (int numPoints, const double points[][3], double& a,
    double& b, double& c)
{
    // You need at least three points to determine the plane.  Even so, if
    // the points are on a vertical plane, there is no least-squares fit in
    // the 'height' sense.  This will be trapped by the determinant of the
    // coefficient matrix.

    // Compute sums for linear system.
    double sumX = (double)0, sumY = (double)0, sumZ = (double)0.0;
    double sumXX = (double)0, sumXY = (double)0, sumXZ = (double)0;
    double sumYY = (double)0, sumYZ = (double)0;
    int i;
    for (i = 0; i < numPoints; ++i)
    {
        sumX += points[i][0];
        sumY += points[i][1];
        sumZ += points[i][2];
        sumXX += points[i][0]*points[i][0];
        sumXY += points[i][0]*points[i][1];
        sumXZ += points[i][0]*points[i][2];
        sumYY += points[i][1]*points[i][1];
        sumYZ += points[i][1]*points[i][2];
    }

    double A[3][3] =
    {
        {sumXX, sumXY, sumX},
        {sumXY, sumYY, sumY},
        {sumX,  sumY,  (double)numPoints}
    };

    double B[3] =
    {
        sumXZ,
        sumYZ,
        sumZ
    };

    double X[3];

    bool nonsingular = Solve3(A, B, X);
    if (nonsingular)
    {
        a = X[0];
        b = X[1];
        c = X[2];
    }
    //else
    //{
    //    a = MAX_REAL;
    //    b = MAX_REAL;
    //    c = MAX_REAL;
    //}

    return nonsingular;
}
