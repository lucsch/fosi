#include "demutil.h"
#include "tmpercent.h"
#include <ogrsf_frmts.h>

//#include <boost/format.hpp>

void rasterizeMask( const char* shapeFName, const char* myMaskFName, int pixW, int pixH, double myGeoTransf[], GDALDriver *myDriver, int fg, int bg) {
    GDALDataset *shapeDataSource = (GDALDataset*) GDALOpenEx(shapeFName, GDAL_OF_VECTOR, NULL, NULL, NULL);
    OGRLayerH myShapeLayerOGR = shapeDataSource->GetLayer( 0 );
	GDALDataset* myDataSetMask = myDriver->Create( myMaskFName, pixW, pixH, 1, GDT_Byte, NULL );
	myDataSetMask->SetGeoTransform( myGeoTransf );
	myDataSetMask->GetRasterBand(1)->Fill(bg);
	int bl[] = {1};
	double bv = fg;
	CPLErr res = GDALRasterizeLayers( myDataSetMask, 1, bl, 1, &myShapeLayerOGR, NULL, NULL, &bv, NULL, NULL, NULL );
    GDALClose( shapeDataSource );
	GDALClose( myDataSetMask );
}



void saveResDiffs( int nX, int nY, DynArray<MEMARRAYTYPE> &dvResZ, const DynArray<MEMARRAYTYPE> &dvOriZ
	             , const DynArray<unsigned char> &dvFixed, GDALRasterBand *resDiffs
				 , int offsetX, int offsetY, GDALRasterBand *resDO ) {

	//double nodata = -1.7976931348623e+308;
	//double nodata = std::numeric_limits<double>::min();
	double nodata = std::numeric_limits<float>::min();

	if( resDiffs || resDO ) {
		for( int row = 0; row < nY; row++ )
			for( int col = 0; col < nX; col++ )
				if( dvFixed[row][col] || fabs(dvResZ[row][col] - dvOriZ[row][col]) < dTolerance )
					dvResZ[row][col] = nodata;
				else
					dvResZ[row][col] = dvOriZ[row][col] - dvResZ[row][col];

		if( resDiffs ) {
			resDiffs->SetNoDataValue(nodata);
			resDiffs->RasterIO( GF_Write, 0, 0, nX, nY, dvResZ[0], nX, nY, GDT_TYPE, 0, 0 );
		}

		if( resDO ) {
			resDO->SetNoDataValue(nodata);
			resDO->RasterIO( GF_Write, offsetX, offsetY, nX, nY, dvResZ[0], nX, nY, GDT_TYPE, 0, 0 );
		}
	}
}



double mmin4( double a, double b, double c, double d, bool (*comp)(MEMARRAYTYPE, MEMARRAYTYPE) ) {
	double minv = a;
	minv = comp(minv, b) ? b : minv;
	minv = comp(minv, c) ? c : minv;
	minv = comp(minv, d) ? d : minv;
	return minv;
}



double returnSlope( const DynArray<MEMARRAYTYPE> &dvZ, double dx, double dy, int row, int col ) {
	double x[2], y[2], z[2];
	double px, py, pz, norm;

	x[0] = dx * 2;
	y[0] = 0;
	z[0] = dvZ[row][col+1] - dvZ[row][col-1];

	x[1] = 0;
	y[1] = dy * 2;
	z[1] = dvZ[row+1][col] - dvZ[row-1][col];

	px = -z[0] * y[1];
	py = -x[0] * z[1];
	pz = fabs(x[0] * y[1]);

	norm = sqrt( px*px + py*py );
	return atan2(norm, pz);
}



