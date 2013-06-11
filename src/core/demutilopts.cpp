#include "demutil.h"
//#include "matterock.h"
#include <gdal_alg.h>
#include <gdal_priv.h>
#include <ogrsf_frmts.h>
#include <fstream>
//#include <boost/format.hpp>

/*ParFit::ParFit() : bSlopeLimit(false), tolerance( 0 ), maxThickness( 1e5 )
	, deepening( ParFit::normal ), mindiff( .1 ), pixR( 1.5 ), maxiter( 3000 )
	, reportProgress( NULL ), reportInterval( 100 ), reportObject( NULL )
    , increasing(false) {} */

/*void getDEMFitOpts( po::options_description &desc, ParFit &param ) {
	desc.add_options()
		("input-DEM", po::value<string>(), "Input DEM filename")
		("input-DEM-lower-limit", po::value<string>(), "Input lower limit DEM filename")
		("input-mask-raster", po::value<string>(), "Mask raster filename")
		("input-mask-shape", po::value<string>(), "Mask shape filename")
		("output-mask-raster", po::value<string>(), "Rasterized mask output filename")
		("output-DEM", po::value<string>(), "Output DEM filename")
		("output-DEM-diff", po::value<string>(), "Output DEM difference filename")
		("output-RGB", po::value<string>(), "Output RGB filename")
		("demfit.method", po::value<string>(), ((string) "Method for fitting: " + enumStringsMethod[SLBL] + " | " + enumStringsMethod[planefit] + " | " + enumStringsMethod[LIP]).c_str())
		("demfit.mindiff", po::value<double>(&param.mindiff)->default_value(param.mindiff), "Stop computing when no difference is greater than this value")
		("demfit.maxiter", po::value<int>(&param.maxiter)->default_value(param.maxiter), "Maximum number of iterations")
		("demfit.slopelimit", po::value<double>(&param.dSlopeLimit), "Slope limit (degrees)")
		("demfit.maxthickness", po::value<double>(&param.maxThickness)->default_value(param.maxThickness), "Maximum thickness")
		("demfit.multi-feature", "Process each feature of mask shape separately")
		("demfit.output-DEM-type", po::value<string>(), ((string) "Output DEM type when multi-feature enabled: " + enumStringsOutputType[single] + " | " + enumStringsOutputType[multi] + " | " + enumStringsOutputType[both]).c_str())
		("demfit.SHP-volume-field", po::value<string>(), "Shape Field name to save volume differences when multi-feature enabled")
		("demfit.increasing", "Make height values increase instead of decrease")
		("SLBL.tolerance", po::value<double>(&param.tolerance)->default_value(param.tolerance), "SLBL tolerance value")
		("SLBL.notdeepening", "SLBL mode not deepening")
		("planefit.radius", po::value<double>(&param.pixR)->default_value(param.pixR), "Radius in pixels for plane fitting")
		("matterock.options-file", po::value<string>(), "File with options for Matterock")
	;
}*/

/*
string appendNumber( const string &input, int number, const string &ext = ".tif" ) {
	if( input.length() > ext.length() && !input.compare( input.length() - ext.length(), ext.length(), ext ) )
		return (boost::format( "%s-%04d%s" ) % input.substr(0, input.length() - ext.length()) % number % ext).str();
	else
		return (boost::format( "%s-%04d%s" ) % input % number % ext).str();
}
 */
 
struct sReportMF {
	bool (*reportProgress)(const char*, void *reportObject);
	void *reportObject;
	string msg;
};

/*
bool reportMF(const char* msg, void *reportObject) {
	sReportMF &sRep(*(sReportMF*)reportObject);
	return sRep.reportProgress((boost::format("%s%s") % sRep.msg % msg).str().c_str(), sRep.reportObject);
}*/

