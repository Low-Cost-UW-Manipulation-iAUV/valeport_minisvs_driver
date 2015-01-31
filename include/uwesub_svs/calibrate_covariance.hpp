#ifndef __CALIBRATE_COVARIANCE__
#define __CALIBRATE_COVARIANCE__
#include <string>
#include <vector>
#include <cstdlib>
namespace calibration {

class calib_covariance {
public:
	calib_covariance(int);
	calib_covariance(void);
	void put_in(double);
	int get_variance(double &);
	int get_offset(double &);
	~calib_covariance();
	bool are_you_calibrated(void);
private:

	bool covariance_found;
	int calibration_length;
	int calibration_counter;
	double variance;
	double offset;
    std::vector<double> variance_data;

	double mean(const std::vector<double> vec);
	double mean(const std::vector<double> vec, typename std::vector<double>::const_iterator , typename std::vector<double>::const_iterator );	
	double std2(const std::vector<double> vec, const double );
};


}

#endif