bool fitPlaneV( const DynArray<MEMARRAYTYPE> &dvZ, int nX, int nY, double dx, double dy
	         , int row, int col, double pixR
			 , double &a, double &b, double &c ) {
	int numPoints = 0;
	int ipixR = ceil(pixR);
	double (*points)[3] = new double[(ipixR*2+1)*(ipixR*2+1)][3];
	for(int r1 = -ipixR; r1 <= ipixR; r1++ )
		for(int c1 = -ipixR; c1 <= ipixR; c1++ ) {
			int r2 = row + r1, c2 = col + c1;
			if( (r1 || c1) && r2 >= 0 && r2 < nY && c2 >= 0 && c2 < nX && sqrt(double(r1*r1+c1*c1)) <= pixR + dTolerance ) {
				points[numPoints][0] = dx*c1;
				points[numPoints][1] = dy*r1;
				points[numPoints][2] = dvZ[r2][c2];
				numPoints++;
			}
		}
	bool result = numPoints > 0 && HeightPlaneFit3( numPoints, points, a, b, c );
	delete[] points;
	return result;
}



bool compLT( MEMARRAYTYPE a, MEMARRAYTYPE b ) {
	return a < b;
}



bool compGT( MEMARRAYTYPE a, MEMARRAYTYPE b ) {
	return a > b;
}