/*
 int demutilrun( po::variables_map& vm, ParFit& param ) {
	string command_file;
	int retval;

	enumCommand theCommand;
	enumMethod theMethod;
	enumOutputType theOutputType;

	bool multiFeature = false, volumeField = false;
	string volumeFieldName;

	//CPLSetConfigOption("AAIGRID_DATATYPE", "Float64");
	po::options_description desc("Allowed options");
	        
	vector<GDALDataset *> openDs;
	try {
		if( !vm.count("command") )
			throw demexception( "command option required.", "command" );

		if( vm["command"].as<string>() == enumStringsCommand[dem2rgb] )
			theCommand = dem2rgb;
		else if( vm["command"].as<string>() == enumStringsCommand[demfit] )
			theCommand = demfit;
		else if( vm["command"].as<string>() == enumStringsCommand[demtest] )
			theCommand = demtest;
		else if( vm["command"].as<string>() == enumStringsCommand[demmatterock] )
			theCommand = demmatterock;
		else
			throw demexception( "Invalid command.", "command" );

		if( (theCommand == dem2rgb || theCommand == demfit) && !vm.count("input-DEM") )
			throw demexception( "Input DEM required.", "input-DEM" );

		if( theCommand == dem2rgb ) {
			if( !vm.count("output-RGB") )
				throw demexception( "Output RGB required.", "output-RGB" );
		} else if( theCommand == demmatterock ) {
			if( !vm.count("matterock.options-file") )
				throw demexception( "Matterock options file required.", "matterock.options-file" );
		} else if( theCommand == demfit ) {
			if( !vm.count("output-DEM") )
				throw demexception( "Output DEM required.", "output-DEM" );

			if( !vm.count("demfit.method") )
				throw demexception( "Method for fitting required.", "demfit.method" );

			if( vm["demfit.method"].as<string>() == enumStringsMethod[SLBL] )
				theMethod = SLBL;
			else if( vm["demfit.method"].as<string>() == enumStringsMethod[planefit] )
				theMethod = planefit;
			else if( vm["demfit.method"].as<string>() == enumStringsMethod[LIP] )
				theMethod = LIP;
			else
				throw demexception( "Invalid fit method.", "demfit.method" );

			if( vm.count("demfit.increasing") )
				param.increasing = true;

			if( vm.count("demfit.multi-feature") ) {
				if( vm.count("input-mask-raster") )
					throw demexception( "Can not use multi-feature option with mask raster.", "input-mask-raster" );
				if( !vm.count("input-mask-shape") )
					throw demexception( "Multi-feature option requires mask shape.", "demfit.multi-feature" );
				multiFeature = true;
				if( vm.count("demfit.output-DEM-type") ) {
					if( vm["demfit.output-DEM-type"].as<string>() == enumStringsOutputType[multi] )
						theOutputType = multi;
					else if( vm["demfit.output-DEM-type"].as<string>() == enumStringsOutputType[single] )
						theOutputType = single;
					else if( vm["demfit.output-DEM-type"].as<string>() == enumStringsOutputType[both] )
						theOutputType = both;
					else
						throw demexception( "Invalid output DEM type.", "demfit.output-DEM-type" );
				} else
					theOutputType = multi;

				if( vm.count("demfit.SHP-volume-field") ) {
					volumeField = true;
					volumeFieldName = vm["demfit.SHP-volume-field"].as<string>();
				}

			} else if( vm.count("input-mask-shape") && !vm.count("output-mask-raster") )
				throw demexception( "output-mask-raster required if input-mask-shape specified.", "output-mask-raster" );

		}

		GDALAllRegister();
		OGRRegisterAll();

		if( theCommand == demtest )
			return demTest();
		else if( theCommand == demmatterock ) {
            ifstream myStream (vm["matterock.options-file"].as<string>().c_str());
			matterockOptions opt = matterockOptionsRead(myStream);
			return matteRun( opt );
		}

		GDALDataset *poDataset1 = (GDALDataset *) GDALOpen( vm["input-DEM"].as<string>().c_str(), GA_ReadOnly );
		if( !poDataset1 )
			throw demexception( "Can't open Input DEM.", "input-DEM" );
		else
			openDs.push_back( poDataset1 );

		int pixW = poDataset1->GetRasterXSize();
		int pixH = poDataset1->GetRasterYSize();

		double myGeoTransf[6];
		poDataset1->GetGeoTransform(myGeoTransf);

		GDALDriver *myDriver = GetGDALDriverManager()->GetDriverByName("GTiff");

		if( theCommand == dem2rgb ) {
			GDALDataset* poDataset2 = myDriver->Create( vm["output-RGB"].as<string>().c_str(), pixW, pixH, 3, GDT_Byte, NULL );
			if( !poDataset2 )
				throw demexception( "Can't create output RGB", "output-RGB" );
			else
				openDs.push_back( poDataset2 );

			poDataset2->SetGeoTransform( myGeoTransf );

			double maxslope = planeSlopes( poDataset2, poDataset1->GetRasterBand(1), myGeoTransf[1], myGeoTransf[5], param.pixR );
			GDALClose( poDataset2 );
		} else if( theCommand == demfit ) {
			GDALDataset* poDatasetDEMBL = NULL;
			if( vm.count("input-DEM-lower-limit") ) {
				poDatasetDEMBL = (GDALDataset *) GDALOpen( vm["input-DEM-lower-limit"].as<string>().c_str(), GA_ReadOnly );
				if( !poDatasetDEMBL )
					throw demexception( "Can't open input lower limit DEM.", "input-DEM-lower-limit" );
				else
					openDs.push_back( poDatasetDEMBL );
				if( poDatasetDEMBL->GetRasterXSize() != pixW || poDatasetDEMBL->GetRasterYSize() != pixH )
					throw demexception( "Input lower limit DEM dimensions different of Input DEM.", "input-DEM-lower-limit" );
				param.deepening = ParFit::lowerLimit;
			} else if( vm.count( "SLBL.notdeepening" ) )
				param.deepening = ParFit::notDeepening;

			if( vm.count( "demfit.slopelimit" ) ) {
				param.bSlopeLimit = true;
				param.dSlopeLimit *= M_PI / 180.;
			}
			if( !multiFeature ) {
				GDALDataset* poDatasetMask = NULL;
				if( vm.count("input-mask-raster") ) {
					poDatasetMask = (GDALDataset *) GDALOpen( vm["input-mask-raster"].as<string>().c_str(), GA_ReadOnly );
					if( !poDatasetMask )
						throw demexception( "Can't open Input Mask Raster.", "input-mask-raster" );
					else
						openDs.push_back( poDatasetMask );

					if( poDatasetMask->GetRasterXSize() != pixW || poDatasetMask->GetRasterYSize() != pixH )
						throw demexception( "Input Mask Raster dimensions different of Input DEM." );
				} else if( vm.count("input-mask-shape") ) {
					rasterizeMask( vm["input-mask-shape"].as<string>().c_str(), vm["output-mask-raster"].as<string>().c_str(), pixW, pixH, myGeoTransf, myDriver );
					poDatasetMask = (GDALDataset *) GDALOpen( vm["output-mask-raster"].as<string>().c_str(), GA_ReadOnly );
					openDs.push_back( poDatasetMask );
				}
				GDALDataset* poDatasetOutputDEM = myDriver->Create( vm["output-DEM"].as<string>().c_str(), pixW, pixH, 1, GDT_Float32, NULL );
				if( !poDatasetOutputDEM )
					throw demexception( "Can't create output DEM.", "output-DEM" );
				else
					openDs.push_back( poDatasetOutputDEM );
				poDatasetOutputDEM->SetGeoTransform( myGeoTransf );

				GDALDataset* poDatasetOutputDEMDiff = NULL;
				if( vm.count("output-DEM-diff") ) {
					poDatasetOutputDEMDiff = myDriver->Create( vm["output-DEM-diff"].as<string>().c_str(), pixW, pixH, 1, GDT_Float32, NULL );
					if( !poDatasetOutputDEMDiff )
						throw demexception( "Can't create output DEM differences.", "output-DEM-diff" );
					else
						openDs.push_back( poDatasetOutputDEMDiff );

					poDatasetOutputDEMDiff->SetGeoTransform( myGeoTransf );
				}

				retval = demFit( theMethod, poDatasetOutputDEM->GetRasterBand(1), poDataset1->GetRasterBand(1)
						, poDatasetMask ? poDatasetMask->GetRasterBand(1) : NULL
						, myGeoTransf[1], myGeoTransf[5], param
						, poDatasetDEMBL ? poDatasetDEMBL->GetRasterBand(1) : NULL
						, poDatasetOutputDEMDiff ? poDatasetOutputDEMDiff->GetRasterBand(1) : NULL );
			} else {
				GDALDataset* poDatasetOutputDEMSingle = NULL;

				OGRDataSource *shapeDataSource = OGRSFDriverRegistrar::Open( vm["input-mask-shape"].as<string>().c_str(), TRUE );
				OGRLayer *poLayer = shapeDataSource->GetLayer( 0 );
				OGRFeature *poFeature;

				if( theOutputType == single || theOutputType == both ) {
					poDatasetOutputDEMSingle = myDriver->CreateCopy(vm["output-DEM"].as<string>().c_str(), poDataset1, 0, NULL, NULL, NULL);
					if( !poDatasetOutputDEMSingle )
						throw demexception( "Can't create single output DEM.", "output-DEM" );
					else
						openDs.push_back( poDatasetOutputDEMSingle );
				}

				GDALDataset* poDatasetOutputDEMDiffSingle = NULL;
				if( (theOutputType == single || theOutputType == both) && vm.count("output-DEM-diff") ) {
					poDatasetOutputDEMDiffSingle = myDriver->Create( vm["output-DEM-diff"].as<string>().c_str(), pixW, pixH, 1, GDT_Float32, NULL );
					if( !poDatasetOutputDEMDiffSingle )
						throw demexception( "Can't create single output DEM differences.", "output-DEM-diff" );
					else
						openDs.push_back( poDatasetOutputDEMDiffSingle );

					poDatasetOutputDEMDiffSingle->SetGeoTransform( myGeoTransf );
					poDatasetOutputDEMDiffSingle->GetRasterBand(1)->Fill(std::numeric_limits<float>::min());
				}

				int c = 1;

				poLayer->ResetReading();

				sReportMF sRep;
				if( param.reportProgress ) {
					sRep.reportProgress = param.reportProgress;
					sRep.reportObject = param.reportObject;

					param.reportProgress = reportMF;
					param.reportObject = &sRep;
				}

				while( (poFeature = poLayer->GetNextFeature()) != NULL ) {
					sRep.msg = (boost::format( "Feature: %d " ) % c).str();

					poDataset1->GetGeoTransform(myGeoTransf);

					OGRGeometry *poGeometry;
					poGeometry = poFeature->GetGeometryRef();
					OGRGeometryH hGeometry = poGeometry;
					OGRGeometryH *phGeometry = &hGeometry;
					OGREnvelope env;
					poGeometry->getEnvelope(&env);
					int pixW = fabs((env.MaxX - env.MinX)/myGeoTransf[1]) + 5;
					int pixH = fabs((env.MaxY - env.MinY)/myGeoTransf[5]) + 5;
					int pixX = ((myGeoTransf[1] < 0 ? env.MaxX : env.MinX) - myGeoTransf[0])/myGeoTransf[1] - 2;
					int pixY = ((myGeoTransf[5] < 0 ? env.MaxY : env.MinY) - myGeoTransf[3])/myGeoTransf[5] - 2;
					if( pixX >= 0 && pixY >= 0 && pixX + pixW - 1 < poDataset1->GetRasterXSize()
						&& pixY + pixH - 1 < poDataset1->GetRasterYSize() 
						&& !hasNoData(poDataset1->GetRasterBand(1), pixW, pixH, pixX, pixY) ) {
						myGeoTransf[0] += pixX * myGeoTransf[1];
						myGeoTransf[3] += pixY * myGeoTransf[5];
						string myMaskFName;
						if( vm.count("output-mask-raster") ) {
							myMaskFName = appendNumber(vm["output-mask-raster"].as<string>(), c);
						} else
							myMaskFName = "temp-mask.tif";

						GDALDataset* myDataSetMask = myDriver->Create( myMaskFName.c_str(), pixW, pixH, 1, GDT_Byte, NULL );
						myDataSetMask->SetGeoTransform( myGeoTransf );
						myDataSetMask->GetRasterBand(1)->Fill(255);
						int band = 1;
						double fg = 0;
						GDALRasterizeGeometries( myDataSetMask, 1, &band, 1, phGeometry, 0, 0, &fg, 0, 0, 0 );

						GDALDataset* poDatasetOutputDEM = NULL;
						if( theOutputType == multi || theOutputType == both ) {
							poDatasetOutputDEM = myDriver->Create( appendNumber(vm["output-DEM"].as<string>(), c).c_str(), pixW, pixH, 1, GDT_Float32, NULL );
							poDatasetOutputDEM->SetGeoTransform( myGeoTransf );
						}

						GDALDataset* poDatasetOutputDEMDiff = NULL;
						if( (theOutputType == multi || theOutputType == both) && vm.count("output-DEM-diff") ) {
							poDatasetOutputDEMDiff = myDriver->Create( appendNumber(vm["output-DEM-diff"].as<string>(), c).c_str(), pixW, pixH, 1, GDT_Float32, NULL );
							poDatasetOutputDEMDiff->SetGeoTransform( myGeoTransf );
						}

						demFit( theMethod, poDatasetOutputDEM ? poDatasetOutputDEM->GetRasterBand(1) : NULL
							    , poDataset1->GetRasterBand(1)
								, myDataSetMask ? myDataSetMask->GetRasterBand(1) : NULL
								, myGeoTransf[1], myGeoTransf[5], param
								, poDatasetDEMBL ? poDatasetDEMBL->GetRasterBand(1) : NULL
								, poDatasetOutputDEMDiff ? poDatasetOutputDEMDiff->GetRasterBand(1) : NULL, pixX, pixY
								, poDatasetOutputDEMSingle ? poDatasetOutputDEMSingle->GetRasterBand(1) : NULL
								, poDatasetOutputDEMDiffSingle ? poDatasetOutputDEMDiffSingle->GetRasterBand(1) : NULL);

						if( poDatasetOutputDEM )
							GDALClose(poDatasetOutputDEM);
						if( poDatasetOutputDEMDiff )
							GDALClose(poDatasetOutputDEMDiff);
						GDALClose(myDataSetMask);

						int fieldNum;
						if( volumeField && (fieldNum = poFeature->GetFieldIndex(volumeFieldName.c_str())) ) {
							poFeature->SetField(fieldNum, param.volume );
							poLayer->SetFeature( poFeature );
						}
					}

					OGRFeature::DestroyFeature( poFeature );
					c++;
				}
				OGRDataSource::DestroyDataSource(shapeDataSource);
				retval = c - 1;
			}
		}

        const char * myFileName = (vm["output-RGB"].as<string>() + ".cmd.txt").c_str();
        if (theCommand == demfit){
            myFileName = (vm["output-DEM"].as<string>() + ".cmd.txt").c_str();
        }
        
		ofstream ofs ( myFileName );
		po::variables_map::iterator it;
		for( it = vm.begin(); it != vm.end(); it++ ) {
			string *sv;
			double *dv;
			int *iv;
			if( it->first != "command-file" ) {
				ofs << it->first;
				if( sv = boost::any_cast<string>( & it->second.value()) )
					ofs << " = " << *sv;
				else if( dv = boost::any_cast<double>( & it->second.value()) )
					ofs << " = " << *dv;
				else if( iv = boost::any_cast<int>( & it->second.value()) )
					ofs << " = " << *iv;
				ofs << endl;
			}
		}

	} catch(exception&) {
		for( vector<GDALDataset *>::iterator it = openDs.begin(); it != openDs.end(); it++ )
			GDALClose( *it );
        throw;
    }
	for( vector<GDALDataset *>::iterator it = openDs.begin(); it != openDs.end(); it++ )
		GDALClose( *it );

	return retval;
}
 */
