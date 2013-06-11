//#include <boost/program_options.hpp>
//namespace po = boost::program_options;

using namespace std;

class ParFit {
public:
    int maxiter;
	double tolerance;
	double maxThickness;
	bool bSlopeLimit;
	double dSlopeLimit;
	enum {normal, notDeepening, lowerLimit} deepening;
	double mindiff;
    bool returnAsDiff;
	double pixR;
	bool (*reportProgress)(const char*, void *reportObject);
	int reportInterval;
	void *reportObject;
	double volume;
	bool increasing;
	ParFit(){
        maxiter = 3000;
        tolerance = 0.0;
        maxThickness = 1e5;
        bSlopeLimit = false;
        dSlopeLimit = 0.0;
        deepening = ParFit::normal;
        mindiff = 0.1;
        returnAsDiff = false;
        pixR = 1.5;
        reportInterval = 100;
        reportObject = NULL;
        reportProgress = NULL;
        volume = 0.0;
        increasing = false;
    };
};

char const *const enumStringsCommand[] = {"dem2rgb", "demfit", "test", "matterock"};
char const *const enumStringsMethod[] = {"SLBL", "planefit", "LIP"};
enum enumCommand { dem2rgb, demfit, demtest, demmatterock };
enum enumMethod { SLBL, planefit, LIP };

char const *const enumStringsOutputType[] = { "single", "multi", "both" };
enum enumOutputType { single, multi, both };

//void getDEMFitOpts( po::options_description &desc, ParFit &param );

class demexception: public exception {
public:
	demexception( string m, string o = "" ) : msg(m), opt(o) { }
	virtual ~demexception() throw() {}
    virtual const char *what() const throw() { return msg.c_str(); };
    
	inline string getOpt() { return opt; };
private:
	string msg, opt;
};

//int demutilrun( po::variables_map& vm, ParFit& param );
int demTest();