int demFit( enumMethod method, GDALRasterBand *resZ, GDALRasterBand *oriZ, GDALRasterBand *fixed, double dx, double dy
	      , ParFit &param, GDALRasterBand *baseZ, GDALRasterBand *resDiffs, int offsetX, int offsetY
		  , GDALRasterBand *resZO, GDALRasterBand *resDO ) {
	int nX = (fixed ? fixed : oriZ)->GetXSize(), nY = (fixed ? fixed : oriZ)->GetYSize();

	bool (*comp)(MEMARRAYTYPE, MEMARRAYTYPE) = (param.increasing ? compLT : compGT);
	int grsign = (param.increasing ? 1 : -1);

	DynArray<MEMARRAYTYPE> dvOriZ(nY, nX);
	oriZ->RasterIO( GF_Read, offsetX, offsetY, nX, nY, dvOriZ[0], nX, nY, GDT_TYPE, 0, 0 );

	DynArray<MEMARRAYTYPE> dvResZ(nY, nX);
	dvResZ.copy( dvOriZ );

	DynArray<unsigned char> dvFixed( fixed ? nY : 0, fixed ? nX : 0 );
	if( fixed )
		fixed->RasterIO( GF_Read,  0, 0, nX, nY, dvFixed[0], nX, nY, GDT_Byte, 0, 0 );

	DynArray<MEMARRAYTYPE> dvBaseZ( param.deepening == ParFit::lowerLimit ? nY : 0, param.deepening == ParFit::lowerLimit ? nX : 0 );
	if( param.deepening == ParFit::lowerLimit )
		baseZ->RasterIO( GF_Read, offsetX, offsetY, nX, nY, dvBaseZ[0], nX, nY, GDT_TYPE, 0, 0 );

	DynArray<MEMARRAYTYPE> dvBorder( nY, nX );
	dvBorder.copy( dvResZ );
	int k = 1, c = 0;
    
    tmPercent myPercent (param.maxiter);
	while( k > 0 && c < param.maxiter ) {
		k = 0;
		c++;
		for( int row = 1; row < nY - 1; row++ )
			for( int col = 1; col < nX - 1; col++ )
				if( !fixed || !dvFixed[row][col] )
				switch(method) {
				case SLBL: {
					double min4 = dvResZ[row][col];
					double lowval;
					switch( param.deepening ) {
					case ParFit::notDeepening:
						lowval = mmin4( dvResZ[row-1][col], dvResZ[row+1][col], dvResZ[row][col-1], dvResZ[row][col+1], comp );
						break;
					case ParFit::lowerLimit:
						lowval = dvBaseZ[row][col];
						break;
					}
					int isMod = 0;
					if( fabs(dvOriZ[row][col] - dvResZ[row][col]) - dTolerance > param.maxThickness ) {
						min4 = dvOriZ[row][col] + grsign * param.maxThickness;
					} else {
						double diff_z_y = fabs(dvResZ[row-1][col] - dvResZ[row+1][col]);
						double diff_z_x = fabs(dvResZ[row][col-1] - dvResZ[row][col+1]);
						double slope = 0;
						if( param.bSlopeLimit )
							slope = returnSlope(dvResZ, dx, dy, row, col);
						double mean = param.tolerance;
						if( diff_z_y > diff_z_x )
							mean += (dvResZ[row-1][col] + dvResZ[row+1][col]) / 2;
						else
							mean += (dvResZ[row][col-1] + dvResZ[row][col+1]) / 2;
						if( comp(dvResZ[row][col], mean) ) {
							if( fabs(dvOriZ[row][col] - mean) - dTolerance <= param.maxThickness )
								min4 = mean;
							else
								min4 = dvOriZ[row][col] + grsign * param.maxThickness;
							if( param.bSlopeLimit && slope < param.dSlopeLimit ) {
								min4 = mean - param.tolerance;
								lowval = min4;
							}
							isMod = 1;
							if( param.deepening != ParFit::normal && comp( lowval, min4 ) )
								min4 = lowval;
						}
					}
					dvBorder[row][col] = min4;
					if( fabs(dvResZ[row][col] - dvBorder[row][col]) > param.mindiff )
						k += isMod;
				} break;
				case planefit: {
					double a, b, c;
					if( fitPlaneV( dvResZ, nX, nY, dx, dy, row, col, param.pixR, a, b, c ) && comp(dvResZ[row][col], c)
						&& fabs(dvOriZ[row][col] - c) < param.maxThickness
						&& (!param.bSlopeLimit || atan(sqrt(a*a+b*b))>param.dSlopeLimit))
						dvBorder[row][col] = c;

					if( dvResZ[row][col] - dvBorder[row][col] > param.mindiff )
						k++;
				} break;
				case LIP: {
					double avg = (dvResZ[row-1][col] + dvResZ[row][col-1] + dvResZ[row+1][col] + dvResZ[row][col+1]) / 4;

					if( param.deepening != ParFit::notDeepening || comp(dvResZ[row][col], avg) )
						dvBorder[row][col] = avg;

					if( fabs(dvResZ[row][col] - dvBorder[row][col]) > param.mindiff )
						k++;
				} break;
				}
		dvResZ.copy( dvBorder );
		
        // update optional wxProgressDialog
        if (param.reportProgressDlg != NULL){
            if (param.reportProgressDlg->WasCancelled() == true) {
                return c;
            }
            
            myPercent.SetValue(c);
            if (myPercent.IsNewStep() == true) {
                param.reportProgressDlg->Update(myPercent.GetPercent(), wxString::Format(_("Please wait (Iteration: %d)"), c));
            }
        }
        
        /*if( param.reportProgress && !(c % param.reportInterval ) )
			if( !param.reportProgress(wxString::Format(_T("Iteration: %d"), c).c_str(), param.reportObject)){
				throw demexception( "Execution Canceled" );
            }*/
	}

	if( resZ )
		resZ->RasterIO( GF_Write, 0, 0, nX, nY, dvResZ[0], nX, nY, GDT_TYPE, 0, 0 );

	if( resZO )
		resZO->RasterIO( GF_Write, offsetX, offsetY, nX, nY, dvResZ[0], nX, nY, GDT_TYPE, 0, 0 );

	param.volume = 0;
	for( int row = 0; row < nY; row++ )
		for( int col = 0; col < nX; col++ )
			param.volume += dvOriZ[row][col] - dvResZ[row][col];

	param.volume *= fabs(dx) * fabs(dy);

	if( param.increasing )
		param.volume = -param.volume;

	saveResDiffs( nX, nY, dvResZ, dvOriZ, dvFixed, resDiffs, offsetX, offsetY, resDO );
	return c;
}



