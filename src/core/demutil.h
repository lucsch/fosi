#include "wx/wxprec.h"
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif
#include <gdal_alg.h>
#include <gdal_priv.h>

#ifndef M_PI
#define M_PI (acos(-1.0))
#endif

#include "demutilopts.h"

static const double dTolerance = 1e-8; //1e-3
bool HeightPlaneFit3 (int numPoints, const double points[][3], double& a,
    double& b, double& c);

void rasterizeMask( const char* shapeFName, const char* myMaskFName, int pixW, int pixH
	              , double myGeoTransf[], GDALDriver *myDriver, int fg = 0, int bg = 255);
int demFit( enumMethod method, GDALRasterBand *resZ, GDALRasterBand *oriZ, GDALRasterBand *fixed
	      , double dx, double dy, ParFit& param, GDALRasterBand *baseZ = NULL
          , GDALRasterBand *resDiffs = NULL, int offsetX = 0, int offsetY = 0
		  , GDALRasterBand *resZO = NULL, GDALRasterBand *resDO = NULL );
bool HeightPlaneFit3 (int numPoints, const double points[][3], double& a, double& b, double& c);
double planeSlopes( GDALDataset *res, GDALRasterBand *oriZ, double dx, double dy, double pixR );
bool hasNoData( GDALRasterBand *inpZ, int nX, int nY, int offsetX, int offsetY );
void mattest();

#define MEMARRAYTYPE float
#define GDT_TYPE GDT_Float32

template<class T> class DynArray {
public:
	DynArray( int rows, int cols ) {
		d = (T *) CPLMalloc(sizeof(T)*rows*cols);
		nCols = cols;
		nRows = rows;
	}
	~DynArray() {
		CPLFree( d );
	}
	DynArray &copy( const DynArray& o ) {
		//Must have the same dimensions
		memcpy( d, o.d, sizeof(T)*nRows*nCols );
		return *this;
	}
	T *operator[](int r) {
		return d + nCols * r;
	}
	T const *operator[](int r) const {
		return d + nCols * r;
	}
private:
	T *d;
	int nCols, nRows;
};