void hsi2rgb( double H, double S, double I, double &R, double &G, double &B ) {
	if (I==0.0) {
		// black image
		R = G = B = 0;
	} else if (S==0.0) {
		// grayscale image
		R = G = B = I;
	} else if (H<1.0/6.0) { // red domain; green acending
		double domainOffset = H;
		R = I;
		B = I * (1-S);
		G = B + (I-B)*domainOffset*6;
	} else if (H<2.0/6) { // yellow domain; red acending
		double domainOffset = H - 1.0/6.0;
		G = I;
		B = I * (1-S);
		R = G - (I-B)*domainOffset*6;
	} else if (H<3.0/6) { // green domain; blue descending
		double domainOffset = H-2.0/6;
		G = I;
		R = I * (1-S);
		B = R + (I-R)*domainOffset * 6;
	} else if (H<4.0/6) { // cyan domain, green acsending
		double domainOffset = H - 3.0/6;
		B = I;
		R = I * (1-S);
		G = B - (I-R) * domainOffset * 6;
	} else if (H<5.0/6) { // blue domain, red ascending
		double domainOffset = H - 4.0/6;
		B = I;
		G = I * (1-S);
		R = G + (I-G) * domainOffset * 6;
	} else { // magenta domain, blue descending
		double domainOffset = H - 5.0/6;
		R = I;
		G = I * (1-S);
		B = R - (I-G) * domainOffset * 6;
	}
}


double planeSlopes( GDALDataset *res, GDALRasterBand *oriZ, double dx, double dy, double pixR ) {
	int nX = oriZ->GetXSize(), nY = oriZ->GetYSize();

	DynArray<MEMARRAYTYPE> dvOriZ(nY, nX);
	oriZ->RasterIO( GF_Read,  0, 0, nX, nY, dvOriZ[0], nX, nY, GDT_TYPE, 0, 0 );

	DynArray<unsigned char> dvr(nY, nX);
	DynArray<unsigned char> dvg(nY, nX);
	DynArray<unsigned char> dvb(nY, nX);

	double maxslope = 0;
	double maxorient = -M_PI, minorient = M_PI;

	for( int row = 0; row < nY; row++ )
		for( int col = 0; col < nX; col++ ) {
			double a, b, c;
			if( fitPlaneV( dvOriZ, nX, nY, dx, dy, row, col, pixR, a, b, c ) ) {
				double orient = atan2( a, b );
				double slope = atan( sqrt( a*a + b*b ) );
				double H = orient / (2*M_PI);
				H = H < 0 ? 1 + H : H;
				double S = 2 * slope / M_PI;
				double R, G, B;
				hsi2rgb( H, S, 1, R, G, B );
				dvr[row][col] = R * 255;
				dvg[row][col] = G * 255;
				dvb[row][col] = B * 255;
				maxslope = maxslope > slope ? maxslope : slope;
				maxorient = maxorient > orient ? maxorient : orient;
				minorient = minorient < orient ? minorient : orient;
			} else
				dvr[row][col] = dvg[row][col] = dvb[row][col] = 255;
		}

	res->GetRasterBand(1)->RasterIO( GF_Write, 0, 0, nX, nY, dvr[0], nX, nY, GDT_Byte, 0, 0 );
	res->GetRasterBand(2)->RasterIO( GF_Write, 0, 0, nX, nY, dvg[0], nX, nY, GDT_Byte, 0, 0 );
	res->GetRasterBand(3)->RasterIO( GF_Write, 0, 0, nX, nY, dvb[0], nX, nY, GDT_Byte, 0, 0 );

	printf( "MinOrient: %g\nMaxOrient: %g\n", minorient * 180. / M_PI, maxorient * 180. / M_PI);

	return maxslope * 180. / M_PI;
}


bool hasNoData( GDALRasterBand *inpZ, int nX, int nY, int offsetX, int offsetY ) {
	DynArray<double> dvInpZ(nY, nX);
	inpZ->RasterIO( GF_Read, offsetX, offsetY, nX, nY, dvInpZ[0], nX, nY, GDT_Float64, 0, 0 );
	int success;
	double nodata = inpZ->GetNoDataValue( &success );
	for( int row = 0; row < nY; row++ )
		for( int col = 0; col < nX; col++ )
			if( dvInpZ[row][col] == nodata || fabs(dvInpZ[row][col]) > 1e4 )
				return true;
	return false;
}
